/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c ApplicationDownloadAdaptor -a /home/mandel/Canonical/junk/app-downloader/app-downloader/app-downloader-lib/application_download_adaptor.h:/home/mandel/Canonical/junk/app-downloader/app-downloader/app-downloader-lib/application_download_adaptor.cpp /home/mandel/Canonical/junk/app-downloader/app-downloader/app-downloader-lib/com.canonical.applications.application_download.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef APPLICATION_DOWNLOAD_ADAPTOR_H_1372786415
#define APPLICATION_DOWNLOAD_ADAPTOR_H_1372786415

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
 * Adaptor class for interface com.canonical.applications.ApplicationDownload
 */
class ApplicationDownloadAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.canonical.applications.ApplicationDownload")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.canonical.applications.ApplicationDownload\">\n"
"    <method name=\"applicationId\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"id\"/>\n"
"    </method>\n"
"    <method name=\"applicationName\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"name\"/>\n"
"    </method>\n"
"    <method name=\"totalSize\">\n"
"      <arg direction=\"out\" type=\"u\" name=\"total\"/>\n"
"    </method>\n"
"    <method name=\"progress\">\n"
"      <arg direction=\"out\" type=\"u\" name=\"received\"/>\n"
"    </method>\n"
"    <method name=\"metadata\">\n"
"      <annotation value=\"QVariantMap\" name=\"org.qtproject.QtDBus.QtTypeName.Out0\"/>\n"
"      <arg direction=\"out\" type=\"a{sv}\" name=\"data\"/>\n"
"    </method>\n"
"    <method name=\"start\"/>\n"
"    <method name=\"pause\"/>\n"
"    <method name=\"resume\"/>\n"
"    <method name=\"cancel\"/>\n"
"    <signal name=\"started\">\n"
"      <arg direction=\"out\" type=\"b\" name=\"success\"/>\n"
"    </signal>\n"
"    <signal name=\"paused\">\n"
"      <arg direction=\"out\" type=\"b\" name=\"success\"/>\n"
"    </signal>\n"
"    <signal name=\"resumed\">\n"
"      <arg direction=\"out\" type=\"b\" name=\"success\"/>\n"
"    </signal>\n"
"    <signal name=\"canceled\">\n"
"      <arg direction=\"out\" type=\"b\" name=\"success\"/>\n"
"    </signal>\n"
"    <signal name=\"finished\"/>\n"
"    <signal name=\"error\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"error\"/>\n"
"    </signal>\n"
"    <signal name=\"progress\">\n"
"      <arg direction=\"out\" type=\"u\" name=\"received\"/>\n"
"      <arg direction=\"out\" type=\"u\" name=\"total\"/>\n"
"    </signal>\n"
"  </interface>\n"
        "")
public:
    ApplicationDownloadAdaptor(QObject *parent);
    virtual ~ApplicationDownloadAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    QString applicationId();
    QString applicationName();
    void cancel();
    QVariantMap metadata();
    void pause();
    uint progress();
    void resume();
    void start();
    uint totalSize();
Q_SIGNALS: // SIGNALS
    void canceled(bool success);
    void error(const QString &error);
    void finished();
    void paused(bool success);
    void progress(uint received, uint total);
    void resumed(bool success);
    void started(bool success);
};

#endif
