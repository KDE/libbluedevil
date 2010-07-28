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

#ifndef BLUEDEVILUTILS_H
#define BLUEDEVILUTILS_H

#include <bluedevil/bluedevil_export.h>

#include <QtCore/QtGlobal>

namespace BlueDevil {

    quint32 BLUEDEVIL_EXPORT classToType(quint32 classNum);
    quint32 BLUEDEVIL_EXPORT stringToType(const QString& stringType);

    enum BluetoothType {
        BLUETOOTH_TYPE_ANY         = 1 << 0,
        BLUETOOTH_TYPE_PHONE       = 1 << 1,
        BLUETOOTH_TYPE_MODEM       = 1 << 2,
        BLUETOOTH_TYPE_COMPUTER    = 1 << 3,
        BLUETOOTH_TYPE_NETWORK     = 1 << 4,
        BLUETOOTH_TYPE_HEADSET     = 1 << 5,
        BLUETOOTH_TYPE_HEADPHONES  = 1 << 6,
        BLUETOOTH_TYPE_OTHER_AUDIO = 1 << 7,
        BLUETOOTH_TYPE_KEYBOARD    = 1 << 8,
        BLUETOOTH_TYPE_MOUSE       = 1 << 9,
        BLUETOOTH_TYPE_CAMERA      = 1 << 10,
        BLUETOOTH_TYPE_PRINTER     = 1 << 11,
        BLUETOOTH_TYPE_JOYPAD      = 1 << 12,
        BLUETOOTH_TYPE_TABLET      = 1 << 13
    };

}

#endif // BLUEDEVILUTILS_H
