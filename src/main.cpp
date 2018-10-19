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
#define APP_MAIN_DEBUG 1

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
  QString strTokenBroker;
  QString strTokenBot;  
  for (auto j = 0; j < argc; ++j) {
    QString strArgName = argv[j];
    if (strArgName == "--tokenbroker") {
      const char* pStr = argv[++j];
      if (nullptr == pStr) {
        qDebug() << "--tokenbroker requires a token identifier.";
        return 0;
      }
      else
      {
        strTokenBroker = pStr;
      }
    }
    else if (strArgName == "--tokenbot") {
      const char* pStr = argv[++j];
      if (nullptr == pStr) {
        qDebug() << "--tokenbot requires a token identifier.";
        return 0;
      }
      else
      {
        strTokenBot = pStr;
      }
    }
  }
  if (strTokenBroker.isEmpty()) {
    qDebug() << "Required --tokenbroker <TOKEN>.";
    return 0;
  }
  if (strTokenBot.isEmpty()) {
    qDebug() << "Required --tokenbot <TOKEN>.";
    return 0;
  }
  pBrokerBinary = new CBrokerBinary(strTokenBroker);
  qDebug() << "Started Broker Binary";

  pTelegramBot = new CTelegramBot(strTokenBot, true, 500, 4);
  qDebug() << "Started Broker Telegram Bot";

  QObject::connect(
      pTelegramBot,  &Telegram::Bot::message
    , pBrokerBinary, &CBrokerBinary::slotOnMessageTelegramBot);  
  
  return a.exec();
}
