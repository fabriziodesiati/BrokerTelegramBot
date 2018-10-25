/* ==========================================================================
 * MODULE FILE NAME: configuration.cpp
 *      MODULE TYPE: class definition
 *
 *         FUNCTION: Define the CConfiguration class.
 *          PURPOSE: 
 *    CREATION DATE: 20181025
 *          AUTHORS: Fabrizio De Siati
 *     DESIGN ISSUE: None
 *       INTERFACES: CConfiguration
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
#include "configuration.h"

/* ==========================================================================
 * MODULE PRIVATE MACROS
 * ========================================================================== */
#define CONFIGURATION_DEBUG 0

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
const CConfiguration& CConfiguration::GetInstance()
{
  static CConfiguration configuration;
  return configuration;
}
 
/* ==========================================================================
 *        FUNCTION NAME: CConfiguration
 * FUNCTION DESCRIPTION: constructor
 *        CREATION DATE: 20181025
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CConfiguration::CConfiguration()
{
}

/* ==========================================================================
 *        FUNCTION NAME: ~CConfiguration
 * FUNCTION DESCRIPTION: destructor
 *        CREATION DATE: 20181025
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CConfiguration::~CConfiguration()
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
bool CConfiguration::load(const QString& strFileCfgPath)
{
  // empty map
  m_mapKeys.clear();
  QFile file(strFileCfgPath);
  RETURN_IFW(!file.open(QIODevice::ReadOnly | QIODevice::Text), false);
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
QString CConfiguration::get(const QString& strKey)
{
  return m_mapKeys.value(strKey);
}

