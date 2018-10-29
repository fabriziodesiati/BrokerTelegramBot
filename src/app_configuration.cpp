/* ==========================================================================
 * MODULE FILE NAME: app_configuration.cpp
 *      MODULE TYPE: class definition
 *
 *         FUNCTION: Define the CAppConfiguration class.
 *          PURPOSE: 
 *    CREATION DATE: 20181025
 *          AUTHORS: Fabrizio De Siati
 *     DESIGN ISSUE: None
 *       INTERFACES: CAppConfiguration
 *     SUBORDINATES: None.
 * 
 *          HISTORY: See table below. 
 *
 * 25-Oct-2018 | Fabrizio De Siati | 0.0 |
 * Initial creation of this file.
 *
 * ========================================================================== */

/* ==========================================================================
 * INCLUDES
 * ========================================================================== */
#include "app_configuration.h"

/* ==========================================================================
 * MODULE PRIVATE MACROS
 * ========================================================================== */
#define APP_DEBUG 0

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
 *        FUNCTION NAME: GetInstance
 * FUNCTION DESCRIPTION: get singleton
 *        CREATION DATE: 20181025
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CAppConfiguration& CAppConfiguration::GetInstance()
{
  static CAppConfiguration configuration;
  return configuration;
}
 
/* ==========================================================================
 *        FUNCTION NAME: CAppConfiguration
 * FUNCTION DESCRIPTION: constructor
 *        CREATION DATE: 20181025
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CAppConfiguration::CAppConfiguration()
{
}

/* ==========================================================================
 *        FUNCTION NAME: ~CAppConfiguration
 * FUNCTION DESCRIPTION: destructor
 *        CREATION DATE: 20181025
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CAppConfiguration::~CAppConfiguration()
{
}

/* ==========================================================================
 *        FUNCTION NAME: load
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181025
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppConfiguration::load(const QString& strFileCfgPath)
{
  // empty map
  m_mapKeys.clear();
  QFile file(strFileCfgPath);
  RETURN_IFW(!file.open(QIODevice::ReadOnly | QIODevice::Text)
    , QString("Cannot open configuration file %1").arg(strFileCfgPath), false);
  while (!file.atEnd()) {
    QString strLine = file.readLine();
    // check if is a comment line
    if (!strLine.startsWith("#")) {
      QStringList strKeyValue = strLine.split("=");
      if (strKeyValue.count() >= 2) {
        m_mapKeys.insert(
            strKeyValue.at(0).trimmed()  /* KEY */
          , strKeyValue.at(1).trimmed());/* VALUE */
      }
    }
  }
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: get
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181025
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
QString CAppConfiguration::get(const QString& strKey, const QString& strDefault)
{
  return m_mapKeys.value(strKey, strDefault);
}

