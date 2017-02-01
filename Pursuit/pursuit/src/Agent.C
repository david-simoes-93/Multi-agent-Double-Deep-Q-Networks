 /**
 * @file   Agent.C
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Fri Aug  1 11:55:44 2003
 * 
 * @brief This file contains the definitions for the methods that are
 * related to the objects (monitors, predators and preys) that connect
 * to the server.
 * 
 * 
 */
#include "Agent.h"


/**************************************************************************/
/******************************  STRUCT  POS ******************************/
/**************************************************************************/

/**
 *   This operator checks whether two positions have the same x- and 
 *   y-coordinate.
 *   @param pos1 first position
 *   @param pos2 second position
 *   @return bool indicating whether both the x and y coordinate of the two
 *   specified positions are equal. 
 */
bool operator == ( Pos pos1, Pos pos2 )
{
  return pos1.x == pos2.x && pos1.y == pos2.y ;
}

/** 
 *   This operator checks whether two positions have different x- and
 *   y-coordinate.  
 *   @param pos1 first position 
 *   @param pos2 second position 
 *   @return bool indicating whether either the x or y coordinate of
 *   the two specified positions are different. 
 */
bool operator != ( Pos pos1, Pos pos2)
{
  return !( pos1 == pos2 );
}

/**************************************************************************/
/****************************** CLASS CONNECTIONOBJECT ********************/
/**************************************************************************/


/**
 * This method sets the type of the object. 
 * @param type new type of this object 
 */
void ConnectionObject::setType( ObjectT type )
{
  if( type != OBJ_ALL || type != OBJ_AGENTS )
    m_type = type;
}

/** 
 * This methods returns the type of this object.
 * 
 * @return the type of the object
 */
ObjectT ConnectionObject::getType( ) 
{ 
  return m_type ;              
}


/** 
 * This method returns the last received message from this object.
 * 
 * @return the last message received from this object
 */
char* ConnectionObject::getLastMsg( )
{ 
  return m_strLastMsg;         
}


/** 
 * This message sets the last message received by this object.
 * 
 * @param str the last message received by this object
 */
void ConnectionObject::setLastMsg( char *str ) 
{ 
  strcpy( m_strLastMsg, str ); 
}   


/**
 * This method makes a connection with the specified host and port
 * number. 
 *
 * @param port number 
 * @param host host 
 * @param bool indicating whether connection was created. 
 * @return bool indicating whether connections was created succesfull
 */
bool ConnectionObject::connect( const port_type& port, const host_type& host )
{
  // first bind a new socket (open a new port on this process) and connect it to
  // the port and host number specified. A new port is created, since the port 
  // number with which the socket initially connected, should be solely used for
  //  connecting new objects. 
  m_connection.bind();                        // create a new socket
  m_connection.connect( port, host );         // connect it to the client
  m_connection.sendMessage( "(init ok)" );    // send confirmation message
  return true;
}

/**
 * This methods is used to check whether this object is currently
 * connected to a client process. 
 * @return bool indicating whether agent has open connection. 
 */
bool ConnectionObject::isConnected()
{
  return m_connection.isConnected();
}

/**
 * This method returns the socket file descriptor associated with the
 * connection of this object. 
 * @return socket descriptor of the connection. -1 when there is no connection.
 */
int ConnectionObject::getSocketFd()
{
  return m_connection.getSocketFd();
}

/**
 *  Receive a message from the client process and store it in the
 *  placeholder where the last received message is stored. 
 */
void ConnectionObject::receiveMessage(  )
{
  m_connection.receiveMessage( m_strLastMsg, MAX_MSG );
}

/** 
 * Send the specified string to the client process associated with
 * this agent.  
 * @param str string that should be sent. */
void ConnectionObject::sendMessage( char* str )
{
  m_connection.sendMessage( str ); 
}

/**************************************************************************/
/****************************** CLASS AGENT *******************************/
/**************************************************************************/



/** 
 * This constructors creates a new Agent with the specified id.
 * 
 * @param iNumber id related to this object
 */
Agent::Agent( int iNumber )
{
  m_bCollided   = false;
  m_bCaught     = false;
  setNumber( iNumber );
}

/**
 *  This method sets the position of the agent. 
 *  @param x new x-coordinate
 *  @param y new y-coordinate 
 */
void Agent::setPosition( int x, int y )
{
  m_pos.x = x;
  m_pos.y = y;
}

/** 
 * This method sets the previous position of this agent to 'pos'
 * 
 * @param pos previous position of this agent
 */
void Agent::setPrevPos( Pos pos ) 
{ 
  m_posPrev = pos;             
}

/** 
 * This method returns the previous position of this agent.
 * 
 * 
 * @return previous position of this agent
 */
Pos Agent::getPrevPos( )
{ 
  return m_posPrev;            
}  


/** 
 * This method sets the current position of this agent.
 * 
 * @param pos current position of this agent
 */
void Agent::setPos( Pos pos ) 
{ 
  m_pos = pos;                
}

/** 
 * This method returns the current position of this agent.
 * 
 * 
 * @return current position of this agent
 */
Pos Agent::getPos( )
{ 
  return m_pos;                
}

/** 
 * This method returns the current x coordinate of the agent.
 * 
 * 
 * @return current x coordinate of the position of the agent.
 */
int Agent::getX( )
{ 
  return m_pos.x ;             
}

/** 
 * This method returns the current y coordinate of the position of
 * the agent.
 * 
 * 
 * @return current y coordinate of the position of the agent.
 */
int Agent::getY( )
{ 
  return m_pos.y ;             
}    

/** 
 * This method returns a boolean indicating whether this agent has
 * collided with another agent.
 * 
 * 
 * @return bool indicating whether this agent has collided with another agent.
 */
bool Agent::isCollided( )
{ 
  return m_bCollided;          
}

/** 
 * This method sets the boolean indicating that this agent has
 * collided with another agent.
 * 
 * 
 * @param b indicating that this agent has collided with another agent.
 */void Agent::setCollided  ( bool b     ) 
{ 
  m_bCollided = b;             
}

/** 
 * This method returns a boolean indicating whether this agent has
 * been caught. 
 * 
 * 
 * @return bool indicating whether this agent has been caught.
 */
bool Agent::isCaught( )
{ 
  return m_bCaught;           
}

/** 
 * This method sets a boolean indicating whether this agent has
 * been caught. 
 * 
 * 
 * @param b indicating whether this agent has been caught.
 */
void Agent::setCaught    ( bool b     ) 
{ 
  m_bCaught = b;               
} 
     

/** 
 * This method returns the number of this agent
 * 
 * 
 * @return number of this agent.
 */
int Agent::getNumber( )
{ 
  return m_iNumber;            
}

/** 
 * This method sets the number of this agent.
 * 
 * @param i new number of this agent
 */
void Agent::setNumber( int  i ) 
{ 
  m_iNumber = i;              
}

/** 
 * This method returns a bool indicating whether this agent has
 * already sent a message this cycle.
 * 
 * 
 * @return bool indicating whether this agent has already sent a
 * message this cycle.
 */
bool Agent::getSendMsg( )
{ 
  return m_bSendMsg;          
}

/** 
 * This method sets a bool indicating whether this agent has
 * already sent a message this cycle.
 * 
 * 
 * @param b  indicates whether this agent has already sent a
 * message this cycle.
 */
void Agent::setSendMsg( bool b ) 
{ 
  m_bSendMsg = b;
}    

/** 
 * This method sets the position of the agent.
 * @param pos new  position 
 */
void Agent::setPosition( Pos pos )
{
  m_pos = pos;
}

/** 
 * This method prints the information of 'agent' to the output 'os'.
 * 
 * @param os output stream to which output is printed
 * @param agent pointer to agent of which information has to be printed
 * 
 * @return resulting output steam with new contents
 */
ostream& operator << ( ostream   &os, Agent *agent )
{
   os << "("  << (( agent->getType() == OBJ_PREY ) ? "prey" : "predator" )
           << " " << agent->getNumber() << " )" ;
   return os;
}  
