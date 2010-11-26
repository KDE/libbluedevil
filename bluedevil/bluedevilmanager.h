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

#ifndef BLUEDEVILMANAGER_H
#define BLUEDEVILMANAGER_H

#include <bluedevil/bluedevil_export.h>

#include <QtCore/QObject>
#include <QtDBus/QDBusObjectPath>

namespace BlueDevil {

class Adapter;

/**
 * @class Manager bluedevilmanager.h bluedevil/bluedevilmanager.h
 *
 * Manager class. The entry point to BlueDevil exposed services.
 *
 * The typical way to proceed is to work with the default adapter, but you can also list all
 * bluetooth adapters and work with the one you want.
 *
 * The interface is a singleton with release-when-you-want capability.
 *
 * All adapters and devices are created by BlueDevil, and the ownership is always of BlueDevil.
 *
 * @author Rafael Fernández López <ereslibre@kde.org>
 */
class BLUEDEVIL_EXPORT Manager
    : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Manager* self READ self)
    Q_PROPERTY(Adapter* defaultAdapter READ defaultAdapter)
    Q_PROPERTY(QList<Adapter*> adapters READ adapters)
    Q_PROPERTY(bool isBluetoothOperational READ isBluetoothOperational)

public:
    virtual ~Manager();

    /**
     * @return The Manager instance.
     */
    static Manager *self();

    /**
     * When you consider you have finished working with BlueDevil you can immediatly release the
     * memory by calling this method. It will automatically delete all Adapters and Devices that
     * were still on memory.
     */
    static void release();

    /**
     * @return The default adapter. NULL if there is no default adapter or the system is not ready
     *         (the bus is not accessible or there is no Bluetooth system running).
     */
    Adapter *defaultAdapter();

    /**
     * @return A list with all the connected adapters.
     */
    QList<Adapter*> adapters() const;

    /**
     * @return Whether the bluetooth system is ready to be used, and there is a default adapter
     *         connected and turned on at the system.
     *
     * @note After this check, if succeeded, you can freely access to all libbluedevil functionality
     *       by retrieving the default adapter through a call to defaultAdapter().
     *
     * @note If this method returns false, you can connect to the defaultAdapterChanged signal, so
     *       you can be notified when bluetooth is operational.
     */
    bool isBluetoothOperational() const;

Q_SIGNALS:
    /**
     * This signal will be emitted when an adapter has been connected.
     */
    void adapterAdded(Adapter *adapter);

    /**
     * This signal will be emitted when an adapter has been disconnected.
     */
    void adapterRemoved(Adapter *adapter);

    /**
     * This signal will be emitted when the default adapter has changed. It also will be emitted
     * when all adapters have been removed, placing 0 at @p adapter.
     */
    void defaultAdapterChanged(Adapter *adapter);

    /**
     * This signal will be emitted when all adapters have been disconnected.
     */
    void allAdaptersRemoved();

private:
    /**
     * @internal
     */
    Manager(QObject *parent = 0);

    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_adapterAdded(QDBusObjectPath))
    Q_PRIVATE_SLOT(d, void _k_adapterRemoved(QDBusObjectPath))
    Q_PRIVATE_SLOT(d, void _k_defaultAdapterChanged(QDBusObjectPath))
    Q_PRIVATE_SLOT(d, void _k_propertyChanged(QString,QDBusVariant))
};

}

#endif // BLUEDEVILMANAGER_H
