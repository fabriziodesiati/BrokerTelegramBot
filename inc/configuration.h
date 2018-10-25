/* ==========================================================================
 *   INCLUDE FILE NAME: configuration.h
 * INCLUDE DESCRIPTION: declares the CConfiguration class.
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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

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
class CConfiguration
{
public:
  /**
   * Get instance for this singleton.
   * @return a reference to instance.
   */
  static const CConfiguration& GetInstance();
  
  /**
   * CConfiguration constructor
   */
  explicit CConfiguration();
  ~CConfiguration();

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

#endif // CONFIGURATION_H
