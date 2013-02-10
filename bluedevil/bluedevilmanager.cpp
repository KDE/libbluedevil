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
#include "bluedevildevice.h"

#include "bluedevil/dbusobjectmanager.h"
#include "bluedevil/bluezagentmanager1.h"

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

    void _k_bluezServiceRegistered();
    void _k_bluezServiceUnregistered();

    void _k_interfacesAdded(const QDBusObjectPath &objectPath, const QVariantMapMap &interfaces);
    void _k_interfacesRemoved(const QDBusObjectPath &objectPath, const QStringList &interfaces);

    org::freedesktop::DBus::ObjectManager *m_dbusObjectManager;
    org::bluez::AgentManager1             *m_bluezAgentManager;
    Adapter                               *m_usableAdapter;
    QHash<QString, Adapter*>               m_adaptersHash;
    QHash<QString, Adapter*>               m_devAdapter;
    bool                                   m_bluezServiceRunning;

    Manager *const m_q;
};

Manager::Private::Private(Manager *q)
    : m_dbusObjectManager(0)
    , m_bluezAgentManager(0)
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
    delete m_dbusObjectManager;
    delete m_bluezAgentManager;
}

void Manager::Private::initialize()
{
    if (QDBusConnection::systemBus().isConnected() && m_bluezServiceRunning) {
        qDBusRegisterMetaType<DBusManagerStruct>();
        qDBusRegisterMetaType<QVariantMapMap>();
        m_dbusObjectManager = new org::freedesktop::DBus::ObjectManager("org.bluez", "/", QDBusConnection::systemBus(), m_q);

        connect(m_dbusObjectManager, SIGNAL(InterfacesAdded(QDBusObjectPath,QVariantMapMap)),
                m_q, SLOT(_k_interfacesAdded(QDBusObjectPath,QVariantMapMap)));
        connect(m_dbusObjectManager, SIGNAL(InterfacesRemoved(QDBusObjectPath,QStringList)),
                m_q, SLOT(_k_interfacesRemoved(QDBusObjectPath,QStringList)));

        QDBusPendingReply<DBusManagerStruct> reply = m_dbusObjectManager->GetManagedObjects();
        reply.waitForFinished();
        if (!reply.isError()) {
            QHash<QString,QString> devices;
            DBusManagerStruct managedObjects = reply.value();
            DBusManagerStruct::const_iterator managedObjectIt;
            for(managedObjectIt = managedObjects.constBegin(); managedObjectIt != managedObjects.constEnd(); ++managedObjectIt) {
                QString path = managedObjectIt.key().path();
                QVariantMapMap interfaces = managedObjectIt.value();
                if(interfaces.contains("org.bluez.Adapter1")) {
                    Adapter *const adapter = new Adapter(path, m_q);
                    m_adaptersHash.insert(managedObjectIt.key().path(), adapter);
                } else if(interfaces.contains("org.bluez.Device1")) {
                    QString adapterPath = managedObjectIt.value().value("org.bluez.Device1").value("Adapter").value<QDBusObjectPath>().path();
                    devices.insert(path,adapterPath);
                } else if(interfaces.contains("org.bluez.AgentManager1")) {
                    m_bluezAgentManager = new org::bluez::AgentManager1("org.bluez",path,QDBusConnection::systemBus(), m_q);
                }
            }

            QHash<QString,QString>::const_iterator deviceIt;
            for(deviceIt = devices.constBegin(); deviceIt != devices.constEnd(); ++deviceIt) {
                QString devicePath = deviceIt.key();
                QString adapterPath = deviceIt.value();

                Adapter * const adapter = m_adaptersHash.value(adapterPath);
                adapter->addDevice(devicePath);
                m_devAdapter.insert(devicePath,adapter);
            }
        } else {
            //TODO: error handling
        }
        m_usableAdapter = findUsableAdapter();
        emit m_q->usableAdapterChanged(m_usableAdapter);
    }
}

void Manager::Private::clean()
{
    qDebug() << "Private::clean";
    delete m_dbusObjectManager;
    delete m_bluezAgentManager;
    QHashIterator<QString, Adapter*> i(m_adaptersHash);
    while (i.hasNext()) {
        i.next();
        Adapter *adapter = m_adaptersHash.take(i.key());
        emit m_q->adapterRemoved(adapter);
        delete adapter;
    }

    m_usableAdapter = 0;

    emit m_q->usableAdapterChanged(0);
}
Adapter *Manager::Private::findUsableAdapter()
{
    Q_FOREACH (Adapter *const adapter, m_q->adapters()) {
        if (adapter->isPowered()) {
            m_usableAdapter = adapter;
            return adapter;
        }
    }
    return 0;
}

void Manager::registerAgent(const QString &agentPath, RegisterCapability registerCapability)
{
    QString capability;

    switch (registerCapability) {
        case DisplayOnly:
            capability = "DisplayOnly";
            break;
        case DisplayYesNo:
            capability = "DisplayYesNo";
            break;
        case KeyboardOnly:
            capability = "KeyboardOnly";
            break;
        case NoInputNoOutput:
            capability = "NoInputNoOutput";
            break;
        default:
            return;
    }

    QDBusObjectPath agentObjectPath = QDBusObjectPath(agentPath);
    QDBusPendingReply<void> reply = d->m_bluezAgentManager->RegisterAgent(agentObjectPath, capability);
    reply.waitForFinished(); // TODO: is it necessary to wait?
    d->m_bluezAgentManager->RequestDefaultAgent(agentObjectPath);
}

void Manager::unregisterAgent(const QString &agentPath)
{
    d->m_bluezAgentManager->UnregisterAgent(QDBusObjectPath(agentPath));
}

void Manager::Private::_k_interfacesAdded(const QDBusObjectPath &objectPath, const QVariantMapMap &interfaces)
{
  QVariantMapMap::const_iterator i;
  for(i = interfaces.constBegin(); i != interfaces.constEnd(); ++i) {
    if(i.key() == "org.bluez.Adapter1") {
      Adapter * const adapter = new Adapter(objectPath.path(), m_q);
      m_adaptersHash.insert(objectPath.path(), adapter);
      if (!m_usableAdapter || !m_usableAdapter->isPowered()) {
          Adapter *const oldUsableAdapter = m_usableAdapter;
          m_usableAdapter = findUsableAdapter();
          if (m_usableAdapter != oldUsableAdapter) {
              emit m_q->usableAdapterChanged(m_usableAdapter);
          }
      }
      emit m_q->adapterAdded(adapter);
    } else if(i.key() == "org.bluez.Device1") {
      QString adapterPath = i.value().value("Adapter").value<QDBusObjectPath>().path();
      Adapter * const adapter = m_adaptersHash.value(adapterPath);
      adapter->addDevice(objectPath.path());
      m_devAdapter.insert(objectPath.path(),adapter);
    }
  }
}

void Manager::Private::_k_interfacesRemoved(const QDBusObjectPath &objectPath, const QStringList &interfaces)
{
    QString object = objectPath.path();
    Q_FOREACH(QString interface, interfaces) {
        if(interface == "org.bluez.Adapter1") {
            Adapter *const adapter = m_adaptersHash.take(object); // return and remove it from the hash
            if (m_adaptersHash.isEmpty()) {
                m_usableAdapter = 0;
            }
            if (adapter) {
                emit m_q->adapterRemoved(adapter);
                delete adapter;
            }
            if (m_adaptersHash.isEmpty()) {
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
        } else if(interface == "org.bluez.Device1") {
            Adapter * const adapter = m_devAdapter.take(object);
            adapter->removeDevice(object);
        }
    }
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
