/**
 * @file   Monitor.h
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Thu Aug  7 17:35:36 2003
 * 
 * @brief This file contains the definitions for the different classes
 * that contain the information about the current world
 * state. Furthermore it defines several methods to visualize the
 * information using the GLUT program.
 */

#ifndef _MONITOR_
#define _MONITOR_

#include <GL/glut.h>     // needed for GLUT related methods
#include <GL/gl.h>       // needed for GLUT related methods
#include <list>          // needed for list

/**
 * Maximum number of agents separately for preys and
 * predators. Maximum number of agents is thus 100 (50 preys and 50
 * predators).
 */
#define MAX_AGENTS  50
/**
 * Maximum number of cells in a row or a column. max size is thus 100x100.
 */
#define MAX_CELLS  100    // maximum number of cells in a row or column

/** This class contains the information that is needed for each agent
 *  to display him properly. No functionality is added to this class.
 */
class Agent 
{
public:
  int     m_iX;         /**< x coordinate of the agent                       */
  int     m_iY;         /**< y coordinate of the agent                       */
  int     m_iNr;        /**< number of this agent                            */
  bool    m_bCollided;  /**< has this agent been collided with another agent */
  bool    m_bCaught;    /**< indicates whether this agent has been caught    */
  
  Agent();
};

/** This class contains the information about the world that needs to
 *  be displayed. The values are only stored, no functionality is
 *  defined upon them.
 */
class MonitorWorld
{
public:

  Agent   m_Predators[MAX_AGENTS]; /**< all predator information             */
  Agent   m_Preys[MAX_AGENTS];     /**< all prey information                 */
  int     m_iNrPredators;          /**< total number of predators            */
  int     m_iNrPreys;              /**< total number of preys                */
  int     m_iNrCollisions;         /**< total number of collisions           */
  int     m_iNrPenalties;          /**< total number of illegal captures     */
  int     m_iNrCycle;              /**< cycle number in this episode         */
  int     m_iNrEpisode;            /**< episode number                       */
  double  m_dAverageCaptureTime;   /**< average capture time until now       */
  bool    m_bRedisplay;            /**< does world needs to be redisplayed   */
  
  MonitorWorld();
};
  
// general GLUT-related functions 
void display         (                                       );
void reshape         ( int width       , int    height       );
void idle            ( int                                   );
void menu            ( int i                                 );
void callBackKeyBoard( unsigned char ch, int    x,     int y );

// message processing functions
void* receiveMessages      ( void*                        );
bool  processIncomingMessge( char *str                    );  
bool  readServerParam      ( char *strParam, char *strMsg );
                 
// option related functions                                
void readOptions( int argc, char ** argv ); // read command options
void showOptions(                        ); // show command options

#endif

