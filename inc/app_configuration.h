/* ==========================================================================
 *   INCLUDE FILE NAME: app_configuration.h
 * INCLUDE DESCRIPTION: declares the CAppConfiguration class.
 *       CREATION DATE: 20181025
 *             AUTHORS: Fabrizio De Siati
 *        DESIGN ISSUE: None. 
 *
 *             HISTORY: See table below.
 * 
 * 25-Oct-2018 | Fabrizio De Siati | 0.0 |
 * Initial creation of this file.
 * 
 * ========================================================================== */

#ifndef APP_CONFIGURATION_H
#define APP_CONFIGURATION_H

/* ==========================================================================
 * INCLUDE: Basic include file.
 * ========================================================================== */
#include "app_priv.h"
#include <QtCore>

/* ==========================================================================
 * MACROS
 * ========================================================================== */

/* ==========================================================================
 * CLASS DECLARATION
 * ========================================================================== */
class CAppConfiguration
{
public:
  /**
   * Get instance for this singleton.
   * @return a reference to instance.
   */
  static const CAppConfiguration& GetInstance();
  
  /**
   * CAppConfiguration constructor
   */
  explicit CAppConfiguration();
  ~CAppConfiguration();

  /**
   * Load congiguration gived cfg file path
   */
  bool load(const QString&);

  /**
   * Get value for given cfg key, if doesn't exist retrieves empty string
   */
  QString get(const QString&);

private:
  QMap<QString, QString> m_mapKeys;
};

#endif // APP_CONFIGURATION_H
