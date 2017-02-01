/**
 * @file   Prey.h
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Mon Aug 11 14:09:03 2003
 * 
 * @brief This file contains a prey implementation that handles all
 * communication with the pursuit server. Four specific methods have
 * to be implemented in which the policy is specified. The default
 * behavior is such that the prey stands still 20% of the time and the
 * remaining 80% are divided over the free cells next to it.
 * 
 */
#ifndef _PREY_
#define _PREY_

#include "Connection.h"

class Prey
{
  Connection m_connection;     /**< Connection with server          */
  bool       m_bEastOccupied;  /**< bool whether east cell is full  */
  bool       m_bWestOccupied;  /**< bool whether west cell is full  */
  bool       m_bNorthOccupied; /**< bool whether north cell is full */
  bool       m_bSouthOccupied; /**< bool whether south cell is full */
  int 	     closestPredatorX;
  int        closestPredatorY;
	
public:
       Prey                           ( char *strHost, 
                                        int   iPort     );
  void mainLoop                       (                 );
  void processVisualInformation       ( char* str       );
  void processCommunicationInformation( char* str       );
  void determineMovementCommand       ( char* str       );
  void determineCommunicationCommand  ( char* str       ); 
  void episodeEnded                   (                 );
	
};


#endif
