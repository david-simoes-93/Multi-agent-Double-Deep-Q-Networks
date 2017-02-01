/**
 * @file   MonitorParameters.h
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Thu Aug  7 17:52:09 2003
 * 
 * @brief  This file contains the parameters for the monitor. 
 * 
 */

#ifndef _MONITORPARAMETERS_
#define _MONITORPARAMETERS_

#include "GenericValues.h"

/******************************************************************************/
/********************** CLASS MONITORPARAMETERS *******************************/
/******************************************************************************/

/**
 * This class contains all parameters to configure the monitor of the
 * pursuit domain. Each parameters is associated with a text
 * string. When these strings are read from configuration file, the
 * corresponding variables are set. See the superclass GenericValues
 * for details.
 */
class MonitorParameters : public GenericValues
{
  int    m_iColumns;             /**< number of columns in the grid           */
  int    m_iRows;                /**< number of rows in the grid              */
  int    m_iWindowWidth;         /**< width of the window                     */
  int    m_iWindowHeight;        /**< height of the window                    */
  int    m_iPort;                /**< port with which connection must be made */  
  char   m_strHost[128];         /**< host to which connection must be made   */
  int    m_iVisibleDistance;     /**< range (cells) in which objects are seen */
  bool   m_bShowNumber;          /**< indicates whether to show number        */    
  bool   m_bShowVisibleRange;    /**< indicates whether to show visible range */      
  bool   m_bDisplayStatistics;   /**< indicates whether to display the stats  */
  char   m_strBackgroundColor[7];/**< string representing background color    */
  char   m_strGridColor[7];      /**< string representing grid color (hex)    */
  char   m_strPreyColor[7];      /**< string representing prey color (hex)    */
  char   m_strPredatorColor[7];  /**< string representing predator color (hex)*/  
  char   m_strCaughtColor[7];    /**< string representing caught prey color   */  
  char   m_strCollisionColor[7]; /**< string representing collided pred. color*/      
  
public:
  MonitorParameters( );

  int    getColumns           (            ) const;
  bool   setColumns           ( int    i   );
  
  int    getRows              (            ) const;
  bool   setRows              ( int    i   );
  
  int    getWindowWidth       (            );
  bool   setWindowWidth       ( int    i   );

  int    getWindowHeight      (            );
  bool   setWindowHeight      ( int    i   );  

  int    getPort              (            );
  bool   setPort              ( int    i   );  

  char*  getHost              (            );
  bool   setHost              ( char * str );  

  int    getVisibleDistance   (            );
  bool   setVisibleDistance   ( int    i   );  

  bool   getShowNumber        (            );
  bool   setShowNumber        ( bool   b   );  

  bool   getShowVisibleRange  (            );
  bool   setShowVisibleRange  ( bool   b   );  

  bool   getDisplayStatistics (            );
  bool   setDisplayStatistics ( bool   b   );  

  char*  getBackgroundColor   (            );
  bool   setBackgroundColor   ( char*  str );

  char*  getGridColor         (            );
  bool   setGridColor         ( char*  str );

  char*  getPreyColor         (            );
  bool   setPreyColor         ( char*  str );

  char*  getPredatorColor     (            );
  bool   setPredatorColor     ( char*  str );

  char*  getCollisionColor    (            );
  bool   setCollisionColor    ( char*  str );

};

#endif



