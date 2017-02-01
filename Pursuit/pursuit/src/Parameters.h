/**
 * @file   Parameters.h
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Fri Aug  1 14:34:23 2003
 * 
 * @brief This file contains the parameters used in this
 * program. These can be read from the configuration file and are
 * stored as generic values.
 * 
 * 
 */

#ifndef _PARAMETERS_
#define _PARAMETERS_

#include "GenericValues.h"
//#include "Agent.h"         

/*****************************************************************************/
/********************** CLASS PARAMETERS *************************************/
/*****************************************************************************/

/**
 * This class contains all parameters that can be configured in the
 * Pursuit domain. Each parameters is associated with a text
 * string. When these strings are read from configuration file, the
 * corresponding variables are set. See the superclass GenericValues
 * for details.
 */
class Parameters : public GenericValues
{
  int    m_iColumns;             /**< number of columns in the grid          */
  int    m_iRows;                /**< number of rows in the grid             */
  int    m_iPort;                /**< port with which connection must be made*/
  int    m_iTimeStep;            /**< time (msec) waited after each update   */
  int    m_iNrEpisodes;          /**< nr of episodes before quitting program */
  int    m_iNextEpisodeWait;     /**< time (msec) waited before new episode  */
  int    m_iVisibleDistance;     /**< range (cells) in which objects are seen*/
  int    m_iCaptureMethod;       /**< integer representing capture methodprey*/
  bool   m_bAllowComm;           /**< indicates if agents can communicate    */
  bool   m_bAllowDiagonalPrey;   /**< indicates if prey can move diagonal    */
  bool   m_bAllowDiagonalPred;   /**< indicates if predator can move diagonal*/
  bool   m_bLogObjectInfo;       /**< indicates if object info is logged     */
  char   m_strLogObjectFile[128];/**< indicates object info is written to    */
  bool   m_bAutomaticStart;      /**< indicates if simulator start automatic */
  char   m_strOutputFile[128];   /**< file to which output is directed       */
  bool   m_bPenalizeAll;         /**< penalize all when one makes a mistake? */
  bool   m_bUseIds;              /**< use ids in communication messages      */
  int    m_iAgent1PredPosX;      /**< fixed x starting coordinate agent 1    */
  int    m_iAgent1PredPosY;      /**< fixed y starting coordinate agent 1    */
  int    m_iAgent2PredPosX;      /**< fixed x starting coordinate agent 2    */
  int    m_iAgent2PredPosY;      /**< fixed y starting coordinate agent 2    */
  int    m_iAgent3PredPosX;      /**< fixed x starting coordinate agent 3    */
  int    m_iAgent3PredPosY;      /**< fixed y starting coordinate agent 3    */
  int    m_iAgent4PredPosX;      /**< fixed x starting coordinate agent 4    */
  int    m_iAgent4PredPosY;      /**< fixed y starting coordinate agent 4    */
  int    m_iAgent1PreyPosX;      /**< fixed x starting coordinate agent 1    */
  int    m_iAgent1PreyPosY;      /**< fixed y starting coordinate agent 1    */
  int    m_iAgent2PreyPosX;      /**< fixed x starting coordinate agent 2    */
  int    m_iAgent2PreyPosY;      /**< fixed y starting coordinate agent 2    */
  

  
public:
  Parameters( );

  int    getColumns           (            ) const;
  bool   setColumns           ( int    i   );
  
  int    getRows              (            ) const;
  bool   setRows              ( int    i   );
  
  int    getPort              (            );
  bool   setPort              ( int    i   );  

  int    getTimeStep          (            );
  bool   setTimeStep          ( int    i   );  

  int    getNrEpisodes        (            );
  bool   setNrEpisodes        ( int    i   );  

  int    getNextEpisodeWait   (            );
  bool   setNextEpisodeWait   ( int    i   );  

  int    getVisibleDistance   (            );
  bool   setVisibleDistance   ( int    i   );  

  int    getCaptureMethod     (            );
  bool   setCaptureMethod     ( int    i   );  

  bool   getAllowDiagonalPrey (            );
  bool   setAllowDiagonalPrey ( bool   b   );  

  bool   getAllowDiagonalPred (            );
  bool   setAllowDiagonalPred ( bool   b   );  

  bool   getAllowComm         (            );
  bool   setAllowComm         ( bool   b   );  

  bool   getLogObjectInfo     (            );
  bool   setLogObjectInfo     ( bool   b   );
  
  char*  getLogObjectFile     (            );
  bool   setLogObjectFile     ( char*  str );

  bool   getAutomaticStart    (            );
  bool   setAutomaticStart    ( bool   b   );

  char*  getOutputFile        (            );
  bool   setOutputFile        ( char*  str );

  bool   getPenalizeAll       (            );
  bool   setPenalizeAll       ( bool   b   );

  bool   getUseIds            (            );
  bool   setUseIds            ( bool   b   );

  int    getAgent1PredPosX    (            );
  bool   setAgent1PredPosX    ( int    i   );

  int    getAgent1PredPosY    (            );
  bool   setAgent1PredPosY    ( int    i   );

  int    getAgent2PredPosX    (            );
  bool   setAgent2PredPosX    ( int    i   );

  int    getAgent2PredPosY    (            );
  bool   setAgent2PredPosY    ( int    i   );

  int    getAgent3PredPosX    (            );
  bool   setAgent3PredPosX    ( int    i   );

  int    getAgent3PredPosY    (            );
  bool   setAgent3PredPosY    ( int    i   );

  int    getAgent4PredPosX    (            );
  bool   setAgent4PredPosX    ( int    i   );

  int    getAgent4PredPosY    (            );
  bool   setAgent4PredPosY    ( int    i   );

  int    getAgent1PreyPosX    (            );
  bool   setAgent1PreyPosX    ( int    i   );

  int    getAgent1PreyPosY    (            );
  bool   setAgent1PreyPosY    ( int    i   );

  int    getAgent2PreyPosX    (            );
  bool   setAgent2PreyPosX    ( int    i   );

  int    getAgent2PreyPosY    (            );
  bool   setAgent2PreyPosY    ( int    i   );

};


#endif
