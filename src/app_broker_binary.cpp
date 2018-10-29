/* ==========================================================================
 * MODULE FILE NAME: app_broker_binary.cpp
 *      MODULE TYPE: class definition
 *
 *         FUNCTION: Define the CAppBrokerBinary class.
 *          PURPOSE: 
 *    CREATION DATE: 20181019
 *          AUTHORS: Fabrizio De Siati
 *     DESIGN ISSUE: None
 *       INTERFACES: CAppBrokerBinary
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
#include "app_broker_binary.h"
#include "ui_wdgmain.h"
#include "ui_wdgcentral.h"

/* ==========================================================================
 * MODULE PRIVATE MACROS
 * ========================================================================== */
#define APP_DEBUG 1

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
 *        FUNCTION NAME: CWdgCentral
 * FUNCTION DESCRIPTION: constructor
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CWdgCentral::CWdgCentral(QWidget *parent)
: QWidget(parent)
, ui(new Ui::CWdgCentral)
{
  ui->setupUi(this);
}

/* ==========================================================================
 *        FUNCTION NAME: ~CWdgCentral
 * FUNCTION DESCRIPTION: destructor
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CWdgCentral::~CWdgCentral()
{
    delete ui;
}

/* ==========================================================================
 *        FUNCTION NAME: CAppBrokerBinary
 * FUNCTION DESCRIPTION: constructor
 *        CREATION DATE: 20181019
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CAppBrokerBinary::CAppBrokerBinary(const QString& app_id, const QString& token
  , const QString& tokenBot, QMainWindow *parent)
: ui                { new Ui::CWdgMain }
, m_pAppTelegramBot {          nullptr }
, m_strAppId        {           app_id }
, m_strToken        {            token }
, m_strTokenBot     {         tokenBot }
, m_i64Session      {               -1 }
{
  DEBUG_APP("Starting Broker Binary ...", "");
  /* Set URL */
  m_url = QUrl(QStringLiteral("wss://ws.binaryws.com/websockets/v3?app_id=%1")
    .arg(m_strAppId));

  /* Setup user interface */
  ui->setupUi(this);

  setCentralWidget(&m_wdgCentral);
  uiC = m_wdgCentral.getUi();
}

/* ==========================================================================
 *        FUNCTION NAME: ~CAppBrokerBinary
 * FUNCTION DESCRIPTION: destructor
 *        CREATION DATE: 20181019
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CAppBrokerBinary::~CAppBrokerBinary()
{
  delete ui;
}

/* ==========================================================================
 *        FUNCTION NAME: slotOnSocketConnected
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181019
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppBrokerBinary::slotOnSocketConnected()
{
  DEBUG_APP("WebSocket connected", "");
  connect(&m_webSocket, &QWebSocket::textMessageReceived, this
    , &CAppBrokerBinary::slotOnMessageSocketReceived);
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
void CAppBrokerBinary::slotOnMessageSocketReceived(QString strMsg)
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
void CAppBrokerBinary::slotOnMessageTelegramBot(Telegram::Message message)
{
  QString strMsg = message.string;
  DEBUG_APP("Telegram message received", strMsg);
  QDateTime dt = QDateTime::currentDateTime();
  m_mapHistoryMsg.insert(dt.toString("yyyy-MM-dd hh:mm:ss.zzz000 TBOT RECV")
    , strMsg);
#if 0 //APP_BROKER_BINARY_DEBUG == 1
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
 *        FUNCTION NAME: slotOnDbConnected
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppBrokerBinary::slotOnDbConnected()
{
  /* Open sockect connection */
  m_OpenSocket();
}

/* ==========================================================================
 *        FUNCTION NAME: m_OpenSocket
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppBrokerBinary::m_OpenSocket()
{
  DEBUG_APP("WebSocket server", m_url.toString());
  QDateTime dt = QDateTime::currentDateTime();
  m_mapHistoryMsg.insert(dt.toString("yyyy-MM-dd hh:mm:ss.zzz000 SOCK OPEN")
    , m_url.toString());
  static bool bFirstTime = true;
  if (bFirstTime) {
    /* Connect signals and slots for socket */
    connect(&m_webSocket, &QWebSocket::connected   , this
      , &CAppBrokerBinary::slotOnSocketConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this
      , &CAppBrokerBinary::closed);

    /* Start bot */
    m_BotStart();

    bFirstTime = false;
  }
  m_webSocket.open(QUrl(m_url));
}

/* ==========================================================================
 *        FUNCTION NAME: m_BotStart
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppBrokerBinary::m_BotStart()
{
  DEBUG_APP("Starting Telegram Bot ...", "");
  /* Instatiate Telegram Bot */
  delete m_pAppTelegramBot;
  m_pAppTelegramBot = new(std::nothrow) CAppTelegramBot(m_strTokenBot
    , true, 500, 4);
  connect(m_pAppTelegramBot,  &Telegram::Bot::message
    , this, &CAppBrokerBinary::slotOnMessageTelegramBot);
}

/* ==========================================================================
 *        FUNCTION NAME: m_SendSocketMessage
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181019
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
QString CAppBrokerBinary::m_SendSocketMessage(const QString& strMsgType
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
void CAppBrokerBinary::m_RecvSocketMessage(const QString& strMsg
  , QString& strMsgType, QMap<QString, QString>& mapValues)
{
  DEBUG_APP("Recv message socket", strMsg);
  QDateTime dt = QDateTime::currentDateTime();
  m_mapHistoryMsg.insert(dt.toString("yyyy-MM-dd hh:mm:ss.zzz000 SOCK RECV")
    , strMsg);
  // Parsing JSON
  QJsonDocument doc = QJsonDocument::fromJson(strMsg.toUtf8());
  RETURN_IFW(doc.isNull(), "JsonDocument is null", );
  RETURN_IFW(!doc.isObject(), "JsonObject is null", );
  QJsonObject msg = doc.object();
  QJsonObject msg__error;
  if (m_JSonObject(msg, "error", msg__error)) {    
    // there is error on response
    QString msg__error__code;
    RETURN_IFW(!m_JSonValueStr(msg__error, "code"   , msg__error__code)
      , "JSonValue 'code' doesn't exist", );
    QString msg__error__message;
    RETURN_IFW(!m_JSonValueStr(msg__error, "message", msg__error__message)
      , "JSonValue 'message' doesn't exist", );
    WARNING_APP("Binary return error", QString("code [%1] error[%2]")
      .arg(msg__error__code).arg(msg__error__message));
    return;
  }
  /* DECODE RESPONSE based on msg_type */
  RETURN_IFW(!m_JSonValueStr(msg, "msg_type", strMsgType)
      , "JSonValue 'msg_type' doesn't exist", );
  if ("authorize" == strMsgType) 
  { /* First message to authorize application */
    QJsonObject msg__authorize;
    RETURN_IFW(!m_JSonObject(msg, "authorize", msg__authorize)
      , "JSonObject 'authorize' doesn't exist", );
    QString msg__authorize__balance;
    RETURN_IFW(!m_JSonValueStr(msg__authorize, "balance"
      , msg__authorize__balance)
      , "JSonValue 'balance' doesn't exist", );
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
bool CAppBrokerBinary::m_JSonObject(const QJsonObject& qJsonObjectParent
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
bool CAppBrokerBinary::m_JSonValueStr(const QJsonObject& qJsonObjectParent
  , const QString& strName, QString& strValueRet)
{
  RETURN_IF(qJsonObjectParent.isEmpty(), false);
  RETURN_IF(!qJsonObjectParent.contains(strName), false);
  QJsonValue qJsonValue = qJsonObjectParent.value(strName);
  RETURN_IF(!qJsonValue.isString(), false);
  strValueRet = qJsonValue.toString();
  return true;
}
