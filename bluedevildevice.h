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

typedef QMap<quint32, QString> QUInt32StringHash;

class Adapter;

class BLUEDEVIL_EXPORT Device
    : public QObject
{
    Q_OBJECT

    friend class Adapter;

public:
    virtual ~Device();

    QString address() const;
    QString name() const;
    QString icon() const;
    quint32 deviceClass() const;
    QStringList UUIDs() const;
    bool isPaired() const;
    bool isConnected() const;
    bool isTrusted() const;
    void setTrusted(bool trusted);
    bool isBlocked() const;
    void setBlocked(bool blocked);
    QString alias() const;
    void setAlias(const QString &alias);
    Adapter *adapter() const;
    bool hasLegacyPairing() const;

    QUInt32StringHash discoverServices(const QString &pattern = QString());
    void cancelDiscovery();
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

Q_DECLARE_METATYPE(BlueDevil::QUInt32StringHash)

#endif // BLUEDEVILDEVICE_H
