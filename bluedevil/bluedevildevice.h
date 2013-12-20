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

#ifndef BLUEDEVILDEVICE_H
#define BLUEDEVILDEVICE_H

#include <bluedevil/bluedevil_export.h>

#include "bluedeviladapter.h"
#include "bluedevilmanager.h"

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtDBus/QDBusObjectPath>

namespace BlueDevil {

class Device;

/**
 * Generates an asynchronous call on any method of the Device class. Only some methods allow the
 * option of returning the result in form of signal, so not all methods can return information
 * in an asynchronous way.
 *
 * A typical usage follows:
 *
 * @code
 * connect(device, SIGNAL(registerDeviceResult(Device*,bool)), this, SLOT(deviceRegistered(Device*,bool)));
 * BlueDevil::asyncCall(device, SLOT(registerDevice()));
 * @endcode
 *
 * We will later receive on our deviceRegistered slot the information when the fetching of
 * information has finished.
 *
 * @warning Only documented methods (they are always slots) can be called asynchronously. They have
 *          been carefully chosen beforehand for those which are blocking.
 */
void BLUEDEVIL_EXPORT asyncCall(Device *device, const char *slot);

/**
 * @internal
 */
typedef QMap<quint32, QString> QUInt32StringMap;

class Adapter;

/**
 * @class Device bluedevildevice.h bluedevil/bluedevildevice.h
 *
 * This class represents a remote device, discovered by an Adapter.
 *
 * This device has some information for free (this meaning there is no need to register the device
 * on the bus). This properties that do not need connection are explicitly marked on their
 * respective documentation.
 *
 * Since this values are cached, you will not get updates on their state until you make a call that
 * forces the device to be registered on the bus (and thus, created, in D-Bus terminology), or you
 * explicitly call to registerDevice.
 *
 * After the Device has been registered, it will automatically update its properties internally, and
 * additionally signals like pairedChanged will be emitted when this properties are updated.
 *
 * Please note that since some functions here are blocking, there exists a way to asynchronous
 * perform certain operations that are known to be expensive. This way your GUI will not block
 * itself when waiting for a response from the remote device.
 *
 * @author Rafael Fernández López <ereslibre@kde.org>
 */
class BLUEDEVIL_EXPORT Device
    : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Adapter* adapter READ adapter)
    Q_PROPERTY(QString address READ address)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString friendlyName READ friendlyName)
    Q_PROPERTY(QString icon READ icon)
    Q_PROPERTY(quint32 deviceClass READ deviceClass)
    Q_PROPERTY(bool isPaired READ isPaired)
    Q_PROPERTY(QString alias READ alias WRITE setAlias)
    Q_PROPERTY(bool hasLegacyPairing READ hasLegacyPairing)
    Q_PROPERTY(QStringList UUIDs READ UUIDs)
    Q_PROPERTY(QString UBI READ UBI)
    Q_PROPERTY(bool isConnected READ isConnected)
    Q_PROPERTY(bool trusted READ isTrusted WRITE setTrusted)
    Q_PROPERTY(bool blocked READ isBlocked WRITE setBlocked)

    friend class Adapter;
    friend class Manager;

public:
    virtual ~Device();

    /**
     * Starts the pairing process, the pairedChanged signal will be emitted if succeeded.
     *
     * @note If the device is registered moments before this function is called, then it might
     *       do not work in some devices.
     */
    void pair() const;

    /**
     * @return The adapter that discovered this remote device.
     */
    Adapter *adapter() const;

    /**
     * @return The physical address of the remote device.
     *
     * @note This request will not trigger a connection to the device.
     */
    QString address() const;

    /**
     * @return The name of the remote device.
     *
     * @note This request will not trigger a connection to the device, unless the name couldn't
     *       yet be retrieved (is empty).
     */
    QString name() const;

    /**
     * @return If there is any alias set, it returns the alias for this device along with its name
     *         in the form of "Alias (Name)". If no alias for this device was set, the name is
     *         directly returned.
     *
     * @note If the name and the alias are the same, "Name" is returned instead of "Name (Name)".
     *
     * @note This request will not trigger a connection to the device, unless the name couldn't
     *       yet be retrieved (is empty).
     */
    QString friendlyName() const;

    /**
     * @return The suggested icon for the remote device.
     *
     * @note This request will not trigger a connection to the device.
     */
    QString icon() const;

    /**
     * @return The class of the remote device.
     *
     * @note This request will not trigger a connection to the device.
     */
    quint32 deviceClass() const;

    /**
     * @return Whether this remote device is paired or not.
     *
     * @note This request will not trigger a connection to the device.
     */
    bool isPaired() const;

    /**
     * @return The alias of the remote device.
     *
     * @note This request will not trigger a connection to the device.
     */
    QString alias() const;

    /**
     * @return Whether this remote device supports legacy pairing or not.
     *
     * @note This request will not trigger a connection to the device.
     */
    bool hasLegacyPairing() const;

    /**
     * @return The list of supported services by the remote device always in uppercase.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     *
     * @note Allows being called with the asynchronous API through asyncCall. UUIDsResult signal
     *       will be emitted with the result.
     */
    QStringList UUIDs();

    /**
     * @return UBI for this device. In case that the connection with the device fails, an empty
     *         string will be returned.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     *
     * @note Allows being called with the asynchronous API through asyncCall. UBIResult signal
     *       will be emitted with the result.
     */
    QString UBI();

    /**
     * @return Whether this remote device is connected or not.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     *
     * @note Allows being called with the asynchronous API through asyncCall. isConnectedResult
     *       signal will be emitted with the result.
     */
    bool isConnected();

    /**
     * @return Whether this remote device is trusted or not.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     *
     * @note Allows being called with the asynchronous API through asyncCall. isTrustedResult
     *       signal will be emitted with the result.
     */
    bool isTrusted();

    /**
     * @return Whether this remote device is blocked or not.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     *
     * @note Allows being called with the asynchronous API through asyncCall. isBlockedResult
     *       signal will be emitted with the result.
     */
    bool isBlocked();

public Q_SLOTS:
    /**
     * Sets whether this remote device is trusted or not.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     *
     * @note Allows being called with the asynchronous API through asyncCall.
     */
    void setTrusted(bool trusted);

    /**
     * Sets whether this remote device is blocked or not.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     *
     * @note Allows being called with the asynchronous API through asyncCall.
     */
    void setBlocked(bool blocked);

    /**
     * Sets the alias of the remote device.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     *
     * @note Allows being called with the asynchronous API through asyncCall.
     */
    void setAlias(const QString &alias);

    /**
     * Disconnect from this remote device.
     *
     * @note Allows being called with the asynchronous API through asyncCall.
     */
    void disconnect();

    /**
     * Connect all profiles marked auto-connectable of this device.
     */
    void connectDevice();

Q_SIGNALS:
    void pairedChanged(bool paired);
    void connectedChanged(bool connected);
    void trustedChanged(bool trusted);
    void blockedChanged(bool blocked);
    void aliasChanged(const QString &alias);
    void nameChanged(const QString &name);
    void UUIDsChanged(const QStringList &UUIDs);
    void propertyChanged(const QString &property, const QVariant &value);
    void disconnectRequested();

/*
 * Signals coming from asynchronous API.
 */
Q_SIGNALS:
    void UUIDsResult(Device *device, const QStringList &UUIDs);
    void UBIResult(Device *device, const QString &UBI);
    void isConnectedResult(Device *device, bool connected);
    void isTrustedResult(Device *device, bool trusted);
    void isBlockedResult(Device *device, bool blocked);

private:
    /**
     * @internal
     */
    Device(const QString &path, Adapter *adapter);

    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_propertyChanged(QString,QVariantMap,QStringList))
};

}

Q_DECLARE_METATYPE(BlueDevil::QUInt32StringMap)

#endif // BLUEDEVILDEVICE_H
