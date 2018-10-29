/* ==========================================================================
 * MODULE FILE NAME: app_telegram_bot.cpp
 *      MODULE TYPE: class definition
 *
 *         FUNCTION: Define the CAppTelegramBot class.
 *          PURPOSE: 
 *    CREATION DATE: 20181019
 *          AUTHORS: Fabrizio De Siati
 *     DESIGN ISSUE: None
 *       INTERFACES: CAppTelegramBot
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
#include "app_telegram_bot.h"

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
 *        FUNCTION NAME: CAppTelegramBot
 * FUNCTION DESCRIPTION: constructor
 *        CREATION DATE: 20181019
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CAppTelegramBot::CAppTelegramBot(QString token, bool updates
  , quint32 updateInterval, quint32 pollingTimeout, QObject *parent)
: Telegram::Bot { token, updates, updateInterval, pollingTimeout, parent }
, m_strToken    { token                                                  }
{
}

/* ==========================================================================
 *        FUNCTION NAME: ~CAppTelegramBot
 * FUNCTION DESCRIPTION: destructor
 *        CREATION DATE: 20181019
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CAppTelegramBot::~CAppTelegramBot()
{
}