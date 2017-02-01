/**
 * @file   Agent.h
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Fri Aug  1 11:37:39 2003
 * 
 * @brief This file contains the header files for the different agents
 * that connect to the pursuit server (both the monitors and the
 * predators and preys.
 */

#ifndef _AGENT_
#define _AGENT_

#include "Connection.h"
#include "types.h"
#include <string.h>  // needed for strcpy

/** 
 * Maximum size of a message received or sent is MAX_MSG.
 */
#define MAX_MSG 2048

/**
 * Pos is a simple struct that holds both a x- and a
 * y-coordinate. Furthermore operators are defined that check whether
 * to positions are equal or not.
 **/
struct Pos
{
  int x;                        /**< x coordinate of this position */
  int y;                        /**< y coordinate of this position */
} ;

/** 
 * This function checks whether two positions are equal.
 * 
 * @param Pos first position
 * @param Pos second position
 * 
 * @return boolean indicating whether the two positions are equal
 */
bool operator == ( Pos, Pos );

/** 
 * This function checks whether two positions are inequal.
 * 
 * @param Pos first position
 * @param Pos second position
 * 
 * @return boolean indicating whether the two positions are inequal
 */
bool operator != ( Pos, Pos );

/** 
 * This class holds all the information of an object that can make a
 * connection with the pursuit server. This can be either an agent
 * (prey or predator) or a monitor. 
 */
class ConnectionObject
{
   ObjectT    m_type;                /**< type of this object                */
   Connection m_connection;          /**< connection to send and receive msg */
   char       m_strLastMsg[MAX_MSG]; /**< last message received from this obj*/

public:

   // methods that deal with UDP connection 
   bool connect       ( const   port_type& port, const host_type& host );
   bool isConnected   (                                                );
   int  getSocketFd   (                                                );
   void receiveMessage(                                                );
   void sendMessage   ( char *  str                                    );

   // standard get and set methods
   void    setType    ( ObjectT type                                   );
   ObjectT getType    (                                                );
   char*   getLastMsg (                                                );
   void    setLastMsg ( char    *str                                   );
};


/** 
 * This class holds all the information that is stored of a
 * monitor. It *is a subclass of the ConnectionObject since it can
 * make a connection *with the server. No additional information is
 * stored.
 */ 
class Monitor : public ConnectionObject
{

};

/** 
 * This class holds all the information that is stored of an
 * agent. Among others this is its position and the fact whether it is
 * collided with another agent. It is a subclass of the
 * ConnectionObject since it can make a connection with the server. 
 */
class Agent : public ConnectionObject
{
   Pos        m_pos;                 /**< position of this agent             */
   Pos        m_posPrev;             /**< position before update             */
   bool       m_bCollided;           /**< boolean indicating agent collided  */
   bool       m_bCaught;             /**< boolean indicating prey was caught */
   int        m_iNumber;             /**< unique number for this agent       */
   bool       m_bSendMsg;            /**< boolean indicated agent sent msg   */
public:     
   Agent( int iNumber );

   // just simple get and set methods for the different variables   
   void    setPosition  ( int              x,    int              y   );
   void    setPosition  ( Pos pos                                     );   
   void    setPrevPos   ( Pos pos                                     ); 
   Pos     getPrevPos   (                                             ); 
   void    setPos       ( Pos pos                                     ); 
   Pos     getPos       (                                             ); 
   int     getX         (                                             ); 
   int     getY         (                                             ); 
   bool    isCollided   (                                             ); 
   void    setCollided  ( bool b                                      ); 
   bool    isCaught     (                                             ); 
   void    setCaught    ( bool b                                      ); 
   int     getNumber    (                                             ); 
   void    setNumber    ( int  i                                      ); 
   bool    getSendMsg   (                                             ); 
   void    setSendMsg   ( bool b                                      ); 

   // output methods
   friend  ostream   &   operator   << ( ostream   &os, Agent    *a   );    

};

#endif

