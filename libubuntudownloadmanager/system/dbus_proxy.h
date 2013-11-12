/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -p dbus_proxy.h:dbus_proxy.cpp -c DBusProxy -i metatypes.h org.freedesktop.DBus.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef DBUS_PROXY_H_1378717910
#define DBUS_PROXY_H_1378717910

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include <ubuntu/download_manager/metatypes.h>

/*
 * Proxy class for interface org.freedesktop.DBus
 */
class DBusProxy: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.freedesktop.DBus"; }

public:
    DBusProxy(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~DBusProxy();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> AddMatch(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QLatin1String("AddMatch"), argumentList);
    }

    inline QDBusPendingReply<QByteArray> GetAdtAuditSessionData(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QLatin1String("GetAdtAuditSessionData"), argumentList);
    }

    inline QDBusPendingReply<QString> GetConnectionAppArmorSecurityContext(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QLatin1String("GetConnectionAppArmorSecurityContext"), argumentList);
    }

    inline QDBusPendingReply<QByteArray> GetConnectionSELinuxSecurityContext(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QLatin1String("GetConnectionSELinuxSecurityContext"), argumentList);
    }

    inline QDBusPendingReply<uint> GetConnectionUnixProcessID(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QLatin1String("GetConnectionUnixProcessID"), argumentList);
    }

    inline QDBusPendingReply<uint> GetConnectionUnixUser(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QLatin1String("GetConnectionUnixUser"), argumentList);
    }

    inline QDBusPendingReply<QString> GetId()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("GetId"), argumentList);
    }

    inline QDBusPendingReply<QString> GetNameOwner(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QLatin1String("GetNameOwner"), argumentList);
    }

    inline QDBusPendingReply<QString> Hello()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("Hello"), argumentList);
    }

    inline QDBusPendingReply<QStringList> ListActivatableNames()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("ListActivatableNames"), argumentList);
    }

    inline QDBusPendingReply<QStringList> ListNames()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("ListNames"), argumentList);
    }

    inline QDBusPendingReply<QStringList> ListQueuedOwners(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QLatin1String("ListQueuedOwners"), argumentList);
    }

    inline QDBusPendingReply<bool> NameHasOwner(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QLatin1String("NameHasOwner"), argumentList);
    }

    inline QDBusPendingReply<uint> ReleaseName(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QLatin1String("ReleaseName"), argumentList);
    }

    inline QDBusPendingReply<> ReloadConfig()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("ReloadConfig"), argumentList);
    }

    inline QDBusPendingReply<> RemoveMatch(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QLatin1String("RemoveMatch"), argumentList);
    }

    inline QDBusPendingReply<uint> RequestName(const QString &in0, uint in1)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
        return asyncCallWithArgumentList(QLatin1String("RequestName"), argumentList);
    }

    inline QDBusPendingReply<uint> StartServiceByName(const QString &in0, uint in1)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
        return asyncCallWithArgumentList(QLatin1String("StartServiceByName"), argumentList);
    }

    inline QDBusPendingReply<> UpdateActivationEnvironment(StringMap in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QLatin1String("UpdateActivationEnvironment"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void NameAcquired(const QString &in0);
    void NameLost(const QString &in0);
    void NameOwnerChanged(const QString &in0, const QString &in1, const QString &in2);
};

namespace org {
  namespace freedesktop {
    typedef ::DBusProxy DBus;
  }
}
#endif
