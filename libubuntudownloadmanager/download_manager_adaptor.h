/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c DownloadManagerAdaptor -a download_manager_adaptor.h:download_manager_adaptor.cpp com.canonical.applications.download_manager.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef DOWNLOAD_MANAGER_ADAPTOR_H_1372949687
#define DOWNLOAD_MANAGER_ADAPTOR_H_1372949687

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface com.canonical.applications.DownloaderManager
 */
class DownloadManagerAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.canonical.applications.DownloaderManager")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.canonical.applications.DownloaderManager\">\n"
"    <method name=\"createDownload\">\n"
"      <annotation value=\"QVariantMap\" name=\"org.qtproject.QtDBus.QtTypeName.In1\"/>\n"
"      <annotation value=\"QVariantMap\" name=\"org.qtproject.QtDBus.QtTypeName.In2\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"url\"/>\n"
"      <arg direction=\"in\" type=\"a{sv}\" name=\"metadata\"/>\n"
"      <arg direction=\"in\" type=\"a{sv}\" name=\"headers\"/>\n"
"      <arg direction=\"out\" type=\"o\" name=\"download\"/>\n"
"    </method>\n"
"    <method name=\"createDownloadWithHash\">\n"
"      <annotation value=\"QVariantMap\" name=\"org.qtproject.QtDBus.QtTypeName.In3\"/>\n"
"      <annotation value=\"QVariantMap\" name=\"org.qtproject.QtDBus.QtTypeName.In4\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"url\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"algorithm\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"hash\"/>\n"
"      <arg direction=\"in\" type=\"a{sv}\" name=\"metadata\"/>\n"
"      <arg direction=\"in\" type=\"a{sv}\" name=\"headers\"/>\n"
"      <arg direction=\"out\" type=\"o\" name=\"download\"/>\n"
"    </method>\n"
"    <method name=\"getAllDownloads\">\n"
"      <arg direction=\"out\" type=\"ao\" name=\"downloads\"/>\n"
"    </method>\n"
"    <method name=\"getAllDownloadsWithMetadata\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"name\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"value\"/>\n"
"      <arg direction=\"out\" type=\"ao\" name=\"downloads\"/>\n"
"    </method>\n"
"    <signal name=\"downloadCreated\">\n"
"      <arg direction=\"out\" type=\"o\" name=\"path\"/>\n"
"    </signal>\n"
"  </interface>\n"
        "")
public:
    DownloadManagerAdaptor(QObject *parent);
    virtual ~DownloadManagerAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    QDBusObjectPath createDownload(const QString &url, const QVariantMap &metadata, const QVariantMap &headers);
    QDBusObjectPath createDownloadWithHash(const QString &url, const QString &algorithm, const QString &hash, const QVariantMap &metadata, const QVariantMap &headers);
    QList<QDBusObjectPath> getAllDownloads();
    QList<QDBusObjectPath> getAllDownloadsWithMetadata(const QString &name, const QString &value);
Q_SIGNALS: // SIGNALS
    void downloadCreated(const QDBusObjectPath &path);
};

#endif
