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

/*!
 * @mainpage libbluedevil
 *
 * libbluedevil is a Qt-based library, written in C++ that makes it very easy and straight-forward
 * to handle almost all Bluetooth related operations. It consists of several key classes, following:
 *
 *     - Manager
 *         - Entry point to the library functionality. It is a singleton class, and it basically gives
 *           you access to the connected adapters. It will also inform through its signals when new
 *           adapters have been connected, removed, or when the default adapter has changed.
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
 * You can have a look at @ref blueDevilExamples in order to see some examples.
 */

/*!
 * @page blueDevilExamples BlueDevil examples
 *
 * WIP
 */

#ifndef BLUEDEVIL_H
#define BLUEDEVIL_H

#include <bluedevil/bluedevildevice.h>
#include <bluedevil/bluedeviladapter.h>
#include <bluedevil/bluedevilmanager.h>
#include <bluedevil/bluedevilutils.h>

#endif // BLUEDEVIL_H
