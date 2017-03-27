/**
 * @file   Prey.C
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
#include "Prey.h"        // all header files
#include <string.h>      // needed for strncmp
#include <stdio.h>       // needed for printf
#include <stdlib.h>      // needed for drand48
#include <sys/time.h>    // needed for gettimeofday

/**
 * Maximum message size
 */
#define MAX_MSG 1024
/**
 * Probability that prey will stand still
 */
#define PROB_STAY_STILL 0.2

/**
 * This method receives the incoming information from the pursuit
 * server. It is currently checked which positions are occupied
 * @param incoming string message
 */
void  Prey::processVisualInformation( char* str )
{
  int nr, x, y, ret, iClosingBrackets = 1;
  m_bEastOccupied=m_bWestOccupied = m_bNorthOccupied=m_bSouthOccupied=false;
  str = str+4; // to after 'see'

  closestPredatorX=0, closestPredatorY=0;
  while( str && str[0] != ')' )
  {
    ret = sscanf( str, " (predator %d %d)", &x, &y );
    if( ret == 0 )
      ret = sscanf( str, " ((predator %d) %d %d)", &nr, &x, &y );

    if( ret != 0 ){
      int dist = abs(closestPredatorX)+abs(closestPredatorY);
      if(dist==0 || (dist>(abs(x)+abs(y)))){
        closestPredatorX=x;
        closestPredatorY=y;
      }
    }

    if( ret == 0 )
      ret = sscanf( str, " (prey %d %d)", &x, &y );
    if( ret == 0 )
      ret = sscanf( str, " ((prey %d) %d %d)", &nr, &x, &y );

    if( ret == 0 )
      cerr << "(Prey::processVisualInformation) something wrong in msg " <<
        str << endl;
    else if( x == 0 && y == 1 )
      m_bNorthOccupied = true;
    else if( x == 0 && y == -1 )
      m_bSouthOccupied = true;
    else if( x == 1 && y == 0 )
      m_bEastOccupied = true;
    else if( x == -1 && y == 0 )
      m_bWestOccupied = true;
    
    if( ret == 3 )
      iClosingBrackets = 2;
    else
      iClosingBrackets = 1;
    int j = 0;
    while( str && j < iClosingBrackets )   // go to end of predator or prey info
    {
      if( str[0] == ')' )
        j++;
      str++;
    }
  }
}

/**
 * This method receives the incoming communication from the other
 * predators (only when communication is turned on in the server
 * configuration file). Nothing special is currently done with this
 * information.  
 * @param incoming communication message
 */
void  Prey::processCommunicationInformation( char* str )
{
}

/** 
 * This method is called to determine the actual communication command
 * of the predator. The actual command string should be entered in the
 * specified string 'str'. 
 * @param str should be filled with the actual command string
 */
void Prey::determineCommunicationCommand( char* str )
{
//	sprintf( str, "(say)" );
  str[0] = '\0';
}

/** 
 * This method is called to determine the actual movement command of
 * the predator. The actual command string should be entered in the
 * specified string 'str'. The prey stands still in PROB_STAND_STILL %
 * of the time and the remaing percentage is divided over the empty
 * cells next to it.
 *
 * @param str should be filled with the actual command string
 */
void Prey::determineMovementCommand( char* str )
{
  if(1){
    if( drand48() < 0.0 )
    {
      sprintf( str, "(move none)" );
      return;
    }
    // smart move
    if(abs(closestPredatorX)<abs(closestPredatorY)){
      if(closestPredatorX>0)
        sprintf( str, "(move west)" );
      else if(closestPredatorX<0)
        sprintf( str, "(move east)" );
      else{
        if( drand48() < 0.5 ) // if at 0 X, then move randomly
          sprintf( str, "(move west)" );
        else
          sprintf( str, "(move east)" );
      }
    }else if(abs(closestPredatorX)>abs(closestPredatorY)){
      if(closestPredatorY>0)
        sprintf( str, "(move south)" );
      else if(closestPredatorY<0)
        sprintf( str, "(move north)" );
      else{
        if( drand48() < 0.5 ) // if at 0 Y, then move randomly
          sprintf( str, "(move south)" );
        else
          sprintf( str, "(move north)" );
      }
    }else{
      if(closestPredatorX>0 && closestPredatorY>0){
        if( drand48() < 0.5 ) // move randomly
          sprintf( str, "(move west)" );
        else
          sprintf( str, "(move south)" );
      }else if(closestPredatorX>0 && closestPredatorY<0){
        if( drand48() < 0.5 ) // move randomly
          sprintf( str, "(move west)" );
        else
          sprintf( str, "(move north)" );
      }else if(closestPredatorX<0 && closestPredatorY>0){
        if( drand48() < 0.5 ) // move randomly
          sprintf( str, "(move east)" );
        else
          sprintf( str, "(move south)" );
      }else if(closestPredatorX<0 && closestPredatorY<0){
        if( drand48() < 0.5 ) // move randomly
          sprintf( str, "(move east)" );
        else
          sprintf( str, "(move north)" );
      }
    }
    return;
    // end smart move
  }

  if( drand48() < PROB_STAY_STILL )
  {
    sprintf( str, "(move none)" );
    return;
  }
  
  int arrayDir[4]; // 0 = north, 1 = south, 2 = east, 3 = west
  int iNr = 0;
  if( m_bNorthOccupied == false )
    arrayDir[iNr++] = 0;
  if( m_bSouthOccupied == false )
    arrayDir[iNr++] = 1;
  if( m_bEastOccupied == false )
    arrayDir[iNr++] = 2;
  if( m_bWestOccupied == false )
	  arrayDir[iNr++] = 3;
  
  int index = (int)(drand48()*iNr);
  switch( arrayDir[index] )
  {
  case 0: sprintf( str, "(move north)" ); break;
  case 1: sprintf( str, "(move south)" ); break;
  case 2: sprintf( str, "(move east)" );  break;
  case 3: sprintf( str, "(move west)" );  break;
  default: break; 
  }									
}

/**
 * This method is called when an episode is ended and can be used to
 * reset some variables. 
 */
void Prey::episodeEnded( )
{
   // this method is called when an episode has ended and can be used to 
   // reinitialize some variables
}
				
/**
 * This constructor creates a socket connection with the port number
 * on the specified host and sends the initialization string.
 *
 * \param strHost host to connect to
 * \param iPort port to connect to 
 */
Prey::Prey( char *strHost, int iPort )
{
  m_connection.connect( strHost, iPort );     // create connection
  m_connection.sendMessage( "(init prey)" );	// initialize prey
}

/**
 * This is the mainloop of the prey. Visual messages are received and
 * movement commands can be send that will be executed by the
 * server. 
 */
void Prey::mainLoop( )
{
  char strBuf [MAX_MSG];
  int  ret = 1;
  ret = m_connection.receiveMessage( strBuf, MAX_MSG );
  cout << "recv: " << strBuf << endl; 
  while( ret > 0 )
  {
    ret = m_connection.receiveMessage( strBuf, MAX_MSG );// receive a message
//  cerr << "process " << strBuf << endl;
    if( strncmp( strBuf, "(quit)", 6 ) == 0 )       // if message == quit
			 ret = -1;                  //   stop
    else if( strncmp( strBuf, "(hear", 4 ) == 0 )   // if message == hear
      processCommunicationInformation( strBuf );    //   process communication
    else if( strncmp( strBuf, "(see", 4 ) == 0 )    // if message == see
    {
      processVisualInformation( strBuf );           //   process visual msg
      determineCommunicationCommand( strBuf );      //   determine communic.
      if( strBuf[0] != '\0' )                       //   and if not empty
      {
        m_connection.sendMessage( strBuf );         //    send it
      }
    }
    else if( strncmp( strBuf, "(send_action", 12 ) == 0 )// if msg==send_action
    {
      determineMovementCommand( strBuf );           //   determine action
      m_connection.sendMessage( strBuf );	    //   and send it
    }
    else if( strncmp( strBuf, "(referee episode_ended", 22 ) == 0 )
    {
      episodeEnded();
    }	
    else
      cout << "message not understood: " << strBuf << endl;
  }
  printf( "exit\n" );
}


/**
 * This is the main of the program and intializes all classes and
 * creates the Connection with server.
 *
 * @param argc number of arguments to command prompt
 * @param argv string arguments specified
 */
int main( int argc, char * argv[] )
{
  // initialize the random seed
  timeval tv_now;
  gettimeofday( &tv_now, NULL );
  srand48( tv_now.tv_usec );
  
  // read in the command arguments format: exectuable [[host] [port]]
  char strHost[MAX_MSG];
  int  iPort = 4001;
  
  if( argc == 1 )  
    strcpy( strHost, "localhost" );	 // no extra argument use localhost
  else 
  {
    if( strncmp( argv[1], "-h", 2 ) == 0 )              // print help
      printf( "usage: %s [[host] [port]]\n", argv[0] );
    else                                                // else set host
      strcpy( strHost, argv[1] );               
    if( argc > 2 )                                      // and port 
      iPort = atoi( argv[2] );
  }
  
  printf( "host: %s\nport : %d\n", strHost, iPort );
  
  Prey prey( strHost, iPort );                      // create the Prey
  prey.mainLoop( );
  
}
