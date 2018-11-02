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
#define APP_DEBUG                       1
#define APP_MODEL_COL_DATE_TIME         2
#define APP_MODEL_COL_STATUS            3
#define APP_MODEL_COL_PROFIT            4
#define APP_MODEL_COL_PROFIT_PERCENTAGE 5

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
  if      (role == Qt::DecorationRole)
  {
    if (idx.column() == APP_MODEL_COL_DATE_TIME)
    {
      QModelIndex idxOperation = createIndex(idx.row(), APP_MODEL_COL_STATUS);
      QString strStatus = data(idxOperation, Qt::DisplayRole).toString();
      static const QMap<QString,QString> mapStatus2Color = {
          {"open",":/icons/resources/circle_yellow.png"}
        , {"sold",":/icons/resources/circle_yellow.png"}
        , {"lost",":/icons/resources/circle_red.png"   }
        , {"won" ,":/icons/resources/circle_green.png"  }
      };
      RETURN_IF(true
        , QIcon{QPixmap{mapStatus2Color.value(strStatus
          , ":/icons/resources/circle_gray.png")}});
    }
  }
  else if (role == Qt::ForegroundRole)
  {
    if       (idx.column() == APP_MODEL_COL_STATUS)
    {
      static const QMap<QString,QColor> mapStatus2Color = {
          {"open",":/icons/resources/circle_yellow.png"}
        , {"sold",":/icons/resources/circle_yellow.png"}
        , {"lost",":/icons/resources/circle_red.png"   }
        , {"won" ,":/icons/resources/circle_green.png"  }
      };
    }
    else if ((idx.column() == APP_MODEL_COL_PROFIT           ) ||
             (idx.column() == APP_MODEL_COL_PROFIT_PERCENTAGE))
    {
      double f64Profit = data(idx, Qt::DisplayRole).toDouble();
      RETURN_IF(f64Profit > 0.0, QColor(  0, 255,   0, 255)); /* green */
      RETURN_IF(f64Profit < 0.0, QColor(255,   0,   0, 255)); /* red */
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
