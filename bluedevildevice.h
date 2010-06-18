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

#include <bluedevil_export.h>

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtDBus/QDBusObjectPath>

namespace BlueDevil {

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
 * explicitly call to create.
 *
 * After the Device has been registered, it will automatically update its properties internally, and
 * additionally signals like pairedChanged will be emitted when this properties are updated.
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
     */
    bool registerDevice();

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
     * @return The list of supported services by the remote device.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     */
    QStringList UUIDs() const;

    /**
     * @return Whether this remote device is paired or not.
     *
     * @note This request will not trigger a connection to the device.
     */
    bool isPaired() const;

    /**
     * @return Whether this remote device is connected or not.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     */
    bool isConnected() const;

    /**
     * @return Whether this remote device is trusted or not.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     */
    bool isTrusted() const;

    /**
     * Sets whether this remote device is trusted or not.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     */
    void setTrusted(bool trusted);

    /**
     * @return Whether this remote device is blocked or not.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     */
    bool isBlocked() const;

    /**
     * Sets whether this remote device is blocked or not.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     */
    void setBlocked(bool blocked);

    /**
     * @return The alias of the remote device.
     *
     * @note This request will not trigger a connection to the device.
     */
    QString alias() const;

    /**
     * Sets the alias of the remote device.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     */
    void setAlias(const QString &alias);

    /**
     * @return The adapter that discovered this remote device.
     */
    Adapter *adapter() const;

    /**
     * @return Whether this remote device supports legacy pairing or not.
     *
     * @note This request will not trigger a connection to the device.
     */
    bool hasLegacyPairing() const;

    /**
     * @return A map with all supported services by this device.
     *
     * @note This request will trigger a connection to the device with the consequent registration
     *       on the bus.
     */
    QUInt32StringMap discoverServices(const QString &pattern = QString());

    /**
     * Cancels service discovery.
     */
    void cancelDiscovery();

    /**
     * Disconnect from this remote device.
     */
    void disconnect();

Q_SIGNALS:
    void pairedChanged(bool paired);
    void connectedChanged(bool connected);
    void trustedChanged(bool trusted);
    void blockedChanged(bool blocked);
    void aliasChanged(const QString &alias);
    void disconnectRequested();

private:
    Device(const QString &address, const QString &alias, quint32 deviceClass, const QString &icon,
           bool legacyPairing, const QString &name, bool paired, Adapter *adapter);

    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_propertyChanged(QString,QDBusVariant))
};

}

Q_DECLARE_METATYPE(BlueDevil::QUInt32StringMap)

#endif // BLUEDEVILDEVICE_H
