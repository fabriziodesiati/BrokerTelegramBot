/* ==========================================================================
 *   INCLUDE FILE NAME: app_database.h
 * INCLUDE DESCRIPTION: declares the CAppDatabase class.
 *       CREATION DATE: 20181026
 *             AUTHORS: Fabrizio De Siati
 *        DESIGN ISSUE: None. 
 *
 *             HISTORY: See table below.
 * 
 * 26-Oct-2018 | Fabrizio De Siati | 0.0 |
 * Initial creation of this file.
 * 
 * ========================================================================== */

#ifndef APP_DATABASE_H
#define APP_DATABASE_H

/* ==========================================================================
 * INCLUDE: Basic include file.
 * ========================================================================== */
#include "app_priv.h"
#include <QSqlDatabase>

/* ==========================================================================
 * MACROS
 * ========================================================================== */

/* ==========================================================================
 * CLASS DECLARATION
 * ========================================================================== */
class CAppDatabase
{
public:
  /**
   * Get instance for this singleton.
   * @return a reference to instance.
   */
  static CAppDatabase& GetInstance();
  
  /**
   * CAppDatabase constructor
   */
  explicit CAppDatabase();
  ~CAppDatabase();

  /**
   * Connect to database
   * @param driver:   database sql driver
   * @param database: database name or path
   * @param hostname: database hostname
   * @param username: database username
   * @param password: database password
   * @param port:     database port
   */
  bool connect(
      const QString& driver
    , const QString& database
    , const QString& hostname
    , const QString& username
    , const QString& password
    , const QString& port);

  /**
   * Close database
   */
  void close();

  /**
   * Database in connected
   */
  bool connected() const;

  /**
   * Execute query
   */
  bool execQuery(const QString&);

  /**
   * Execute insert query and retrieve inserted record id
   * retrieve -1 if error
   */
  int64_t execInsertQuery(const QString&);
};

#endif // APP_DATABASE_H
