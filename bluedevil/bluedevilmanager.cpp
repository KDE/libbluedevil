/*****************************************************************************
 * This file is part of the KDE project                                      *
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

namespace BlueDevil {

static Manager *instance = 0;

class Manager::Private
{
public:
    Private(Manager *q);
    ~Private();

    void _k_adapterAdded(const QDBusObjectPath &objectPath);
    void _k_adapterRemoved(const QDBusObjectPath &objectPath);
    void _k_defaultAdapterChanged(const QDBusObjectPath &objectPath);
    void _k_propertyChanged(const QString &property, const QDBusVariant &value);

    OrgBluezManagerInterface *m_bluezManagerInterface;
    Adapter                  *m_defaultAdapter;
    QHash<QString, Adapter*>  m_adaptersHash;

    Manager *const m_q;
};

Manager::Private::Private(Manager *q)
    : m_defaultAdapter(0)
    , m_q(q)
{
}

Manager::Private::~Private()
{
    delete m_bluezManagerInterface;
}

void Manager::Private::_k_adapterAdded(const QDBusObjectPath &objectPath)
{
    Adapter *const adapter = new Adapter(objectPath.path(), m_q);
    m_adaptersHash.insert(objectPath.path(), adapter);
    if (!m_defaultAdapter) {
        m_defaultAdapter = adapter;
    }
    emit m_q->adapterAdded(adapter);
}

void Manager::Private::_k_adapterRemoved(const QDBusObjectPath &objectPath)
{
    Adapter *const adapter = m_adaptersHash.take(objectPath.path()); // return and remove it from the hash
    if (m_adaptersHash.isEmpty()) {
        m_defaultAdapter = 0;
    }
    if (adapter) {
        emit m_q->adapterRemoved(adapter);
        delete adapter;
    }
    if (m_adaptersHash.isEmpty()) {
        emit m_q->defaultAdapterChanged(0);
        emit m_q->allAdaptersRemoved();
    }
}

void Manager::Private::_k_defaultAdapterChanged(const QDBusObjectPath &objectPath)
{
    Adapter *adapter = m_adaptersHash[objectPath.path()];
    if (!adapter) {
        adapter = new Adapter(objectPath.path(), m_q);
        m_adaptersHash.insert(objectPath.path(), adapter);
    }
    m_defaultAdapter = adapter;
    emit m_q->defaultAdapterChanged(adapter);
}

void Manager::Private::_k_propertyChanged(const QString &property, const QDBusVariant &value)
{
    Q_UNUSED(property)
    Q_UNUSED(value)
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Manager::Manager(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
    d->m_bluezManagerInterface = new OrgBluezManagerInterface("org.bluez", "/", QDBusConnection::systemBus(), this);

    connect(d->m_bluezManagerInterface, SIGNAL(AdapterAdded(QDBusObjectPath)),
            this, SLOT(_k_adapterAdded(QDBusObjectPath)));
    connect(d->m_bluezManagerInterface, SIGNAL(AdapterRemoved(QDBusObjectPath)),
            this, SLOT(_k_adapterRemoved(QDBusObjectPath)));
    connect(d->m_bluezManagerInterface, SIGNAL(DefaultAdapterChanged(QDBusObjectPath)),
            this, SLOT(_k_defaultAdapterChanged(QDBusObjectPath)));
    connect(d->m_bluezManagerInterface, SIGNAL(PropertyChanged(QString,QDBusVariant)),
            this, SLOT(_k_propertyChanged(QString,QDBusVariant)));

    if (QDBusConnection::systemBus().isConnected()) {
        const QString adapterPath = d->m_bluezManagerInterface->DefaultAdapter().value().path();
        if (!adapterPath.isEmpty()) {
            d->m_defaultAdapter = new Adapter(adapterPath, this);
            d->m_adaptersHash.insert(adapterPath, d->m_defaultAdapter);
        }
        const QVariantMap properties = d->m_bluezManagerInterface->GetProperties().value();
        const QList<QDBusObjectPath> adapters = qdbus_cast<QList<QDBusObjectPath> >(properties["Adapters"].value<QDBusArgument>());
        Q_FOREACH (const QDBusObjectPath &path, adapters) {
            Adapter *const adapter = new Adapter(path.path(), this);
            d->m_adaptersHash.insert(path.path(), adapter);
        }
    }
}

Manager::~Manager()
{
    delete d;
}

Manager* Manager::self()
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
    if (!QDBusConnection::systemBus().isConnected()) {
        return 0;
    }

    if (!d->m_defaultAdapter) {
        const QString adapterPath = d->m_bluezManagerInterface->DefaultAdapter().value().path();
        if (!adapterPath.isEmpty()) {
            d->m_defaultAdapter = new Adapter(adapterPath, this);
            d->m_adaptersHash.insert(adapterPath, d->m_defaultAdapter);
        }
    }

    return d->m_defaultAdapter;
}

QList<Adapter*> Manager::adapters() const
{
    if (!QDBusConnection::systemBus().isConnected()) {
        return QList<Adapter*>();
    }

    return d->m_adaptersHash.values();
}

bool Manager::isBluetoothOperational() const
{
    return QDBusConnection::systemBus().isConnected() && d->m_defaultAdapter;
}

}

#include "bluedevilmanager.moc"
