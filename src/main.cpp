/* ==========================================================================
 * MODULE FILE NAME: main.cpp
 *      MODULE TYPE: application entry point
 *
 *         FUNCTION: Application Entry point.
 *          PURPOSE: Application Entry point.
 *    CREATION DATE: 20181016
 *          AUTHORS: Fabrizio De Siati
 *     DESIGN ISSUE: None
 *       INTERFACES: None.
 *     SUBORDINATES: None.
 *
 *          HISTORY: See table below.
 *
 * 16-Oct-2018 | Fabrizio De Siati | 0.0 |
 * Initial creation of this file.
 *
 * ========================================================================== */

/* ==========================================================================
 * MODULE PRIVATE MACROS
 * ========================================================================== */
#define APP_MAIN_DEBUG 0

/* ==========================================================================
 * INCLUDES
 * ========================================================================== */
#include "app_priv.h"
#include <QCoreApplication>
#include <QDebug>
#include "QtTelegramBot/qttelegrambot.h"
#include "broker_binary.h"
#include "telegram_bot.h"


/* ==========================================================================
 * CUSTOMIZABLE MODULE PRIVATE MACROS
 * ========================================================================== */

/* ==========================================================================
 * MODULE TAGGING
 * ========================================================================== */

/* ==========================================================================
 * STATIC VARIABLES FOR MODULE
 * ========================================================================== */
CBrokerBinary* pBrokerBinary;
CTelegramBot* pTelegramBot;

/* ==========================================================================
 * MODULE PRIVATE TYPE DECLARATIONS
 * ========================================================================== */

/* ==========================================================================
 * STATIC VARIABLES FOR MODULE
 * ========================================================================== */

/* ==========================================================================
 * STATIC MEMBERS
 * ========================================================================== */

/* ==========================================================================
 * APPLICATION ENTRY POINT: Main
 * ========================================================================== */
int main(int argc,char* argv[])
{
  QCoreApplication a(argc, argv);
  QString strAppIdBroker;
  QString strTokenBroker;
  QString strTokenBot;  
  for (auto j = 0; j < argc; ++j) {
    QString strArgName = argv[j];
    if (strArgName == "--app_id-broker") {
      const char* pStr = argv[++j];
      if (nullptr == pStr) {
        qWarning() << "--app_id-broker requires a app_id identifier.";
        return 0;
      }
      else
      {
        strAppIdBroker = pStr;
      }
    }
    else if (strArgName == "--token-broker") {
      const char* pStr = argv[++j];
      if (nullptr == pStr) {
        qWarning() << "--token-broker requires a token identifier.";
        return 0;
      }
      else
      {
        strTokenBroker = pStr;
      }
    }
    else if (strArgName == "--token-bot") {
      const char* pStr = argv[++j];
      if (nullptr == pStr) {
        qWarning() << "--token-bot requires a token identifier.";
        return 0;
      }
      else
      {
        strTokenBot = pStr;
      }
    }
  }
  if (strAppIdBroker.isEmpty()) {
    qWarning() << "Required --tokenbroker <TOKEN>.";
    return 0;
  }
  if (strTokenBroker.isEmpty()) {
    qWarning() << "Required --tokenbroker <TOKEN>.";
    return 0;
  }
  if (strTokenBot.isEmpty()) {
    qWarning() << "Required --tokenbot <TOKEN>.";
    return 0;
  }
  pBrokerBinary = new CBrokerBinary(strAppIdBroker, strTokenBroker);
#if APP_MAIN_DEBUG == 1
  qDebug() << "Started Broker Binary";
#endif

  pTelegramBot = new CTelegramBot(strTokenBot, true, 500, 4);
#if APP_MAIN_DEBUG == 1
  qDebug() << "Started Broker Telegram Bot";
#endif

  QObject::connect(
      pBrokerBinary, &CBrokerBinary::closed
    , &a, &QCoreApplication::quit);

  QObject::connect(
      pTelegramBot,  &Telegram::Bot::message
    , pBrokerBinary, &CBrokerBinary::slotOnMessageTelegramBot);  
  
  return a.exec();
}
