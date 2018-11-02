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
#include "app_model_history.h"
#include "app_model_proposals.h"
#include <QMainWindow>
#include <QtWebSockets/QWebSocket>
#include <QJsonDocument>
#include <QItemSelection>

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
  /* Status */
  enum class Status
  {
      kINITIALIZE
    , kAUTHORIZED
    , kWAITFORCON
  };

  struct sProposalInfo {
    QString strProposalId;
    QString strContractId;
    QString strContractType;
    QString strPrice;
    QString strStatus;
  };

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
  void slotOnSocketDisconnected();
  void slotOnMessageSocketReceived(const QString&);
  void slotOnMessageTelegramBot(Telegram::Message);
  void slotOnComboSessionsCurrentTextChanged(const QString&);
  void slotOnLookApply(const QString&);
  void slotOnBalanceClicked();
  void slotOnItemSelectedHistory(const QItemSelection&, const QItemSelection&);
  void slotOnItemSelectedProposal(const QItemSelection&, const QItemSelection&);

protected slots:
  void slotOnDbConnected();

private:
  /* Pointer to user interface */
  Ui::CWdgMain *uiMain;
  Ui::CWdgCentral *ui;

  CAppTelegramBot* m_pAppTelegramBot;
  QWebSocket m_webSocket;

  CWdgCentral m_wdgCentral;
  CAppModelHistory m_modelHistory;
  CAppModelProposals m_modelProposals;
  
  QUrl m_url;

  Status m_status;

  QString m_strAppId;
  QString m_strToken;
  QString m_strTokenBot;  

  bool m_bFirstAuthorized;
  QString m_strBalanceStart;
  int64_t m_i64SessionId;
  int64_t m_i64SessionIdSelected;
  
  int64_t m_i64LastIdProposal;
  QMap<int64_t,sProposalInfo> m_mapProposalId2Info;

  void m_StatusUpdate(Status);
  void m_LookApply(const QString&);
  bool m_DbCreateTables();
  bool m_DbHistoryRelaod(bool = false);
  bool m_DbProposalsRelaod(bool = false);
  int64_t m_DbSessionInsert();
  int64_t m_DbHistoryInsert(const QMap<QString,QString>&);
  int64_t m_DbProposalInsert(const QMap<QString,QString>&);
  bool m_DbProposalUpdate(const QMap<QString,QString>&, const int64_t&);
  bool m_ComboSessionLoad();
  bool m_SocketOpen();
  bool m_BotStart();
  void m_BalanceUpdate(const QString&);
  bool m_RcvTelegramMessage(const QString&);
  bool m_SendSocketMessage(const QString&
    , const QMap<QString,QString>& = QMap<QString,QString>()
    , QString& = QString());
  bool m_RecvSocketMessage(const QString&, QString& = QString()
    , QMap<QString,QString>& = QMap<QString,QString>());
  bool m_JSonObject(const QJsonObject&, const QString&, QJsonObject&);
  bool m_JSonValueStr(const QJsonObject&, const QString&, QString&);
  bool m_JSonValueDouble(const QJsonObject&, const QString&, double&);
  bool m_JSonValueStrOrLong(const QJsonObject&, const QString&, QString&);
  bool m_JSonValueDoubleOrStr(const QJsonObject&, const QString&, double&);
  int64_t m_i64IdProposalByInfo(const QString&, const QString&, sProposalInfo&);
  bool m_ProposalResumeUpdate();
  void m_DetailsUpdate(const QSqlQueryModel&, int);  
};

#endif // APP_BROKER_BINARY_H
