/* ==========================================================================
 *   INCLUDE FILE NAME: telegram_bot.h
 * INCLUDE DESCRIPTION: declares the CTelegramBot class.
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

#ifndef TELEGRAM_BOT_H
#define TELEGRAM_BOT_H

/* ==========================================================================
 * INCLUDE: Basic include file.
 * ========================================================================== */
#include "app_priv.h"
#include "QtTelegramBot/qttelegrambot.h"

/* ==========================================================================
 * MACROS
 * ========================================================================== */

/* ==========================================================================
 * CLASS DECLARATION
 * ========================================================================== */
class CTelegramBot : public Telegram::Bot
{
  public:
    /**
     * CTelegramBot constructor
     * @param token
     * @param updates - enable automatic update polling
     * @param updateInterval - interval between update polls in msec
     * @param pollingTimeout - timeout in sec
     * @param parent
     */
    explicit CTelegramBot(QString token, bool updates = false
      , quint32 updateInterval = 1000, quint32 pollingTimeout = 0
      , QObject *parent = 0);
    ~CTelegramBot();
};

#endif // TELEGRAM_BOT_H
