#ifndef LIBUBUNTUDOWNLOADMANAGERCOMMON_GLOBAL_H
#define LIBUBUNTUDOWNLOADMANAGERCOMMON_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBUBUNTUDOWNLOADMANAGERCOMMON_LIBRARY)
#  define LIBUBUNTUDOWNLOADMANAGERCOMMONSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBUBUNTUDOWNLOADMANAGERCOMMONSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBUBUNTUDOWNLOADMANAGERCOMMON_GLOBAL_H
