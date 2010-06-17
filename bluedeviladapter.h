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

#ifndef BLUEDEVILADAPTER_H
#define BLUEDEVILADAPTER_H

#include <bluedevil_export.h>

#include <QtCore/QObject>
#include <QtDBus/QDBusObjectPath>

namespace BlueDevil {

class Device;
class Manager;

class BLUEDEVIL_EXPORT Adapter
    : public QObject
{
    Q_OBJECT

    friend class Manager;
    friend class Device;

public:
    virtual ~Adapter();

    /**
     * @return The adapter path on the DBus system.
     */
    QString adapterPath() const;

    /**
     * @return The address of the adapter.
     */
    QString address() const;

    /**
     * Sets the name of this adapter to @p name.
     *
     * This is the friendly name of the adapter.
     */
    void setName(const QString &name);

    /**
     * @return The friendly name of the adapter.
     */
    QString name() const;

    /**
     * Sets whether this adapter is consuming energy or not.
     */
    void setPowered(bool powered);

    /**
     * @return Whether this adapter is consuming energy or not.
     */
    bool isPowered() const;

    /**
     * Sets whether this adapter is discoverable or not.
     */
    void setDiscoverable(bool discoverable);

    /**
     * @return Whether this adapter is discoverable or not.
     */
    bool isDiscoverable() const;

    /**
     * Sets whether this adapter is pairable or not.
     */
    void setPairable(bool pairable);

    /**
     * @return Whether this adapter is pairable or not.
     */
    bool isPairable() const;

    /**
     * Sets the timeout for the pairing process for this adapter in seconds.
     *
     * @note A @p paireableTimeout of 0 means that this adapter can be paired forever.
     */
    void setPaireableTimeout(quint32 paireableTimeout);

    /**
     * @return The timeout for the pairing process for this adapter in seconds.
     */
    quint32 paireableTimeout() const;

    /**
     * Sets the timeout for this adapter to be discovered in seconds.
     *
     * @note A @p discoverableTimeout of 0 means that this adapter can be discovered forever.
     */
    void setDiscoverableTimeout(quint32 discoverableTimeout);

    /**
     * @return The timeout for this adapter to be discovered in seconds.
     */
    quint32 discoverableTimeout() const;

    /**
     * @return Whether this adapter is discovering at the moment or not.
     */
    bool isDiscovering() const;

    void requestSession() const;

    void releaseSession() const;

    void startDiscovery() const;

    void stopDiscovery() const;

Q_SIGNALS:
    void deviceFound(Device *device);
    void deviceDisappeared(Device *device);

private:
    Adapter(const QString &adapterPath, QObject *parent = 0);

    QDBusObjectPath findDevice(const QString &address) const;
    QDBusObjectPath createDevice(const QString &address) const;

    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_deviceCreated(QDBusObjectPath))
    Q_PRIVATE_SLOT(d, void _k_deviceFound(QString,QVariantMap))
    Q_PRIVATE_SLOT(d, void _k_deviceDisappeared(QString))
    Q_PRIVATE_SLOT(d, void _k_deviceRemoved(QDBusObjectPath))
    Q_PRIVATE_SLOT(d, void _k_propertyChanged(QString,QDBusVariant))
};

}

#endif // BLUEDEVILADAPTER_H
