/* ==========================================================================
 *   INCLUDE FILE NAME: broker_binary.h
 * INCLUDE DESCRIPTION: declares the CBrokerBinary class.
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

#ifndef BROKER_BINARY_H
#define BROKER_BINARY_H

/* ==========================================================================
 * INCLUDE: Basic include file.
 * ========================================================================== */
#include "app_priv.h"
#include <QObject>
#include <QtWebSockets/QWebSocket>
#include "QtTelegramBot/types/message.h"

/* ==========================================================================
 * MACROS
 * ========================================================================== */

/* ==========================================================================
 * CLASS DECLARATION
 * ========================================================================== */
class CBrokerBinary : public QObject
{
  Q_OBJECT
public:
  static void DEBUG_APP(const QString&, const QString& = {});

  enum class RequestType
  {
      kNOTHING
    , kAUTHORIZE
  };

  /**
    * CBrokerBinary constructor
    * @param app_id
    * @param url
    * @param parent
    */
  explicit CBrokerBinary(const QString& app_id, const QString& token
    , QObject* parent = 0);
  ~CBrokerBinary();

signals:
  void closed();

public slots:
  void slotOnSocketConnected();
  void slotOnMessageSocketReceived(QString message);
  void slotOnMessageTelegramBot(Telegram::Message);    
    
private:
  QString m_strAppId;
  QString m_strToken;
  QUrl m_url;
  QWebSocket m_webSocket;
  RequestType m_lastRequestType;
  
  QString m_ReuestTypeStr(RequestType);
  void m_SendSocketMessage(RequestType, const QString&);
};

#endif // BROKER_BINARY_H
