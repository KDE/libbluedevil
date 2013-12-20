/*****************************************************************************
 * This file is part of the BlueDevil project                                *
 *                                                                           *
 * Copyright (C) 2011 Rafael Fernández López <ereslibre@kde.org>             *
 * Copyright (C) 2011 UFO Coders <info@ufocoders.com>                        *
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

#include "adaptertest.h"

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

#include <bluedevil/bluedeviladapter.h>
#include <bluedevil/bluedevilmanager.h>
#include <bluedevil/bluedevildevice.h>

using namespace BlueDevil;

AdapterTest::AdapterTest(QObject *parent)
    : QThread(parent)
{
}

AdapterTest::~AdapterTest()
{
}

void AdapterTest::adapterAdded(Adapter *adapter)
{
    qDebug() << "Adapter added: " << adapter;
    Manager *const manager = Manager::self();
    qDebug() << "\tBluetooth Operational: " << manager->isBluetoothOperational();
    qDebug() << "\tUsable Adapter: " << manager->usableAdapter();
}

void AdapterTest::adapterRemoved(Adapter *adapter)
{
    qDebug() << "Adapter removed: " << adapter;
    Manager *const manager = Manager::self();
    qDebug() << "\tBluetooth Operational: " << manager->isBluetoothOperational();
    qDebug() << "\tUsable Adapter: " << manager->usableAdapter();
}

void AdapterTest::usableAdapterChanged(Adapter *adapter)
{
    qDebug() << "Usable adapter changed: " << adapter;
    Manager *const manager = Manager::self();
    qDebug() << "\tBluetooth Operational: " << manager->isBluetoothOperational();
    qDebug() << "\tUsable Adapter: " << manager->usableAdapter();
}

void AdapterTest::allAdaptersRemoved()
{
    qDebug() << "All adapters have been removed";
    Manager *const manager = Manager::self();
    qDebug() << "\tBluetooth Operational: " << manager->isBluetoothOperational();
    qDebug() << "\tUsable Adapter: " << manager->usableAdapter();
}

void AdapterTest::run()
{
    Manager *const manager = Manager::self();
    while (true) {
        qDebug() << "Bluetooth Operational: " << manager->isBluetoothOperational();
        qDebug() << "Usable Adapter: " << manager->usableAdapter();
        sleep(5);
    }
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    qDebug() << "Looping until stopped";

    AdapterTest *const adapterTest = new AdapterTest;

    Manager *const manager = Manager::self();
    QObject::connect(manager, SIGNAL(adapterAdded(Adapter*)), adapterTest, SLOT(adapterAdded(Adapter*)));
    QObject::connect(manager, SIGNAL(adapterRemoved(Adapter*)), adapterTest, SLOT(adapterRemoved(Adapter*)));
    QObject::connect(manager, SIGNAL(usableAdapterChanged(Adapter*)), adapterTest, SLOT(usableAdapterChanged(Adapter*)));
    QObject::connect(manager, SIGNAL(allAdaptersRemoved()), adapterTest, SLOT(allAdaptersRemoved()));

    adapterTest->start();

    return app.exec();
}

#include "adaptertest.moc"
