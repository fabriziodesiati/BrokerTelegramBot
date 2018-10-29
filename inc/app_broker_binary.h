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
#include "app_priv.h"
#include <QMainWindow>
#include <QtWebSockets/QWebSocket>
#include "QtTelegramBot/types/message.h"
#include <QJsonDocument>

/* ==========================================================================
 * MACROS
 * ========================================================================== */

/* ==========================================================================
 * FORWARD DECLARATIONS
 * ========================================================================== */
namespace Ui {
  class CWdgMain;
}

/* ==========================================================================
 * CLASS DECLARATION
 * ========================================================================== */
class CAppBrokerBinary : public QMainWindow
{
  Q_OBJECT
public:

  /**
   * CAppBrokerBinary constructor
   * @param app_id
   * @param url
   * @param parent
   */
  explicit CAppBrokerBinary(const QString& app_id, const QString& token
    , QMainWindow *parent = 0);
  ~CAppBrokerBinary();

signals:
  void closed();

public slots:
  void slotOnSocketConnected();
  void slotOnMessageSocketReceived(QString message);
  void slotOnMessageTelegramBot(Telegram::Message);    
    
private:
  /* Pointer to user interface */
  Ui::CWdgMain *ui;
  
  QString m_strAppId;
  QString m_strToken;
  QUrl m_url;
  QWebSocket m_webSocket;
  QMap<QString,QString> m_mapHistoryMsg;

  QString m_SendSocketMessage(const QString&, const QMap<QString,QString>&);
  void m_RecvSocketMessage(const QString&, QString&, QMap<QString, QString>&);

  bool m_JSonObject(const QJsonObject&, const QString&, QJsonObject&);
  bool m_JSonValueStr(const QJsonObject&, const QString&, QString&);

};

#endif // APP_BROKER_BINARY_H
