/* ==========================================================================
 *   INCLUDE FILE NAME: app_model_proposals.h
 * INCLUDE DESCRIPTION: declares the CAppModelProposals class.
 *       CREATION DATE: 20181029
 *             AUTHORS: Fabrizio De Siati
 *        DESIGN ISSUE: None. 
 *
 *             PROPOSALS: See table below.
 * 
 * 29-Oct-2018 | Fabrizio De Siati | 0.0 |
 * Initial creation of this file.
 * 
 * ========================================================================== */

#ifndef APP_MODEL_PROPOSALS_H
#define APP_MODEL_PROPOSALS_H

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
class CAppModelProposals : public QSqlQueryModel
{
  Q_OBJECT
public:

  /**
   * CAppModelProposals constructor
   */
  explicit CAppModelProposals(QObject *parent = 0);
  virtual ~CAppModelProposals();

  /* INTERFACE from QSqlQueryModel */
  QVariant data(const QModelIndex&, int) const override;
  Qt::ItemFlags flags(const QModelIndex&) const override;
  bool setData(const QModelIndex&, const QVariant&, int) override;
};

#endif // APP_MODEL_PROPOSALS_H
