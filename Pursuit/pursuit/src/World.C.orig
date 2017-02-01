/**
 * @file   World.C
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Fri Aug  1 15:41:38 2003
 * 
 * @brief This file contains the World class which contains all
 * functionality to model the agents and handles all message
 * communication with the objects.
 * 
 * 
 */

#include "World.h"
#include "Parameters.h"
#include <stdlib.h>     // needed for drand48
#include <string.h>     // needed for strncmp
#include <sys/time.h>   // needed for getttimeofday
#include <unistd.h>     // needed for usleep
#include <fstream>      // needed for ofstream
#include <iomanip>      // needed for width
      
/**
 * This constructors initializes all the member variables. 
 */
World::World( )
{
  // initialize the random seed
  timeval tv_now;                 
  gettimeofday( &tv_now, NULL );
  srand48( tv_now.tv_usec );

  m_iNrPreyCaught = 0;
  m_bCont         = false;
  m_bHalt         = false;
  m_bStepByStep   = false;
  m_iNrCycle      = 1;
  m_iNrEpisode    = 1;
  m_iNrCollisions = 0;
  m_iNrPenalties  = 0;
  m_dAvgCapture   = 0.0;

  // only used in synchronization mode
  pthread_mutex_init( &mutex_newInfo, NULL );
  pthread_cond_init ( &cond_newInfo, NULL );

  //  char *id="$Id: World.C,v 1.2 2005/06/01 10:15:47 jellekok Exp $";
  //  cerr << id << endl;
  cerr << "Starting pursuit server" << endl;
  
}

/**
 * This method initializes the field. The 'initAgent' method is called
 * for all connected agents. 
 */
void World::initField( )
{
  object_iter iter; 

  // last argument is true thus also include caught agents (= caught preys)
  for( Agent * agent = (Agent*)iterateStart( iter, OBJ_AGENTS, true ); 
       agent != NULL;
       agent = (Agent*)iterateNext( iter, OBJ_AGENTS, true  ) )
    initAgent( agent );
  iterateEnd( iter );

  m_iNrPreyCaught = 0;
  sendInfoToMonitors( );
}

/**
 * This method intializes an agent by placing it on a random position
 * on the field. If this position is already occupied a new random
 * position is selected. 
 *
 * @param agent pointer to an agent object which is initialized. 
 */
void World::initAgent( Agent *agent )
{
  Pos pos;
  int i = 0;
  while( i == 0 || ( isPosOccupied( pos ) && i < 10 ))
  {
    pos.x = -1;
    pos.y = -1;

    // check for fixed position defined in Parameters
    if( agent->getType() == OBJ_PREDATOR )
    {
      if( agent->getNumber() == 1 &&
          PS.getAgent1PredPosX() >= 0 )
      {
        pos.x = PS.getAgent1PredPosX();
        pos.y = PS.getAgent1PredPosY();
      }
      else if( agent->getNumber() == 2 &&
               PS.getAgent2PredPosX() >= 0 )
      {
        pos.x = PS.getAgent2PredPosX();
        pos.y = PS.getAgent2PredPosY();
      }
      else if( agent->getNumber() == 3 &&
               PS.getAgent3PredPosX() >= 0 )
      {
        pos.x = PS.getAgent3PredPosX();
        pos.y = PS.getAgent3PredPosY();
      }
      else if( agent->getNumber() == 4 &&
               PS.getAgent4PredPosX() >= 0 )
      {
        pos.x = PS.getAgent4PredPosX();
        pos.y = PS.getAgent4PredPosY();
      }
    }
    else if( agent->getType() == OBJ_PREY )
    {
      if( agent->getNumber() == 1 &&
          PS.getAgent1PreyPosX() >= 0 )
      {
        pos.x = PS.getAgent1PreyPosX();
        pos.y = PS.getAgent1PreyPosY();
      }
      else if( agent->getNumber() == 2 &&
               PS.getAgent2PreyPosX() >= 0 )
      {
        pos.x = PS.getAgent2PreyPosX();
        pos.y = PS.getAgent2PreyPosY();
      }
    }

    if( pos.x < 0 || pos.x >= PS.getColumns() )
      pos.x = (int)(drand48()*PS.getColumns() );
    if( pos.y < 0 || pos.y >= PS.getRows() )
      pos.y = (int)(drand48()*PS.getRows   () );
    i++;
  }
  if( i == 10 )
    cerr << "(World::initAgent) couldn't find empty spot" << endl;

  agent->setPosition( pos );
  agent->setCaught( false );    
}

/**
 * This method checks whether the specified position 'pos' is already
 * occupied by an agent of type 'type'. When 'type' equals OBJ_AGENTS,
 * all agents are taken into consideration. 
 *
 * @param pos position that should be checked
 * @param type agent types that should be considered
 * @param agent is non-NULL will be filled with Agent that occupies this pos
 *
 * @return bool indicating wether the specified position is occupied or not
 */
bool World::isPosOccupied( Pos pos, ObjectT type, int *iAgentOnPos )
{
  object_iter iter;
  for( Agent * agent = (Agent*)iterateStart( iter, type );
       agent != NULL;
       agent = (Agent*)iterateNext( iter, type  ) )
  {
     if( agent->getPos( ) == pos )
     {
       if( iAgentOnPos != NULL )
         *iAgentOnPos = agent->getNumber();
       iterateEnd( iter );
       return true;
     }
  } 
  iterateEnd( iter );
  return false;
}

/**
 * This methods checks whether all prey are captured. 
 *
 * @return bool indicating whether all preys are captured
 */
bool World::isAllPreyCaught( )
{
  object_iter iter;
  
  // walk through all prey; return false when prey is found that is not caught 
  for( Agent * agent = (Agent*)iterateStart( iter, OBJ_PREY );
       agent != NULL;
       agent = (Agent*)iterateNext( iter, OBJ_PREY ) )
  {
     if( agent->isCaught( ) == false )
       return false;
  } 
  iterateEnd( iter );

  // when there is no prey connected, do not return true
  return m_iNrPreyCaught > 0;
}

/**
 * This method checks whether a prey is caught using the capture
 * method that is specified in the Parameters class. 
 *
 * @return bool indicating whether a prey has been caught. 
 */
bool World::updatePreyCaught( )
{
  bool bReturn = false;
  set<int> s; // gets filled with agent numbers that perform capture

  switch( PS.getCaptureMethod() )
  {  
    case 1: bReturn = isPreyCaughtMethod1( &s ); break;
    case 2: bReturn = isPreyCaughtMethod2( &s ); break;
    case 3: bReturn = isPreyCaughtMethod3( &s ); break;
    case 4: bReturn = isPreyCaughtMethod4( &s ); break;
    default: break;
  }

  if( bReturn == true )
  {
    object_iter iterPred;
    for( Agent * agentPred = (Agent*)iterateStart(  iterPred, OBJ_PREDATOR );
         agentPred != NULL;
         agentPred = (Agent*)iterateNext( iterPred, OBJ_PREDATOR  ) )
    {
      // if predator involved or all predators are informed
      if( s.find( agentPred->getNumber() ) != s.end() || PS.getPenalizeAll() )
      {
        // just sent the message in case no ids are used
        if( PS.getUseIds() == false )
          agentPred->sendMessage( "(referee prey_caught)" );
        else
        {
          // otherwise sent this agent all ids of involved agents
          set<int>::iterator iter;
          for( iter = s.begin(); iter != s.end(); iter++ )
          {
            char strTmp[MAX_MSG];
            sprintf( strTmp, "(referee prey_caught (predator %d))", *iter );
            agentPred->sendMessage( strTmp );
          }
        }
      }
    }
    
    m_iNrPreyCaught++;
  }
  return bReturn;
}
 
/**
 * This method returns whether a prey is captured according to the
 * first capture method. In this method a prey is captured when its
 * cell is also occupied by a predator. 
 *
 * @param s will be filled with predators involved in capture
 * @return bool indicating whether one or more preys are caught 
 */
bool World::isPreyCaughtMethod1( set<int> *s  )
{
  object_iter iter;
  bool        bReturn = false;
  int         iAgentOnPos ;

  for( Agent * agentPrey = (Agent*)iterateStart(  iter, OBJ_PREY ); 
       agentPrey != NULL;
       agentPrey = (Agent*)iterateNext( iter, OBJ_PREY  ) )
  {
    if( isPosOccupied( agentPrey->getPos(), OBJ_PREDATOR, &iAgentOnPos ) )
    {
      s->insert( iAgentOnPos );
      agentPrey->setCaught( true );
      bReturn = true;
    }
  }
  iterateEnd( iter );                    

  return bReturn;
}

/**
 * This method returns whether a prey is captured according to the
 * second capture method. In this method a prey is captured when its
 * cell is surrounded by four predators. That is, a predator is
 * located north, south, east and west of the prey. 
 *
 * @param s will be filled with predators involved in capture
 * @return bool indicating whether one or more preys are caught 
 */
bool World::isPreyCaughtMethod2( set<int> *s )
{
  object_iter iter;
  bool        bReturn = false;
  set<int>    s2;
  int         iAgentOnPos;

  for( Agent * agentPrey = (Agent*)iterateStart(  iter, OBJ_PREY );
       agentPrey != NULL;
       agentPrey = (Agent*)iterateNext( iter, OBJ_PREY  ) )
  {
    Pos posPrey      = agentPrey->getPos();
    int iNrPredators = 0;
  
    // add one to iNrPredators for each location n,s,e and w with a predator
    if(isPosOccupied(getPosAfterAction(posPrey,ACT_MOVE_NORTH),
                     OBJ_PREDATOR, &iAgentOnPos))
    {
      iNrPredators++;
      s2.insert( iAgentOnPos );
    }
    if(isPosOccupied(getPosAfterAction(posPrey,ACT_MOVE_SOUTH),
                     OBJ_PREDATOR, &iAgentOnPos))
    {
      iNrPredators++;
      s2.insert( iAgentOnPos );
    }
    if(isPosOccupied(getPosAfterAction(posPrey,ACT_MOVE_EAST),
                     OBJ_PREDATOR, &iAgentOnPos))
    {
      iNrPredators++;
      s2.insert( iAgentOnPos );
    }
    if(isPosOccupied(getPosAfterAction(posPrey,ACT_MOVE_WEST),
                     OBJ_PREDATOR, &iAgentOnPos))
    {
      iNrPredators++;
      s2.insert( iAgentOnPos );
    }
    // at least four positions should be occupied
    if( iNrPredators >= 4 )
    {
      agentPrey->setCaught( true );
      s->insert( s2.begin(), s2.end() );
      bReturn = true;      
    }
  }
  iterateEnd( iter );                    

  return bReturn;
}

/**
 * This method returns whether a prey is captured according to the
 * third capture method. In this method a prey is captured when its
 * cell is surrounded by two predators. 
 *
 * @param s will be filled with predators involved in capture
 * @return bool indicating whether one or more preys are caught 
 */
bool World::isPreyCaughtMethod3( set<int> *s )
{
  object_iter  iterPrey;
  bool         bReturn = false;
  set<int>     s2;
  int          iAgentOnPos;
   
  for( Agent * agentPrey = (Agent*)iterateStart(  iterPrey, OBJ_PREY );
       agentPrey != NULL;
       agentPrey = (Agent*)iterateNext( iterPrey, OBJ_PREY  ) )
  {
    Pos posPrey      = agentPrey->getPos();
    int iNrPredators = 0;
    int iPenalizedNr = -1;
    // check whether there is one agent that deserves a penalty
    bool bPenalize = false;
    object_iter iterPred;
    for( Agent * agentPred = (Agent*)iterateStart(  iterPred, OBJ_PREDATOR );
         agentPred != NULL;
         agentPred = (Agent*)iterateNext( iterPred, OBJ_PREDATOR  ) )
    {
       if( agentPred->getPos() == posPrey )
       {
         bPenalize = true;
         iPenalizedNr = agentPred->getNumber();
       }
    }

    //  penalize all agents that are moved onto the prey
    if( bPenalize == true )
    {
      for( Agent * agentPred = (Agent*)iterateStart(  iterPred, OBJ_PREDATOR );
           agentPred != NULL;
           agentPred = (Agent*)iterateNext( iterPred, OBJ_PREDATOR  ) )
      {
        if( agentPred->getPos() == posPrey )
          penalizeAgent( agentPred, iPenalizedNr );
        else if( PS.getPenalizeAll() == true ) // when all agents should recv 
          penalizeAgent( agentPred, iPenalizedNr, false ); //  do not replace 
      }
    }

    // add one to iNrPredators for each location n,s,e and w with a predator  
    if(isPosOccupied(getPosAfterAction(posPrey,ACT_MOVE_NORTH),
                     OBJ_PREDATOR, &iAgentOnPos))
    {
      iNrPredators++;
      s2.insert( iAgentOnPos );
    }
    if(isPosOccupied(getPosAfterAction(posPrey,ACT_MOVE_SOUTH),
                     OBJ_PREDATOR, &iAgentOnPos))
    {
      iNrPredators++;
      s2.insert( iAgentOnPos );
    }
    if(isPosOccupied(getPosAfterAction(posPrey,ACT_MOVE_EAST),
                     OBJ_PREDATOR, &iAgentOnPos))
    {
      iNrPredators++;
      s2.insert( iAgentOnPos );
    }
    if(isPosOccupied(getPosAfterAction(posPrey,ACT_MOVE_WEST),
                     OBJ_PREDATOR, &iAgentOnPos))
    {
      iNrPredators++;
      s2.insert( iAgentOnPos );
    }

    // at least two positions should be occupied  
    if( iNrPredators >= 2 )
    {
      agentPrey->setCaught( true );
      s->insert( s2.begin(), s2.end() );
      bReturn = true;      
    }
  }
  iterateEnd( iterPrey );                    
  return bReturn;
}

/**
 * This method returns whether a prey is captured according to the
 * fourth capture method. In this method a prey is captured when a
 * prey was surrounded by two predators in the previous turn, but only
 * one went for the prey.  If both went for the prey, both predators
 * are penalized and put in random positions. If one went for the
 * prey, but in the previous turn there wasn't a predator to back him
 * up, the predator that went for the prey is penalized and put on a
 * random position in the field.  
 *
 * @param s will be filled with predators involved in capture
 * @return bool indicating whether one or more preys are caught 
 */
bool World::isPreyCaughtMethod4( set<int> *s )
{
  object_iter  iterPrey, iterPred, iterPred2;
  bool         bReturn = false;
  bool         bSlavePred = false;  // the guy who stayed fixed
  bool         bMasterPred = false; // the guy who went for the prey
  Agent       *agentMaster, *agentSlave;  // who went for the prey
  agentMaster = NULL;

  // determine whether a prey, surrounded in the previous turn, is caught
  for( Agent * agentPrey = (Agent*)iterateStart(  iterPrey, OBJ_PREY );
       agentPrey != NULL;
       agentPrey = (Agent*)iterateNext( iterPrey, OBJ_PREY  ) )
  {
    bMasterPred = false; bSlavePred = false;
    agentMaster = agentSlave = NULL;
    Pos posPrey      = agentPrey->getPos();

    for( Agent * agentPred = (Agent*)iterateStart(  iterPred, OBJ_PREDATOR );
         agentPred != NULL;
         agentPred = (Agent*)iterateNext( iterPred, OBJ_PREDATOR  ) )
    {
      // if predator is at prey position, a predator went for the kill
      // but there also has to be a slave that was at a distance of 1 in the
      // previous cycle to the current prey position and is still there
      if( posPrey == agentPred->getPos() )      
      {
        agentMaster = agentPred;
        bMasterPred = true;
      }
      else if( getShortestDistance( agentPred->getPrevPos(),
                                    agentPrey->getPos() ) == 1 &&
               getShortestDistance( agentPred->getPos(), 
                                    agentPrey->getPos() ) == 1 )
      {
        agentSlave = agentPred;
        bSlavePred = true;
      }
    }
    iterateEnd( iterPred );
    if( bMasterPred == true &&  bSlavePred == true )
    {
      s->insert( agentSlave->getNumber() );
      s->insert( agentMaster->getNumber() );
      agentPrey->setCaught( true );
      bReturn = true;      
    }
    else if( agentMaster != NULL )  // not caught, but this predator is on prey
    {
      penalizeAgent( agentMaster, agentMaster->getNumber() );
      if( PS.getPenalizeAll () == true )
      {        
        for( Agent * agentPred2 = 
               (Agent*)iterateStart( iterPred2, OBJ_PREDATOR );
             agentPred2 != NULL;
             agentPred2 = (Agent*)iterateNext( iterPred2, OBJ_PREDATOR  ) )
        {
          if( agentPred2 != agentMaster ) // penalize all others
            penalizeAgent( agentPred2, agentMaster->getNumber(), false ); 
        }
      }
    }  
  }
  iterateEnd( iterPrey );    
  
  return bReturn;  
}

/**
 * This method penalizes an agent. Messages are logged and the agent
 * is placed on a random position on the field. 
 *
 * @param agent agent that receives penalization message
 * @param iNr agent that is penalized (only used when ids are sent)
 * @param bReplace bool indiciating whether 'agent' should be replaced
 * @return bool indicating whether one or more preys are caught 
 */
void World:: penalizeAgent( Agent * agent, int iNr, bool bReplace )
{
  char strTmp[MAX_MSG];
  //  cerr << "Movement to prey without back-up: penalize" << endl;
  if( bReplace == true )
  {
    m_iNrPenalties++;
    agent->setCollided( true );
    initAgent( agent );     // penalize him
  }

  if( PS.getUseIds() == false )
    agent->sendMessage( "(referee penalize)" );
  else
  {
    sprintf( strTmp, "(referee penalize (predator %d))", iNr );
    agent->sendMessage( strTmp );
  }
  if( PS.getLogObjectInfo() == true )
    m_osPos << "predator penalized!" << endl;
}      

/**
 * This method sends visual information to all the objects of type
 * 'objType'.  Visual information is given to an object when it is
 * located within distance 'visible_distance' and has the following
 * format: (see (predator|prey x y)*). X and y denote respectively the
 * relative x and y coordinate of the perceived object relative to the
 * object that perceives the information.
 *
 * @param type send visuals to all objects with type 'objType'
 */
void World::sendVisuals( ObjectT objType )
{
  object_iter iter, iterAdd;
  char str[MAX_MSG];
  char strTmp[128];
  int  iX, iY;
  
  for( Agent * agentSee = (Agent*)iterateStart(  iter, objType );
       agentSee != NULL;
       agentSee = (Agent*)iterateNext( iter, objType  ) )
  {
    sprintf( str, "(see" );   
    for( Agent * agentAdd = (Agent*)iterateStart(  iterAdd, OBJ_AGENTS );
         agentAdd != NULL;
         agentAdd = (Agent*)iterateNext( iterAdd, OBJ_AGENTS  ) )
    {
      // skip visual info of myself
      if( agentAdd == agentSee && PS.getUseIds() == false ) 
        continue ;
        
      // first get postive relative x coordinate to my position  
      iX = agentAdd->getX() - agentSee->getX() ;
      if( iX < 0 )
        iX += PS.getColumns();
      // if negative relative x coordinate is closer, make it negative
      if( abs( iX - PS.getColumns() ) < iX )
        iX -= PS.getColumns();
        
      // do the same for y-coordinate
      iY = agentAdd->getY() - agentSee->getY() ;
      if( iY < 0 )
        iY += PS.getRows();
      if( abs( iY - PS.getRows() ) < iY )
        iY -= PS.getRows();
      
      // if in visible distance add it to the visual message.
      if( PS.getVisibleDistance() == -1 ||
          ( abs(iX) <= PS.getVisibleDistance() && 
            abs(iY) <= PS.getVisibleDistance() ) )
      {
        if( PS.getUseIds() == false )
          sprintf( strTmp, " (%s %d %d)", 
                   (agentAdd->getType()==OBJ_PREDATOR ) ? "predator" : "prey", 
                   iX, iY );
        else
          sprintf( strTmp, " ((%s %d) %d %d)", 
                   (agentAdd->getType()==OBJ_PREDATOR ) ? "predator" : "prey", 
                   agentAdd->getNumber(), iX, iY );
        strcat( str, strTmp );
      }         
    }
    iterateEnd( iterAdd );
    strcat( str, ")" );             // end the message
    agentSee->sendMessage( str );   // and send it
  } 
  iterateEnd( iter );
}

/**
 * This method broadcast the send_action message to all objects of
 * type type 
 *
 * @param type send message that we are ready to receive actionsto all
 * objects with type 'objType'
 */
void World::sendSendAction( ObjectT type )
{
  char strMsg[MAX_MSG];
	
	sprintf( strMsg, "(send_action %d %d)", getNrEpisode(), getNrCycle() );
	broadcast( type, strMsg );
}

/**
 * This method communicates the message 'str', originated from 'agent'
 * to all agents of the same type of 'agent'. 
 *
 * @param agent agent object to which the message should be communicated
 * @param str string that should be commnunicated
 */
void World::communicate( ConnectionObject *agent, char * str )
{
  object_iter iter;
  
  for( ConnectionObject * agentTo = iterateStart(  iter, agent->getType() );
       agentTo != NULL;
       agentTo = iterateNext( iter, agent->getType()  ) )
  {
    if( agent != agentTo )
      agentTo->sendMessage( str );
  }
  iterateEnd( iter );  
}

/**
 * This method broadcasts the message 'str' to all objects of type
 * 'type'.
 *
 * @param type type of the objects that should receive information
 * @param str string that should be broadcasted
 * @param bIncludeCaught bool indicating whether caught preys are included
 */
void World::broadcast( ObjectT type, char * str, bool bIncludeCaught )
{
  object_iter iter;
  for( ConnectionObject * object = iterateStart(  iter, type, bIncludeCaught );
       object != NULL;
       object = iterateNext( iter, type, bIncludeCaught ) )
  {
    object->sendMessage( str );		
	}
  iterateEnd( iter );  
}

/**
 * This method updates all the objects on the grid that are of type
 * 'objType'.  Using the last received movement command from an
 * object, the location of the object is updated. 
 *
 * @param objType type of the objects that should be updated. 
 */
void World::updateField( ObjectT objType )
{
  object_iter iter;
  char        str[MAX_MSG];
  ActionT     action;
  Pos         pos;
  
  // update the agents using their last sent message.
  for( Agent * agent = (Agent*)iterateStart(  iter, objType );
       agent != NULL;
       agent = (Agent*)iterateNext( iter, objType  ) )
  {
    pos    = agent->getPos();
    action = ACT_ILLEGAL;
    strcpy( str, agent->getLastMsg() ) ;
    if( str[0] !=  '\0' ) // received a message
    {
      if( strncmp( str, "(move north)", 12 ) == 0 )
        action = ACT_MOVE_NORTH;       
      else if( strncmp( str, "(move south)", 12 ) == 0 )
        action = ACT_MOVE_SOUTH;       
      else if( strncmp( str, "(move east)", 11 ) == 0 )
        action = ACT_MOVE_EAST;       
      else if( strncmp( str, "(move west)", 11 ) == 0 )
        action = ACT_MOVE_WEST;       
      else if( strncmp( str, "(move none)", 11 ) == 0 )
        action = ACT_MOVE_NONE;
      else if( agent->getType() == OBJ_PREY &&
               PS.getAllowDiagonalPrey() == false )
        ; // do not check further since prey can not move diagonally        
      else if( agent->getType() == OBJ_PREDATOR &&
               PS.getAllowDiagonalPred() == false )
        ; // do not check further  since predator can not move diagonally        
      else if( strncmp( str, "(move northeast)", 16 ) == 0 )
        action = ACT_MOVE_NORTHEAST;       
      else if( strncmp( str, "(move northwest)", 16 ) == 0 )
        action = ACT_MOVE_NORTHWEST;       
      else if( strncmp( str, "(move southeast)", 16 ) == 0 )
        action = ACT_MOVE_SOUTHEAST;       
      else if( strncmp( str, "(move southwest)", 16 ) == 0 )
        action = ACT_MOVE_SOUTHWEST;       
      str[0] = '\0';
    }
    pos = getPosAfterAction( pos, action );
    agent->setPrevPos( agent->getPos() );
    agent->setPos( pos );
    agent->setCollided( false )  ;
  }
  iterateEnd( iter );
  
  // and check for collisions
  checkCollisions( objType );
}

/**
 * This method returns the new position of an agent currently located
 * at position 'pos' after it has performed the action 'action'. 
 *
 * @param original position
 * @param action action that will be executed
 *
 * @return updated position after taken action 'action'. */
Pos World::getPosAfterAction( Pos pos, ActionT action )
{
  switch( action )
  {
    case ACT_MOVE_NORTH:         pos.y++;          break;
    case ACT_MOVE_SOUTH:         pos.y--;          break;
    case ACT_MOVE_EAST:          pos.x++;          break;
    case ACT_MOVE_WEST:          pos.x--;          break;
    case ACT_MOVE_NORTHEAST:     pos.x++; pos.y++; break;
    case ACT_MOVE_NORTHWEST:     pos.x--; pos.y++; break;
    case ACT_MOVE_SOUTHEAST:     pos.x++; pos.y--; break;
    case ACT_MOVE_SOUTHWEST:     pos.x--; pos.y--; break;
    case ACT_MOVE_NONE:                            break;
     default:                                      break;
  }

  pos.x = ( pos.x + PS.getColumns() ) % PS.getColumns();
  pos.y = ( pos.y + PS.getRows()    ) % PS.getRows();
  return pos;
}

/**
 * This method checks whether objects of the object type 'objType'
 * have collided. That is, they are located at the same position. If a
 * collision has been detected, one of the objects is placed at his
 * previous position. 
 *
 * @return objType type of the obejcts that should be checked
 */
void World::checkCollisions( ObjectT objType )
{
  object_iter iter1, iter2, iter3;
  
  // iterate over all agents of type 'objType'
  for( Agent * agent1 = (Agent*)iterateStart(  iter1, objType );
       agent1 != NULL;
       agent1 = (Agent*)iterateNext( iter1, objType  ) )
  {
    // and check whether it collided with an agent of type 'objType' further
    // in the list (do not have to check all, since if 1 is checked with 2, it
    // is not necessary to test 2 with 1 later on)
    iter2 = iter1;
    for( Agent * agent2 = (Agent*)iterateNext( iter2, objType );
         agent2 != NULL;
         agent2 = (Agent*)iterateNext( iter2, objType  ) )
    {        
       if( agent1->getPos() == agent2->getPos() ) // if collision detected
       {
#if 0
	 cerr << "collision detected between two ";
          if( objType == OBJ_PREY ) 
            cerr << "preys." << endl;         
          else if( objType == OBJ_PREDATOR ) 
          {
            cerr << "predators." << endl;             
            m_iNrCollisions++;						
          }
          else
            cerr << "unknown objects!" << endl;
#else
	  if( objType == OBJ_PREDATOR ) 
            m_iNrCollisions++;						
#endif
          if( PS.getLogObjectInfo() == true )
          {
            m_osPos << "collision detected between two ";
            if( objType == OBJ_PREY ) 
              m_osPos << "preys." << endl;         
            else if( objType == OBJ_PREDATOR ) 
              m_osPos << "predators." << endl;             
            else
              m_osPos << "unknown objects." << endl;
          }
          initAgent( agent1 );                    // re-initialize both agents
          agent1->setCollided( true );            // and set collided to true
          initAgent( agent2 );
          agent2->setCollided( true );

          // if predators collided, notify the predators 
          if( objType == OBJ_PREDATOR )
          {
            for( Agent * agent3 = (Agent*)iterateStart(  iter3, objType );
                 agent3 != NULL;
                 agent3 = (Agent*)iterateNext( iter3, objType  ) )
            {
              // notify the involved agents and the others when  penalize_all=1
              if( PS.getPenalizeAll() == true || 
                  agent3 == agent2 || agent3 == agent1 )
              {
                if( PS.getUseIds() == false ) // no ids, just state collision
                  agent3->sendMessage( "(referee collision)" );
                else                          // state which agents collided
                {
                  char strTmp[MAX_MSG];
                  sprintf( strTmp, "(referee collision (predator %d))", 
                           agent1->getNumber() );
                  agent3->sendMessage( strTmp );
                  sprintf( strTmp, "(referee collision (predator %d))", 
                           agent2->getNumber() );
                  agent3->sendMessage( strTmp );
                }
              }
            }
          }
          iterateStart( iter1, objType );         // start anew with iteration
      }
    }
    iterateEnd( iter2 );
  }    
  iterateEnd( iter1 );
}

/**
 * This is the mainloop of the world. It loops until the maximum
 * number of episodes (defined in 'Parameters') have been passed or
 * the user has explicitly halted the program. Each epsidode consists
 * of a number of cycles. Each cycle consists of different phases in
 * which either the preys or the predators are allowed to exchange
 * messages with the server. At the beginning of a cycle, visual
 * messages are send to all preys after which the preys have
 * 'time_step' msec to send a communication message to the server that
 * is communicated to all other preys (when communication is turned
 * off, this period is skipped.}. After this period has elapsed the
 * server sends the (send_action) message to the preys to indicate
 * that the communication period is over. Now the preys are allowed to
 * send their movement command to the server. When again time_step
 * msec have elapsed the sending period is over and the field is
 * updated using the received movement commands from the preys. Now it
 * is the turn of the predators and the whole procedure repeats
 * itself. The predators thus move concurrently after the preys have
 * moved concurrently. After all predators are updated, the server
 * checks whether a collision occurs (two predators share the same
 * cell). In case of a collision both predators are penalized and
 * placed at random positions on the field. These predators will be
 * colored differently for one cycle. Finally the server checks
 * whether a prey was captured by the predators.  
 */
void World::mainLoop( )
{

  if( PS.getLogObjectInfo() == true )
  {
    char str[MAX_MSG];
    setOutputStream( PS.getLogObjectFile() );
    makeServerParamMsg( str );
    m_osPos << str << endl;
  }
    
  if( PS.getAutomaticStart() == true )
  {
    usleep( 20 * 1000 * 1000 );  
    m_bCont = true;  
  }
  
  while( m_bHalt == false  )   // program is halted 
  {
    if( m_bCont == true )      // do we want to continue updating the world?
    {
      if( isAllPreyCaught() )  // if all prey is captured, end of episode      
      {
        if( PS.getTimeStep() != -1 || getNrEpisode() % 100 == 0 )
          cerr << "episode " << getNrEpisode() << ": prey caught in " 
               << setw(4) << getNrCycle() << " cycles" <<  endl;
        m_lCycles.push_back( m_iNrCycle ); // store the number of cycles

        m_dAvgCapture = 
          ((double)(m_iNrEpisode-1)*m_dAvgCapture +(double)m_iNrCycle)/
          (double)m_iNrEpisode;
        m_iNrEpisode++;                    // and update the number of episodes
        broadcast( OBJ_AGENTS, "(referee episode_ended)", true );
        if( m_iNrEpisode > PS.getNrEpisodes() )
        {
          m_bCont = false;
          m_bHalt = true;
        }
        else
        {
          usleep( PS.getNextEpisodeWait() * 1000 );
          m_iNrCycle = 1;
          initField( );
        }
      }

      if( PS.getLogObjectInfo() == true )
        logObjectInfo( );                               
      if( m_iNrCycle > 1 ) // let predators do first action
        mainLoop( OBJ_PREY  );     // first update preys
      sendInfoToMonitors( );     // and tell to monitors   
      if( PS.getLogObjectInfo() == true )
        logObjectInfo( );
      mainLoop( OBJ_PREDATOR );  // then predators 
      updatePreyCaught();      
      sendInfoToMonitors( );
      m_iNrCycle++;     			
    }
    else 
      usleep( 100 * 1000 ); // wait and try again in 100 msec. 
  }
}

/**
 * This method is the main loop that handles all communication with a
 * specific object type. First it allows the agents to pass
 * communication messages to each other. This is done by waiting
 * time_step msec (defined in Parameters).  After the communication
 * period is over, the message (send_action) is sent to all
 * agents. Thereafter, they again have time_step msec to send their
 * movement commmands after wich the field is updated.  
 *
 * @param objType type of the object that is updated 
 */
void World::mainLoop( ObjectT objType )
{
  resetSendMsg( objType );     // important for synchronization mode
  if( PS.getAllowComm( ) == true  && PS.getLogObjectInfo() == true &&
      objType == OBJ_PREDATOR )
    m_osPos << "EPISODE " << m_iNrEpisode << " CYCLE " << m_iNrCycle << 
      ((objType == OBJ_PREY ) ? "a" : "b") << " COMMUNICATION PHASE" << endl;

  // if agents are allowed to communicate and are in synchronization mode
  if( PS.getAllowComm() == true && PS.getTimeStep() == -1 &&
      objType == OBJ_PREDATOR )
  {
    pthread_mutex_lock( &mutex_newInfo );        // lock the mutex
    struct timeval now;
    struct timespec timeout;
    gettimeofday(&now, NULL);
    timeout.tv_sec = now.tv_sec + 5;
    timeout.tv_nsec = now.tv_usec*1000;          // set time_out
    m_bPredatorSync = (objType == OBJ_PREDATOR ) ? true : false ;
    sendVisuals( objType );      // send all visual information    
    // and wait till signal comes that indicates that all agents have sent msg
    if( m_bReadySync == false )                  
      pthread_cond_timedwait( &cond_newInfo, &mutex_newInfo, &timeout );
    m_bReadySync = false;
    pthread_mutex_unlock( &mutex_newInfo );
  }
  else if( PS.getAllowComm( ) == true && objType == OBJ_PREDATOR )
  {
    sendVisuals( objType );             // send all visual information      
    usleep( PS.getTimeStep() * 1000 );  // give them time_step msec to send msg
  }
  else
    sendVisuals( objType );             // send all visual information      

  // again reset the boolean that indicates whether agents have sent message
  // send message to indicate communication is over and actions can be sent
  resetSendMsg( objType );  
  if( PS.getLogObjectInfo() == true )
    m_osPos << "EPISODE " << m_iNrEpisode << " CYCLE " << m_iNrCycle << 
     ((objType == OBJ_PREY ) ? "a" : "b")   << " ACTION PHASE" << endl;

  // in synchronization mode, do the same as above, wait till all msgs arrived  
  if( PS.getTimeStep() == -1 )
  {
    pthread_mutex_lock( &mutex_newInfo );
    struct timeval now;
    struct timespec timeout;
    gettimeofday(&now, NULL);
    timeout.tv_sec = now.tv_sec + 5;
    timeout.tv_nsec = now.tv_usec*1000;
    m_bPredatorSync = (objType == OBJ_PREDATOR ) ? true : false ;
    sendSendAction( objType );
    if( m_bReadySync == false )         
      pthread_cond_timedwait( &cond_newInfo, &mutex_newInfo, &timeout );        
    m_bReadySync = false;
    pthread_mutex_unlock( &mutex_newInfo );         
  }
  else
  {
    sendSendAction( objType );
    usleep( PS.getTimeStep() * 1000 );
  }
  
  if( m_bStepByStep == true )  // when run stepwise, pause before processing
  {
     m_bCont = false;
     while( m_bCont == false && m_bHalt == false )
       usleep( 100 * 1000 );  
  }

  updateField( objType );            
}

/**
 * This method resets the variable of all agents of type 'objType'
 * that indicates that a message is received from this agent. 
 *
 * @param objType type of object of which variables are reset
 */
void World::resetSendMsg( ObjectT objType )
{
  object_iter iter;
 
  for( Agent * agent = (Agent*)iterateStart(  iter, objType );
       agent != NULL;
       agent = (Agent*)iterateNext( iter, objType  ) )
  {   
    agent->setSendMsg( false );
    agent->setLastMsg( "\0" );
  }
  iterateEnd( iter );
}

/** 
 * This method sends all information about the current state of the
 * world to the monitors.
 */
void World::sendInfoToMonitors( )
{
  static char str[MAX_MSG];
  createInfoString( str );
  broadcast( OBJ_MONITOR, str );
}

/**
 * This method sends an exit messages to all connected client to
 * indicate that the server will be halted. Furthermore this method
 * prints the number of cycles needed to capture the prey for each
 * episode to either a file or standard output. 
 */ 
void World::sendExitMessages()
{
  static bool bFirstTime = true;
	
  if( bFirstTime == false ) // only do it once
    return;
  
  bFirstTime = false;
  cerr << "send exit messages" << endl;
  object_iter iter;
 
  for( ConnectionObject * object = iterateStart(  iter, OBJ_ALL, true );
       object != NULL;
       object = iterateNext( iter, OBJ_ALL, true  ) )
    object->sendMessage( "(quit)" );
  iterateEnd( iter );

  ofstream fos;
  ostream *os;

  // create output file depending on parameter. 
  if( strncmp( PS.getOutputFile( ), "stdout", 6 ) != 0 )
  {
    fos.open( PS.getOutputFile() );
    os = &fos;
    cout << "write output to " << PS.getOutputFile() << endl;
  }
  else
    os = &cout;
 
  // write all episode times to this output file
  while( ! m_lCycles.empty() )
  {
    *os << m_lCycles.front() << endl ;
    m_lCycles.pop_front();
  }      
  cerr << "average time: " << getAverageCaptureTime() << endl;

  closeOutputStream();
  fflush( stdout );
}

/**
 * This method initializes a new agent and puts it into the
 * world. This method is called when a new process connects itself to
 * the port of the world. The information from the created connection
 * (host and port of connecting client) is used to set-up a two-way
 * communication channel. This process now starts receiving visual
 * information and is taken into account in the handling of
 * messages. 
 * @param port port to which object is connected
 * @param host host on which object runs
 * @param type of the object
 * @return bool indicating whether object was added
 */
bool World::addObject( const port_type& port, const host_type& host, 
                       ObjectT type )
{
  static int iNrPrey = 1;
  static int iNrPredator = 1;
  ConnectionObject* object;
  
  if( type == OBJ_PREY || type == OBJ_PREDATOR )
  {
    int iNr = (type == OBJ_PREY ) ? iNrPrey++ : iNrPredator++;
    object = new Agent( iNr );
    object->connect( port, host );
    object->setType( type );
    initAgent( (Agent*)object );
    cerr << (type == OBJ_PREY ? "prey" : "predator" ) << " connected" << endl;
  }
  else if( type == OBJ_MONITOR )
  {
    char str[MAX_MSG];  
    object = new Monitor( );
    object->connect( port, host );
    object->setType( type );
    object->sendMessage( "(init ok)" );
    makeServerParamMsg( str );
    object->sendMessage( str );
    cerr << "monitor connected" << endl;
  }
  else
    return false;
    
  m_Objects.push_back( object );
  sendInfoToMonitors();  
  return true;

}

/**
 * This method is called in a separate thread and waits for incoming
 * connection requests. When such a request arrives, the connecting
 * client process is connected using a different port number using the
 * 'addAgent' method. 
 */
void World::acceptNewConnection()
{
  Connection server;      
  char       message[MAX_MSG];
  
  server.bind( PS.getPort() );   // open the port
  
  while( m_bHalt == false )
  {
    struct sockaddr_in cli_addr = server.waitForNewConnection(message,MAX_MSG);

    // received new connection, use host and port number to connect 
    unsigned long host = ntohl ( cli_addr.sin_addr.s_addr );
    int           port = ntohs ( cli_addr.sin_port );  

    ObjectT type = processInitMessage( message ) ;
    
    if( type != OBJ_ILLEGAL )
      addObject( port, host, type );
    else
      cout << "wrong init messages: " << message << endl;
  }
}

/**
 * This method is called in a separate thread an handles the incoming
 * messages of all the client processes. When one of the client
 * processes sends a message, it is processed by placing it in the
 * member variable of the 'Agent' class which resembles the latest
 * received message. 
 */ 
void World::receiveMessages()
{
  struct timeval tv;
  int ret, iMax = -1;
  object_iter iter;
  char str[MAX_MSG];
    
  while( m_bHalt == false )
  {
    tv.tv_sec = 5;
    tv.tv_usec  = 0;
    iMax = -1;
    fd_set readfds;
    FD_ZERO( &readfds );

    // get largest file descriptor
    for( ConnectionObject * object = iterateStart( iter, OBJ_ALL, true );
         object != NULL;
         object = iterateNext( iter, OBJ_ALL, true  ) )
    {
      if( object->isConnected() )
       {
         int fd = object->getSocketFd();
         FD_SET( fd, &readfds );
         if( fd > iMax ) 
           iMax = fd;
       }
    }
    iterateEnd( iter );
    
    iMax++; // select has as input largest file descriptor + 1

    // block till a message is received
    if(( ret = select( iMax, &readfds, NULL, NULL, &tv )) < 0 )
    {
      printf("max: %d\n", iMax);
      perror("select");
      break;
    }
    else if( ret != 0 )
    {
      // get and store the message of the file descriptors which sent a message
      for( ConnectionObject * object = iterateStart(  iter, OBJ_ALL, true );
           object != NULL;
           object = iterateNext( iter, OBJ_ALL, true  ) )
      {
        if( FD_ISSET( object->getSocketFd(), &readfds ) )
        {				  
          object->receiveMessage( );
          if( object->getType() != OBJ_MONITOR )
          {
            logRecvAction( object, object->getLastMsg() );          
            ((Agent*)object)->setSendMsg( true );      
          }
    
          if( object->getType() == OBJ_MONITOR )
            processMonitorMessage( object->getLastMsg() ); 
          // if msg was communication msg, communicate it immediately
          else if( PS.getAllowComm() == true &&
              strncmp( object->getLastMsg(), "(say", 4 ) == 0 )
          {
            sprintf( str, "%s", "(hear" );
            strcat( str, object->getLastMsg() + 4 );
            communicate( object, str );
          }
                
          // in synchronization mode, check whether all agents of the object
          // type that is currently handled have sent a message. If this is the
          // case sent a condition signal, that will wake the mainLoop to continue
          if( PS.getTimeStep() == -1 && object->getType() != OBJ_MONITOR )
          {
            pthread_mutex_lock( &mutex_newInfo );                  
            ObjectT type = ( m_bPredatorSync == true  ) ? OBJ_PREDATOR : OBJ_PREY;
            object_iter iter2;
            bool bReadySync = true;
            for( Agent *agent2 = (Agent*)iterateStart(  iter2, type );
                 agent2 != NULL;
                 agent2 = (Agent*)iterateNext( iter2, type ) )
               bReadySync &= ( agent2->getSendMsg( ) == true );
            iterateEnd( iter2 );
            if( bReadySync == true )
            {
              m_bReadySync = true;
              pthread_cond_signal( &cond_newInfo );
            }
            pthread_mutex_unlock( &mutex_newInfo );                  
          }
				}
      }
      iterateEnd( iter );
    }
  }
}

/**
 * This method processes a message from the monitor and sets the
 * corresponding variables accordingly. 
 */
void World::processMonitorMessage( char * str )
{
  int time = PS.getTimeStep();

  if( strncmp( str, "(monitor continue)", 18 ) == 0 )
    setContinue( true );
  else if( strncmp( str, "(monitor pause)", 15 ) == 0 )
    setContinue( false );  
  else if( strncmp( str, "(monitor quit)", 14 ) == 0 )
    setHalted( true );
  else if( strncmp( str, "(monitor step)", 14 )  == 0 )
    setStepByStep( ! getStepByStep() );
  else if( strncmp( str, "(monitor speed_up)", 18 )  == 0 )    
    PS.setTimeStep( ( time > 20 ) ? time - 10 : time );
  else if( strncmp( str, "(monitor speed_down)", 20 )  == 0 )    
    PS.setTimeStep( ( time < 1000 ) ? time +10 : time );
  else
    cerr << "(WORLD::processMonitorMessage) unknown msg: " << str << endl;

}

/**
 * This method processes an init message.  It checks whether the
 * connecting client process is either a OBJ_PREY or an OBJ_PREDATOR
 * and returns the corresponding object type. In case of failure
 * OBJ_ILLEGAL is returned. 
 */
ObjectT World::processInitMessage( char * str ) 
{
  if( strncmp( str, "(init prey)", 11 ) == 0  )
    return OBJ_PREY;
  else if( strncmp( str, "(init predator)", 15 ) == 0 )
    return OBJ_PREDATOR;
  else if( strncmp( str, "(init monitor)", 14 ) == 0 )
    return OBJ_MONITOR;

  return OBJ_ILLEGAL;
}

/**
 * This method starts an iteration over a specific object type. When
 * all objects of a specific type have to be traversed, call this
 * method first. Then use 'iterateNext' repeatedly to get the first
 * and following objects. Pass the same 'index' variable that was used
 * for 'iterateStart' to 'iterateNext'. The last boolean argument
 * specifies whether the caught preys should also be returned in the
 * iteration. In some cases this is desirable (when the field is
 * re-initialized), but in some cases it is not (when the field is
 * displayed) 
 *
 * @param iter pointer to object_iter that will be initialized. This is
 * a list of the current object over which we iterate. 
 * @param type object types over which we want to iterate
 * @param bIncludeCaught indicates whether caught preys should also be returned
 *
 * @return next object in the iteration, NULL when there is no object left
 *
 */
ConnectionObject* World::iterateStart( object_iter &iter, ObjectT type, 
                            bool bIncludeCaught ) 
{
  iter = m_Objects.begin();                           // get the first item 
  return iterateNext( iter, type, bIncludeCaught );  
}

/**
 * This method gets the next object in the iteration. This method can
 * only be called after a call to 'iterateStart'. The same 'index'
 * variable should be used when this method is called. This method
 * returns 'NULL' when all objects have been traversed. The last
 * boolean argument specifies whether the caught preys should also be
 * returned in the iteration. In some cases this is desirable (when
 * the field is re-initialized), but in some cases it is not (when the
 * field is displayed) 
 *
 * @param iter pointer to object_iter that will be initialized. This is
 * a list of the current object over which we iterate. 
 * @param type object types over which we want to iterate
 * @param bIncludeCaught indicates whether caught preys should also be returned
 *
 * @return next object in the iteration, NULL when there is no object
 */
ConnectionObject* World::iterateNext( object_iter &iter, ObjectT type, 
                                      bool bIncludeCaught ) 
{
  ObjectT cur_type;
  while( iter != m_Objects.end() )  // until at the end of the list
  {
    // return the item when it is of the correct type and when it is connected.
    // Furthermore when bIncludeCaught is true the above conditions are enough,
    // if it is false we have to check if the returned agent is caught,
    // since when he is caught we don't want to return this agent.
    cur_type = (*iter)->getType();

    if( 
      ( cur_type == type                // type ok -> ok
        ||
        (
          type == OBJ_ALL               // type OBJ_ALL and cur_type exists -> ok
          && 
          cur_type != OBJ_ILLEGAL 
        ) 
        ||
        (
          type == OBJ_AGENTS           // type OBJ_AGENT -> prey and pred -> ok
          && 
          ( 
            cur_type==OBJ_PREY 
            || 
            cur_type==OBJ_PREDATOR 
          )
        )
      ) 
      &&
      (*iter)->isConnected()           // object must be connected
      &&
      ( 
        type == OBJ_MONITOR            // obj is monitor
        || 
        bIncludeCaught == true         // only check for isCaught when caught
        ||                             // objects should not be returned
        ((Agent*)(*iter))->isCaught() == false 
      ) 
    )
    {
      return *iter++;
    }
    else if( ! (*iter)->isConnected() )             // if agent is not connected
    { 
      if( (*iter)->getType() == OBJ_MONITOR ) 
         cout << "monitor disconnected" << endl;
      else if( (*iter)->getType() == OBJ_PREY ) 
         cout << "prey disconnected" << endl;
      else if( (*iter)->getType() == OBJ_PREDATOR ) 
         cout << "predator disconnected" << endl;               
      delete *iter;                                // delete him
      iter = m_Objects.erase( iter );
    }
    else
    {
      iter++;                                      // otherwise, continue
    }
  }

  return NULL;
}

/**
 * This method should be called after all objects have been traversed
 * (and 'iterateNext' has returned 'NULL'. It finalizes the
 * iteration. 
 *
 * @param iter object iteration used in the iteration. */
void World::iterateEnd( object_iter & iter )         
{
  iter = m_Objects.end();
}

/** 
 * This method sets whether the server should be halted or not. 
 * 
 * @param b bool indicating new value
 */
void World::setHalted( bool  b )
{
  m_bHalt = b;
}
    
/** 
 * This method returns whether the server should be halted or not. 
 * 
 * @return bool indicating whether server should be halted or not
 */
bool World::isHalted( )
{
 return m_bHalt;
}

/** 
 * This method sets whether the server should go to the next step or not. 
 * 
 * @param b bool indicating new value
 */
void World::setContinue( bool  b )
{
 m_bCont = b;
}
    
/** 
 * This method returns whether the server should perform one step at a time
 * 
 * @return bool indicating whether server should progress step by step
 */
bool World::getStepByStep( )
{
 return m_bStepByStep;
}
     
/** 
 * This method sets whether the server should go step by  step or not. 
 * 
 * @param b bool indicating new value
 */
void World::setStepByStep( bool  b )
{
 m_bStepByStep = b ;
}

/** 
 * This method sets the output stream of all position info for the logplayer
 * 
 * @param str filename to which the output stream should be set.
 */
void World::setOutputStream( char* str )
{
 m_osPos.open( str )
;
}

/** 
 * Close the output stream.
 * 
 */
void World::closeOutputStream( )
{
 m_osPos.close();
}


/** 
 * 
 * This method returns the current cycle number.
 * 
 * @return current cycle number.
 */
int World::getNrCycle( )
{
 return m_iNrCycle;
}

/** 
 * 
 * This method returns the current episode number.
 * 
 * @return current episode number.
 */
int World::getNrEpisode( )
{
 return m_iNrEpisode;
}
    
/** 
 * 
 * This method returns the number of collisions
 * 
 * @return current number of collisions
 */
int World::getNrCollisions( )
{
 return m_iNrCollisions;
}

/** 
 * 
 * This method returns the number of penalties
 * 
 * @return current number of penalties
 */
int World::getNrPenalties( )
{
 return m_iNrPenalties;
}
    
/**
 * This method returns the shortest distance between two points when
 * an agent is only allowed to move north, south, east and west. In
 * this case the shortest distance equals the addition of the relative
 * x and y position of the two points. 
 *
 * @param pos1 position 1
 * @param pos2 position 2
 *
 * @return manhattan distance between pos1 and pos2. */
int World::getShortestDistance( Pos pos1, Pos pos2 )
{  
  int x = abs( pos1.x - pos2.x );      
  if( abs( x - PS.getColumns() ) < x ) // if shorter distance from other side
    x = abs( x - PS.getColumns() );    // change x

  int y = abs( pos1.y - pos2.y );
  if( abs( y - PS.getRows() ) < y )
    y = abs( y - PS.getRows() );
  return x + y;
}


/**
 * This method returns the average capture times of all the previous
 * episodes
 *
 * @return average capture time
 */
double World::getAverageCaptureTime()
{
  return m_dAvgCapture;

}

/**
 * This method logs the information of all the objects to the
 * specified output stream using 'setOutputStream'. 
 */
void World::logObjectInfo( )
{
  static char str[MAX_MSG];
  createInfoString( str );
  m_osPos << str ;
}

/**
 * This methods creates an info string that contains all the current
 * world state information. This is sent to the monitor from which it
 * can derive all the information to draw the world. 
 *
 * @param str will be set to the information string. 
 */
void World::createInfoString( char *str )
{
  static char strTmp[MAX_MSG];  
  object_iter iter;
  
  sprintf( str, "(world (stats %d %d %f %d %d)",
                      getNrEpisode(),
                      getNrCycle(),
                      getAverageCaptureTime(),
                      getNrPenalties(),
                      getNrCollisions() );

  for( Agent * agent = (Agent*)iterateStart(  iter, OBJ_AGENTS, true );
       agent != NULL;
       agent = (Agent*)iterateNext( iter, OBJ_AGENTS, true  ) )
  {
     sprintf( strTmp, " ((%s %d%s) %d %d)",
                 (agent->getType() == OBJ_PREY ) ? "prey" : "predator",
                  agent->getNumber(),
                 (agent->isCaught() ? "x" : agent->isCollided() ? "c" : " " ), 
                  agent->getPos().x,
                  agent->getPos().y );
     strcat( str, strTmp );
  }
  
  strcat( str, ")\n" );
}

/**
 * This method makes a string with all important parameters that are
 * needed by the monitors. This are for example the visible distance
 * of the agents and the number of columns and rows in the world. 
 *
 * @param str will be set to the server parameter mesasge. */
void World::makeServerParamMsg( char *str )
{
  sprintf( str, "(server_param (visible_distance %d) (rows %d) (columns %d))",
       PS.getVisibleDistance(), PS.getRows(), PS.getColumns() );
}

/**
 * This method logs a received action 'str' from an agent 'agent' to
 * the output stream. 
 *
 * @param agent object from which action was received. 
 * @param received message 
 */
void World::logRecvAction( ConnectionObject *agent, char * str )
{
  if( PS.getLogObjectInfo() == false )
    return;

  struct timeval time2;
  gettimeofday( &time2, NULL );
    
  m_osPos << "Recv\t" << (Agent*)agent << ": " << str << endl;
}

/**
 * This method reads the value parameters from the file 'file'. All
 * values have been separated by 'strSep'. 
 *
 * @param file filename with the parameters
 * @param strSep separation character(s) between parameter name and value
 *
 * @return bool indicating whether values were read correctly. 
 */
bool World::readValues( char *file, char* strSep )
{
  bool bReturn = PS.readValues( file, strSep );
  return bReturn;
}

