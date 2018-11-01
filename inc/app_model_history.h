/* ==========================================================================
 *   INCLUDE FILE NAME: app_model_history.h
 * INCLUDE DESCRIPTION: declares the CAppModelHistory class.
 *       CREATION DATE: 20181029
 *             AUTHORS: Fabrizio De Siati
 *        DESIGN ISSUE: None. 
 *
 *             HISTORY: See table below.
 * 
 * 29-Oct-2018 | Fabrizio De Siati | 0.0 |
 * Initial creation of this file.
 * 
 * ========================================================================== */

#ifndef APP_MODEL_HISTORY_H
#define APP_MODEL_HISTORY_H

/* ==========================================================================
 * INCLUDE: Basic include file.
 * ========================================================================== */
#include "app_priv.h"
#include <QSqlQueryModel>

/* ==========================================================================
 * MACROS
 * ========================================================================== */

/* ==========================================================================
 * CLASS DECLARATION
 * ========================================================================== */
class CAppModelHistory : public QSqlQueryModel
{
  Q_OBJECT
public:

  /**
   * CAppModelHistory constructor
   */
  explicit CAppModelHistory(QObject *parent = 0);
  virtual ~CAppModelHistory();

  /* INTERFACE from QSqlQueryModel */
  QVariant data(const QModelIndex&, int) const override;
  Qt::ItemFlags flags(const QModelIndex&) const override;
  bool setData(const QModelIndex&, const QVariant&, int) override;
};

#endif // APP_MODEL_HISTORY_H
