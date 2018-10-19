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
    /**
     * CBrokerBinary constructor
     * @param token
     * @param parent
     */
    explicit CBrokerBinary(QString token, QObject *parent = 0);
    ~CBrokerBinary();
  
  public slots:
    void slotOnMessageTelegramBot(Telegram::Message);
    
  private:
    QString m_strToken;
};

#endif // BROKER_BINARY_H
