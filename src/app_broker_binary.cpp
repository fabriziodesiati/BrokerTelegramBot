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
#include <QSqlRecord>
#include "ui_wdgmain.h"
#include "ui_wdgcentral.h"

/* ==========================================================================
 * MODULE PRIVATE MACROS
 * ========================================================================== */
#define APP_DEBUG 1
#define APP_BROKER_BINARY_TIMEOUT_MS  10000
#define APP_BROKER_BINARY_RECV_TICK   3

/**
 * Calls the wdg/message handler with the debug function/message.
 */
#define DEBUG_APP_WDG(strFunc,strMsg)\
  do { \
  ui->lblInfo->setText(QString("[DEBUG] %1: %2").arg(strFunc).arg(strMsg)); \
  DEBUG_APP(strFunc,strMsg); \
  } while (false)

/**
 * Calls the wdg/message handler with the info function/message.
 */
#define INFO_APP_WDG(strFunc,strMsg)\
  do { \
  ui->lblInfo->setText(QString("[INFO] %1: %2").arg(strFunc).arg(strMsg)); \
  INFO_APP(strFunc,strMsg); \
  } while (false)

/**
 * Calls the wdg/message handler with the warning function/message.
 */
#define WARNING_APP_WDG(strFunc,strMsg)\
  do { \
  QString strFullMsg = QString("[WARNING] %1 : %2").arg(strFunc).arg(strMsg); \
  ui->lblError->setText(strFullMsg); \
  ui->lblError->setStyleSheet("QLabel { color : yellow; }"); \
  m_DbHistoryInsert({ \
      {"operation" , "APPL WARN"} \
    , {"parameters", strFullMsg}}); \
  WARNING_APP(strFunc,strMsg); \
  } while (false)

/**
 * Calls the wdg/message handler with the critical function/message.
 */
#define CRITICAL_APP_WDG(strFunc,strMsg)\
  do { \
  QString strFullMsg = QString("[CRITICAL] %1 : %2").arg(strFunc).arg(strMsg); \
  ui->lblError->setText(strFullMsg); \
  ui->lblError->setStyleSheet("QLabel { color : red; }"); \
  m_DbHistoryInsert({ \
      {"operation" , "APPL CRIT"} \
    , {"parameters", strFullMsg}}); \
  CRITICAL_APP(strFunc,strMsg); \
  } while (false)

/**
 * Macro for catching bugs on wdg.
 */
#define CATCH_BUG_WDG(cond,msg)\
  do { \
  if (cond) { \
    ui->lblError->setText(QString("[CATCH_BUG] %1 @%2: %3 (C=%4)") \
      .arg(THIS).arg(__LINE__).arg(#msg).arg(#cond)); \
    ui->lblError->setStyleSheet("QLabel { background-color : red; }"); \
    QEventLoop loop; loop.processEvents(); \
    CATCH_BUG(cond,msg); \
  }} while (false)

/**
 * Macro for aborting instantaneously on wdg.
 */
#define CATCH_ABORT_WDG(cond,msg)\
  do { \
  if (cond) { \
    ui->lblError->setText(QString("[CATCH_ABORT] %1 @%2: %3 (C=%4)") \
      .arg(THIS).arg(__LINE__).arg(#msg).arg(#cond)); \
    ui->lblError->setStyleSheet("QLabel { background-color : red; }"); \
    QEventLoop loop; loop.processEvents(); \
    CATCH_ABORT(cond,msg); \
  }} while (false)

/**
 * Use this macro for early-exit on input parameter validation on wdg.
 */
#define RETURN_IFW_WDG(cond,msg,c)\
  do { \
  if (cond) { \
    QString strFunc = QString("%1 @%2").arg(THIS).arg(__LINE__); \
    QString strMsg = QString("%1 (C=%2, ret=%3)").arg(#msg).arg(#cond).arg(#c);\
    WARNING_APP_WDG(strFunc,strMsg); \
    return c; \
  }} while (false)

/**
 * Use this macro for early-exit on input parameter validation on wdg.
 */
#define RETURN_IFC_WDG(cond,msg,c)\
  do { \
  if (cond) { \
    QString strFunc = QString("%1 @%2").arg(THIS).arg(__LINE__); \
    QString strMsg = QString("%1 (C=%2, ret=%3)").arg(#msg).arg(#cond).arg(#c);\
    CRITICAL_APP_WDG(strFunc,strMsg); \
    return c; \
  }} while (false)


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
, m_bSocketOpened        {               false }
, m_bFirstAuthorized     {                true }
, m_strBalanceStart      {                  "" }
, m_i64SessionId         {                  -1 }
, m_i64SessionIdSelected {                  -1 }
, m_i64LastIdProposal    {                  -1 }
{
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

  /* START FRM HERE TO USE XXX_WDG MACROs */
  DEBUG_APP_WDG("Starting Broker Binary ...", "");
  
  /* Set stylesheet for proposal resume */
  ui->leProposalsOpenSold->setStyleSheet("QLabel { color : yellow; }");
  ui->leProposalsLost->setStyleSheet("QLabel { color : red; }");
  ui->leProposalsWon->setStyleSheet ("QLabel { color : lightgreen; }");

  /* Hide details */
  ui->textDetails->setVisible(false);

  /* Connect model */
  ui->tbHistory->setModel(&m_modelHistory);
  ui->tbProposals->setModel(&m_modelProposals);

  /* TableView aspects */
  ui->tbHistory->setAlternatingRowColors(true);
  ui->tbHistory->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tbHistory->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->tbProposals->setAlternatingRowColors(true);
  ui->tbProposals->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tbProposals->setSelectionMode(QAbstractItemView::SingleSelection);

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
  connect(ui->pbBalance
    , SIGNAL(clicked())
    , SLOT(slotOnBalanceClicked()));
  connect(ui->tbHistory->selectionModel()
    , SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&))
    , SLOT(slotOnItemSelectedHistory(
      const QItemSelection&, const QItemSelection&)));
  connect(ui->tbProposals->selectionModel()
    , SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&))
    , SLOT(slotOnItemSelectedProposal(
      const QItemSelection&, const QItemSelection&)));

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

  QTimer* pTimer = new QTimer{ this };
  pTimer->setInterval(APP_BROKER_BINARY_TIMEOUT_MS);
  connect(pTimer, SIGNAL(timeout()), SLOT(slotOnTimeout()));
  pTimer->start();
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
 *        FUNCTION NAME: slotOnSocketError
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181111
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppBrokerBinary::slotOnSocketError(QAbstractSocket::SocketError)
{
  DEBUG_APP_WDG("WebSocket error", m_webSocket.errorString());
  CATCH_ABORT_WDG(!m_DbHistoryInsert({
      {"operation" , "SOCK ERRO"}
    , {"parameters", m_webSocket.errorString()}})
    , "Unable to insert history record on database");
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
  DEBUG_APP_WDG("WebSocket connected", "");
  CATCH_ABORT_WDG(!m_DbHistoryInsert({
      {"operation" , "SOCK CONN"}
    , {"parameters", m_url.toString()}})
    , "Unable to insert history record on database");
  static bool bFirstTime = true;
  if (bFirstTime) {
    connect(&m_webSocket
      , SIGNAL(textMessageReceived(const QString&))
      , SLOT(slotOnMessageSocketReceived(const QString&)));
    bFirstTime = false;
  }
  slotOnBalanceClicked();  
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
  DEBUG_APP_WDG("WebSocket disconnected", "");
  CATCH_ABORT_WDG(!m_DbHistoryInsert({
      {"operation" , "SOCK DISC"}
    , {"parameters", m_url.toString()}})
    , "Unable to insert history record on database");
  // Reconnect socket
  CATCH_ABORT_WDG(!m_SocketOpen(), "Cannot reconnect socket");
  //emit closed();
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
  // decode response
  RETURN_IFC_WDG(!m_RecvSocketMessage(strMsg)
    , "Error on decode received socket message", );
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
  RETURN_IFC_WDG(!m_RcvTelegramMessage(message.string)
    , "Error on decode received telegram message", ); 
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
    CATCH_BUG_WDG(list.count() < 2, "ComboBox for sessions is malformed");
    m_i64SessionIdSelected = strSelectedSession.split(":").at(0).toLongLong();
  }
  CATCH_ABORT_WDG(!m_DbHistoryRelaod(true)
    , "Cannot reload history from database");
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
 *        FUNCTION NAME: slotOnBalanceClicked
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181031
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppBrokerBinary::slotOnBalanceClicked()
{
  // authorize
  RETURN_IFC_WDG(!m_SendSocketMessage("authorize", {{"authorize", m_strToken}})
    , "Error on send authorize request", );
}

/* ==========================================================================
 *        FUNCTION NAME: slotOnItemSelectedHistory
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181102
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppBrokerBinary::slotOnItemSelectedHistory(const QItemSelection& sel
  , const QItemSelection&)
{
  ui->textDetails->setVisible(false);
  if (!sel.isEmpty())
  {
    auto il = sel.indexes();
    if (!il.isEmpty()) {
      ui->textDetails->setVisible(true);
      m_DetailsUpdate(m_modelHistory, il.first().row());
    }    
  }
}

/* ==========================================================================
 *        FUNCTION NAME: slotOnItemSelectedProposal
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181102
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppBrokerBinary::slotOnItemSelectedProposal(const QItemSelection& sel
  , const QItemSelection&)
{
  ui->textDetails->setVisible(false);
  if (!sel.isEmpty())
  {
    auto il = sel.indexes();
    if (!il.isEmpty()) {
      ui->textDetails->setVisible(true);
      m_DetailsUpdate(m_modelProposals, il.first().row());
    }    
  }
}

/* ==========================================================================
 *        FUNCTION NAME: slotOnTimeout
 * FUNCTION DESCRIPTION:
 *        CREATION DATE: 20181111
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppBrokerBinary::slotOnTimeout()
{
  RETURN_IF(!m_bSocketOpened, );
  static uint8_t u8Tick = APP_BROKER_BINARY_RECV_TICK;
  if (m_bFirstAuthorized) {
    /* Monitor Authorization */
    if (0 == --u8Tick) {
      u8Tick = APP_BROKER_BINARY_RECV_TICK;
      DEBUG_APP_WDG("WebSocket error", "Timeout on receive authorization");
      CATCH_ABORT_WDG(!m_DbHistoryInsert({
          {"operation" , "SOCK ERRO"}
        , {"parameters", "Timeout on receive authorization"}})
        , "Unable to insert history record on database");
      // Reconnect socket
      CATCH_ABORT_WDG(!m_SocketOpen(), "Cannot reconnect socket");
    }
  }
  else if (ui->sbPingSent->value() == ui->sbPongRecv->value()) {
    /* Send Ping request */
    u8Tick = APP_BROKER_BINARY_RECV_TICK;
    RETURN_IFW_WDG(!m_SendSocketMessage("ping")
      , "Error on send ping request", );
  }
  if (ui->sbPingSent->value() != ui->sbPongRecv->value()) {
    /* Monitor Pong response */
    if (0 == --u8Tick) {
      u8Tick = APP_BROKER_BINARY_RECV_TICK;
      DEBUG_APP_WDG("WebSocket error", "Timeout on receive ping/pong response");
      CATCH_ABORT_WDG(!m_DbHistoryInsert({
          {"operation" , "SOCK ERRO"}
        , {"parameters", "Timeout on receive socket response"}})
        , "Unable to insert history record on database");
      // Reconnect socket
      CATCH_ABORT_WDG(!m_SocketOpen(), "Cannot reconnect socket");
    }
  }
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
  CATCH_ABORT_WDG(!m_DbCreateTables(), "Unable to create database tables");
  /* Create session on database */
  CATCH_ABORT_WDG(-1 == m_DbSessionInsert()
    , "Unable to create a valid session on database");
  /* Reload combo sessions */
  CATCH_ABORT_WDG(!m_ComboSessionLoad()
    , "Unable to load session list from database");
  /* Reload proposals */
  CATCH_ABORT_WDG(!m_DbProposalsRelaod()
    , "Unable to load propsals from database");
  /* Open sockect connection */
  CATCH_ABORT_WDG(!m_SocketOpen(), "Unable to open socket connection");
}

/* ==========================================================================
 *        FUNCTION NAME: m_statusUpdate
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181030
 *              AUTHORS: Fabrizio De Siati
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
  }
}

/* ==========================================================================
 *        FUNCTION NAME: m_LookApply
 * FUNCTION DESCRIPTION: apply look and theme
 *        CREATION DATE: 20181030
 *              AUTHORS: Fabrizio De Siati
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
      palette.setColor(QPalette::Disabled, QPalette::Button,QColor(40, 40, 40));
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
      palette.setColor(QPalette::Disabled, QPalette::Button,QColor(40, 40, 40));
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
      palette.setColor(QPalette::Disabled, QPalette::Button,QColor(40, 40, 40));
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
      palette.setColor(QPalette::Disabled, QPalette::Button,QColor(40, 40, 40));
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
 *        FUNCTION NAME: m_DbCreateTables
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_DbCreateTables()
{
  /* Create Table sessions */
  RETURN_IFC_WDG(!CAppDatabase::GetInstance().execQuery(
      "CREATE TABLE IF NOT EXISTS sessions ( \
          id integer PRIMARY KEY AUTOINCREMENT \
        , date_time text NOT NULL UNIQUE)")
    , "Unable to create sessions table"
    , false);
  /* Create Table history */
  RETURN_IFC_WDG(!CAppDatabase::GetInstance().execQuery(
      "CREATE TABLE IF NOT EXISTS history ( \
          id integer PRIMARY KEY AUTOINCREMENT \
        , session_id NOT NULL \
        , date_time text NOT NULL \
        , operation text NOT NULL \
        , balance text \
        , currency text \
        , parameters text \
        , details text \
        , FOREIGN KEY(session_id) REFERENCES sessions(id))")
    , "Unable to create history table"
    , false);
  /* Create Table proposals */
  RETURN_IFC_WDG(!CAppDatabase::GetInstance().execQuery(
      "CREATE TABLE IF NOT EXISTS proposals ( \
          id integer PRIMARY KEY AUTOINCREMENT \
        , session_id NOT NULL \
        , date_time text NOT NULL \
        , status_tbot text \
        , status text \
        , profit real \
        , profit_percentage text \
        , countdown int \
        , contract_type text NOT NULL \
        , symbolA text NOT NULL \
        , symbolB text NOT NULL \
        , amount text NOT NULL \
        , currency text NOT NULL \
        , date_start text \
        , date_expiry text \
        , proposal_id text \
        , contract_id text \
        , FOREIGN KEY(session_id) REFERENCES sessions(id))")
    , "Unable to create proposals table"
    , false);
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: m_DbHistoryRelaod
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_DbHistoryRelaod(bool bForceResize)
{
  m_modelHistory.setQuery(
    QString("SELECT * from history %1 ORDER BY date_time DESC")
      .arg(-1 == m_i64SessionIdSelected ? ""
  : QString("WHERE session_id=%1").arg(m_i64SessionIdSelected)));
  /* Hide columns first time */
  static bool bHideColumns = true;
  static const QList<bool> listHiddenCols = QList<bool>()
    << true  //id
    << true  //session_id
    << false //date_time
    << false //operation
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
  if (bResizeToContents && m_modelHistory.rowCount() > 0)
  { /* Reset column width based on size*/
    for(auto iCol=0; iCol < m_modelHistory.columnCount(); ++iCol)
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
 *        FUNCTION NAME: m_DbProposalsRelaod
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181031
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_DbProposalsRelaod(bool bForceResize)
{
  m_modelProposals.setQuery(
    QString("SELECT * from proposals ORDER BY date_time DESC"));
  /* Hide columns first time */
  static bool bHideColumns = true;
  static const QList<bool> listHiddenCols = QList<bool>()
    << true  //id
    << true  //session_id
    << false //date_time
    << false //status_tbot
    << false //status
    << false //profit
    << false //profit_percentage
    << false //countdown
    << false //contract_type
    << false //symbolA
    << false //symbolB
    << false //amount
    << false //currency
    << false //date_start
    << false //date_expiry
    << true  //proposal_id
    << true; //contract_id;
  if (bHideColumns)
  {
    int iCol = 0;
    for(auto bHidden: listHiddenCols) {
      ui->tbProposals->setColumnHidden(iCol++, bHidden);
    }
    bHideColumns = false;
  }
  /* Resize to contents */
  static bool bResizeToContents = true;
  if (bForceResize) {
    bResizeToContents = true;
  }
  if (bResizeToContents && m_modelProposals.rowCount() > 0)
  { /* Reset column width based on size*/
    for(auto iCol=0; iCol < m_modelProposals.columnCount(); ++iCol)
    { // Resize only shown clolumns
      if (!listHiddenCols.at(iCol)) {
        ui->tbProposals->resizeColumnToContents(iCol);
      }
    }
    bResizeToContents = false;
  }
  // Reload proposals
  RETURN_IFW(!m_ProposalResumeUpdate(), "Unable to update resume proposals"
    , false);
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: m_DbSessionInsert
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
int64_t CAppBrokerBinary::m_DbSessionInsert()
{
  QString strCurrentDateTime = CurrentDateTime();
  m_i64SessionId = CAppDatabase::GetInstance().execInsertQuery(QString(
    "INSERT INTO sessions (date_time) VALUES ('%1')").arg(strCurrentDateTime));
  RETURN_IFC_WDG(-1 == m_i64SessionId
    , "Unable to create a valid session id on database", m_i64SessionId);
  ui->leSession->setText(QString("%1: %2")
     .arg(m_i64SessionId, 3, 10, QChar('0'))
    .arg(strCurrentDateTime));
  return m_i64SessionId;
}

/* ==========================================================================
 *        FUNCTION NAME: m_DbHistoryInsert
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
int64_t CAppBrokerBinary::m_DbHistoryInsert(
  const QMap<QString,QString>& mapValues)
{
  //Alert if new message is inserted and application isn't at top level
  QApplication::alert(this,0);
  int64_t i64Id = CAppDatabase::GetInstance().execInsertQuery(QString(
      "INSERT INTO history (\
        session_id, date_time, operation, balance, currency, parameters\
        , details) \
      VALUES (%1,'%2','%3','%4','%5','%6','%7')")
      .arg(m_i64SessionId)
      .arg(CurrentDateTime())
      .arg(mapValues.value("operation"))
      .arg(ui->pbBalance->text())
      .arg(ui->leCurrency->text())
      .arg(mapValues.value("parameters"))
      .arg(mapValues.value("details")));
  RETURN_IFC_WDG(-1 == i64Id
    , "Unable to insert a history entry on database", i64Id);
  // Reload history if session is selected (or ALL)
  if (-1 == m_i64SessionIdSelected || m_i64SessionId == m_i64SessionIdSelected){
    RETURN_IFC_WDG(!m_DbHistoryRelaod(true)
      , "Unable to reload history from database", i64Id);
  }
  return i64Id;
}

/* ==========================================================================
 *        FUNCTION NAME: m_DbProposalInsert
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181031
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
int64_t CAppBrokerBinary::m_DbProposalInsert(
  const QMap<QString,QString>& mapValues)
{
  int64_t i64Id = CAppDatabase::GetInstance().execInsertQuery(QString(
      "INSERT INTO proposals (\
        session_id, date_time, status, contract_type, symbolA, symbolB, amount\
        , currency) \
      VALUES (%1,'%2','%3','%4','%5','%6','%7','%8')")
      .arg(m_i64SessionId)
      .arg(CurrentDateTime())
      .arg(mapValues.value("status"))
      .arg(mapValues.value("contract_type"))
      .arg(mapValues.value("symbolA"))
      .arg(mapValues.value("symbolB"))
      .arg(mapValues.value("amount"))
      .arg(mapValues.value("currency")));
  RETURN_IFC_WDG(-1 == i64Id
    , "Unable to insert a proposal entry on database", i64Id);
  // Reload proposals
  RETURN_IFC_WDG(!m_DbProposalsRelaod(true)
    , "Unable to reload proposals from database", i64Id);
  return i64Id;
}

/* ==========================================================================
 *        FUNCTION NAME: m_DbProposalUpdate
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181031
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_DbProposalUpdate(const QMap<QString,QString>& mapValues
  , const int64_t& i64Id)
{
  QString strSetValues;
  for (auto strFieldName: mapValues.keys()) {
    strSetValues.append(QString("%1%2 = '%3'")
      .arg(strSetValues.isEmpty() ? "" : ", ")
      .arg(strFieldName)
      .arg(mapValues.value(strFieldName)));
  }
  RETURN_IFW(!CAppDatabase::GetInstance().execQuery(QString(
      "UPDATE proposals SET %1 \
       WHERE id=%2")
      .arg(strSetValues)
      .arg(i64Id))
    , "Unable to update proppsals table", false);  
  return m_DbProposalsRelaod(true);

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
  RETURN_IFC_WDG(!qry.exec(strQuery)
    , QString("Unable to execute query [%1] E=%2")
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
 *        FUNCTION NAME: m_SocketOpen
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_SocketOpen()
{
  DEBUG_APP_WDG("WebSocket server", m_url.toString());
  m_webSocket.close();
  m_bFirstAuthorized = true;
  m_bSocketOpened = false;
  ui->sbPingSent->setValue(0);
  ui->sbPongRecv->setValue(0);
  CATCH_ABORT_WDG(!m_DbHistoryInsert({
      {"operation" , "SOCK OPEN"}
    , {"parameters", m_url.toString()}})
    , "Unable to insert history record on database");
  static bool bFirstTime = true;
  if (bFirstTime) {
    /* Connect signals and slots for socket */
    connect(&m_webSocket, SIGNAL(error(QAbstractSocket::SocketError))
      , SLOT(slotOnSocketError(QAbstractSocket::SocketError)));
    connect(&m_webSocket, SIGNAL(connected()),   SLOT(slotOnSocketConnected()));
    connect(&m_webSocket, SIGNAL(disconnected()),SLOT(slotOnSocketConnected()));
    /* Start bot */
    CATCH_ABORT_WDG(!m_BotStart(), "Unable to start Telegram Bot");
    bFirstTime = false;
  }
  m_webSocket.open(QUrl(m_url));
  m_bSocketOpened = true;
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
  DEBUG_APP_WDG("Starting Telegram Bot ...", "");
  /* Instatiate Telegram Bot */
  delete m_pAppTelegramBot;
  m_pAppTelegramBot = new(std::nothrow) CAppTelegramBot(m_strTokenBot
    , true, 500, 4);
  connect(m_pAppTelegramBot,  &Telegram::Bot::message
    , this, &CAppBrokerBinary::slotOnMessageTelegramBot);
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: m_BalanceUpdate
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181031
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppBrokerBinary::m_BalanceUpdate(const QString& strNewValue)
{
  QString strOldValue = ui->pbBalance->text();
  ui->pbBalance->setText(strNewValue);
  RETURN_IF(strOldValue.isEmpty(), );
  RETURN_IF(strNewValue.isEmpty(), );
  double f64OldValue = strOldValue.toDouble();
  double f64NewValue = strNewValue.toDouble();  
  if (f64OldValue != f64NewValue)
  { /* Change icon for increase/decrease balance */
    ui->pbBalance->setIcon(f64OldValue < f64NewValue
      ? QIcon{ QPixmap{":/icons/resources/arrow_up_green.png"} }
      : QIcon{ QPixmap{":/icons/resources/arrow_down_red.png"} });
  }
  RETURN_IF(m_strBalanceStart.isEmpty(), );
  double f64BalanceStart = m_strBalanceStart.toDouble();
  if (f64BalanceStart != f64NewValue)
  { /* Change color for increase/decrease balance from start */
    ui->pbBalance->setStyleSheet(f64BalanceStart < f64NewValue
      ? "QPushButton { color : lightgreen; }"
      : "QPushButton { color : red; }");
  }
}

/* ==========================================================================
 *        FUNCTION NAME: m_RcvTelegramMessage
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181030
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_RcvTelegramMessage(const QString& strMsgTot)
{
  QString strMsg = strMsgTot.split("\n").last().toUpper().replace("  "," ");
  DEBUG_APP_WDG("Telegram message received", strMsg);
  RETURN_IFC_WDG(!m_DbHistoryInsert({
      {"operation" , "TBOT RECV"}
    , {"parameters", strMsg}
    , {"details"   , strMsg} })
    , "Unable to insert history record on database", false);
  //GBP USD CALL 5 MIN WAIT CONFIRM
  //USD CHF PUT 5 MIN WAIT CONFIRM
  //GO
  //NO
  //WIN OPTION
  //LOST OPTION
  if      (strMsg.endsWith("WAIT CONFIRM")) {
    QStringList list = strMsg.split(" ");
    // Early return without CATCH_ABORT (return true)
    RETURN_IFW_WDG(list.count() < 7, "WAIT CONFIRM request malformed", true);
    QString strFrxA = list.at(0);
    QString strFrxB = list.at(1);
    QString strContractType = list.at(2);
    QString strDuration = list.at(3);
    QString strDurationUnit = list.at(4);
    static const QMap<QString,QString> mapDurationUnit = {
      {"SEC", "s"}, {"MIN", "m"}
    };
    // proposal
    RETURN_IFW_WDG(!m_SendSocketMessage("proposal"
        , { 
              {"amount"       , QString::number(ui->dsbProposal->value())} 
            , {"contract_type", strContractType}
            , {"currency"     , ui->leCurrency->text()}
            , {"duration"     , strDuration}
            , {"duration_unit", mapDurationUnit.value(strDurationUnit, "m")}
            , {"symbolA"      , strFrxA}
            , {"symbolB"      , strFrxB}
            , {"symbol"       , QString("frx%1%2").arg(strFrxA).arg(strFrxB)}
          })
      , "Error on send proposal request", false);    
  }
  else if (strMsg == "GO") {
    // Early return without CATCH_ABORT (return true)
    RETURN_IFW_WDG(Status::kWAITFORCON != m_status
      , "Cannot place proposal in this status", true);
    RETURN_IFW_WDG(-1 == m_i64LastIdProposal
      , "Cannot retrieve last inserted proposal", false);
    RETURN_IFW_WDG(!m_mapProposalId2Info.contains(m_i64LastIdProposal)
      , QString("Id Proposal %1 not present on database")
        .arg(QString::number(m_i64LastIdProposal))
      , false);
    /* retrieve info for mapped proposal*/
    QString strContractType = m_mapProposalId2Info.value(m_i64LastIdProposal)
      .strContractType;
    QString strProposalId = m_mapProposalId2Info.value(m_i64LastIdProposal)
      .strProposalId;
    QString strPrice = m_mapProposalId2Info.value(m_i64LastIdProposal)
      .strPrice;
    /* Update proposal on database */
    RETURN_IFW(!m_DbProposalUpdate({{"status", "GO"}}, m_i64LastIdProposal)
      , "Unable to update proposal on database", false);
    /* reset last proposal: all next queries pass by map */
    m_i64LastIdProposal = -1;
    m_StatusUpdate(Status::kAUTHORIZED);
    // buy
    RETURN_IFW_WDG(!m_SendSocketMessage("buy"
        , { 
              {"buy"  , strProposalId} 
            , {"price", strPrice}
          })
      , "Error on send proposal request", false);    
  }
  else if (strMsg == "NO") {
    // Early return without CATCH_ABORT (return true)
    RETURN_IFW_WDG(Status::kWAITFORCON != m_status
      , "Cannot place proposal in this status", true);
    RETURN_IFW_WDG(-1 == m_i64LastIdProposal
      , "Cannot retrieve last inserted proposal", false);
    /* Update proposal on database */
    RETURN_IFW(!m_DbProposalUpdate({{"status", "NO"}}, m_i64LastIdProposal)
      , "Unable to update proposal on database", false);
    /* reset last proposal: all next queries pass by map */
    m_i64LastIdProposal = -1;
    m_StatusUpdate(Status::kAUTHORIZED);
  }
  else if (strMsg == "WIN OPTION" || strMsg == "LOST OPTION") {
    // Update status_tbot on database
#if APP_DEBUG == 1
    {
      QString strDebug;
      for(auto id: m_listProposalsOpenOrExpired) {
        strDebug.append(QString("%1 ").arg(QString::number(id)));
      }
      DEBUG_APP("m_listProposalsOpenOrExpired", strDebug);
    }
#endif
    RETURN_IF(m_listProposalsOpenOrExpired.empty(), true);
    int64_t i64IdProposal = m_listProposalsOpenOrExpired.at(0);
    m_listProposalsOpenOrExpired.removeFirst();
    /* Update proposal on database */
    RETURN_IFW_WDG(!m_DbProposalUpdate({{"status_tbot", strMsg}}, i64IdProposal)
      , "Unable to update proposal on database", false);
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
  bool bInsertHistory = true;
  // Prepare Send in JSon format
  if      ("ping" == strMsgType)
  { /* ping */
    bInsertHistory = false;
    ui->sbPingSent->setValue(ui->sbPingSent->value() + 1);
    strMsg = QStringLiteral(
      "{              \
        \"ping\": 1,  \
        \"req_id\": %1\
       }")
      .arg(ui->sbPingSent->value());
  }
  else if      ("authorize" == strMsgType)
  { /* authorize */
    ui->sbReqIdSent->setValue(ui->sbReqIdSent->value() + 1);
    strMsg = QStringLiteral(
      "{                      \
        \"authorize\": \"%1\",\
        \"req_id\": %2        \
       }")
      .arg(mapValues.value("authorize"))
      .arg(ui->sbReqIdSent->value());
  }
  else if ("balance" == strMsgType)
  { /* balance */
    ui->sbReqIdSent->setValue(ui->sbReqIdSent->value() + 1);
    strMsg = QStringLiteral(
      "{                 \
        \"balance\": 1,  \
        \"subscribe\": 1,\
        \"req_id\": %1   \
       }")
      .arg(ui->sbReqIdSent->value());
  }
  else if ("proposal" == strMsgType)
  { /* proposal */
    ui->sbReqIdSent->setValue(ui->sbReqIdSent->value() + 1);
    strMsg = QStringLiteral(
      "{                          \
        \"proposal\": 1,          \
        \"amount\": \"%1\",       \
        \"basis\": \"stake\",     \
        \"contract_type\":\"%2\", \
        \"currency\": \"%3\",     \
        \"duration\": \"%4\",     \
        \"duration_unit\": \"%5\",\
        \"symbol\": \"%6\",       \
        \"req_id\": %7            \
       }")
      .arg(mapValues.value("amount"))
      .arg(mapValues.value("contract_type"))
      .arg(mapValues.value("currency"))
      .arg(mapValues.value("duration"))
      .arg(mapValues.value("duration_unit"))
      .arg(mapValues.value("symbol"))
      .arg(ui->sbReqIdSent->value());
    /* Insert proposal on database */
    QMap<QString, QString> mapValuesDbInsert = mapValues;
    mapValuesDbInsert.insert("status", "proposal send");
    m_i64LastIdProposal = m_DbProposalInsert(mapValuesDbInsert);
    RETURN_IFW(-1 == m_i64LastIdProposal
      , "Unable to insert proposal on database", false);
    // store contract_type and price to send buy fro CALL or PUT request
    sProposalInfo info;
    info.strProposalId = "";
    info.strContractId = "";
    info.strContractType = mapValues.value("contract_type");
    info.strPrice = mapValues.value("amount");
    info.strStatus = "";
    info.i64CountDown = 0;
    m_mapProposalId2Info.insert(m_i64LastIdProposal, info);
  }
  else if ("buy" == strMsgType)
  { /* buy */
    ui->sbReqIdSent->setValue(ui->sbReqIdSent->value() + 1);
    strMsg = QStringLiteral(
      "{                          \
        \"buy\": \"%1\",          \
        \"price\": %2,            \
        \"req_id\": %3            \
       }")
      .arg(mapValues.value(strMsgType))
      .arg(mapValues.value("price"))
      .arg(ui->sbReqIdSent->value());
    /* Update proposal on database */
    RETURN_IFW(!m_DbProposalUpdate(
          {{"status", QString("%1 send").arg(strMsgType)}}
        , m_i64LastIdProposal)
      , "Unable to update proposal on database", false);
  }
  else if ("proposal_open_contract" == strMsgType)
  { /* proposal_open_contract */
    ui->sbReqIdSent->setValue(ui->sbReqIdSent->value() + 1);
    strMsg = QStringLiteral(
      "{                               \
        \"proposal_open_contract\": 1, \
        \"contract_id\": %1,           \
        \"subscribe\": 1,              \
        \"req_id\": %2                 \
       }")
      .arg(mapValues.value("contract_id"))
      .arg(ui->sbReqIdSent->value());
  }
  else {
    RETURN_IFC_WDG(true, QString("Message type %1 is unrecognized").arg(strMsgType)
      , false);    
  }
  // Send
  if (!strMsg.isEmpty()) {
    DEBUG_APP_WDG(QString("Send message socket %1").arg(strMsgType), strMsg);
    QString strParameters = QString("%1: ").arg(strMsgType);
    for(auto str: mapValues.keys()) {
      strParameters.append(QString("%1=%2, ")
        .arg(str).arg(mapValues.value(str)));
    }
    RETURN_IFC_WDG(bInsertHistory && !m_DbHistoryInsert({
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
  DEBUG_APP_WDG("Recv message socket", strMsg);
  // Parsing JSON
  QJsonDocument doc = QJsonDocument::fromJson(strMsg.toUtf8());
  RETURN_IFC_WDG(doc.isNull(), "JsonDocument is null", false);
  RETURN_IFC_WDG(!doc.isObject(), "JsonObject is null", false);
  QJsonObject msg = doc.object();
  QJsonObject msg__error;
  bool bInsertHistory = true;
  /* Update req_id for repsonses */
  int64_t i64ReqIdRecv;
  /* Checks if message is error */
  if (m_JSonObject(msg, "error", msg__error)) {    
    // there is error on response
    QString msg__error__code;
    RETURN_IFW_WDG(!m_JSonValueStr(msg__error, "code"   , msg__error__code)
      , "JSonValue 'code' doesn't exist", false);
    QString msg__error__message;
    RETURN_IFW_WDG(!m_JSonValueStr(msg__error, "message", msg__error__message)
      , "JSonValue 'message' doesn't exist", false);
    WARNING_APP_WDG("Binary return error", QString("code [%1] error[%2]")
      .arg(msg__error__code).arg(msg__error__message));
    strMsgType = "error";
    mapValues.insert("code", msg__error__code);
    mapValues.insert("message", msg__error__message);
  }
  else
  { /* Decode response based on msg_type */
    RETURN_IFW_WDG(!m_JSonValueStr(msg, "msg_type", strMsgType)
      , "JSonValue 'msg_type' doesn't exist", false);
    if      ("ping" == strMsgType) 
    { /* Pong response */
      bInsertHistory = false;
      if (m_JSonValueLong(msg, "req_id", i64ReqIdRecv)) {
        ui->sbPongRecv->setValue(i64ReqIdRecv);
      }
      QString msg__ping;
      RETURN_IFW_WDG(!m_JSonValueStr(msg, "ping"
        , msg__ping)
        , "JSonValue 'ping' doesn't exist", false);
      INFO_APP_WDG("ping:", msg__ping);
      mapValues.insert("ping", msg__ping);
    }
    else
    {
      if (m_JSonValueLong(msg, "req_id", i64ReqIdRecv)) {
        ui->sbReqIdRecv->setValue(i64ReqIdRecv);
      }
      if     ("authorize" == strMsgType) 
      { /* First message to authorize application */
        QJsonObject msg__authorize;
        RETURN_IFW_WDG(!m_JSonObject(msg, "authorize", msg__authorize)
          , "JSonObject 'authorize' doesn't exist", false);
        QString msg__authorize__balance;
        RETURN_IFW_WDG(!m_JSonValueStr(msg__authorize, "balance"
          , msg__authorize__balance)
          , "JSonValue 'balance' doesn't exist", false);
        QString msg__authorize__currency;
        RETURN_IFW_WDG(!m_JSonValueStr(msg__authorize, "currency"
          , msg__authorize__currency)
          , "JSonValue 'currency' doesn't exist", false);
        INFO_APP_WDG("balance:", msg__authorize__currency);
        // Save start balance
        if (m_strBalanceStart.isEmpty()) {
          m_strBalanceStart = msg__authorize__balance;
        }
        m_BalanceUpdate(msg__authorize__balance);
        ui->leCurrency->setText(msg__authorize__currency);
        mapValues.insert("balance", msg__authorize__balance);
        mapValues.insert("currency", msg__authorize__currency);
        /* Update Status: AUTHORIZED (only first time) */
        if (m_bFirstAuthorized) {
          m_StatusUpdate(Status::kAUTHORIZED);
          m_bFirstAuthorized = false;
          RETURN_IFW_WDG(!m_SendSocketMessage("balance")
            , "Error on send balance request", false);
        }
      }
      else if ("balance" == strMsgType) 
      { /* Monitoring balance changes */
        QJsonObject msg__balance;
        RETURN_IFW_WDG(!m_JSonObject(msg, "balance", msg__balance)
          , "JSonObject 'balance' doesn't exist", false);
        QString msg__balance__balance;
        RETURN_IFW_WDG(!m_JSonValueStr(msg__balance, "balance"
          , msg__balance__balance)
          , "JSonValue 'balance' doesn't exist", false);
        INFO_APP_WDG("balance:", msg__balance__balance);
        m_BalanceUpdate(msg__balance__balance);
        mapValues.insert("balance", msg__balance__balance);
      }
      else if ("proposal" == strMsgType) 
      { /* Proposal wait feedback */
        QJsonObject msg__proposal;
        RETURN_IFW_WDG(!m_JSonObject(msg, "proposal", msg__proposal)
          , "JSonObject 'proposal' doesn't exist", false);
        QString msg__proposal__id;
        RETURN_IFW_WDG(!m_JSonValueStr(msg__proposal, "id"
          , msg__proposal__id)
          , "JSonValue 'id' doesn't exist", false);
        INFO_APP_WDG("id proposal:", msg__proposal__id);
        mapValues.insert("id", msg__proposal__id);
        /* update map Proposal with ProposalId */
        RETURN_IFW_WDG(-1 == m_i64LastIdProposal
          , "Cannot retrieve last inserted proposal", false);
        RETURN_IFW_WDG(!m_mapProposalId2Info.contains(m_i64LastIdProposal)
          , QString("Id Proposal %1 not present on database")
            .arg(QString::number(m_i64LastIdProposal))
          , false);
        sProposalInfo info = m_mapProposalId2Info.value(m_i64LastIdProposal);
        info.strProposalId = msg__proposal__id;
        m_mapProposalId2Info.insert(m_i64LastIdProposal, info);
        /* Update proposal on database */
        RETURN_IFW(!m_DbProposalUpdate(
              {{"status", "proposal recv"},{"proposal_id",msg__proposal__id}}
            , m_i64LastIdProposal)
          , "Unable to update proposal on database", false);
        /* Update Status: WAIT FOR CONFIRM */
        m_StatusUpdate(Status::kWAITFORCON);
      }
      else if ("buy" == strMsgType)
      { /* Buy wait feedback */
        QJsonObject msg__buy;
        RETURN_IFW_WDG(!m_JSonObject(msg, strMsgType, msg__buy)
          , "JSonObject 'buy' doesn't exist", false);
        QString msg__buy__contract_id;
        RETURN_IFW_WDG(!m_JSonValueStr(msg__buy, "contract_id"
          , msg__buy__contract_id)
          , "JSonValue 'balance_after' doesn't exist", false);
        QJsonObject msg__echo_req;
        RETURN_IFW_WDG(!m_JSonObject(msg, "echo_req", msg__echo_req)
          , "JSonObject 'echo_req' doesn't exist", false);
        QString msg__echo_req__buy;
        RETURN_IFW_WDG(!m_JSonValueStr(msg__echo_req, strMsgType
          , msg__echo_req__buy)
          , "JSonValue 'buy' doesn't exist", false);
        INFO_APP_WDG("contract_id:", msg__buy__contract_id);
        mapValues.insert("contract_id", msg__buy__contract_id);
        /* update map Proposal with ContractId */
        sProposalInfo info;
        int i64Id = m_i64IdProposalByInfo("proposal_id"
          , msg__echo_req__buy, info);
        RETURN_IFW_WDG(-1 == i64Id
          , QString("Cannot retrieve from Proposal Info a proposal_id = %1")
            .arg(msg__echo_req__buy)
          , false);
        info.strContractId = msg__buy__contract_id;
        m_mapProposalId2Info.insert(i64Id, info);
        /* Update proposal on database */
        RETURN_IFW(!m_DbProposalUpdate(
              {{"status"     , QString("%1 recv").arg(strMsgType)},
               {"contract_id", msg__buy__contract_id       }}
            , i64Id)
          , "Unable to update proposal on database", false);
        /* Send proposal_open_contract */
        RETURN_IFW_WDG(!m_SendSocketMessage("proposal_open_contract"
          , {{"contract_id",msg__buy__contract_id}})
          , "Error on send proposal_open_contract request", false);
        /* Update Status: AUTHORIZED */
        m_StatusUpdate(Status::kAUTHORIZED);
      }
      else if ("proposal_open_contract" == strMsgType) 
      { /* Proposal open contract response */
        QJsonObject msg__proposal_open_contract;
        RETURN_IFW_WDG(!m_JSonObject(msg, "proposal_open_contract"
          , msg__proposal_open_contract)
          , "JSonObject 'proposal_open_contract' doesn't exist", false);
        QString msg__proposal_open_contract__contract_id;
        RETURN_IFW_WDG(!m_JSonValueStrOrLong(msg__proposal_open_contract
          , "contract_id"
          , msg__proposal_open_contract__contract_id)
          , "JSonValue 'contract_id' doesn't exist", false);
        double msg__proposal_open_contract__is_expired;
        RETURN_IFW_WDG(!m_JSonValueDouble(msg__proposal_open_contract
          , "is_expired"
          , msg__proposal_open_contract__is_expired)
          , "JSonValue 'is_expired' doesn't exist", false);
        QString msg__proposal_open_contract__status;
        RETURN_IFW_WDG(!m_JSonValueStr(msg__proposal_open_contract
          , "status"
          , msg__proposal_open_contract__status)
          , "JSonValue 'status' doesn't exist", false);
        double msg__proposal_open_contract__profit;
        RETURN_IFW_WDG(!m_JSonValueDoubleOrStr(msg__proposal_open_contract
          , "profit"
          , msg__proposal_open_contract__profit)
          , "JSonValue 'profit' doesn't exist", false);
        QString msg__proposal_open_contract__profit_percentage;
        RETURN_IFW_WDG(!m_JSonValueStr(msg__proposal_open_contract
          , "profit_percentage"
          , msg__proposal_open_contract__profit_percentage)
          , "JSonValue 'profit_percentage' doesn't exist", false);
        int64_t msg__proposal_open_contract__date_start;
        RETURN_IFW_WDG(!m_JSonValueLong(msg__proposal_open_contract
          , "date_start"
          , msg__proposal_open_contract__date_start)
          , "JSonValue 'date_start' doesn't exist", false);
        QDateTime dtStart = QDateTime::fromSecsSinceEpoch(
          msg__proposal_open_contract__date_start);
        QString strDateStart = dtStart.toString("yyyy-MM-dd hh:mm:ss");
        int64_t msg__proposal_open_contract__date_expiry;
        RETURN_IFW_WDG(!m_JSonValueLong(msg__proposal_open_contract
          , "date_expiry"
          , msg__proposal_open_contract__date_expiry)
          , "JSonValue 'date_expiry' doesn't exist", false);
        QDateTime dtExpiry = QDateTime::fromSecsSinceEpoch(
          msg__proposal_open_contract__date_expiry);
        QString strDateExpiry = dtExpiry.toString("yyyy-MM-dd hh:mm:ss");
        int64_t msg__proposal_open_contract__current_spot_time = 0;
        RETURN_IFW_WDG(!m_JSonValueLong(msg__proposal_open_contract
          , "current_spot_time"
          , msg__proposal_open_contract__current_spot_time)
          , "JSonValue 'current_spot_time' doesn't exist", false);
        int64_t i64CountDown = msg__proposal_open_contract__date_expiry 
          - msg__proposal_open_contract__current_spot_time;
        mapValues.insert("contract_id"
          , msg__proposal_open_contract__contract_id);
        mapValues.insert("is_expired" 
          , QString::number(msg__proposal_open_contract__is_expired));
        mapValues.insert("status"     , msg__proposal_open_contract__status);
        mapValues.insert("profit"     
          , QString::number(msg__proposal_open_contract__profit));
        mapValues.insert("profit_percentage"
          , msg__proposal_open_contract__profit_percentage);
        mapValues.insert("countdown", QString::number(i64CountDown));      
        INFO_APP_WDG("status proposal:", msg__proposal_open_contract__status);
        sProposalInfo info;
        int i64Id = m_i64IdProposalByInfo("contract_id"
          , msg__proposal_open_contract__contract_id, info);
        RETURN_IFW_WDG(-1 == i64Id
          , QString("Cannot retrieve from Proposal Info a contract_id = %1")
            .arg(msg__proposal_open_contract__contract_id)
          , false);
        if (!m_listProposalsOpenOrExpired.contains(i64Id)) {
          m_listProposalsOpenOrExpired.append(i64Id);
        }
        bInsertHistory = msg__proposal_open_contract__status != info.strStatus;
        info.strStatus = msg__proposal_open_contract__status;
        info.i64CountDown = i64CountDown;
        m_mapProposalId2Info.insert(i64Id, info);        
        /* Update proposal on database */
        RETURN_IFW(!m_DbProposalUpdate({
                {"status", msg__proposal_open_contract__status}
              , {"profit", QString::number(msg__proposal_open_contract__profit)}
              , {"profit_percentage"
                , msg__proposal_open_contract__profit_percentage}
              , {"countdown", QString::number(i64CountDown)}
              , {"date_start", strDateStart}
              , {"date_expiry", strDateExpiry}}
            , i64Id)
          , "Unable to update proposal on database", false);
      }
    }
  }
  QString strParameters = QString("%1: ").arg(strMsgType);
  for(auto str: mapValues.keys()) {
    strParameters.append(QString("%1=%2, ")
      .arg(str).arg(mapValues.value(str)));
  }
  RETURN_IFW_WDG(bInsertHistory && !m_DbHistoryInsert({
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
 *        FUNCTION NAME: m_JSonValueStr
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

/* ==========================================================================
 *        FUNCTION NAME: m_JSonValueDouble
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181023
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_JSonValueDouble(const QJsonObject& qJsonObjectParent
  , const QString& strName, double& f64ValueRet)
{
  RETURN_IF(qJsonObjectParent.isEmpty(), false);
  RETURN_IF(!qJsonObjectParent.contains(strName), false);
  QJsonValue qJsonValue = qJsonObjectParent.value(strName);
  RETURN_IF(!qJsonValue.isDouble(), false);
  f64ValueRet = qJsonValue.toDouble();
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: m_JSonValueLong
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181102
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_JSonValueLong(const QJsonObject& qJsonObjectParent
  , const QString& strName, int64_t& i64ValueRet)
{
  RETURN_IF(qJsonObjectParent.isEmpty(), false);
  RETURN_IF(!qJsonObjectParent.contains(strName), false);
  QJsonValue qJsonValue = qJsonObjectParent.value(strName);
  RETURN_IF(!qJsonValue.isDouble(), false);
  i64ValueRet = static_cast<int64_t>(qJsonValue.toDouble());
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: m_JSonValueStrOrLong
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181102
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_JSonValueStrOrLong(const QJsonObject& qJsonObjectParent
  , const QString& strName, QString& strValueRet)
{
  RETURN_IF(qJsonObjectParent.isEmpty(), false);
  RETURN_IF(!qJsonObjectParent.contains(strName), false);
  QJsonValue qJsonValue = qJsonObjectParent.value(strName);
  if (qJsonValue.isString()) {
    strValueRet = qJsonValue.toString();
  }
  else if (qJsonValue.isDouble()) {
    strValueRet = QString::number(static_cast<int64_t>(qJsonValue.toDouble()));
  }
  else {
    RETURN_IF(true, false);
  }
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: m_JSonValueDoubleOrStr
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181102
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_JSonValueDoubleOrStr(
    const QJsonObject& qJsonObjectParent, const QString& strName
  , double& f64ValueRet)
{
  RETURN_IF(qJsonObjectParent.isEmpty(), false);
  RETURN_IF(!qJsonObjectParent.contains(strName), false);
  QJsonValue qJsonValue = qJsonObjectParent.value(strName);
  if (qJsonValue.isDouble()) {
    f64ValueRet = qJsonValue.toDouble();
  }
  else if (qJsonValue.isString()) {
    f64ValueRet = qJsonValue.toString().toDouble();
  }
  else {
    RETURN_IF(true, false);
  }
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: m_i64IdProposalByInfo
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181023
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
int64_t CAppBrokerBinary::m_i64IdProposalByInfo(const QString& strInfoName
  , const QString& strInfoValue, sProposalInfo& infoRet)
{
  int64_t i64IdProposal = -1;
  for(auto id: m_mapProposalId2Info.keys())
  {
    auto info = m_mapProposalId2Info.value(id);
    if      (("proposal_id" == strInfoName       ) && 
             (strInfoValue  == info.strProposalId))
    {
      i64IdProposal = id; infoRet = info;  break;
    }
    else if (("contract_id" == strInfoName       ) && 
             (strInfoValue  == info.strContractId))
    {
      i64IdProposal = id; infoRet = info;  break;
    }
  }
  return i64IdProposal;
}

/* ==========================================================================
 *        FUNCTION NAME: m_ProposalResumeUpdate
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181102
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppBrokerBinary::m_ProposalResumeUpdate()
{
  /* Select proposal resume */
  static const QMap<QString,QString> mapRole2Query = {
      {"TOT" ,"SELECT count(*) as TOT  FROM Proposals"}
    , {"SOLD","SELECT count(*) as SOLD FROM Proposals WHERE status IN \
       ('open','sold')"}
    , {"LOST","SELECT count(*) as LOST FROM Proposals WHERE status IN ('lost')"}
    , {"WON" ,"SELECT count(*) as WON  FROM Proposals WHERE status IN ('won')" }
    , {"PROF","SELECT sum(profit) as PROF FROM Proposals"                      }    
  };
  for (auto strRole: mapRole2Query.keys())
  {
    QSqlQuery qry;
    QString strQuery = mapRole2Query.value(strRole);
    RETURN_IFW_WDG(!qry.exec(strQuery)
      , QString("Unable to execute query [%1] E=%2")
        .arg(strQuery).arg(qry.lastError().text()), false);
    RETURN_IFW_WDG(!qry.next()
      , QString("Unable to retrieve query results for %1").arg(strRole)
      , false);
    QVariant qV = qry.value(0);
    if      ("TOT" == strRole) {
      ui->leProposalsTotal->setText(QString("TOT: %1").arg(qV.toInt()));
    }
    else if ("SOLD" == strRole) {
      ui->leProposalsOpenSold->setText(QString("OPEN/SOLD: %1")
        .arg(qV.toInt()));
    }
    else if ("LOST" == strRole) {
      ui->leProposalsLost->setText(QString("LOST: %1").arg(qV.toInt()));
    }
    else if ("WON" == strRole) {
      ui->leProposalsWon->setText(QString("WON: %1").arg(qV.toInt()));
    }
    else if ("PROF" == strRole) {
      double f64Profit = qV.toDouble();
      ui->leProposalsProfit->setText(QString::number(f64Profit));
      ui->leProposalsProfit->setStyleSheet(f64Profit > 0.0
        ? "QLabel { color : lightgreen; }" : "QLabel { color : red; }");
    }
    else {
      RETURN_IFW_WDG(true
        , QString("Unable to found query for %1").arg(strRole)
        , false);
    }
  }
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: m_DetailsUpdate
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181102
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppBrokerBinary::m_DetailsUpdate(const QSqlQueryModel& model, int row)
{
  QString strDetails;
  QSqlRecord record = model.record(row);
  for(auto i=0; i < record.count(); ++i) {
    strDetails.append(QString("%1 = %2\n")
      .arg(record.fieldName(i)).arg(record.value(i).toString()));
  }
  ui->textDetails->setText(strDetails);
}
