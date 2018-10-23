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
 *        FUNCTION NAME: DEBUG_APP
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181019
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CBrokerBinary::DEBUG_APP(const QString& strFunc, const QString& strMsg)
{
#if BROKER_BINARY_DEBUG == 1
  QDateTime dt = QDateTime::currentDateTime();
  qDebug() << "========================= DEBUG ===============================";
  qDebug() << dt.toString("yyyy-MM-dd hh:mm:ss.zzz000") << strFunc;
  if (!strMsg.isEmpty()) {
    qDebug() << strMsg;
  }
  qDebug() << "===============================================================";
#endif
}

/* ==========================================================================
 *        FUNCTION NAME: DEBUG_APP
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181019
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CBrokerBinary::WARNING_APP(const QString& strFunc, const QString& strMsg)
{
  QDateTime dt = QDateTime::currentDateTime();
  qWarning() << "========================= WARNING ===========================";
  qWarning() << dt.toString("yyyy-MM-dd hh:mm:ss.zzz000") << strFunc;
  if (!strMsg.isEmpty()) {
    qWarning() << strMsg;
  }
  qWarning() << "=============================================================";
}

/* ==========================================================================
 *        FUNCTION NAME: CWdgGCPView
 * FUNCTION DESCRIPTION: constructor
 *        CREATION DATE: 20181019
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CBrokerBinary::CBrokerBinary(const QString& app_id, const QString& token
  , QObject *parent)
: QObject    { parent }
, m_strAppId { app_id }
, m_strToken {  token }
{
  m_url = QUrl(QStringLiteral("wss://ws.binaryws.com/websockets/v3?app_id=%1")
    .arg(m_strAppId));
  DEBUG_APP("WebSocket server", m_url.toString());  
  connect(&m_webSocket, &QWebSocket::connected   , this
    , &CBrokerBinary::slotOnSocketConnected);
  connect(&m_webSocket, &QWebSocket::disconnected, this
    , &CBrokerBinary::closed);
  m_webSocket.open(QUrl(m_url));
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
 *        FUNCTION NAME: slotOnSocketConnected
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181019
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CBrokerBinary::slotOnSocketConnected()
{
  DEBUG_APP("WebSocket connected");
  connect(&m_webSocket, &QWebSocket::textMessageReceived, this
    , &CBrokerBinary::slotOnMessageSocketReceived);
  // authorize
  m_SendSocketMessage("authorize", { {"authorize", m_strToken} });  
}

/* ==========================================================================
 *        FUNCTION NAME: slotOnMessageSocketReceived
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181019
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CBrokerBinary::slotOnMessageSocketReceived(QString strMsg)
{
  DEBUG_APP("Received message socket", strMsg);
  QString strMsgType = "";
  QMap<QString, QString> mapValues;
  // decode response
  m_RecvSocketMessage(strMsg, strMsgType, mapValues);
  //m_webSocket.close();
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
  DEBUG_APP("Telegram message received", message.string);
#if 0 //BROKER_BINARY_DEBUG == 1
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
 *        FUNCTION NAME: m_SendSocketMessage
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181019
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
QString CBrokerBinary::m_SendSocketMessage(const QString& strMsgType
  , const QMap<QString,QString>& mapValues)
{
  QString strMsg;
  // Prepare
  if ("authorize" == strMsgType) {
    strMsg = QStringLiteral("{\"authorize\": \"%1\"}")
      .arg(mapValues.value("authorize"));
  }
  else {
    WARNING_APP(QString("Send message socket %1").arg(strMsgType)
      , QString("%1 is unrecognized").arg(strMsgType));
  }
  // Send
  if (!strMsg.isEmpty()) {
    DEBUG_APP(QString("Send message socket %1").arg(strMsgType), strMsg);
    m_historySendList.append(strMsg);
    m_webSocket.sendTextMessage(strMsg);
  }
  return strMsg;
}

/* ==========================================================================
 *        FUNCTION NAME: m_RecvSocketMessage
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181023
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CBrokerBinary::m_RecvSocketMessage(const QString& strMsg
  , QString& strMsgType, QMap<QString, QString>& mapValues)
{

}