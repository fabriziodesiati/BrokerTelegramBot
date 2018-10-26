/* ==========================================================================
 * MODULE FILE NAME: app_database.cpp
 *      MODULE TYPE: class definition
 *
 *         FUNCTION: Define the CAppDatabase class.
 *          PURPOSE: 
 *    CREATION DATE: 20181026
 *          AUTHORS: Fabrizio De Siati
 *     DESIGN ISSUE: None
 *       INTERFACES: CAppDatabase
 *     SUBORDINATES: None.
 * 
 *          HISTORY: See table below. 
 *
 * 26-Oct-2018 | Fabrizio De Siati | 0.0 |
 * Initial creation of this file.
 *
 * ========================================================================== */

/* ==========================================================================
 * INCLUDES
 * ========================================================================== */
#include "app_database.h"
#include <QtCore>
#include <QSqlQuery>
#include <QSqlError>

/* ==========================================================================
 * MODULE PRIVATE MACROS
 * ========================================================================== */
#define APP_DATABASE_DEBUG 1

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
CAppDatabase& CAppDatabase::GetInstance()
{
  static CAppDatabase database;
  return database;
}
 
/* ==========================================================================
 *        FUNCTION NAME: CAppDatabase
 * FUNCTION DESCRIPTION: constructor
 *        CREATION DATE: 20181025
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CAppDatabase::CAppDatabase()
{
}

/* ==========================================================================
 *        FUNCTION NAME: ~CAppDatabase
 * FUNCTION DESCRIPTION: destructor
 *        CREATION DATE: 20181025
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CAppDatabase::~CAppDatabase()
{
}

/* ==========================================================================
 *        FUNCTION NAME: connect
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181026
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppDatabase::connect(
    const QString& driver
  , const QString& database
  , const QString& hostname
  , const QString& username
  , const QString& password
  , const QString& port)
{
#if APP_DATABASE_DEBUG == 1
  { QString strMsg;
    auto list = QSqlDatabase::drivers();
    strMsg.append(QString("Drivers sql avalilable are [%1]:\n")
      .arg(list.count()));
    for(auto str: list) {
      strMsg.append(QString("  %1\n").arg(str));
    }
    DEBUG_APP("Connect databse", strMsg);
  }
#endif
  // check driver availability
  RETURN_IFW(!QSqlDatabase::isDriverAvailable(driver)
    , QString("Driver %1 is unavailable").arg(driver), false);
  // add database
  QSqlDatabase db = QSqlDatabase::addDatabase(driver);
  // set hostname
  db.setHostName(hostname);
  // set port
  db.setPort(port.toInt());
  // set database name
  db.setDatabaseName(database);
  // set user name name
  db.setUserName(username);
  // set password
  db.setPassword(password);
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: close
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181026
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
void CAppDatabase::close()
{
  // get database
  QSqlDatabase db = QSqlDatabase::database();
  if (db.isValid() && db.isOpen()) {
    // close database
    db.close();
  }
}

/* ==========================================================================
 *        FUNCTION NAME: connected
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181026
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppDatabase::connected() const
{
  // get database
  QSqlDatabase db = QSqlDatabase::database();
  // open db if closed, ignore return state
  if (db.isValid() && !db.isOpen()) { db.open(); }
  // return database connection status
  return (db.isValid() && db.isOpen());
}

/* ==========================================================================
 *        FUNCTION NAME: execQuery
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181026
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppDatabase::execQuery(const QString& strQuery)
{
  // check if database is connected
  RETURN_IFW(!connected(), "Database isn't connected", false);
  // execute query
  QSqlQuery qry;
  RETURN_IFW(!qry.exec(strQuery)
    , QString("Unable to execute query [%1] E=%2")
      .arg(strQuery).arg(qry.lastError().text()), false);
  return true;
}

/* ==========================================================================
 *        FUNCTION NAME: execInsertQuery
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181026
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
int64_t CAppDatabase::execInsertQuery(const QString& strQuery)
{
  // check if database is connected
  RETURN_IFW(!connected(), "Database isn't connected", false);
  // execute query
  QSqlQuery qry;
  RETURN_IFW(!qry.exec(strQuery)
    , QString("Unable to execute insert query [%1] E=%2")
      .arg(strQuery).arg(qry.lastError().text()), false);
  RETURN_IFW(1 > qry.numRowsAffected(), "Could not insert a record", false);
  return qry.lastInsertId().toInt();
}
