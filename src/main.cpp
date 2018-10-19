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

/* ==========================================================================
 * CUSTOMIZABLE MODULE PRIVATE MACROS
 * ========================================================================== */

/* ==========================================================================
 * MODULE TAGGING
 * ========================================================================== */

/* ==========================================================================
 * STATIC VARIABLES FOR MODULE
 * ========================================================================== */
Telegram::Bot *bot;

/* ==========================================================================
 * MODULE PRIVATE TYPE DECLARATIONS
 * ========================================================================== */

/* ==========================================================================
 * STATIC VARIABLES FOR MODULE
 * ========================================================================== */

/* ==========================================================================
 * STATIC MEMBERS
 * ========================================================================== */
void newMessage(Telegram::Message message)
{
#if APP_MAIN_DEBUG == 1
  QDateTime dt = QDateTime::currentDateTime();
  qDebug() << "===============================================================";
  qDebug() << dt.toString();
  //qDebug() << "new message:" << message;
  qDebug() << "       text:" << message.string;
  QString strMessage = message.string;
  //GBP USD CALL 5 MIN WAIT CONFIRM
  //USD CHF CALL 5 MIN WAIT CONFIRM
  //GO
  //NO
  if (strMessage.endsWith("WAIT CONFIRM")) {
    QString strChangeFrom = strMessage.mid(0,3);
    QString strChangeTo   = strMessage.mid(4,3);
    qDebug() << "Broker attendi conferma " 
      << strChangeFrom << "->" << strChangeTo 
      << " ...";
  }
  else if (strMessage == "GO") {
    qDebug() << "Broker SCOMMETTI ORA! ";
  }
  else if (strMessage == "NO") {
    qDebug() << "Broker NON SCOMMETTERE! ";
  }
#endif
  /*if (bot && message.type == Telegram::Message::TextType) {
    bot->sendMessage(message.from.id, message.string);
  }*/
}

/* ==========================================================================
 * APPLICATION ENTRY POINT: Main
 * ========================================================================== */
int main(int argc,char* argv[])
{
  QCoreApplication a(argc, argv);
  QString strToken;
  for (auto j = 0; j < argc; ++j) {
    QString strArgName = argv[j];
    if (strArgName == "--token") {
      const char* pStr = argv[++j];
      if (nullptr == pStr) {
        qDebug() << "--token requires a token identifier.";
        return 0;
      }
      else
      {
        strToken = pStr;
      }
    }
  }

  bot = new Telegram::Bot(strToken, true, 500, 4);
  QObject::connect(bot, &Telegram::Bot::message, &newMessage);
  qDebug() << "Started Broker Telegram Bot";
  
  return a.exec();
}
