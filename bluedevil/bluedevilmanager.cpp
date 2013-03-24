/*****************************************************************************
 * This file is part of the BlueDevil project                                *
 *                                                                           *
 * Copyright (C) 2010 Rafael Fernández López <ereslibre@kde.org>             *
 * Copyright (C) 2010 UFO Coders <info@ufocoders.com>                        *
 *                                                                           *
 * This library is free software; you can redistribute it and/or             *
 * modify it under the terms of the GNU Library General Public               *
 * License as published by the Free Software Foundation; either              *
 * version 2 of the License, or (at your option) any later version.          *
 *                                                                           *
 * This library is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public License *
 * along with this library; see the file COPYING.LIB.  If not, write to      *
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
 * Boston, MA 02110-1301, USA.                                               *
 *****************************************************************************/

#include "bluedevilmanager.h"
#include "bluedeviladapter.h"

#include "bluedevil/bluezmanager.h"

#include <QtCore/QHash>

#include <QtDBus/QDBusConnectionInterface>

namespace BlueDevil {

static Manager *instance = 0;

class Manager::Private
{
public:
    Private(Manager *q);
    ~Private();

    void initialize();
    void clean();
    Adapter *findUsableAdapter();

    void _k_adapterAdded(const QDBusObjectPath &objectPath);
    void _k_adapterRemoved(const QDBusObjectPath &objectPath);
    void _k_defaultAdapterChanged(const QDBusObjectPath &objectPath);
    void _k_adapterPoweredChanged(bool powered);
    void _k_propertyChanged(const QString &property, const QDBusVariant &value);

    void _k_bluezServiceRegistered();
    void _k_bluezServiceUnregistered();

    OrgBluezManagerInterface *m_bluezManagerInterface;
    Adapter                  *m_defaultAdapter;
    Adapter                  *m_usableAdapter;
    QHash<QString, Adapter*>  m_adaptersHash;
    bool                      m_bluezServiceRunning;

    Manager *const m_q;
};

Manager::Private::Private(Manager *q)
    : m_bluezManagerInterface(0)
    , m_defaultAdapter(0)
    , m_usableAdapter(0)
    , m_q(q)
{
    m_bluezServiceRunning = false;
    if (QDBusConnection::systemBus().isConnected()) {
        QDBusReply<bool> reply = QDBusConnection::systemBus().interface()->isServiceRegistered("org.bluez");

        if (reply.isValid()) {
            m_bluezServiceRunning = reply.value();
        }
    }
}

Manager::Private::~Private()
{
    delete m_bluezManagerInterface;
}

void Manager::Private::initialize()
{
    if (QDBusConnection::systemBus().isConnected() && m_bluezServiceRunning) {
        m_bluezManagerInterface = new OrgBluezManagerInterface("org.bluez", "/", QDBusConnection::systemBus(), m_q);

        connect(m_bluezManagerInterface, SIGNAL(AdapterAdded(QDBusObjectPath)),
                m_q, SLOT(_k_adapterAdded(QDBusObjectPath)));
        connect(m_bluezManagerInterface, SIGNAL(AdapterRemoved(QDBusObjectPath)),
                m_q, SLOT(_k_adapterRemoved(QDBusObjectPath)));
        connect(m_bluezManagerInterface, SIGNAL(DefaultAdapterChanged(QDBusObjectPath)),
                m_q, SLOT(_k_defaultAdapterChanged(QDBusObjectPath)));
        connect(m_bluezManagerInterface, SIGNAL(PropertyChanged(QString,QDBusVariant)),
                m_q, SLOT(_k_propertyChanged(QString,QDBusVariant)));

        QString defaultAdapterPath;
        const QDBusReply<QDBusObjectPath> reply = m_bluezManagerInterface->DefaultAdapter();
        if (reply.isValid()) {
            defaultAdapterPath = reply.value().path();
            if (!defaultAdapterPath.isEmpty()) {
                m_defaultAdapter = new Adapter(defaultAdapterPath, m_q);
                connect(m_defaultAdapter, SIGNAL(poweredChanged(bool)), m_q, SLOT(_k_adapterPoweredChanged(bool)));
                m_adaptersHash.insert(defaultAdapterPath, m_defaultAdapter);
                emit m_q->defaultAdapterChanged(m_defaultAdapter);
            }
        }
        const QVariantMap properties = m_bluezManagerInterface->GetProperties().value();
        const QList<QDBusObjectPath> adapters = qdbus_cast<QList<QDBusObjectPath> >(properties["Adapters"].value<QDBusArgument>());
        if (adapters.count() > 1) {
            Q_FOREACH (const QDBusObjectPath &path, adapters) {
                if (path.path() != defaultAdapterPath) {
                    Adapter *const adapter = new Adapter(path.path(), m_q);
                    connect(adapter, SIGNAL(poweredChanged(bool)), m_q, SLOT(_k_adapterPoweredChanged(bool)));
                    m_adaptersHash.insert(path.path(), adapter);
                }
            }
        }

        m_usableAdapter = findUsableAdapter();
        emit m_q->usableAdapterChanged(m_usableAdapter);
    }
}

void Manager::Private::clean()
{
    qDebug() << "Private::clean";
    delete m_bluezManagerInterface;
    QHashIterator<QString, Adapter*> i(m_adaptersHash);
    while (i.hasNext()) {
        i.next();
        Adapter *adapter = m_adaptersHash.take(i.key());
        emit m_q->adapterRemoved(adapter);
        delete adapter;
    }

    m_usableAdapter = 0;
    m_defaultAdapter = 0;

    emit m_q->usableAdapterChanged(0);
    emit m_q->defaultAdapterChanged(0);
}
Adapter *Manager::Private::findUsableAdapter()
{
    Adapter *const defAdapter = m_q->defaultAdapter();
    if (defAdapter && defAdapter->isPowered()) {
        m_usableAdapter = defAdapter;
        return defAdapter;
    }
    Q_FOREACH (Adapter *const adapter, m_q->adapters()) {
        if (adapter->isPowered()) {
            m_usableAdapter = adapter;
            return adapter;
        }
    }
    return 0;
}

void Manager::Private::_k_adapterAdded(const QDBusObjectPath &objectPath)
{
    qDebug() << "Added: " << objectPath.path();
    Adapter *const adapter = new Adapter(objectPath.path(), m_q);
    connect(adapter, SIGNAL(poweredChanged(bool)), m_q, SLOT(_k_adapterPoweredChanged(bool)));

    m_adaptersHash.insert(objectPath.path(), adapter);
    if (!m_defaultAdapter) {
        m_defaultAdapter = adapter;
        emit m_q->defaultAdapterChanged(m_defaultAdapter);
    }
    if (!m_usableAdapter || !m_usableAdapter->isPowered()) {
        Adapter *const oldUsableAdapter = m_usableAdapter;
        m_usableAdapter = findUsableAdapter();
        if (m_usableAdapter != oldUsableAdapter) {
            emit m_q->usableAdapterChanged(m_usableAdapter);
        }
    }
    emit m_q->adapterAdded(adapter);
}

void Manager::Private::_k_adapterRemoved(const QDBusObjectPath &objectPath)
{
    qDebug() << "Removed: " << objectPath.path();
    Adapter *const adapter = m_adaptersHash.take(objectPath.path()); // return and remove it from the hash
    if (m_adaptersHash.isEmpty()) {
        m_defaultAdapter = 0;
        m_usableAdapter = 0;
    }
    if (adapter) {
        emit m_q->adapterRemoved(adapter);
        delete adapter;
    }
    if (m_adaptersHash.isEmpty()) {
        emit m_q->defaultAdapterChanged(0);
        emit m_q->usableAdapterChanged(0);
        emit m_q->allAdaptersRemoved();
    } else {
        if (m_usableAdapter) {
            Adapter *const oldUsableAdapter = m_usableAdapter;
            m_usableAdapter = findUsableAdapter();
            if (m_usableAdapter != oldUsableAdapter) {
                emit m_q->usableAdapterChanged(m_usableAdapter);
            }
        }
    }
}

void Manager::Private::_k_defaultAdapterChanged(const QDBusObjectPath &objectPath)
{
    Adapter *adapter = m_adaptersHash[objectPath.path()];
    if (!adapter) {
        adapter = new Adapter(objectPath.path(), m_q);
        connect(adapter, SIGNAL(poweredChanged(bool)), m_q, SLOT(_k_adapterPoweredChanged(bool)));
        m_adaptersHash.insert(objectPath.path(), adapter);
    }
    m_defaultAdapter = adapter;
    emit m_q->defaultAdapterChanged(adapter);
}

void Manager::Private::_k_adapterPoweredChanged(bool powered)
{
    Adapter *adapter = qobject_cast< Adapter* >(m_q->sender());
    if (!adapter) {
        qWarning("adapterPoweredChanged was called from a null adapter");
        return;
    }

    if (m_usableAdapter && adapter != m_usableAdapter) {
        return;//The modified adapter is not the usable one, ignore the event (we do not care)
    }

    if (m_defaultAdapter && !powered) {
        m_defaultAdapter = 0;
        emit m_q->usableAdapterChanged(0);
        return;
    }

    m_defaultAdapter = adapter;
    emit m_q->usableAdapterChanged(m_defaultAdapter);
}

void Manager::Private::_k_propertyChanged(const QString &property, const QDBusVariant &value)
{
    Q_UNUSED(property)
    Q_UNUSED(value)
}

void Manager::Private::_k_bluezServiceRegistered()
{
    m_bluezServiceRunning = true;
    initialize();
}

void Manager::Private::_k_bluezServiceUnregistered()
{
    m_bluezServiceRunning = false;
    clean();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Manager::Manager(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
    // Keep an eye open if bluez stops running
    QDBusServiceWatcher *serviceWatcher = new QDBusServiceWatcher("org.bluez", QDBusConnection::systemBus(),
                                                                  QDBusServiceWatcher::WatchForRegistration |
                                                                  QDBusServiceWatcher::WatchForUnregistration, this);
    connect(serviceWatcher, SIGNAL(serviceRegistered(QString)), this, SLOT(_k_bluezServiceRegistered()));
    connect(serviceWatcher, SIGNAL(serviceUnregistered(QString)), this, SLOT(_k_bluezServiceUnregistered()));

    d->initialize();
}

Manager::~Manager()
{
    delete d;
}

Manager *Manager::self()
{
    if (!instance) {
        instance = new Manager;
    }
    return instance;
}

void Manager::release()
{
    delete instance;
    instance = 0;
}

Adapter *Manager::defaultAdapter()
{
    if (!QDBusConnection::systemBus().isConnected() || !d->m_bluezServiceRunning) {
        return 0;
    }

    if (!d->m_defaultAdapter) {
        const QString adapterPath = d->m_bluezManagerInterface->DefaultAdapter().value().path();
        if (!adapterPath.isEmpty()) {
            d->m_defaultAdapter = new Adapter(adapterPath, const_cast<Manager*>(this));
            connect(d->m_defaultAdapter, SIGNAL(poweredChanged(bool)), SLOT(_k_adapterPoweredChanged(bool)));
            d->m_adaptersHash.insert(adapterPath, d->m_defaultAdapter);
        }
    }

    return d->m_defaultAdapter;
}

Adapter *Manager::usableAdapter() const
{
    if (!QDBusConnection::systemBus().isConnected() || !d->m_bluezServiceRunning) {
        return 0;
    }

    if (d->m_usableAdapter && d->m_usableAdapter->isPowered()) {
        return d->m_usableAdapter;
    }
    return d->findUsableAdapter();
}

QList<Adapter*> Manager::adapters() const
{
    if (!QDBusConnection::systemBus().isConnected() || !d->m_bluezServiceRunning) {
        return QList<Adapter*>();
    }

    return d->m_adaptersHash.values();
}

bool Manager::isBluetoothOperational() const
{
    return QDBusConnection::systemBus().isConnected() && d->m_bluezServiceRunning && usableAdapter();
}

}

#include "bluedevilmanager.moc"
