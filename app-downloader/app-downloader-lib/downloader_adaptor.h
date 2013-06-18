/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c DownloaderAdaptor -a /home/mandel/Canonical/junk/app-downloader/app-downloader/app-downloader-lib/downloader_adaptor.h:/home/mandel/Canonical/junk/app-downloader/app-downloader/app-downloader-lib/downloader_adaptor.cpp /home/mandel/Canonical/junk/app-downloader/app-downloader/app-downloader-lib/com.canonical.applications.downloader.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef DOWNLOADER_ADAPTOR_H_1371554321
#define DOWNLOADER_ADAPTOR_H_1371554321

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
 * Adaptor class for interface com.canonical.applications.Downloader
 */
class DownloaderAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.canonical.applications.Downloader")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.canonical.applications.Downloader\">\n"
"    <method name=\"createDownload\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"id\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"name\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"url\"/>\n"
"      <arg direction=\"out\" type=\"o\" name=\"download\"/>\n"
"    </method>\n"
"    <method name=\"createDownloadWithHash\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"id\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"name\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"url\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"algorithm\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"hash\"/>\n"
"      <arg direction=\"out\" type=\"o\" name=\"download\"/>\n"
"    </method>\n"
"    <method name=\"getAllDownloads\">\n"
"      <arg direction=\"out\" type=\"ao\" name=\"downloads\"/>\n"
"    </method>\n"
"    <signal name=\"downloadCreated\">\n"
"      <arg direction=\"out\" type=\"o\" name=\"path\"/>\n"
"    </signal>\n"
"  </interface>\n"
        "")
public:
    DownloaderAdaptor(QObject *parent);
    virtual ~DownloaderAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    QDBusObjectPath createDownload(const QString &id, const QString &name, const QString &url);
    QDBusObjectPath createDownloadWithHash(const QString &id, const QString &name, const QString &url, const QString &algorithm, const QString &hash);
    QList<QDBusObjectPath> getAllDownloads();
Q_SIGNALS: // SIGNALS
    void downloadCreated(const QDBusObjectPath &path);
};

#endif
