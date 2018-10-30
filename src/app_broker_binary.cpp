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
#include "app_configuration.h"
#include "app_database.h"
#include <QSettings>
#include <QStyleFactory>
#include <QSqlQuery>
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
 *        FUNCTION NAME: CurrentDateTime
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
QString CAppBrokerBinary::CurrentDateTime()
{
  QDateTime dt = QDateTime::currentDateTime();
  return dt.toString("yyyy-MM-dd hh:mm:ss.zzz000");
}

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
: uiMain                 {    new Ui::CWdgMain }
, m_pAppTelegramBot      {             nullptr }
, m_status               { Status::kINITIALIZE }
, m_strAppId             {              app_id }
, m_strToken             {               token }
, m_strTokenBot          {            tokenBot }
, m_i64SessionId         {                  -1 }
, m_i64SessionIdSelected {                  -1 }
, m_strIdProposal        {                  "" }
, m_strContractType      {                  "" }
, m_strPrice             {                  "" }
{
  DEBUG_APP("Starting Broker Binary ...", "");
  /* Set URL */
  m_url = QUrl(QStringLiteral("wss://ws.binaryws.com/websockets/v3?app_id=%1")
    .arg(m_strAppId));

  /* Setup user interface */
  uiMain->setupUi(this);

  /* Hide controls */
  uiMain->menubar->setVisible(false);  
  uiMain->toolBar->setVisible(false);
  uiMain->statusbar->setVisible(true);

  setCentralWidget(&m_wdgCentral);
  ui = m_wdgCentral.getUi();

  /* Connect model */
  ui->tbHistory->setModel(&m_model);

  /* connect signals and slots */
  connect(&CAppDatabase::GetInstance()
    , SIGNAL(connected())
    , SLOT(slotOnDbConnected()));
  connect(ui->comboSession
    , SIGNAL(currentTextChanged(const QString&))
    , SLOT(slotOnComboSessionsCurrentTextChanged(const QString&)));
  connect(ui->cbLookApply
    , SIGNAL(currentTextChanged(QString))
    , SLOT(slotOnLookApply(QString)));

  /* Apply theme settings */
  QSettings s(HMI_COMPANY, HMI_TITLE);
  auto strTheme = s.value("gui_theme").toString();
  QString strCurrTextPrev = ui->cbLookApply->currentText();
  ui->cbLookApply->setCurrentText(
    (-1 != ui->cbLookApply->findText(strTheme))?strTheme:"Darkino");
  // Force look apply if combo text is unchanged (first time)
  if (strCurrTextPrev == ui->cbLookApply->currentText()) {
    slotOnLookApply(strCurrTextPrev);
  }

  /* Initialize proposal */
  ui->dsbProposal->setValue(
    CAppConfiguration::GetInstance().get("proposal", "1.0").toDouble());

  /* Update Status: INITIALIZE */
  m_StatusUpdate(Status::kINITIALIZE);
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
  delete uiMain;
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
  static bool bFirstTime = true;
  if (bFirstTime) {
    connect(&m_webSocket
      , SIGNAL(textMessageReceived(const QString&))
      , SLOT(slotOnMessageSocketReceived(const QString&)));
    bFirstTime = false;
  }
  // authorize
  QString strMsg;
  CATCH_ABORT(
      !m_SendSocketMessage("authorize", { {"authorize", m_strToken} }, strMsg)
    , "Error on send authorize request");
}

/* ==========================================================================
 *        FUNCTION NAME: slotOnSocketDisconnected
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181030
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppBrokerBinary::slotOnSocketDisconnected()
{
  DEBUG_APP("WebSocket disconnected", "");
  CATCH_ABORT(true
    , "Socket has been closed");
  emit closed();
}

/* ==========================================================================
 *        FUNCTION NAME: slotOnMessageSocketReceived
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181019
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppBrokerBinary::slotOnMessageSocketReceived(const QString& strMsg)
{
  QString strMsgType = "";
  QMap<QString, QString> mapValues;
  // decode response
  CATCH_ABORT(
      !m_RecvSocketMessage(strMsg, strMsgType, mapValues)
    , "Error on decode received socket message"); 
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
  // decode telegram message
  CATCH_ABORT(
      !m_RcvTelegramMessage(message.string)
    , "Error on decode received telegram message"); 
}

/* ==========================================================================
 *        FUNCTION NAME: slotOnComboSessionsCurrentTextChanged
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppBrokerBinary::slotOnComboSessionsCurrentTextChanged(
  const QString& strSelectedSession)
{
  m_i64SessionIdSelected = -1;
  RETURN_IF(strSelectedSession.isEmpty(), );  
  if ("ALL" != strSelectedSession) {
    auto list = strSelectedSession.split(":");
    CATCH_BUG(list.count() < 2, "ComboBox for sessions is malformed");
    m_i64SessionIdSelected = strSelectedSession.split(":").at(0).toLongLong();
  }
  CATCH_ABORT(!m_HistoryRelaod(true), "Cannot reload history from database");
}

/* ==========================================================================
 *        FUNCTION NAME: slotOnLookApply
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181030
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppBrokerBinary::slotOnLookApply(const QString& strLook)
{
  QSettings s(HMI_COMPANY, HMI_TITLE);
  s.setValue("gui_theme", strLook);
  m_LookApply(QString("Fusion::%1").arg(strLook));
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
  /* Create database tables is don't exist */
  CATCH_ABORT(!m_DbCreateTable(), "Unable to create database tables");
  /* Create session on database */
  CATCH_ABORT(-1 == m_SessionCreate()
    , "Unable to create a valid session on database");
  /* Reload combo sessions */
  CATCH_ABORT(!m_ComboSessionLoad()
    , "Unable to load session list from database");
  /* Open sockect connection */
  CATCH_ABORT(!m_OpenSocket(), "Unable to open socket connection");
}

/* ==========================================================================
 *        FUNCTION NAME: m_statusUpdate
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181030
 *              AUTHORS: Michele Iacobellis
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppBrokerBinary::m_StatusUpdate(Status status)
{
  m_status = status;
  switch (m_status)
  {
  case Status::kINITIALIZE: uiMain->statusbar->showMessage("INITIALIZE");
    break;
  case Status::kAUTHORIZED: uiMain->statusbar->showMessage("AUTHORIZED");
    break;
  case Status::kWAITFORCON: uiMain->statusbar->showMessage("WAIT FOR CONFIRM");
    break;
  case Status::kPROPOSALGO: uiMain->statusbar->showMessage("PROPOSAL GO");
    break;
  }
}

/* ==========================================================================
 *        FUNCTION NAME: m_LookApply
 * FUNCTION DESCRIPTION: apply look and theme
 *        CREATION DATE: 20181030
 *              AUTHORS: Michele Iacobellis
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppBrokerBinary::m_LookApply(const QString& strTheme)
{
  if (!strTheme.isEmpty())
  {
    QString strQtTheme;
    QString strPkCustm;

    QStringList sl = strTheme.split("::");
    if (sl.count() >= 2)
    {
      strQtTheme = sl.at(0);
      strPkCustm = sl.at(1);
    }

    /* Setup style */
    qApp->setStyle(QStyleFactory::create(strQtTheme));

    /* Configure theme */
    if ("Dark" == strPkCustm)
    {
      QPalette palette;
      palette.setColor(QPalette::Window, QColor(53, 53, 53));
      palette.setColor(QPalette::WindowText, Qt::white);
      palette.setColor(QPalette::Base, QColor(25, 25, 25));
      palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
      palette.setColor(QPalette::ToolTipBase, Qt::white);
      palette.setColor(QPalette::ToolTipText, Qt::white);
      palette.setColor(QPalette::Text, Qt::white);
      palette.setColor(QPalette::Button, QColor(53, 53, 53));
      palette.setColor(QPalette::Disabled, QPalette::Button, QColor(40, 40, 40));
      palette.setColor(QPalette::ButtonText, Qt::white);
      palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::lightGray);
      palette.setColor(QPalette::BrightText, Qt::red);
      palette.setColor(QPalette::Link, QColor(42, 130, 218));
      palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
      palette.setColor(QPalette::HighlightedText, Qt::black);
      qApp->setPalette(palette);
      qApp->setStyleSheet(
        "QToolTip { color: #ffffff; "
        "background-color: #2a82da; border: 1px solid white; }");
    }
    else if ("Darkino" == strPkCustm)
    {
      QPalette palette;
      palette.setColor(QPalette::Window, QColor(100, 100, 100));
      palette.setColor(QPalette::WindowText, Qt::white);
      palette.setColor(QPalette::Base, QColor(53, 53, 53));
      palette.setColor(QPalette::AlternateBase, QColor(83, 83, 83));
      palette.setColor(QPalette::ToolTipBase, Qt::white);
      palette.setColor(QPalette::ToolTipText, Qt::white);
      palette.setColor(QPalette::Text, Qt::white);
      palette.setColor(QPalette::Button, QColor(100, 100, 100));
      palette.setColor(QPalette::Disabled, QPalette::Button, QColor(40, 40, 40));
      palette.setColor(QPalette::ButtonText, Qt::white);
      palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::lightGray);
      palette.setColor(QPalette::BrightText, Qt::red);
      palette.setColor(QPalette::Link, QColor(42, 130, 218));
      palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
      palette.setColor(QPalette::HighlightedText, Qt::black);
      qApp->setPalette(palette);
      qApp->setStyleSheet(
        "QToolTip { color: #ffffff; "
        "background-color: #2a82da; border: 1px solid white; }");
    }
    else if ("Mountain" == strPkCustm)
    {
      QPalette palette;
      palette.setColor(QPalette::Window, QColor(0x90, 0xAF, 0xC5));
      palette.setColor(QPalette::WindowText, QColor(0x76, 0x36, 0x26));
      palette.setColor(QPalette::Base, QColor(0x33, 0x6B, 0x87));
      palette.setColor(QPalette::AlternateBase, QColor(0x2A, 0x31, 0x32));
      palette.setColor(QPalette::ToolTipBase, Qt::blue);
      palette.setColor(QPalette::ToolTipText, Qt::white);
      palette.setColor(QPalette::Text, Qt::white);
      palette.setColor(QPalette::Button, QColor(0x90, 0xAF, 0xC5));
      palette.setColor(QPalette::Disabled, QPalette::Button, QColor(40, 40, 40));
      palette.setColor(QPalette::ButtonText, QColor(0x76, 0x36, 0x26));
      palette.setColor(QPalette::BrightText, Qt::red);
      palette.setColor(QPalette::Link, QColor(42, 130, 218));
      palette.setColor(QPalette::Highlight, QColor(0x76, 0x36, 0x26));
      palette.setColor(QPalette::HighlightedText, Qt::white);
      qApp->setPalette(palette);
      qApp->setStyleSheet(
        "QToolTip { color: #ffffff; "
        "background-color: #2a82da; border: 1px solid white; }");
    }
    else if ("Desert" == strPkCustm)
    {
      QPalette palette;
      palette.setColor(QPalette::Window, QColor(0xA4, 0x38, 0x20));
      palette.setColor(QPalette::WindowText, Qt::white);
      palette.setColor(QPalette::Base, QColor(0xA4, 0x38, 0x20));
      palette.setColor(QPalette::AlternateBase, QColor(0xBA, 0x55, 0x36));
      palette.setColor(QPalette::ToolTipBase, Qt::blue);
      palette.setColor(QPalette::ToolTipText, Qt::white);
      palette.setColor(QPalette::Text, Qt::white);
      palette.setColor(QPalette::Button, QColor(0xBA, 0x55, 0x36));
      palette.setColor(QPalette::Disabled, QPalette::Button, QColor(40, 40, 40));
      palette.setColor(QPalette::ButtonText, Qt::white);
      palette.setColor(QPalette::BrightText, Qt::red);
      palette.setColor(QPalette::Link, QColor(42, 130, 218));
      palette.setColor(QPalette::Highlight, QColor(0xA4, 0x38, 0x20));
      palette.setColor(QPalette::HighlightedText, Qt::white);
      qApp->setPalette(palette);
      qApp->setStyleSheet(
        "QToolTip { color: #ffffff; "
        "background-color: #2a82da; border: 1px solid white; }");
    }
    else if ("Water" == strPkCustm)
    {
      QPalette palette;
      palette.setColor(QPalette::Window, QColor(0x50, 0x51, 0x60));
      palette.setColor(QPalette::WindowText, Qt::white);
      palette.setColor(QPalette::Base, QColor(0x68, 0x82, 0x9E));
      palette.setColor(QPalette::AlternateBase, QColor(0x50, 0x51, 0x60));
      palette.setColor(QPalette::ToolTipBase, Qt::blue);
      palette.setColor(QPalette::ToolTipText, Qt::white);
      palette.setColor(QPalette::Text, Qt::white);
      palette.setColor(QPalette::Button, QColor(0x68, 0x82, 0x9E));
      palette.setColor(QPalette::Disabled, QPalette::Button, QColor(40, 40, 40));
      palette.setColor(QPalette::ButtonText, Qt::white);
      palette.setColor(QPalette::BrightText, Qt::red);
      palette.setColor(QPalette::Link, QColor(42, 130, 218));
      palette.setColor(QPalette::Highlight, QColor(0x59, 0x82, 0x34));
      palette.setColor(QPalette::HighlightedText, Qt::white);
      qApp->setPalette(palette);
      qApp->setStyleSheet(
        "QToolTip { color: #ffffff; "
        "background-color: #2a82da; border: 1px solid white; }");
    }
  }
}

/* ==========================================================================
 *        FUNCTION NAME: m_DbCreateTable
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_DbCreateTable()
{
  /* Create Table sessions */
  RETURN_IFW(!CAppDatabase::GetInstance().execQuery(
      "CREATE TABLE IF NOT EXISTS sessions ( \
          id integer PRIMARY KEY AUTOINCREMENT \
        , date_time text NOT NULL UNIQUE)")
    , "Unable to create sessions table"
    , false);
  /* Create Table history */
  RETURN_IFW(!CAppDatabase::GetInstance().execQuery(
      "CREATE TABLE IF NOT EXISTS history ( \
          id integer PRIMARY KEY AUTOINCREMENT \
        , session_id NOT NULL \
        , date_time text NOT NULL \
        , operation text NOT NULL \
        , state text \
        , balance text \
        , currency text \
        , parameters text \
        , details text \
        , FOREIGN KEY(session_id) REFERENCES sessions(id))")
    , "Unable to create history table"
    , false);
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: m_SessionCreate
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
int64_t CAppBrokerBinary::m_SessionCreate()
{
  QString strCurrentDateTime = CurrentDateTime();
  m_i64SessionId = CAppDatabase::GetInstance().execInsertQuery(QString(
    "INSERT INTO sessions (date_time) VALUES ('%1')").arg(strCurrentDateTime));
  RETURN_IFW(-1 == m_i64SessionId
    , "Unable to create a valid sesison id on database", false);
  ui->leSession->setText(QString("%1: %2")
     .arg(m_i64SessionId, 3, 10, QChar('0'))
    .arg(strCurrentDateTime));
  return m_i64SessionId;
}

/* ==========================================================================
 *        FUNCTION NAME: m_ComboSessionLoad
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_ComboSessionLoad()
{
  /* Clear combo and map */
  ui->comboSession->clear();
  /* Select sessions */
  const QString strQuery = "SELECT DISTINCT id, date_time FROM sessions \
                            ORDER BY date_time DESC";
  QSqlQuery qry;
  RETURN_IFW(!qry.exec(strQuery), QString("Unable to execute query [%1] E=%2")
    .arg(strQuery).arg(qry.lastError().text()), false);
  /* Add entry for ALL */
  QStringList listItems = QStringList() << "ALL";
  QString strSelected;
  while(qry.next()) {
    int64_t i64ComboValue = qry.value("id").toLongLong();
    QString strComboItem = QString("%1: %2")
      .arg(i64ComboValue, 3, 10, QChar('0'))
      .arg(qry.value("date_time").toString());    
    listItems.append(strComboItem);
    if (i64ComboValue == m_i64SessionId) {
      strSelected = strComboItem;
    }
  }
  /* Populate combo */
  ui->comboSession->addItems(listItems);
  /* Select current session */
  if (!strSelected.isEmpty()) {
    ui->comboSession->setCurrentText(strSelected);
  }
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: m_HistoryRelaod
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_HistoryRelaod(bool bForceResize)
{
  m_model.setQuery(QString("SELECT * from history %2 ORDER BY date_time DESC")
    .arg(-1 == m_i64SessionIdSelected ? ""
                 : QString("WHERE session_id=%1").arg(m_i64SessionIdSelected)));
  /* Hide columns first time */
  static bool bHideColumns = true;
  static const QList<bool> listHiddenCols = QList<bool>()
    << true  //id
    << true  //session_id
    << false //date_time
    << false //operation
    << false //state
    << false //balance
    << false //currency
    << false //parameters
    << true; //details    
  if (bHideColumns)
  {
    int iCol = 0;
    for(auto bHidden: listHiddenCols) {
      ui->tbHistory->setColumnHidden(iCol++, bHidden);
    }
    bHideColumns = false;
  }
  /* Resize to contents */
  static bool bResizeToContents = true;
  if (bForceResize) {
    bResizeToContents = true;
  }
  if (bResizeToContents && m_model.rowCount() > 0)
  { /* Reset column width based on size*/
    for(auto iCol=0; iCol < m_model.columnCount(); ++iCol)
    { // Resize only shown clolumns
      if (!listHiddenCols.at(iCol)) {
        ui->tbHistory->resizeColumnToContents(iCol);
      }
    }
    bResizeToContents = false;
  }
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: m_OpenSocket
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_OpenSocket()
{
  DEBUG_APP("WebSocket server", m_url.toString());
  CATCH_ABORT(!m_HistoryInsert({
      {"operation" , "SOCK OPEN"}
    , {"parameters", m_url.toString()}})
    , "Unable to insert history record on database");
  static bool bFirstTime = true;
  if (bFirstTime) {
    /* Connect signals and slots for socket */
    connect(&m_webSocket, SIGNAL(connected()),   SLOT(slotOnSocketConnected()));
    connect(&m_webSocket, SIGNAL(disconnected()),SLOT(slotOnSocketConnected()));
    /* Start bot */
    CATCH_ABORT(!m_BotStart(), "Unable to start Telegram Bot");
    bFirstTime = false;
  }
  m_webSocket.open(QUrl(m_url));
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: m_HistoryInsert
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_HistoryInsert(const QMap<QString,QString>& mapValues)
{
  RETURN_IFW(!CAppDatabase::GetInstance().execQuery(QString(
      "INSERT INTO history (\
        session_id, date_time, operation, state, balance, currency, parameters\
        , details) \
      VALUES (%1,'%2','%3','%4','%5','%6','%7','%8')")
      .arg(m_i64SessionId)
      .arg(CurrentDateTime())
      .arg(mapValues.value("operation"))
      .arg(mapValues.value("state"))
      .arg(ui->leBalance->text())
      .arg(ui->labelCurrency->text())
      .arg(mapValues.value("parameters"))
      .arg(mapValues.value("details")))
    , "Unable to insert history record"
    , false);
  // Reload history if session is selected (or ALL)
  RETURN_IF(
      -1 == m_i64SessionIdSelected || m_i64SessionId == m_i64SessionIdSelected
    , m_HistoryRelaod(true));
  return true;
}


/* ==========================================================================
 *        FUNCTION NAME: m_BotStart
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_BotStart()
{
  DEBUG_APP("Starting Telegram Bot ...", "");
  /* Instatiate Telegram Bot */
  delete m_pAppTelegramBot;
  m_pAppTelegramBot = new(std::nothrow) CAppTelegramBot(m_strTokenBot
    , true, 500, 4);
  connect(m_pAppTelegramBot,  &Telegram::Bot::message
    , this, &CAppBrokerBinary::slotOnMessageTelegramBot);
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: m_RcvTelegramMessage
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181030
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_RcvTelegramMessage(const QString& strMsg)
{
  DEBUG_APP("Telegram message received", strMsg);
  RETURN_IFW(!m_HistoryInsert({
      {"operation" , "TBOT RECV"}
    , {"parameters", strMsg}
    , {"details"   , strMsg} })
    , "Unable to insert history record on database", false);
  //GBP USD CALL 5 MIN WAIT CONFIRM
  //USD CHF CALL 5 MIN WAIT CONFIRM
  //GO
  //NO
  QString strMsgSockSend;
  if      (strMsg.endsWith("WAIT CONFIRM")) {
    QStringList list = strMsg.split(" ");
    // Early return without CATCH_ABORT (return true)
    RETURN_IFW(list.count() < 7, "WAIT CONFIRM request malformed", true);
    QString strFrxA = list.at(0);
    QString strFrxB = list.at(1);
    QString strContractType = list.at(2);
    QString strDuration = list.at(3);
    QString strDurationUnit = list.at(4);
    // proposal
    RETURN_IFW(!m_SendSocketMessage("proposal"
        , { 
              {"amout",         QString::number(ui->dsbProposal->value())} 
            , {"contract_type", strContractType}
            , {"currency"     , ui->labelCurrency->text()}
            , {"duration"     , strDuration}
            , {"duration_unit", strDurationUnit == "MIN" ? "m" : "m"}
            , {"symbol"       , QString("frx%1%2").arg(strFrxA).arg(strFrxB)}
          }
        , strMsgSockSend)
      , "Error on send proposal request", false);
  }
  else if (strMsg == "GO") {
    // Early return without CATCH_ABORT (return true)
    RETURN_IFW(Status::kWAITFORCON != m_status
      , "Cannot place proposal in this status", true);
    RETURN_IFW(m_strIdProposal.isEmpty()
      , "Cannot place proposal beacause id is empty", true);
    RETURN_IFW(m_strContractType != "CALL" && m_strContractType != "PUT"
      , "Cannot place proposal beacause type isn't CALL or PUT", true);
    RETURN_IFW(m_strPrice.isEmpty()
      , "Cannot place proposal beacause price is empty", true);
    // buy/sell
    QString strBuySell = m_strContractType == "CALL" ? "buy" : "sell";
    RETURN_IFW(!m_SendSocketMessage(strBuySell
        , { 
              {strBuySell, m_strIdProposal} 
            , {"price"   , m_strPrice}
          }
        , strMsgSockSend)
      , "Error on send proposal request", false);
  }
  else if (strMsg == "NO") {
    // Reset status
    m_strIdProposal = "";
    m_strContractType = "";
    m_strPrice = "";
    m_StatusUpdate(Status::kAUTHORIZED);
  }
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: m_SendSocketMessage
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181019
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_SendSocketMessage(const QString& strMsgType
  , const QMap<QString,QString>& mapValues, QString& strMsg)
{
  strMsg = "";
  // Prepare Send in JSon format
  if      ("authorize" == strMsgType) {
    /* authorize */
    strMsg = QStringLiteral(
      "{                     \
        \"authorize\": \"%1\"\
       }")
      .arg(mapValues.value("authorize"));
  }
  else if ("proposal" == strMsgType) {
    /* proposal */
    strMsg = QStringLiteral(
      "{                          \
        \"proposal\": 1,          \
        \"amount\": \"%1\",       \
        \"basis\": \"stake\",     \
        \"contract_type\":\"%2\", \
        \"currency\": \"%3\",     \
        \"duration\": \"%4\",     \
        \"duration_unit\": \"%5\",\
        \"symbol\": \"%6\"        \
       }")
      .arg(mapValues.value("amout"))
      .arg(mapValues.value("contract_type"))
      .arg(mapValues.value("currency"))
      .arg(mapValues.value("duration"))
      .arg(mapValues.value("duration_unit"))
      .arg(mapValues.value("symbol"));
    // store contract_type to send buy (CALL) or sell (PUT) request
    m_strContractType = mapValues.value("contract_type");
    m_strPrice = mapValues.value("amout");
  }
  else if ("buy" == strMsgType) {
    /* proposal */
    strMsg = QStringLiteral(
      "{                          \
        \"buy\": \"%1\",          \
        \"price\": %2             \
       }")
      .arg(mapValues.value("buy"))
      .arg(mapValues.value("price"));
  }
  else if ("sell" == strMsgType) {
    /* proposal */
    strMsg = QStringLiteral(
      "{                          \
        \"sell\": \"%1\",         \
        \"price\": %2             \
       }")
      .arg(mapValues.value("buy"))
      .arg(mapValues.value("price"));
  }
  else {
    RETURN_IFW(true, QString("Message type %1 is unrecognized").arg(strMsgType)
      , false);    
  }
  // Send
  if (!strMsg.isEmpty()) {
    DEBUG_APP(QString("Send message socket %1").arg(strMsgType), strMsg);
    QString strParameters = QString("%1: ").arg(strMsgType);
    for(auto str: mapValues.keys()) {
      strParameters.append(QString("%1=%2, ")
        .arg(str).arg(mapValues.value(str)));
    }
    RETURN_IFW(!m_HistoryInsert({
        {"operation" , "SOCK SEND"}
      , {"parameters", strParameters}
      , {"details"   , strMsg} })
      , "Unable to insert history record on database", false);
    m_webSocket.sendTextMessage(strMsg);
  }
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: m_RecvSocketMessage
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181023
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_RecvSocketMessage(const QString& strMsg
  , QString& strMsgType, QMap<QString, QString>& mapValues)
{
  DEBUG_APP("Recv message socket", strMsg);
  // Parsing JSON
  QJsonDocument doc = QJsonDocument::fromJson(strMsg.toUtf8());
  RETURN_IFW(doc.isNull(), "JsonDocument is null", false);
  RETURN_IFW(!doc.isObject(), "JsonObject is null", false);
  QJsonObject msg = doc.object();
  QJsonObject msg__error;
  if (m_JSonObject(msg, "error", msg__error)) {    
    // there is error on response
    QString msg__error__code;
    RETURN_IFW(!m_JSonValueStr(msg__error, "code"   , msg__error__code)
      , "JSonValue 'code' doesn't exist", false);
    QString msg__error__message;
    RETURN_IFW(!m_JSonValueStr(msg__error, "message", msg__error__message)
      , "JSonValue 'message' doesn't exist", false);
    WARNING_APP("Binary return error", QString("code [%1] error[%2]")
      .arg(msg__error__code).arg(msg__error__message));
    strMsgType = "error";
    mapValues.insert("code", msg__error__code);
    mapValues.insert("message", msg__error__message);
  }
  else
  { /* DECODE RESPONSE based on msg_type */
    RETURN_IFW(!m_JSonValueStr(msg, "msg_type", strMsgType)
      , "JSonValue 'msg_type' doesn't exist", false);
    if      ("authorize" == strMsgType) 
    { /* First message to authorize application */
      QJsonObject msg__authorize;
      RETURN_IFW(!m_JSonObject(msg, "authorize", msg__authorize)
        , "JSonObject 'authorize' doesn't exist", false);
      QString msg__authorize__balance;
      RETURN_IFW(!m_JSonValueStr(msg__authorize, "balance"
        , msg__authorize__balance)
        , "JSonValue 'balance' doesn't exist", false);
      QString msg__authorize__currency;
      RETURN_IFW(!m_JSonValueStr(msg__authorize, "currency"
        , msg__authorize__currency)
        , "JSonValue 'currency' doesn't exist", false);
      INFO_APP("balance:", msg__authorize__currency);
      ui->leBalance->setText(msg__authorize__balance);
      ui->labelCurrency->setText(msg__authorize__currency);
      mapValues.insert("balance", msg__authorize__balance);
      mapValues.insert("currency", msg__authorize__currency);
      /* Update Status: AUTHORIZED */
      m_StatusUpdate(Status::kAUTHORIZED);
    }
    else if ("proposal" == strMsgType) 
    { /* Proposal wait feedback */
      QJsonObject msg__authorize;
      RETURN_IFW(!m_JSonObject(msg, "proposal", msg__authorize)
        , "JSonObject 'proposal' doesn't exist", false);
      QString msg__proposal__id;
      RETURN_IFW(!m_JSonValueStr(msg__authorize, "id"
        , msg__proposal__id)
        , "JSonValue 'id' doesn't exist", false);
      INFO_APP("id proposal:", msg__proposal__id);
      mapValues.insert("id", msg__proposal__id);
      /* Update Status: WAIT FOR CONFIRM */
      m_strIdProposal = msg__proposal__id;
      m_StatusUpdate(Status::kWAITFORCON);
    }
    else if ("buy" == strMsgType) 
    { /* Buy wait feedback */
      QJsonObject msg__buy;
      RETURN_IFW(!m_JSonObject(msg, "buy", msg__buy)
        , "JSonObject 'buy' doesn't exist", false);
      QString msg__buy__balance_after;
      RETURN_IFW(!m_JSonValueStr(msg__buy, "balance_after"
        , msg__buy__balance_after)
        , "JSonValue 'balance_after' doesn't exist", false);
      INFO_APP("balance_after proposal:", msg__buy__balance_after);
      mapValues.insert("balance_after", msg__buy__balance_after);
      ui->leBalance->setText(msg__buy__balance_after);
      /* Update Status: AUTHORIZED */
      m_StatusUpdate(Status::kAUTHORIZED);
    }
    else if ("sell" == strMsgType) 
    { /* Sell wait feedback */
      QJsonObject msg__sell;
      RETURN_IFW(!m_JSonObject(msg, "sell", msg__sell)
        , "JSonObject 'sell' doesn't exist", false);
      QString msg__sell__balance_after;
      RETURN_IFW(!m_JSonValueStr(msg__sell, "balance_after"
        , msg__sell__balance_after)
        , "JSonValue 'balance_after' doesn't exist", false);
      INFO_APP("balance_after proposal:", msg__sell__balance_after);
      mapValues.insert("balance_after", msg__sell__balance_after);      
      ui->leBalance->setText(msg__sell__balance_after);
      /* Update Status: AUTHORIZED */
      m_StatusUpdate(Status::kAUTHORIZED);
    }
  }
  QString strParameters = QString("%1: ").arg(strMsgType);
  for(auto str: mapValues.keys()) {
    strParameters.append(QString("%1=%2, ")
      .arg(str).arg(mapValues.value(str)));
  }
  RETURN_IFW(!m_HistoryInsert({
      {"operation" , "SOCK RECV"}
    , {"parameters", strParameters}
    , {"details"   , strMsg} })
    , "Unable to insert history record on database", false);
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
