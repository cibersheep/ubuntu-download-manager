/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c TestingManagerAdaptor -a testing_manager_adaptor.h:testing_manager_adaptor.cpp com.canonical.applications.testing.download_manager.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef TESTING_MANAGER_ADAPTOR_H_1391594312
#define TESTING_MANAGER_ADAPTOR_H_1391594312

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include <ubuntu/download_manager/metatypes.h>

QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface com.canonical.applications.testing.DownloadManager
 */
class TestingManagerAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.canonical.applications.testing.DownloadManager")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.canonical.applications.testing.DownloadManager\">\n"
"    <method name=\"returnDBusErrors\">\n"
"      <arg direction=\"in\" type=\"b\" name=\"returnErrors\"/>\n"
"    </method>\n"
"    <method name=\"returnHttpError\">\n"
"      <annotation value=\"HttpErrorStruct\" name=\"org.qtproject.QtDBus.QtTypeName.In1\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"download\"/>\n"
"      <arg direction=\"in\" type=\"(is)\" name=\"error\"/>\n"
"    </method>\n"
"    <method name=\"returnNetworkError\">\n"
"      <annotation value=\"NetworkErrorStruct\" name=\"org.qtproject.QtDBus.QtTypeName.In1\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"download\"/>\n"
"      <arg direction=\"in\" type=\"(is)\" name=\"error\"/>\n"
"    </method>\n"
"    <method name=\"returnProcessError\">\n"
"      <annotation value=\"ProcessErrorStruct\" name=\"org.qtproject.QtDBus.QtTypeName.In1\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"download\"/>\n"
"      <arg direction=\"in\" type=\"(isiss)\" name=\"error\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    TestingManagerAdaptor(QObject *parent);
    virtual ~TestingManagerAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void returnDBusErrors(bool returnErrors);
    void returnHttpError(const QString &download, HttpErrorStruct error);
    void returnNetworkError(const QString &download, NetworkErrorStruct error);
    void returnProcessError(const QString &download, ProcessErrorStruct error);
Q_SIGNALS: // SIGNALS
};

#endif
