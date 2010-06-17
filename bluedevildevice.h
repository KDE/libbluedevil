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
#include <QtDBus/QDBusObjectPath>

namespace BlueDevil {

typedef QMap<quint32, QString> QAlternativeMap;

class Adapter;

class BLUEDEVIL_EXPORT Device
    : public QObject
{
    Q_OBJECT

    friend class Adapter;

public:
    virtual ~Device();

    QString address() const;
    QString alias() const;
    quint32 deviceClass() const;
    QString icon() const;
    bool hasLegacyPairing() const;
    QString name() const;
    bool isPaired() const;
    short RSSI() const;

    QAlternativeMap discoverServices(const QString &pattern);
    void cancelDiscovery();
    void disconnect();

Q_SIGNALS:
    void disconnectRequested();

private:
    Device(const QString &address, const QString &alias, quint32 deviceClass, const QString &icon,
           bool legacyPairing, const QString &name, bool paired, short RSSI, Adapter *adapter = 0);

    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_propertyChanged(QString,QDBusVariant))
};

}

Q_DECLARE_METATYPE(BlueDevil::QAlternativeMap)

#endif // BLUEDEVILDEVICE_H
