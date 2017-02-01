/**
 * @file   LogPlayer.h
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Mon Aug 11 11:40:51 2003
 * 
 * @brief This file contains the definitions of the LogPlayer
 * class. This class stores all the world information and methods to
 * iterate over this information. Furthermore, it also handles the
 * interaction with the monitor(s).
 */
#ifndef _LOGPLAYER_
#define _LOGPLAYER_

#include "types.h"        // needed for StatusT
#include "LogWorld.h"     // needed for LogWorld 
#include "Connection.h"   // needed for Connection

/**
 *  This class is the actual body of the logplayer. It holds all the
 *  state information of the world and an iterator to the current
 *  frame. Furthermore it defines several methods to change the
 *  position of the iterator (either going directly to a frame or
 *  advance one frame. Finally it also handles the interaction with
 *  the monitor. */
class LogPlayer
{
  StatusT           m_status;                 /**< current status of player */
  list<LogWorld*>   m_World;                  /**< list of all world states */
  list<LogWorld*>::iterator m_iter_world;     /**< iterator to current frame*/
  list<Connection*> m_Monitors;               /**< list of all monitors     */
  bool              m_bHalt;                  /**< do we want to continue?  */

  char              m_strServerParam[MAX_MSG];/**< parameters for monitor   */
  int               m_iPort;                  /**< port number for monitors */
  int               m_iTimeStep;              /**< time waited between frames*/
    
  void              readFileContents     ( char *strFile );  
  void              acceptNewConnection  (               );
  void              receiveMessages      (               );
  void              processMonitorMessage( char* str     );  
public:

  LogPlayer();
  ~LogPlayer();
  
  void init( int iPort, char * strFile, StatusT );

  // callback functions that receive LogPlayer class as (void) argument;
  static void* thrAccept    ( void * v                 );
  static void* thrReceive   ( void * v                 );
  static void  exitHandler  ( int i,        void * v   );

  // methods related to walking through all possible world states
  StatusT      getStatus    (                           );
  void         setStatus    ( StatusT                   );
  void         mainLoop     (                           );
  void         goToNextFrame(                           );
  void         goToPrevFrame(                           );
  void         jumpToFrame  ( int  iEpisode, int iCycle );
  void         setHalt      ( bool b                    );

  // method to send current world frame information to all monitors
  void         sendInfoToMonitors();
  
  // methods related to exiting the program
  void         exitHandler            ( );  
  void         sendMonitorExitMessages( );  

};

#endif
