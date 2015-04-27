/*************************************************************************************
 *  Copyright (C) 2013 by Alejandro Fiestas Fiestas <afiestas@kde.org>               *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#include "bluedevilmanager.h"
#include "bluedevilmanager_p.h"
#include "bluedeviladapter.h"

namespace BlueDevil {

ManagerPrivate::ManagerPrivate(Manager *q)
    : QObject(q)
    , m_dbusObjectManager(0)
    , m_bluezAgentManager(0)
    , m_usableAdapter(0)
    , m_q(q)
{
    qDBusRegisterMetaType<DBusManagerStruct>();
    qDBusRegisterMetaType<QVariantMapMap>();

    m_bluezServiceRunning = false;
    if (QDBusConnection::systemBus().isConnected()) {
        QDBusReply<bool> reply = QDBusConnection::systemBus().interface()->isServiceRegistered("org.bluez");

        if (reply.isValid()) {
            m_bluezServiceRunning = reply.value();
        }
    }
}

ManagerPrivate::~ManagerPrivate()
{
    delete m_dbusObjectManager;
    delete m_bluezAgentManager;
}

void ManagerPrivate::initialize()
{
    if (QDBusConnection::systemBus().isConnected() && m_bluezServiceRunning) {
        m_dbusObjectManager = new org::freedesktop::DBus::ObjectManager("org.bluez", "/", QDBusConnection::systemBus(), m_q);

        connect(m_dbusObjectManager, SIGNAL(InterfacesAdded(QDBusObjectPath,QVariantMapMap)),
                SLOT(_k_interfacesAdded(QDBusObjectPath,QVariantMapMap)));
        connect(m_dbusObjectManager, SIGNAL(InterfacesRemoved(QDBusObjectPath,QStringList)),
                SLOT(_k_interfacesRemoved(QDBusObjectPath,QStringList)));

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
                    connect(adapter, SIGNAL(poweredChanged(bool)), SLOT(_k_bluezAdapterPoweredChanged(bool)));
                    m_adapters.insert(managedObjectIt.key().path(), adapter);
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

                Adapter * const adapter = m_adapters.value(adapterPath);
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

void ManagerPrivate::clean()
{
    qDebug() << "Private::clean";
    delete m_dbusObjectManager;
    delete m_bluezAgentManager;
    QMapIterator<QString, Adapter*> i(m_adapters);
    while (i.hasNext()) {
        i.next();
        Adapter *adapter = m_adapters.take(i.key());
        emit m_q->adapterRemoved(adapter);
        delete adapter;
    }

    m_usableAdapter = 0;

    emit m_q->usableAdapterChanged(0);
}

Adapter *ManagerPrivate::findUsableAdapter()
{
    Q_FOREACH (Adapter *const adapter, m_q->adapters()) {
        if (adapter->isPowered()) {
            return adapter;
        }
    }
    return 0;
}

void ManagerPrivate::_k_interfacesAdded(const QDBusObjectPath &objectPath, const QVariantMapMap &interfaces)
{
  QVariantMapMap::const_iterator i;
  for(i = interfaces.constBegin(); i != interfaces.constEnd(); ++i) {
    if(i.key() == "org.bluez.Adapter1") {
      Adapter * const adapter = new Adapter(objectPath.path(), m_q);
      connect(adapter, SIGNAL(poweredChanged(bool)), SLOT(_k_bluezAdapterPoweredChanged(bool)));
      m_adapters.insert(objectPath.path(), adapter);
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
      Adapter * const adapter = m_adapters.value(adapterPath);
      if (adapter) {
          adapter->addDevice(objectPath.path());
          m_devAdapter.insert(objectPath.path(),adapter);
      }
    }
  }
}

void ManagerPrivate::_k_interfacesRemoved(const QDBusObjectPath &objectPath, const QStringList &interfaces)
{
    QString object = objectPath.path();
    Q_FOREACH(QString interface, interfaces) {
        if(interface == "org.bluez.Adapter1") {
            Adapter *const adapter = m_adapters.take(object); // return and remove it from the map
            if (m_adapters.isEmpty()) {
                m_usableAdapter = 0;
            }
            if (adapter) {
                emit m_q->adapterRemoved(adapter);
            }
            if (m_adapters.isEmpty()) {
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
            if (adapter) {
                adapter->removeDevice(object);

                if (adapter->devices().isEmpty()) {
                    adapter->deleteLater();
                }
            }
        }
    }
}

void ManagerPrivate::_k_bluezServiceRegistered()
{
    m_bluezServiceRunning = true;
    initialize();
}

void ManagerPrivate::_k_bluezServiceUnregistered()
{
    m_bluezServiceRunning = false;
    clean();
}

void ManagerPrivate::_k_bluezAdapterPoweredChanged(bool powered)
{
    //If the power change has had no effect on usableAdpater, do nothing
    Adapter *adapter = findUsableAdapter();
    if (m_usableAdapter == adapter) {
        return;
    }

    m_usableAdapter = adapter;
    emit m_q->usableAdapterChanged(adapter);
}

}

#include "bluedevilmanager_p.moc"
