/* ==========================================================================
 * MODULE FILE NAME: broker_binary.cpp
 *      MODULE TYPE: class definition
 *
 *         FUNCTION: Define the CBrokerBinary class.
 *          PURPOSE: 
 *    CREATION DATE: 20181019
 *          AUTHORS: Fabrizio De Siati
 *     DESIGN ISSUE: None
 *       INTERFACES: CBrokerBinary
 *     SUBORDINATES: None.
 * 
 *          HISTORY: See table below. 
 *
 * 19-Oct-2018 | Fabrizio De Siati | 0.0 |
 * Initial creation of this file.
 *
 * ========================================================================== */

/* ==========================================================================
 * INCLUDES
 * ========================================================================== */
#include "broker_binary.h"
#include <QDebug>

/* ==========================================================================
 * MODULE PRIVATE MACROS
 * ========================================================================== */
#define BROKER_BINARY_DEBUG 1

/* ==========================================================================
 * MODULE TAGGING
 * ========================================================================== */

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
 *        FUNCTION NAME: CWdgGCPView
 * FUNCTION DESCRIPTION: constructor
 *        CREATION DATE: 20181019
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CBrokerBinary::CBrokerBinary(QString token, QObject *parent)
: QObject    { parent }
, m_strToken {  token }
{  
}

/* ==========================================================================
 *        FUNCTION NAME: ~CBrokerBinary
 * FUNCTION DESCRIPTION: destructor
 *        CREATION DATE: 20181019
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CBrokerBinary::~CBrokerBinary()
{
}

/* ==========================================================================
 *        FUNCTION NAME: slotOnMessageTelegramBot
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181019
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CBrokerBinary::slotOnMessageTelegramBot(Telegram::Message message)
{
#if BROKER_BINARY_DEBUG == 1
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
