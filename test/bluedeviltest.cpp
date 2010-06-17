/*  This file is part of the KDE project
    Copyright (C) 2010 Rafael Fernández López <ereslibre@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "bluedeviltest.h"

#include <QtCore/QDebug>
#include <QtGui/QApplication>

#include <bluedeviladapter.h>
#include <bluedevilmanager.h>
#include <bluedevildevice.h>

using namespace BlueDevil;

DeviceReceiver::DeviceReceiver(QObject *parent)
    : QObject(parent)
{
}

DeviceReceiver::~DeviceReceiver()
{
}

void DeviceReceiver::deviceFound(Device *device)
{
    qDebug() << "*** Remote device found:";
    qDebug() << "\tAddress:\t" << device->address();
    qDebug() << "\tAlias:\t\t" << device->alias();
    qDebug() << "\tClass:\t\t" << device->deviceClass();
    qDebug() << "\tIcon:\t\t" << device->icon();
    qDebug() << "\tLegacy Pairing:\t" << (device->hasLegacyPairing() ? "yes" : "no");
    qDebug() << "\tName:\t\t" << device->name();
    qDebug() << "\tPaired:\t\t" << (device->isPaired() ? "yes" : "no");
    qDebug() << "\tRSSI:\t\t" << device->RSSI();
    qDebug() << "\tServices:\n" << device->discoverServices();
    qDebug();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Adapter *defaultAdapter = Manager::self()->defaultAdapter();
    if (defaultAdapter) {
        DeviceReceiver *deviceReceiver = new DeviceReceiver;

        QObject::connect(defaultAdapter, SIGNAL(deviceFound(Device*)), deviceReceiver,
                         SLOT(deviceFound(Device*)));

        defaultAdapter->startDiscovery();

        return app.exec();
    }

    qDebug() << "!!! No bluetooth adapters were found";
    return 0;
}

#include "bluedeviltest.moc"
