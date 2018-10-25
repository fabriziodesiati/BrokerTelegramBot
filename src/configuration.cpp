/* ==========================================================================
 * MODULE FILE NAME: configuration.cpp
 *      MODULE TYPE: class definition
 *
 *         FUNCTION: Define the CConfiguration class.
 *          PURPOSE: 
 *    CREATION DATE: 20181025
 *          AUTHORS: Fabrizio De Siati
 *     DESIGN ISSUE: None
 *       INTERFACES: CConfiguration
 *     SUBORDINATES: None.
 * 
 *          HISTORY: See table below. 
 *
 * 25-Oct-2018 | Fabrizio De Siati | 0.0 |
 * Initial creation of this file.
 *
 * ========================================================================== */

/* ==========================================================================
 * INCLUDES
 * ========================================================================== */
#include "configuration.h"

/* ==========================================================================
 * MODULE PRIVATE MACROS
 * ========================================================================== */
#define CONFIGURATION_DEBUG 0

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
const CConfiguration& CConfiguration::GetInstance()
{
  static CConfiguration configuration;
  return configuration;
}
 
/* ==========================================================================
 *        FUNCTION NAME: CConfiguration
 * FUNCTION DESCRIPTION: constructor
 *        CREATION DATE: 20181025
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CConfiguration::CConfiguration()
{
}

/* ==========================================================================
 *        FUNCTION NAME: ~CConfiguration
 * FUNCTION DESCRIPTION: destructor
 *        CREATION DATE: 20181025
 *              AUTHORS: Fabrizio De Siati
 *           INTERFACES: None
 *         SUBORDINATES: None
 * ========================================================================== */
CConfiguration::~CConfiguration()
{
}
