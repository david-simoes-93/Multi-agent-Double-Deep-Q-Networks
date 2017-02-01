/**
 * @file   Predator.h
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Mon Aug 11 14:09:03 2003
 * 
 * @brief This file contains a skeleton predator implementation that
 * handles all communication with the pursuit server. Only four
 * specific methods have to be implemented in which the policy is
 * specified.
 * 
 */
#ifndef _PREDATOR_
#define _PREDATOR_

#include "Connection.h"

class Vec
{
	public:
		int x,y;
};

/** 
 * This class contains the methods to specify the behavior of the predator. 
 */
class Predator
{
  Connection m_connection;             /**< Connection with server */
	
public:
       Predator                       ( char *strHost, 
                                        int   iPort     );
  void mainLoop                       (                 );
  void processVisualInformation       ( char* str       );
  void processCommunicationInformation( char* str       );
  void determineMovementCommand       ( char* str       );
  void determineCommunicationCommand  ( char* str       ); 
  void episodeEnded                   (                 );
  void collisionOccured               ( char* str       );
  void penalizeOccured                ( char* str       );
  void preyCaught                     ( char* str       );

protected:
  int seenPreds, seenPreys;
  Vec posPrey[10],posPred[10];
	
};


#endif
