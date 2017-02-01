/**
 * @file   LogWorld.C
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Mon Aug 11 13:29:52 2003
 * 
 * @brief This file contains the definitions of the Agent and the
 * LogWorld class. The first contains the information of one specific
 * agent on the grid, the second holds one frame of the world state
 * with all containing agents. It thus contains the agents (prey and
 * position) positions and the statistics of that frame.
 */
#include "LogWorld.h"  
#include "types.h"  // needed for MAX_MSG
#include <stdio.h>  // needed for sprintf
#include <string.h> // needed for strcat
#include <iostream> // needed for endl

using namespace std;

/**
 *  This constructors sets all the member variables using the supplied
 *  arguments.
 * @param isPred bool indicating whether this is a predator or not
 * @param iNr id of this agent
 * @param iX current x position of this agent
 * @param iY current y position of this agent
 * @param bCollided bool indicating whether this agent is currently collided
 * @param bCaught bool indicating whether this agent is currently caught
 */
Agent::Agent( bool isPred, int iNr, int iX, int iY, 
              bool bCollided, bool bCaught )
{
  m_iX        = iX;
  m_iY        = iY; 
  m_iNr       = iNr; 
  m_bPredator = isPred;
  m_bCollided = bCollided;
  m_bCaught   = bCaught;
}

/** 
 * Return the x coordinate of this agent.
 * 
 * @return x coordinate of this agent
 */
int Agent::getX()
{ 
  return m_iX;        
}

/** 
 * Return the y coordinate of this agent.
 * 
 * @return y coordinate of this agent
 */
int Agent::getY()
{ 
  return m_iY;        
}

/** 
 * Return the id  of this agent.
 * 
 * @return id of this agent
 */
int  Agent::getNr()
{
  return m_iNr;       
}
    
/** 
 * Return whether this agent is a predator
 * 
 * @return bool indicating whether this agent is a predator
 */
bool Agent::isPred()
{ 
  return m_bPredator; 
}

/** 
 * Return whether this agent collided
 * 
 * @return bool indicating whether this agent is collided
 */
bool Agent::isCollided() 
{ 
  return m_bCollided; 
}

/** 
 * Return whether this agent is caught
 * 
 * @return bool indicating whether this agent is caught
 */
bool Agent::isCaught()   
{ 
  return m_bCaught;   
}


/**
 * This methods prints the information about this agent to the
 * specified output stream. 
 * @param os output stream
 */
void  Agent::show( ostream & os )
{
  os << " " << (isPred() ? "predator " : "prey ") << getNr() << " " 
     << getX() << " " << getY() << endl;
}

/**
 *  This destructor removes all agent information from memory. 
 */
LogWorld::~LogWorld( )
{
  list<Agent*>::iterator iter ;  
  iter = m_Agents.begin();
  while( ! m_Agents.empty() )
  {
    delete *iter;                                // delete him
    iter = m_Agents.erase( iter );
  }  
}

/**
 * This method adds all the supplied statistics to this LogWorld.
 * @param iEpisode this episode number
 * @param iCycle this cycle number
 * @param iPenalties total number of penalties until now
 * @param iPenalties total number of collisions until now
 * @param dAverageCaptureTime average capture time until now 
 */
void LogWorld::addStats( int iEpisode, int iCycle, int iPenalties, 
                         int iCollisions, double dAverageCaptureTime )
{
  m_iNrEpisode          = iEpisode;
  m_iNrCycle            = iCycle;
  m_iNrPenalties        = iPenalties;
  m_iNrCollisions       = iCollisions;
  m_dAverageCaptureTime = dAverageCaptureTime;
}


/** 
 * This method returns the number of collisions until now. 
 * 
 * @return number of collisions
 */
int LogWorld::getNrCollisions()       
{ 
  return m_iNrCollisions;      
}

/** 
 * This method returns the number of penalties until now. 
 * 
 * @return number of penalties
 */
int LogWorld::getNrPenalties()        
{ 
  return m_iNrPenalties;        
}  

/** 
 * This method returns the current cycle number
 * 
 * @return current cycle number
 */
int LogWorld::getNrCycle()            
{ 
  return m_iNrCycle;            
}    

/** 
 * This method returns the current episode number
 * 
 * @return current episode number
 */
int LogWorld::getNrEpisode()         
{ 
  return m_iNrEpisode;          
}      

/** 
 * This method returns the average capture time until now
 * 
 * @return the average capture time until now
 */
double LogWorld::getAverageCaptureTime()
{ 
  return m_dAverageCaptureTime; 
}

/**
 * This method adds an agent to the world. All the information
 * related to this agent are supplied as arguments. 
 * @param isPred bool indicating whether this is a predator or not
 * @param iNr id of this agent
 * @param iX current x position of this agent
 * @param iY current y position of this agent
 * @param bCollided bool indicating whether this agent is currently collided
 * @param bCaught bool indicating whether this agent is currently caught
 */
void LogWorld::addAgent( bool isPred, int iNr, int iX, int iY, 
                         bool bCollided, bool bCaught )
{
  Agent * agent = new Agent( isPred, iNr, iX, iY, bCollided, bCaught );
  m_Agents.push_back( agent );
}

/**
 * This method prints all information of this world state to the
 * specified output stream. 
 * @param os output stream 
 */
void  LogWorld::show( ostream & os )
{
  os << "EPISODE " << getNrEpisode() << " CYCLE " << getNrCycle() << endl;

  list<Agent*>::iterator iter ;  
  iter = m_Agents.begin();
  while( iter != m_Agents.end() )
  {
    (*iter)->show( cout );
    iter++;
  }  
}

/**
 * This method creates an information string from the current world
 * state that is understood by the monitor. 
 * @param str will be filled with information string
 */
void LogWorld::createInfoString( char *str )
{
  static char strTmp[MAX_MSG];

  // add statistics information.
  sprintf( str, "(world (stats %d %d %f %d %d)",
                      getNrEpisode(),
                      getNrCycle(),
                      getAverageCaptureTime(),
                      getNrPenalties(),
                      getNrCollisions() );

  // add all agent information
  list<Agent*>::iterator iter ;  
  iter = m_Agents.begin();
  while( iter != m_Agents.end() )
  {
     Agent * agent = *iter;
     sprintf( strTmp, " ((%s %d%s) %d %d)",
                 (agent->isPred()) ? "predator" : "prey",
                  agent->getNr(),
                 (agent->isCaught() ? "x" : agent->isCollided() ? "c" : " " ), 
                  agent->getX(),
                  agent->getY() );
     strcat( str, strTmp );

    iter++;
    
  }  
}
