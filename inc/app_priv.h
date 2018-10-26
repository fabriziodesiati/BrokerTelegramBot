/* ==========================================================================
 *   INCLUDE FILE NAME: app_priv.h
 * INCLUDE DESCRIPTION: sub-library private include file.
 *       CREATION DATE: 20181016
 *             AUTHORS: Fabrizio De Siati
 *        DESIGN ISSUE: None. 
 *
 *             HISTORY: See table below.
 * 
 * 16-Oct-2018 | Fabrizio De Siati | 0.0 |
 * Initial creation of this file.
 * 
 * ========================================================================== */

#ifndef APP_PRIV_H
#define APP_PRIV_H

/* ==========================================================================
 * INCLUDE: This file is generated by CMake for managing macro based source
 * code configuration. The HAVE_CONFIG_H macro is set into the CMakeLists.txt
 * project file.
 * ========================================================================== */
#include <iostream>
#ifdef HAVE_CONFIG_H
# include "BrokerTelegramBot_config.h"
#endif

#ifdef _MSC_VER
# define THIS __FUNCTION__
#else
# define THIS __PRETTY_FUNCTION__
#endif

/**
 * Calls the message handler with the debug function/message.
 */
#define DEBUG_APP(strFunc,strMsg)\
  do { \
  QDateTime dt = QDateTime::currentDateTime(); \
  qDebug()    << "=========================== DEBUG ========================"; \
  qDebug()    << dt.toString("yyyy-MM-dd hh:mm:ss.zzz000") << strFunc; \
  qDebug()    << strMsg; \
  qDebug()    << "=========================================================="; \
  } while (false)

/**
 * Calls the message handler with the info function/message.
 */
#define INFO_APP(strFunc,strMsg)\
  do { \
  QDateTime dt = QDateTime::currentDateTime(); \
  qInfo()     << "=========================== INFO ========================="; \
  qInfo()     << dt.toString("yyyy-MM-dd hh:mm:ss.zzz000") << strFunc; \
  qInfo()     << strMsg; \
  qInfo()     << "=========================================================="; \
  } while (false)

/**
 * Calls the message handler with the warning function/message.
 */
#define WARNING_APP(strFunc,strMsg)\
  do { \
  QDateTime dt = QDateTime::currentDateTime(); \
  qWarning()  << "=========================== WARNING ======================"; \
  qWarning()  << dt.toString("yyyy-MM-dd hh:mm:ss.zzz000") << strFunc; \
  qWarning()  << strMsg; \
  qWarning()  << "=========================================================="; \
  } while (false)

/**
 * Calls the message handler with the critical function/message.
 */
#define CRITICAL_APP(strFunc,strMsg)\
  do { \
  QDateTime dt = QDateTime::currentDateTime(); \
  qCritical() << "=========================== CRITICAL ====================="; \
  qCritical() << dt.toString("yyyy-MM-dd hh:mm:ss.zzz000") << strFunc; \
  qCritical() << strMsg; \
  qCritical() << "=========================================================="; \
  } while (false)

/**
 * Macro for catching bugs.
 */
#define CATCH_BUG(cond,msg)\
  do { \
  if (cond) { \
    QString strFunc = QString("DETECTED BUG %1 @%2").arg(THIS).arg(__LINE__);  \
    QString strMsg = QString("%1 (C=%2)").arg(#msg).arg(#cond); \
    CRITICAL_APP(strFunc,strMsg); \
    do { \
      std::cout << '\n' << "Press a key to continue..."; \
    } while (std::cin.get() != '\n'); \
    ::exit(0); \
  }} while (false)

/**
 * Macro for aborting instantaneously.
 */
#define CATCH_ABORT(cond,msg)\
  do { \
  if (cond) { \
    QString strFunc = QString("DETECTED ABORT %1 @%2").arg(THIS).arg(__LINE__);\
    QString strMsg = QString("%1 (C=%2)").arg(#msg).arg(#cond); \
    CRITICAL_APP(strFunc,strMsg); \
    do { \
      std::cout << '\n' << "Press a key to continue..."; \
    } while (std::cin.get() != '\n'); \
    ::exit(0); \
  }} while (false)

/**
 * Use this macro for early-exit on input parameter validation.
 */
#define RETURN_IF(cond,c)\
  do { if (cond) { return c; }} while (false)

/**
 * Use this macro for early-exit on input parameter validation.
 */
#define RETURN_IFW(cond,c)\
  do { \
  if (cond) { \
    QString strFunc = QString("%1 @%2").arg(THIS).arg(__LINE__); \
    QString strMsg = QString("%1 (C=%2)").arg(#cond).arg(#c); \
    WARNING_APP(strFunc,strMsg); \
    return c; \
  }} while (false)

#endif  // APP_PRIV_H
