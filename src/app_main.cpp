/* ==========================================================================
 * MODULE FILE NAME: app_main.cpp
 *      MODULE TYPE: application entry point
 *
 *         FUNCTION: Application Entry point.
 *          PURPOSE: Application Entry point.
 *    CREATION DATE: 20181016
 *          AUTHORS: Fabrizio De Siati
 *     DESIGN ISSUE: None
 *       INTERFACES: None.
 *     SUBORDINATES: None.
 *
 *          HISTORY: See table below.
 *
 * 16-Oct-2018 | Fabrizio De Siati | 0.0 |
 * Initial creation of this file.
 *
 * ========================================================================== */

/* ==========================================================================
 * MODULE PRIVATE MACROS
 * ========================================================================== */
#define APP_DEBUG 0

/* ==========================================================================
 * INCLUDES
 * ========================================================================== */
#include <QApplication>
#include "QtTelegramBot/qttelegrambot.h"
#include "app_configuration.h"
#include "app_database.h"
#include "app_broker_binary.h"

/* ==========================================================================
 * CUSTOMIZABLE MODULE PRIVATE MACROS
 * ========================================================================== */

/* ==========================================================================
 * MODULE TAGGING
 * ========================================================================== */

/* ==========================================================================
 * STATIC VARIABLES FOR MODULE
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
 * APPLICATION ENTRY POINT: Main
 * ========================================================================== */
int main(int argc,char* argv[])
{
  /* Load settings */
  QSettings s(HMI_COMPANY, HMI_TITLE);
  QString strCfgFilePath = "./etc/BrokerTelegramBot_default.cfg";
  QString strDbFilePath = "./db/BrokerTelegramBot.sqlite";
  QString strTheme;
  QString strAppIdBroker;
  QString strTokenBroker;
  QString strTokenBot;
  /* Parse arguments */
  QMap<QString,QString*> mapArgs = {
      {"--cfg"          , &strCfgFilePath}
    , {"--db"           , &strDbFilePath }
    , {"--theme"        , &strTheme      }
    , {"--app_id-broker", &strAppIdBroker}
    , {"--token-broker" , &strTokenBroker}
    , {"--token-bot"    , &strTokenBot   }
    
  };
  for (auto j = 0; j < argc; ++j) {
    QString strArgName = argv[j];
    if      ("--clear" == strArgName) {
      /* Clear settings */
      s.clear();
    }
    else if (mapArgs.contains(strArgName)) {
      const char* pStr = argv[++j];
      CATCH_ABORT(nullptr == pStr
        , QString("arg %1 requires a value").arg(strArgName));
      *mapArgs.value(strArgName) = pStr;
    }
  }
  /* Update theme settings */
  if (!strTheme.isEmpty()) {
    s.setValue("gui_theme", strTheme);
  }
  /* Load configuration if exists */
  if (CAppConfiguration::GetInstance().load(strCfgFilePath)) {
    QMap<QString,QString*> mapCfg = {
        {"db.database"  , &strDbFilePath  }
      , {"app_id-broker", &strAppIdBroker }
      , {"token-broker" , &strTokenBroker }
      , {"token-bot"    , &strTokenBot    }
    };
    for(auto strKeyName: mapCfg.keys()) {
      QString* pKeyValue = mapCfg.value(strKeyName);
      *pKeyValue = CAppConfiguration::GetInstance().get(strKeyName, *pKeyValue);
    }
  }
  CATCH_ABORT(strAppIdBroker.isEmpty(), "Required --app_id-broker <APP_ID>");
  CATCH_ABORT(strTokenBroker.isEmpty(), "Required --token-broker <TOKEN>");
  CATCH_ABORT(strTokenBot.isEmpty()   , "Required --token-bot <TOKEN>");

  /* Create Application */
  // Set important Qt flags
  QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
  QApplication app(argc, argv);

  /* Create an instance of BrokerBinary */
  CAppBrokerBinary* pAppBrokerBinary = new(std::nothrow) CAppBrokerBinary(
    strAppIdBroker, strTokenBroker, strTokenBot);
  /* Connect signals and slots */
  QObject::connect(
      pAppBrokerBinary, &CAppBrokerBinary::closed
    , &app
    , &QCoreApplication::quit);
  /* Show widget main */
  pAppBrokerBinary->show();

  /* Connect database: db connection must be last operation */
  CATCH_ABORT(!CAppDatabase::GetInstance().connect(
      CAppConfiguration::GetInstance().get("db.driver", "QSQLITE")
    , strDbFilePath
    , CAppConfiguration::GetInstance().get("db.hostname")
    , CAppConfiguration::GetInstance().get("db.username")
    , CAppConfiguration::GetInstance().get("db.password")
    , CAppConfiguration::GetInstance().get("db.port")
  ), QString("Cannot connect to database %1").arg(strDbFilePath));

  return app.exec();
}
