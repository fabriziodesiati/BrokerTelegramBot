/* ==========================================================================
 * MODULE FILE NAME: app_model_proposals.cpp
 *      MODULE TYPE: class definition
 *
 *         FUNCTION: Define the CAppModelProposals class.
 *          PURPOSE: 
 *    CREATION DATE: 20181029
 *          AUTHORS: Fabrizio De Siati
 *     DESIGN ISSUE: None
 *       INTERFACES: CAppModelProposals
 *     SUBORDINATES: None.
 * 
 *          PROPOSALS: See table below. 
 *
 * 29-Oct-2018 | Fabrizio De Siati | 0.0 |
 * Initial creation of this file.
 *
 * ========================================================================== */

/* ==========================================================================
 * INCLUDES
 * ========================================================================== */
#include "app_model_proposals.h"
#include <QPixmap>
#include <QIcon>

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
 *        FUNCTION NAME: CAppModelProposals
 * FUNCTION DESCRIPTION: constructor
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CAppModelProposals::CAppModelProposals(QObject *parent)
: QSqlQueryModel { parent }
{
}

/* ==========================================================================
 *        FUNCTION NAME: ~CAppModelProposals
 * FUNCTION DESCRIPTION: destructor
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CAppModelProposals::~CAppModelProposals()
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
QVariant CAppModelProposals::data(const QModelIndex &idx, int role) const
{
  if (role == Qt::DecorationRole)
  {
    if (idx.column() == 2) /* date_time */
    {
      QModelIndex idxOperation = createIndex(idx.row(), 8); /* status */
      QString strStatus = data(idxOperation, Qt::DisplayRole).toString();
      RETURN_IF("won" == strStatus
        , QIcon{QPixmap{":/icons/resources/circle_green.png"}});
      RETURN_IF("lose" == strStatus
        , QIcon{QPixmap{":/icons/resources/circle_red.png"}});
      RETURN_IF(true
        , QIcon{QPixmap{":/icons/resources/circle_yellow.png"}});
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
Qt::ItemFlags CAppModelProposals::flags(const QModelIndex& idx) const
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
bool CAppModelProposals::setData(const QModelIndex& idx, const QVariant& value, int role)
{
  return QSqlQueryModel::setData(idx, value, role);
}
