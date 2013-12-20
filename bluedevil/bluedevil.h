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

/*!
 * @mainpage libbluedevil
 *
 * libbluedevil is a Qt-based library, written in C++ that makes it very easy and straight-forward
 * to handle almost all Bluetooth related operations. It consists of several key classes, following:
 *
 *     - Manager
 *         - Entry point to the library functionality. It is a singleton class, and it basically gives
 *           you access to the connected adapters. It will also inform through its signals when new
 *           adapters have been connected or removed.
 *
 *     - Adapter
 *         - It gives you all kind of information (and also a way to modify it) about an adapter. With
 *           this class you can start scanning for remote devices, and being notified through its
 *           signals when new devices have been found. Here you can also retrieve devices given that
 *           you know their hardware address (MAC) or their UBI, both being unique for each device.
 *
 *     - Device
 *         - With this class you are given information about a particular remote device. You can also
 *           set certain properties like whether the device is trusted, blocked, or provide an alias
 *           for it.
 *
 *     - Utils
 *         - Contains general usage routines.
 *
 * All the libbluedevil classes are wrapped into a namespace called BlueDevil.
 *
 * You can have a look at some @ref examples.
 */

/*!
 * @page examples Examples
 *
 * @section manager Manager
 *
 * The Manager task is to serve as entry point to the library, being a singleton class. It will also
 * inform about the service state (operational or not), as well as notify of adapters being
 * connected or disconnected.
 *
 * For brevity, we will directly include the whole namespace:
 *
 * @code
 * using namespace BlueDevil;
 * @endcode
 *
 * In order to include the Manager API, you have to perform:
 *
 * @code
 * #include <bluedevil/bluedevilmanager.h>
 * @endcode
 *
 * So, all the dance usually starts as:
 *
 * @code
 * Manager *const manager = Manager::self();
 *
 * // If Bluetooth is operational, we can directly retrieve the first usable adapter, and start working
 * // with it. Otherwise, we can connect to the usableAdapterChanged signal, so we will be notified
 * // when we have an adapter ready to be used.
 * if (manager->isBluetoothOperational()) {
 *     Adapter *const adapter = manager->usableAdapter();
 *     // Do something interesting with the adapter...
 * } else {
 *     connect(manager, SIGNAL(usableAdapterChanged(Adapter*)), this, SLOT(usableAdapterChanged(Adapter*)));
 * }
 * @endcode
 *
 * It is very common that in some few calls we will be using the Manager, Adapter and Device APIs.
 * In order to decrease the number of includes that you need to do, you have a handy trick:
 *
 * @code
 * #include <bluedevil/bluedevil.h>
 * @endcode
 *
 * @section adapter Adapter
 *
 * An adapter is a device physically connected to the system. Its main job is to let other devices
 * discover it as well as discover remote devices.
 *
 * We can perform a very typical task as discover remote devices.
 *
 * @code
 * Adapter *const adapter = Manager::self()->usableAdapter();
 * connect(adapter, SIGNAL(deviceFound(Device*)), this, SLOT(deviceFound(Device*)));
 * adapter->startDiscovery();
 * QTimer::singleShot(10000, adapter, SLOT(stopDiscovery())));
 * @endcode
 *
 * This snippet will discover devices for 10 seconds. For each device discovered, the slot
 * deviceFound() will be called.
 *
 * The Adapter API also allows you to set up other Adapter settings such as if the adapter is powered
 * or not, its visibility, the visibility timeout...
 *
 * @section device Device
 *
 * This class represents a remote device. This class basically retrieves information, but it is also
 * possible to set certain properties for this device, such as whether this device is trusted or not,
 * or blocked, or the local alias for this device.
 *
 * We can have a look at the slot deviceFound() that we named early before on the Adapter example:
 *
 * @code
 * void MyClass::deviceFound(Device *device)
 * {
 *     qDebug() << "Device found: " << device->name() << " (" << device->address() << ")";
 *     qDebug() << "\tServices: " << device->UUIDs();
 * }
 * @endcode
 *
 * The UUIDs are the services supported by this device, so for each found device, we ask (and print)
 * its name, its hardware address (MAC) and the supported services by this device.
 */

#ifndef BLUEDEVIL_H
#define BLUEDEVIL_H

#include <bluedevil/bluedevildevice.h>
#include <bluedevil/bluedeviladapter.h>
#include <bluedevil/bluedevilmanager.h>
#include <bluedevil/bluedevilutils.h>

#endif // BLUEDEVIL_H
