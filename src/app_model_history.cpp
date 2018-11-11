/* ==========================================================================
 * MODULE FILE NAME: app_model_history.cpp
 *      MODULE TYPE: class definition
 *
 *         FUNCTION: Define the CAppModelHistory class.
 *          PURPOSE: 
 *    CREATION DATE: 20181029
 *          AUTHORS: Fabrizio De Siati
 *     DESIGN ISSUE: None
 *       INTERFACES: CAppModelHistory
 *     SUBORDINATES: None.
 * 
 *          HISTORY: See table below. 
 *
 * 29-Oct-2018 | Fabrizio De Siati | 0.0 |
 * Initial creation of this file.
 *
 * ========================================================================== */

/* ==========================================================================
 * INCLUDES
 * ========================================================================== */
#include "app_model_history.h"
#include <QPixmap>
#include <QIcon>

/* ==========================================================================
 * MODULE PRIVATE MACROS
 * ========================================================================== */
#define APP_DEBUG               1
#define APP_MODEL_COL_DATE_TIME 2
#define APP_MODEL_COL_OPERATION 3

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
 *        FUNCTION NAME: CAppModelHistory
 * FUNCTION DESCRIPTION: constructor
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CAppModelHistory::CAppModelHistory(QObject *parent)
: QSqlQueryModel { parent }
{
}

/* ==========================================================================
 *        FUNCTION NAME: ~CAppModelHistory
 * FUNCTION DESCRIPTION: destructor
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CAppModelHistory::~CAppModelHistory()
{
  /* Nothing to do here */
}

/* ==========================================================================
 *        FUNCTION NAME: data
 * FUNCTION DESCRIPTION: model data
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
QVariant CAppModelHistory::data(const QModelIndex &idx, int role) const
{
  if (role == Qt::DecorationRole)
  {
    if (idx.column() == APP_MODEL_COL_DATE_TIME)
    {
      QModelIndex idxOperation = createIndex(idx.row(),APP_MODEL_COL_OPERATION);
      QString strOperation = data(idxOperation, Qt::DisplayRole).toString();
      static const QMap<QString, QString> mapOper2Img {
          {"SOCK ERRO", ":/icons/resources/history_sock_erro.png"}
        , {"SOCK OPEN", ":/icons/resources/history_sock_open.png"}
        , {"SOCK CONN", ":/icons/resources/history_sock_conn.png"}
        , {"SOCK DISC", ":/icons/resources/history_sock_disc.png"}
        , {"SOCK RECV", ":/icons/resources/history_sock_recv.png"}
        , {"SOCK SEND", ":/icons/resources/history_sock_send.png"}
        , {"TBOT RECV", ":/icons/resources/history_tbot_recv.png"}
      };
      RETURN_IF(mapOper2Img.contains(strOperation)
        , QIcon{QPixmap{mapOper2Img.value(strOperation)}});
    }
  }
  return QSqlQueryModel::data(idx, role);
}

/* ==========================================================================
 *        FUNCTION NAME: flags
 * FUNCTION DESCRIPTION: 
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
Qt::ItemFlags CAppModelHistory::flags(const QModelIndex& idx) const
{
  return QSqlQueryModel::flags(idx);
}

/* ==========================================================================
 *        FUNCTION NAME: setData
 * FUNCTION DESCRIPTION: set model data
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
bool CAppModelHistory::setData(const QModelIndex& idx, const QVariant& value, int role)
{
  return QSqlQueryModel::setData(idx, value, role);
}
