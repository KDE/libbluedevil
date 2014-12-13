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

#ifndef BLUEDEVILADAPTER_H
#define BLUEDEVILADAPTER_H

#include <bluedevil/bluedevil_export.h>

#include <QtCore/QObject>
#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusPendingCallWatcher>

namespace BlueDevil {

class Device;
class Manager;

/**
 * @class Adapter bluedeviladapter.h bluedevil/bluedeviladapter.h
 *
 * This class represents an adapter.
 *
 * The task of an adapter is basically to discover remote devices.
 *
 * @author Rafael Fernández López <ereslibre@kde.org>
 */
class BLUEDEVIL_EXPORT Adapter
    : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString address READ address)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString systemName READ systemName)
    Q_PROPERTY(quint32 adapterClass READ adapterClass)
    Q_PROPERTY(bool powered READ isPowered WRITE setPowered)
    Q_PROPERTY(bool discoverable READ isDiscoverable WRITE setDiscoverable)
    Q_PROPERTY(bool pairable READ isPairable WRITE setPairable)
    Q_PROPERTY(quint32 paireableTimeout READ paireableTimeout WRITE setPaireableTimeout)
    Q_PROPERTY(quint32 discoverableTimeout READ discoverableTimeout WRITE setDiscoverableTimeout)
    Q_PROPERTY(bool isDiscovering READ isDiscovering)
    Q_PROPERTY(QList<Device*> unpairedDevices READ unpairedDevices)
    Q_PROPERTY(QList<Device*> devices READ devices)
    Q_PROPERTY(QStringList UUIDs READ UUIDs)

    friend class Manager;
    friend class ManagerPrivate;
    friend class Device;

public:
    virtual ~Adapter();

    /**
     * @return The address of the adapter.
     */
    QString address() const;

    /**
     * Returns the friendly name of the adapter.
     *
     * Friendly name is alias of the adapter. If no alias is set, it returns
     * the system name.
     *
     * @return The friendly name of the adapter.
     */
    QString name() const;

    /**
     * @return The system name (hostname) of the adapter.
     */
    QString systemName() const;

    /**
     * @return The class of the adapter.
     */
    quint32 adapterClass() const;

    /**
     * @return Whether this adapter is consuming energy or not.
     */
    bool isPowered() const;

    /**
     * @return Whether this adapter is discoverable or not.
     */
    bool isDiscoverable() const;

    /**
     * @return Whether this adapter is pairable or not.
     */
    bool isPairable() const;

    /**
     * @return The timeout for the pairing process for this adapter in seconds.
     */
    quint32 paireableTimeout() const;

    /**
     * @return The timeout for this adapter to be discovered in seconds.
     */
    quint32 discoverableTimeout() const;

    /**
     * @return Whether this adapter is discovering at the moment or not.
     */
    bool isDiscovering() const;

    /**
     * @return A list with all unpaired devices found on the discovery phase.
     */
    QList<Device*> unpairedDevices() const;

    /**
     * @return A device defined by its hardware address.
     */
    Device *deviceForAddress(const QString &address);

    /**
     * @return A device defined by its UBI.
     */
    Device *deviceForUBI(const QString &UBI);

    /**
     * @return All known devices by this adapter. They haven't been necessarily discovered in this
     *         session.
     */
    QList<Device*> devices();

    /**
     * @return Services provided by this adapter.
     */
    QStringList UUIDs();

public Q_SLOTS:
    /**
     *  Set the name (alias) of the adapter
     */
    void setName(const QString &name);

    /**
     * Sets whether this adapter is consuming energy or not.
     */
    void setPowered(bool powered);

    /**
     * Sets whether this adapter is discoverable or not.
     */
    void setDiscoverable(bool discoverable);

    /**
     * Sets whether this adapter is pairable or not.
     */
    void setPairable(bool pairable);

    /**
     * Sets the timeout for the pairing process for this adapter in seconds.
     *
     * @note A @p paireableTimeout of 0 means that this adapter can be paired forever.
     */
    void setPaireableTimeout(quint32 paireableTimeout);

    /**
     * Sets the timeout for this adapter to be discovered in seconds.
     *
     * @note A @p discoverableTimeout of 0 means that this adapter can be discovered forever.
     */
    void setDiscoverableTimeout(quint32 discoverableTimeout);

    /**
     * Removes device.
     */
    void removeDevice(Device *device);

    /**
     * Starts device discovery. deviceFound signal will be emitted for each device found.
     *
     * @note It is possible that when discovering devices appear back and forth. This call will also
     *       allow the adapter to signal when devices have disappeared when discovering, what could
     *       not be exactly what you want. If the desired behavior is to only be notified of new
     *       discovered devices, please see startStableDiscovery.
     */
    void startDiscovery() const;

    /**
     * Starts device discovery. deviceFound signal will be emitted for each device found.
     *
     * @note This discovery type will never trigger deviceDisappeared signal while discovering, so
     *       you will only get deviceFound signals emitted. This also ensures that you will never get
     *       deviceFound repeated emissions for the same devices, in this sense is more stable.
     */
    void startStableDiscovery() const;

    /**
     * Stops device discovery.
     */
    void stopDiscovery() const;

Q_SIGNALS:
    void deviceRemoved(Device *device);
    void deviceFound(Device *device);
    void unpairedDeviceFound(Device *device);
    void nameChanged(const QString &name);
    void poweredChanged(bool powered);
    void discoverableChanged(bool discoverable);
    void pairableChanged(bool pairable);
    void pairableTimeoutChanged(quint32 pairableTimeout);
    void discoverableTimeoutChanged(quint32 discoverableTimeout);
    void deviceChanged(Device* device);
    void discoveringChanged(bool discovering);
    void propertyChanged(const QString &property, const QVariant &value);

private:
    /**
     * @internal
     */
    Adapter(const QString &adapterPath, QObject *parent = 0);

    /**
     * @internal
     */
    void addDevice(const QString &objectPath);

    /**
     * @internal
     */
    void removeDevice(const QString &objectPath);

    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_deviceRemoved(QString))
    Q_PRIVATE_SLOT(d, void _k_propertyChanged(QString,QVariantMap,QStringList))
    Q_PRIVATE_SLOT(d, void _k_devicePropertyChanged(QString,QVariant))
};

}

#endif // BLUEDEVILADAPTER_H
