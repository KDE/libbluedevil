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
#include "bluedevilmanager_p.h"
#include "bluedevildbustypes.h"

#include "bluedevil/dbusobjectmanager.h"
#include "bluedevil/bluezagentmanager1.h"

#include <QtCore/QHash>
#include <QVariantMap>

#include <QtDBus/QDBusConnectionInterface>

namespace BlueDevil {

static Manager *instance = 0;

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
    d->m_bluezAgentManager->RegisterAgent(agentObjectPath, capability);
}

void Manager::requestDefaultAgent(const QString& agentPath)
{
    QDBusObjectPath agentObjectPath = QDBusObjectPath(agentPath);
    d->m_bluezAgentManager->RequestDefaultAgent(agentObjectPath);
}

void Manager::unregisterAgent(const QString &agentPath)
{
    d->m_bluezAgentManager->UnregisterAgent(QDBusObjectPath(agentPath));
}



////////////////////////////////////////////////////////////////////////////////////////////////////

Manager::Manager(QObject *parent)
    : QObject(parent)
    , d(new ManagerPrivate(this))
{
    // Keep an eye open if bluez stops running
    QDBusServiceWatcher *serviceWatcher = new QDBusServiceWatcher("org.bluez", QDBusConnection::systemBus(),
                                                                  QDBusServiceWatcher::WatchForRegistration |
                                                                  QDBusServiceWatcher::WatchForUnregistration, this);
    connect(serviceWatcher, SIGNAL(serviceRegistered(QString)), d, SLOT(_k_bluezServiceRegistered()));
    connect(serviceWatcher, SIGNAL(serviceUnregistered(QString)), d, SLOT(_k_bluezServiceUnregistered()));

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

    return d->m_adapters.values();
}

Device* Manager::deviceForUBI(const QString& UBI) const
{
    Device *device = 0;
    Q_FOREACH(Adapter *adapter, d->m_adapters) {
        device = adapter->deviceForUBI(UBI);
        if (device) {
            return device;
        }
    }

    return 0;
}

QList<Device*> Manager::devices() const
{
    QList<Device*> devices;
    Q_FOREACH(Adapter *adapter, d->m_adapters) {
        devices << adapter->devices();
    }

    return devices;
}

bool Manager::isBluetoothOperational() const
{
    return QDBusConnection::systemBus().isConnected() && d->m_bluezServiceRunning && usableAdapter();
}

}

#include "bluedevilmanager.moc"
