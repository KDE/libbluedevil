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

class Device;
class Adapter;
class ManagerPrivate;

/**
 * @class Manager bluedevilmanager.h bluedevil/bluedevilmanager.h
 *
 * Manager class. The entry point to BlueDevil exposed services.
 *
 * The typical way to proceed is to work with the first adapter, but you can also list all
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
    Q_PROPERTY(QList<Adapter*> adapters READ adapters)
    Q_PROPERTY(bool isBluetoothOperational READ isBluetoothOperational)

    friend class ManagerPrivate;
public:
    enum RegisterCapability {
        DisplayOnly = 0,
        DisplayYesNo = 1,
        KeyboardOnly = 2,
        NoInputNoOutput = 3
    };

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
     * @return The first adapter that is ready to be used (is powered). If there are no usable
     *         adapters, NULL will be returned.
     */
    Adapter *usableAdapter() const;

    /**
     * @return A list with all the connected adapters.
     */
    QList<Adapter*> adapters() const;

    /**
     * Returns a device for a given UBI independently of the adapter they are in
     *
     * All devices belong to an adapter so in order to find a device when we have more than
     * one adapter is iterating on all adapters and call deviceForUBI. This method basically
     * does that so application developers doesn't have to do it.
     *
     * @param Device UBI to find
     * @return A device for the given UBI or null if none is found
     */
    Device *deviceForUBI(const QString &UBI) const;

    /**
     * Return a list of all known devices by all connected adaptors
     * @return a list of all known devices
     */
    QList<Device*> devices() const;
    /**
     * @return Whether the bluetooth system is ready to be used, and there is a usable adapter
     *         connected and turned on at the system.
     *
     * @note After this check, if succeeded, you can freely access to all libbluedevil functionality
     *       by retrieving the an adapter through a call to usableAdapter().
     *
     * @note If this method returns false, you can connect to the usableAdapterChanged signal, so
     *       you can be notified when bluetooth is operational.
     */
    bool isBluetoothOperational() const;

public Q_SLOTS:
    /**
     * Registers agent.
     */
    void registerAgent(const QString &agentPath, RegisterCapability registerCapability);

    /**
     * Unregisters agent.
     */
    void unregisterAgent(const QString &agentPath);

    /**
     * Request to set Agent with agentPath as default agent.
     */
    void requestDefaultAgent(const QString &agentPath);

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
     * This signal will be emitted when the current usable adapter has changed. This basically
     * means two cases:
     *
     *     - There were no usable adapters (powered off, or not present), and a new one has been
     *       connected and is powered on.
     *     - The adapter that was considered usable has been removed or powered off.
     *
     * If any of those cases happen, and it was possible to find a usable adapter, this signal
     * will report the new adapter. If no usable adapter could be found, 0 will be placed at @p
     * adapter.
     *
     */
    void usableAdapterChanged(Adapter *adapter);

    /**
     * This signal will be emitted when all adapters have been disconnected.
     */
    void allAdaptersRemoved();

private:
    /**
     * @internal
     */
    Manager(QObject *parent = 0);

    ManagerPrivate *const d;
};

}

#endif // BLUEDEVILMANAGER_H
