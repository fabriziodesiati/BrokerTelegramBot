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
  QDateTime dt = QDateTime::currentDateTime();
  m_mapHistoryMsg.insert(dt.toString("yyyy-MM-dd hh:mm:ss.zzz000 SOCK OPEN")
    , m_url.toString());
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
  DEBUG_APP("WebSocket connected", "");
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
  QString strMsg = message.string;
  DEBUG_APP("Telegram message received", strMsg);
  QDateTime dt = QDateTime::currentDateTime();
  m_mapHistoryMsg.insert(dt.toString("yyyy-MM-dd hh:mm:ss.zzz000 TBOT RECV")
    , strMsg);
#if 0 //BROKER_BINARY_DEBUG == 1
  QDateTime dt = QDateTime::currentDateTime();
  qDebug() << "===============================================================";
  qDebug() << dt.toString();
  //qDebug() << "new message:" << message;
  qDebug() << "       text:" << strMsg;
  //GBP USD CALL 5 MIN WAIT CONFIRM
  //USD CHF CALL 5 MIN WAIT CONFIRM
  //GO
  //NO
  if (strMessage.endsWith("WAIT CONFIRM")) {
    QString strChangeFrom = strMsg.mid(0,3);
    QString strChangeTo   = strMsg.mid(4,3);
    qDebug() << "Broker attendi conferma " 
      << strChangeFrom << "->" << strChangeTo 
      << " ...";
  }
  else if (strMsg == "GO") {
    qDebug() << "Broker SCOMMETTI ORA! ";
  }
  else if (strMsg == "NO") {
    qDebug() << "Broker NON SCOMMETTERE! ";
  }
#endif
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
  // Prepare Send in JSon format
  if ("authorize" == strMsgType) {
    strMsg = QStringLiteral("{\"authorize\": \"%1\"}")
      .arg(mapValues.value("authorize"));
  }
  else {
    WARNING_APP(
        QString("Send message socket %1").arg(strMsgType)
      , QString("%1 is unrecognized").arg(strMsgType));
  }
  // Send
  if (!strMsg.isEmpty()) {
    DEBUG_APP(QString("Send message socket %1").arg(strMsgType), strMsg);
    QDateTime dt = QDateTime::currentDateTime();
    m_mapHistoryMsg.insert(dt.toString("yyyy-MM-dd hh:mm:ss.zzz000 SOCK SEND")
      , strMsg);
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
  DEBUG_APP("Recv message socket", strMsg);
  QDateTime dt = QDateTime::currentDateTime();
  m_mapHistoryMsg.insert(dt.toString("yyyy-MM-dd hh:mm:ss.zzz000 SOCK RECV")
    , strMsg);
  // Parsing JSON
  QJsonDocument doc = QJsonDocument::fromJson(strMsg.toUtf8());
  RETURN_IFW(doc.isNull(), );
  RETURN_IFW(!doc.isObject(), );
  QJsonObject msg = doc.object();
  QJsonObject msg__error;
  if (m_JSonObject(msg, "error", msg__error)) {    
    // there is error on response
    QString msg__error__code;
    RETURN_IFW(!m_JSonValueStr(msg__error, "code"   , msg__error__code), );
    QString msg__error__message;
    RETURN_IFW(!m_JSonValueStr(msg__error, "message", msg__error__message), );
    WARNING_APP("Binary return error", QString("code [%1] error[%2]")
      .arg(msg__error__code).arg(msg__error__message));
    return;
  }
  /* DECODE RESPONSE based on msg_type */
  RETURN_IFW(!m_JSonValueStr(msg, "msg_type", strMsgType), );
  if ("authorize" == strMsgType) 
  { /* First message to authorize application */
    QJsonObject msg__authorize;
    RETURN_IFW(!m_JSonObject(msg, "authorize", msg__authorize), );
    QString msg__authorize__balance;
    RETURN_IFW(!m_JSonValueStr(msg__authorize, "balance"
      , msg__authorize__balance), );
    INFO_APP("balance:", msg__authorize__balance);
    mapValues.insert("balance", msg__authorize__balance);
  }
}

/* ==========================================================================
 *        FUNCTION NAME: m_JSonObject
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181023
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CBrokerBinary::m_JSonObject(const QJsonObject& qJsonObjectParent
  , const QString& strName, QJsonObject& qJsonObjectRet)
{
  RETURN_IF(qJsonObjectParent.isEmpty(), false);
  RETURN_IF(!qJsonObjectParent.contains(strName), false);
  QJsonValue qJsonValue = qJsonObjectParent.value(strName);
  RETURN_IF(!qJsonValue.isObject(), false);
  qJsonObjectRet = qJsonValue.toObject();
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: m_JSonObject
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181023
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CBrokerBinary::m_JSonValueStr(const QJsonObject& qJsonObjectParent
  , const QString& strName, QString& strValueRet)
{
  RETURN_IF(qJsonObjectParent.isEmpty(), false);
  RETURN_IF(!qJsonObjectParent.contains(strName), false);
  QJsonValue qJsonValue = qJsonObjectParent.value(strName);
  RETURN_IF(!qJsonValue.isString(), false);
  strValueRet = qJsonValue.toString();
  return true;
}
