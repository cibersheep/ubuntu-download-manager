/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c DownloadAdaptor -a download_adaptor.h:download_adaptor.cpp com.canonical.applications.download.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef DOWNLOAD_ADAPTOR_H_1373626501
#define DOWNLOAD_ADAPTOR_H_1373626501

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
 * Adaptor class for interface com.canonical.applications.Download
 */
class DownloadAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.canonical.applications.Download")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.canonical.applications.Download\">\n"
"    <method name=\"totalSize\">\n"
"      <arg direction=\"out\" type=\"t\" name=\"total\"/>\n"
"    </method>\n"
"    <method name=\"progress\">\n"
"      <arg direction=\"out\" type=\"t\" name=\"received\"/>\n"
"    </method>\n"
"    <method name=\"metadata\">\n"
"      <annotation value=\"QVariantMap\" name=\"org.qtproject.QtDBus.QtTypeName.Out0\"/>\n"
"      <arg direction=\"out\" type=\"a{sv}\" name=\"data\"/>\n"
"    </method>\n"
"    <method name=\"setThrottle\">\n"
"      <arg direction=\"in\" type=\"t\" name=\"speed\"/>\n"
"    </method>\n"
"    <method name=\"throttle\">\n"
"      <arg direction=\"out\" type=\"t\" name=\"speed\"/>\n"
"    </method>\n"
"    <method name=\"allowGSMDownload\">\n"
"      <arg direction=\"in\" type=\"b\" name=\"allowed\"/>\n"
"    </method>\n"
"    <method name=\"isGSMDownloadAllowed\">\n"
"      <arg direction=\"out\" type=\"b\" name=\"allowed\"/>\n"
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
"    <signal name=\"finished\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"path\"/>\n"
"    </signal>\n"
"    <signal name=\"error\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"error\"/>\n"
"    </signal>\n"
"    <signal name=\"progress\">\n"
"      <arg direction=\"out\" type=\"t\" name=\"received\"/>\n"
"      <arg direction=\"out\" type=\"t\" name=\"total\"/>\n"
"    </signal>\n"
"  </interface>\n"
        "")
public:
    DownloadAdaptor(QObject *parent);
    virtual ~DownloadAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void allowGSMDownload(bool allowed);
    void cancel();
    bool isGSMDownloadAllowed();
    QVariantMap metadata();
    void pause();
    qulonglong progress();
    void resume();
    void setThrottle(qulonglong speed);
    void start();
    qulonglong throttle();
    qulonglong totalSize();
Q_SIGNALS: // SIGNALS
    void canceled(bool success);
    void error(const QString &error);
    void finished(const QString &path);
    void paused(bool success);
    void progress(qulonglong received, qulonglong total);
    void resumed(bool success);
    void started(bool success);
};

#endif
