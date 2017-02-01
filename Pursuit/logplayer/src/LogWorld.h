/**
 * @file   LogWorld.h
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Mon Aug 11 13:29:52 2003
 * 
 * @brief This file contains the declarations of the LogWorld class
 * which holds one frame of the world state. It thus contains the
 * agents (prey and position) positions and the statistics of that
 * frame.
 */
#ifndef _LOGWORLD_
#define _LOGWORLD_

#include <list>      // needed for list
#include <iostream>      // needed for ostream

using namespace std;

/**
 *  This class contains the information that is needed for each agent
 *  to display him properly at one particular episode,
 *  cycle. Furthermore it contains several statistics about this
 *  state. No functionality is added to this class. 
 */
class Agent 
{
  int     m_iX;         /**< x coordinate of the agent                       */
  int     m_iY;         /**< y coordinate of the agent                       */
  int     m_iNr;        /**< number of this agent                            */
  bool    m_bPredator;  /**< indicates whether this agent is a predator      */
  bool    m_bCollided;  /**< has this agent been collided with another agent */
  bool    m_bCaught;    /**< indicates whether this agent has been caught    */

public:
  Agent( bool isPred, int iNr, int iX, int iY, bool bCollided, bool bCaught );
  
  // standard get methods. 
  int     getX      (        );
  int     getY      (        );      
  int     getNr     (        );         
  bool    isPred    (        );    
  bool    isCollided(        );
  bool    isCaught  (        );  
  
  void    show( ostream & os );
};

/**
 *  This class contains the information about the world that needs to
 *  be displayed. them.
 */
class LogWorld
{
  list<Agent*> m_Agents;           /**< all agent information                */
  int     m_iNrCollisions;         /**< total number of collisions           */
  int     m_iNrPenalties;          /**< total number of illegal captures     */
  int     m_iNrCycle;              /**< cycle number in this episode         */
  int     m_iNrEpisode;            /**< episode number                       */
  double  m_dAverageCaptureTime;   /**< average capture time until now       */
  
public:
  ~LogWorld( );  
  
  // standard get methods.
  int     getNrCollisions      ();      
  int     getNrPenalties       ();        
  int     getNrCycle           ();               
  int     getNrEpisode         ();               
  double  getAverageCaptureTime();

  // add information.
  void    addAgent( bool isPred,     int    iNr,          int  iX,  
                    int  iY,         bool   bCollided,    bool bCaught);
  void    addStats( int  iEpsiode,   int    iCycle,       int  iPenalties, 
                    int iCollisions, double dAverageCaptureTime );
  
  // display methods
  void    createInfoString( char *str )    ;
  void    show( ostream & os );
};
  
#endif
