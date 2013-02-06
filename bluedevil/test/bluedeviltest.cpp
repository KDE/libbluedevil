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

#include "bluedeviltest.h"

#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QCoreApplication>

#include <bluedevil/bluedeviladapter.h>
#include <bluedevil/bluedevilmanager.h>
#include <bluedevil/bluedevildevice.h>

using namespace BlueDevil;

DeviceReceiver::DeviceReceiver(QObject *parent)
    : QObject(parent)
{
}

DeviceReceiver::~DeviceReceiver()
{
}

void DeviceReceiver::scanDevices()
{
    qDebug() << "*** Will scan devices until stopped...";
    qDebug();

    Adapter *usableAdapter = Manager::self()->usableAdapter();

    QObject::connect(usableAdapter, SIGNAL(deviceFound(Device*)), this, SLOT(deviceFound(Device*)));

    usableAdapter->startStableDiscovery();
}

void DeviceReceiver::deviceFound(Device *device)
{
    qDebug() << "*** Remote device found:" << device->name() << "(" << device->address() << ")";
    qDebug();
    connect(device, SIGNAL(propertyChanged(QString,QVariant)), this, SLOT(devicePropertyChanged(QString,QVariant)));
}

void DeviceReceiver::devicePropertyChanged(const QString &property, const QVariant &value)
{
    Device *device = static_cast<Device*>(sender());

    qDebug() << "*** Device with address" << device->address() << "changed some property (" << property << "):";
    qDebug() << "\tAddress:\t" << device->address();
    qDebug() << "\tAlias:\t\t" << device->alias();
    qDebug() << "\tClass:\t\t" << device->deviceClass();
    qDebug() << "\tIcon:\t\t" << device->icon();
    qDebug() << "\tLegacy Pairing:\t" << (device->hasLegacyPairing() ? "yes" : "no");
    qDebug() << "\tName:\t\t" << device->name();
    qDebug() << "\tPaired:\t\t" << (device->isPaired() ? "yes" : "no");
    qDebug() << "\tTrusted:\t" << (device->isTrusted() ? "yes" : "no");
    qDebug() << "\tServices:\n" << device->UUIDs();
    qDebug();
}

void DeviceReceiver::adapterAdded(Adapter *adapter)
{
    qDebug() << "*** An adapter has been connected.";
    qDebug();
    scanDevices();
}

static void stopDiscovering()
{
    foreach (Adapter *const adapter, Manager::self()->adapters()) {
        adapter->stopDiscovery();
    }
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    qAddPostRoutine(stopDiscovering);

    DeviceReceiver *deviceReceiver = new DeviceReceiver;

    Adapter *usableAdapter = Manager::self()->usableAdapter();
    if (usableAdapter) {
        deviceReceiver->scanDevices();
    } else {
        qDebug() << "!!! No bluetooth adapters were found. Waiting for bluetooth adapters. Ctrl + C to cancel...";
        qDebug();

        QObject::connect(Manager::self(), SIGNAL(adapterAdded(Adapter*)), deviceReceiver,
                         SLOT(adapterAdded(Adapter*)));
    }

    return app.exec();
}

#include "bluedeviltest.moc"
