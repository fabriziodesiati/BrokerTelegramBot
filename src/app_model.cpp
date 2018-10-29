/* ==========================================================================
 * MODULE FILE NAME: app_model.cpp
 *      MODULE TYPE: class definition
 *
 *         FUNCTION: Define the CAppModel class.
 *          PURPOSE: 
 *    CREATION DATE: 20181029
 *          AUTHORS: Fabrizio De Siati
 *     DESIGN ISSUE: None
 *       INTERFACES: CAppModel
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
#include "app_model.h"

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
 *        FUNCTION NAME: CAppModel
 * FUNCTION DESCRIPTION: constructor
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CAppModel::CAppModel(QObject *parent)
: QSqlQueryModel { parent }
{
}

/* ==========================================================================
 *        FUNCTION NAME: ~CAppModel
 * FUNCTION DESCRIPTION: destructor
 *        CREATION DATE: 20181029
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CAppModel::~CAppModel()
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
QVariant CAppModel::data(const QModelIndex &idx, int role) const
{
#if 0
  if      (role == Qt::CheckStateRole)
  {
    if (idx.column() == m_iColSELECTION)
    {
      QModelIndex idxID = createIndex(idx.row(), m_iColPRIMARYKEY);
      if (idxID.isValid())
      {
        int iID = data(idxID, Qt::DisplayRole).toInt();
        if (m_bExportSelection) {
          PK_RETURN_IF(true, m_mapSelectionExport.value(iID, false) 
            ? Qt::Checked: Qt::Unchecked);
        }
        else if (m_bSelectionWorldBrowser) {
          PK_RETURN_IF(true, m_mapSelection.value(iID, false) 
            ? Qt::Checked: Qt::Unchecked);
        }
      }
    }
  }
  else if (role == Qt::DecorationRole)
  {
    if (idx.column() == m_iColSELECTION)
    {
      QModelIndex idxVALIDITYFLAG = createIndex(idx.row(), m_iColVALIDITYFLAG);
      bool bValidityFlag = data(idxVALIDITYFLAG, Qt::DisplayRole).toBool();
      PK_RETURN_IF(true
        , bValidityFlag
          ? QIcon{QPixmap{":/icons/resources/circle_green.png"}}
          : QIcon{QPixmap{":/icons/resources/circle_red.png"  }});
    }
  }
  else if (role == Qt::ToolTipRole)
  {
    if (idx.column() == m_iColSELECTION)
    {
      QModelIndex idxVALIDITYFLAG = createIndex(idx.row(), m_iColVALIDITYFLAG);
      bool bValidityFlag = data(idxVALIDITYFLAG, Qt::DisplayRole).toBool();
      PK_RETURN_IF(true
        , bValidityFlag
          ? tr("VALID")
          : tr("INVALID"));
    }
  }
  else if (role == Qt::FontRole)
  {
    QModelIndex idxTOEXPORT = createIndex(idx.row(), m_iColTOEXPORT);
    uint8_t u8ToExport = 
      static_cast<uint8_t>(data(idxTOEXPORT, Qt::DisplayRole).toUInt());
    PK_RETURN_IF(static_cast<uint8_t>(ToExport::kCOUNT) <= u8ToExport
      , QSqlQueryModel::data(idx, role));
    ToExport toExport = static_cast<ToExport>(u8ToExport);
    QFont font;
    font.setItalic(ToExport::kEXPORTED_INVALID != toExport &&
                   ToExport::kEXPORTED_VALID   != toExport);
    PK_RETURN_IF(true, font);
  }
  else if (role == Qt::ForegroundRole)
  {
    QModelIndex idxTOEXPORT = createIndex(idx.row(), m_iColTOEXPORT);
    uint8_t u8ToExport = 
      static_cast<uint8_t>(data(idxTOEXPORT, Qt::DisplayRole).toUInt());
    PK_RETURN_IF(static_cast<uint8_t>(ToExport::kCOUNT) <= u8ToExport
      , QSqlQueryModel::data(idx, role));
    ToExport toExport = static_cast<ToExport>(u8ToExport);
    switch (toExport)
    {
      case pk_qp_wdg_gcp::ToExport::kEXPORTED_INVALID:
        PK_RETURN_IF(true,  QColor(PK_COLOR_EXPORTED_INVALID));
      case pk_qp_wdg_gcp::ToExport::kEXPORTED_VALID:
        PK_RETURN_IF(true,  QColor(PK_COLOR_EXPORTED_VALID));
      case pk_qp_wdg_gcp::ToExport::kNEVER_EXPORTED:
        PK_RETURN_IF(true,  QColor(PK_COLOR_NEVER_EXPORTED));
      case pk_qp_wdg_gcp::ToExport::kTOEXPORT_CHANGE:
        PK_RETURN_IF(true,  QColor(PK_COLOR_TOEXPORT_CHANGE));
      case pk_qp_wdg_gcp::ToExport::kTOEXPORT_INVALID:
        PK_RETURN_IF(true,  QColor(PK_COLOR_TOEXPORT_INVALID));
      case pk_qp_wdg_gcp::ToExport::kTOEXPORT_VALID:
        PK_RETURN_IF(true,  QColor(PK_COLOR_TOEXPORT_VALID));
      default:
        PK_RETURN_IF(true, QSqlQueryModel::data(idx, role));
        break;
    }
  }
#endif
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
Qt::ItemFlags CAppModel::flags(const QModelIndex& idx) const
{
#if 0
  if (idx.column() == m_iColSELECTION)
  {
    PK_RETURN_IF(m_bExportSelection || m_bSelectionWorldBrowser
      , (QSqlQueryModel::flags(idx)
        | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled) & ~Qt::ItemIsSelectable);
  }
#endif
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
bool CAppModel::setData(const QModelIndex& idx, const QVariant& value, int role)
{
#if 0
  if ((idx.column() == m_iColSELECTION) && (role == Qt::CheckStateRole))
  {
    QModelIndex idxID = createIndex(idx.row(), m_iColPRIMARYKEY);
    if (idxID.isValid())
    {
      int iID = data(idxID, Qt::DisplayRole).toInt();
      if (m_bExportSelection) {
        m_mapSelectionExport.insert(iID, Qt::Checked == value);
        emit valueChanged(idx, value);
      }
      else if (m_bSelectionWorldBrowser) {
        m_mapSelection.insert(iID, Qt::Checked == value);
        emit valueChanged(idx, value);
      }
    }
  }
#endif
  return QSqlQueryModel::setData(idx, value, role);
}
