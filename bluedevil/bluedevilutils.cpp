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

#include "bluedevilutils.h"

#include <QtCore/QString>

namespace BlueDevil {

quint32 stringToType(const QString &stringType)
{
    if (stringType == "any") {
        return BLUETOOTH_TYPE_ANY;
    } else if (stringType == "mouse") {
        return BLUETOOTH_TYPE_MOUSE;
    } else if (stringType == "keyboard") {
        return BLUETOOTH_TYPE_KEYBOARD;
    } else if (stringType == "headset") {
        return BLUETOOTH_TYPE_HEADSET;
    } else if (stringType == "headphones") {
        return BLUETOOTH_TYPE_HEADPHONES;
    } else if (stringType == "audio") {
        return BLUETOOTH_TYPE_OTHER_AUDIO;
    } else if (stringType == "printer") {
        return BLUETOOTH_TYPE_PRINTER;
    } else if (stringType == "network") {
        return BLUETOOTH_TYPE_NETWORK;
    }
    return BLUETOOTH_TYPE_ANY;
}

quint32 classToType(quint32 classNum)
{
    switch ((classNum & 0x1f00) >> 8) {
    case 0x01:
        return BLUETOOTH_TYPE_COMPUTER;
    case 0x02:
        switch ((classNum & 0xfc) >> 2) {
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x05:
            return BLUETOOTH_TYPE_PHONE;
        case 0x04:
            return BLUETOOTH_TYPE_MODEM;
        }
        break;
    case 0x03:
        return BLUETOOTH_TYPE_NETWORK;
    case 0x04:
        switch ((classNum & 0xfc) >> 2) {
        case 0x01:
        case 0x02:
            return BLUETOOTH_TYPE_HEADSET;
        case 0x06:
            return BLUETOOTH_TYPE_HEADPHONES;
        default:
            return BLUETOOTH_TYPE_OTHER_AUDIO;
        }
        break;
    case 0x05:
        switch ((classNum & 0xc0) >> 6) {
        case 0x00:
            switch ((classNum & 0x1e) >> 2) {
            case 0x01:
            case 0x02:
                return BLUETOOTH_TYPE_JOYPAD;
            }
            break;
        case 0x01:
            return BLUETOOTH_TYPE_KEYBOARD;
        case 0x02:
            switch ((classNum & 0x1e) >> 2) {
            case 0x05:
                return BLUETOOTH_TYPE_TABLET;
            default:
                return BLUETOOTH_TYPE_MOUSE;
            }
        }
        break;
    case 0x06:
        if (classNum & 0x80)
            return BLUETOOTH_TYPE_PRINTER;
        if (classNum & 0x20)
            return BLUETOOTH_TYPE_CAMERA;
        break;
    }

    return 0;
}

}
