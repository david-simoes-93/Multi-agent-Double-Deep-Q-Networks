/**
 * @file   World.h
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Fri Aug  1 14:36:56 2003
 * 
 * @brief This file contains not only the World class which holds all
 * information of all objects currently in the game but also all
 * statistics of the game.
 */

#ifndef _WORLD_
#define _WORLD_

#include "Agent.h"     // needed for Agent
#include <list>        // needed for list
#include <pthread.h>   // needed for pthread_mutex_t
#include <fstream>     // needed for ofstream
#include <set>         // needed for set

/**
 * This typedef is used to iterate over a list of objects.
 */
typedef list<ConnectionObject*>::iterator object_iter ;

/**
 * This class contains all the information that is stored about the
 * world.  Most important are the agents that are currently located in
 * the world.
*/
class World
{
  Parameters   PS;             /**< parameters that influence the dynamics.  */
  list<ConnectionObject*> m_Objects;       /**< list of connected objects    */
  list<int>    m_lCycles;      /**< list of all nr of cycles to capture prey */
  bool         m_bHalt;        /**< indicates whether program should behalted*/
  bool         m_bCont;        /**< indicates whether program should continue*/
  bool         m_bStepByStep;  /**< should program run stepwise              */
  int          m_iNrPreyCaught;/**< number of preys captured in this episode */
  ofstream     m_osPos;        /**< output file for position information     */
  int          m_iNrEpisode;   /**< episode number                           */
  int          m_iNrCycle;     /**< cycle number in the current episode      */
  int          m_iNrCollisions;/**< total number of collisions               */
  int          m_iNrPenalties; /**< total number of penalties                */
  double       m_dAvgCapture;  /**< average capture time.                    */
  
  // only used in synchronization mode, where we immediately continue when all
  // message of a specific agent type are received.
  pthread_mutex_t mutex_newInfo;/**< mutex to protect m_bReadySync variable  */
  pthread_cond_t  cond_newInfo; /**< condition var t protect m_bReadySync var*/
  bool m_bPredatorSync;         /**< are we waiting for predator messages    */
  bool m_bReadySync;            /**< true when all messages been received    */
 
  public:
    World( );
    
    // methods that deal with the update of the field
    void    initAgent          ( Agent            *agent                     );
    void    initField          (                                             );
    void    updateField        ( ObjectT          type                       );
    Pos     getPosAfterAction  ( Pos              pos,
                                 ActionT          action                     );
    int     getShortestDistance( Pos              pos1, 
                                 Pos              pos2                       );
    void    checkCollisions    ( ObjectT          type                       );
    bool    isPosOccupied      ( Pos              pos,
                                 ObjectT          type = OBJ_AGENTS,
                                 int *            iAgent = NULL              );
    bool    isAllPreyCaught    (                                             );
    bool    updatePreyCaught   (                                             );
    bool    isPreyCaughtMethod1( set<int>         *s                         );
    bool    isPreyCaughtMethod2( set<int>         *s                         );
    bool    isPreyCaughtMethod3( set<int>         *s                         );
    bool    isPreyCaughtMethod4( set<int>         *s                         );
    void    penalizeAgent      ( Agent            *agent,
                                 int              iPenalizedAgentNr, 
                                 bool             bReplace = true            );

    // methods that deal with the handling of messages from the clients
    void    acceptNewConnection(                                             );
    ObjectT processInitMessage ( char*            str                        );
    bool    addObject          ( const port_type& port, const host_type& host,
                                 ObjectT          type                       );
    void    sendVisuals        ( ObjectT          type                       );
    void    sendSendAction     ( ObjectT          type                       );
    void    communicate        ( ConnectionObject *agent,
                                 char *           str                        );
    void    broadcast          ( ObjectT          type,
                                 char *           str,
                                 bool             bIncludeCaught = false     );
    void    resetSendMsg       ( ObjectT          objType                    );
    void    sendInfoToMonitors (                                             );
    void    sendExitMessages   (                                             );
    void    receiveMessages    (                                             );
    void    processMonitorMessage( char *         str                        );
    void    makeServerParamMsg ( char *           str                        );

    // mainloops
    void    mainLoop           (                                             );
    void    mainLoop           ( ObjectT          objType                    );

    // methods that deal with the iteration over the different agents
    ConnectionObject*  iterateStart( object_iter      & iter,  
                                     ObjectT          type,
                                     bool             bIncludeCaught = false );
    ConnectionObject*  iterateNext ( object_iter      & iter,  
                                     ObjectT          type, 
                                     bool             bIncludeCaught = false );
    void    iterateEnd             ( object_iter      & iter                 );

    // some simple get and set methods for the different variables
    void    setHalted            ( bool  b   );
    bool    isHalted             (           );
    void    setContinue          ( bool  b   );
    bool    getStepByStep        (           );
    void    setStepByStep        ( bool  b   );
    void    setOutputStream      ( char* str );
    void    closeOutputStream    (           );
    int     getNrCycle           (           );
    int     getNrEpisode         (           );
    int     getNrCollisions      (           );
    int     getNrPenalties       (           );  
    double  getAverageCaptureTime(           );

    // log methods
    void    createInfoString( char *str                              );
    void    logObjectInfo   (                                        );    
    void    logRecvAction   ( ConnectionObject * agent, char* str    );

    // read in parameter values from file 
    bool    readValues      ( char *file,               char* strSep );

} ;

#endif
