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

#ifndef BLUEDEVILDEVICE_H
#define BLUEDEVILDEVICE_H

#include <bluedevil/bluedevil_export.h>

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

    friend class Adapter;

public:
    virtual ~Device();

    /**
     * Starts the pairing process, the pairedChange signal will be emitted if succeed
     */
    void pair(const QString &dbusPath, const QString &options) const;

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
     * @note This request will not trigger a connection to the device.
     */
    QString name() const;

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
     * @return UBI for this device.
     */
    QString UBI() const;

public Q_SLOTS:
    /**
     * It is not mandatory to call to this method. If you are just retrieving some information that
     * will not trigger a connection to the device, and you do not need to check if some properties
     * were updated, please do not call to this method, since it is expensive (it will force a
     * registration of the device on the bus).
     *
     * On the other hand, if what you want is to receive signals of properties being updated and
     * you have not called a method that triggers a connection to the device, you should explicitly
     * call to this method, so the device is registered.
     *
     * @return Whether it was possible to correctly register this remote device on the bus.
     *
     * @note Allows being called with the asynchronous API through asyncCall. registerDeviceResult
     *       signal will be emitted with the result.
     */
    bool registerDevice();

    /**
     * @return The list of supported services by the remote device.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     *
     * @note Allows being called with the asynchronous API through asyncCall. UUIDsResult signal
     *       will be emitted with the result.
     */
    QStringList UUIDs();

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
     * Sets whether this remote device is trusted or not.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     *
     * @note Allows being called with the asynchronous API through asyncCall.
     */
    void setTrusted(bool trusted);

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
     * @return A map with all supported services by this device.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     *
     * @note Allows being called with the asynchronous API through asyncCall. discoverServicesResult
     *       signal will be emitted with the result.
     */
    QUInt32StringMap discoverServices(const QString &pattern = QString());

    /**
     * Cancels service discovery.
     *
     * @note Allows being called with the asynchronous API through asyncCall.
     */
    void cancelDiscovery();

    /**
     * Disconnect from this remote device.
     *
     * @note Allows being called with the asynchronous API through asyncCall.
     */
    void disconnect();

Q_SIGNALS:
    void pairedChanged(bool paired);
    void connectedChanged(bool connected);
    void trustedChanged(bool trusted);
    void blockedChanged(bool blocked);
    void aliasChanged(const QString &alias);
    void propertyChanged(const QString &property, const QVariant &value);
    void disconnectRequested();

/*
 * Signals coming from asynchronous API.
 */
Q_SIGNALS:
    void registerDeviceResult(Device *device, bool deviceRegistered);
    void UUIDsResult(Device *device, const QStringList &UUIDs);
    void isConnectedResult(Device *device, bool connected);
    void isTrustedResult(Device *device, bool trusted);
    void isBlockedResult(Device *device, bool blocked);
    void discoverServicesResult(Device *device, const QUInt32StringMap &services);

private:
    enum Type {
        DevicePath = 0,
        DeviceAddress
    };

    /**
     * @internal
     */
    Device(const QString &address, const QString &alias, quint32 deviceClass, const QString &icon,
           bool legacyPairing, const QString &name, bool paired, Adapter *adapter);
    Device(const QString &pathOrAddress, Type type, Adapter *adapter);

    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_propertyChanged(QString,QDBusVariant))
};

}

Q_DECLARE_METATYPE(BlueDevil::QUInt32StringMap)

#endif // BLUEDEVILDEVICE_H