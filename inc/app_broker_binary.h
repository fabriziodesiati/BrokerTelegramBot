/* ==========================================================================
 *   INCLUDE FILE NAME: app_broker_binary.h
 * INCLUDE DESCRIPTION: declares the CAppBrokerBinary class.
 *       CREATION DATE: 20181019
 *             AUTHORS: Fabrizio De Siati
 *        DESIGN ISSUE: None. 
 *
 *             HISTORY: See table below.
 * 
 * 19-Oct-2018 | Fabrizio De Siati | 0.0 |
 * Initial creation of this file.
 * 
 * ========================================================================== */

#ifndef APP_BROKER_BINARY_H
#define APP_BROKER_BINARY_H

/* ==========================================================================
 * INCLUDE: Basic include file.
 * ========================================================================== */
#include "app_telegram_bot.h"
#include "app_model.h"
#include <QMainWindow>
#include <QtWebSockets/QWebSocket>
#include <QJsonDocument>

/* ==========================================================================
 * MACROS
 * ========================================================================== */

/* ==========================================================================
 * FORWARD DECLARATIONS
 * ========================================================================== */
namespace Ui {
  class CWdgMain;
  class CWdgCentral;
}

class CWdgCentral : public QWidget
{
  Q_OBJECT

public:
  explicit CWdgCentral(QWidget *parent = 0);
  ~CWdgCentral();

  Ui::CWdgCentral* getUi() { return ui; }    

private:
  Ui::CWdgCentral *ui;
};

/* ==========================================================================
 * CLASS DECLARATION
 * ========================================================================== */
class CAppBrokerBinary : public QMainWindow
{
  Q_OBJECT
public:
  static QString CurrentDateTime();

  /**
   * CAppBrokerBinary constructor
   * @param app_id
   * @param token
   * @param tokenBot
   * @param url
   * @param parent
   */
  explicit CAppBrokerBinary(const QString& app_id, const QString& token
    , const QString& tokenBot, QMainWindow *parent = 0);
  virtual ~CAppBrokerBinary();

signals:
  void closed();

public slots:
  void slotOnSocketConnected();
  void slotOnMessageSocketReceived(QString message);
  void slotOnMessageTelegramBot(Telegram::Message);
  void slotOnComboSessionsCurrentTextChanged(const QString&);

protected slots:
  void slotOnDbConnected();

private:
  /* Pointer to user interface */
  Ui::CWdgMain *uiMain;
  Ui::CWdgCentral *ui;

  CAppTelegramBot* m_pAppTelegramBot;

  CWdgCentral m_wdgCentral;
  CAppModel m_model;
  
  QString m_strAppId;
  QString m_strToken;
  QString m_strTokenBot;
  QUrl m_url;

  int64_t m_i64SessionId;
  int64_t m_i64SessionIdSelected;
  QWebSocket m_webSocket;

  bool m_DbCreateTable();
  int64_t m_SessionCreate();
  bool m_ComboSessionLoad();
  bool m_HistoryRelaod(bool = false);
  bool m_OpenSocket();
  bool m_HistoryInsert(const QMap<QString,QString>&);
  bool m_BotStart();
  QString m_SendSocketMessage(const QString&, const QMap<QString,QString>&);
  void m_RecvSocketMessage(const QString&, QString&, QMap<QString, QString>&);
  bool m_JSonObject(const QJsonObject&, const QString&, QJsonObject&);
  bool m_JSonValueStr(const QJsonObject&, const QString&, QString&);
};

#endif // APP_BROKER_BINARY_H
