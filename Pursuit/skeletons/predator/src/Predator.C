/**
 * @file   Predator.C
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Mon Aug 11 14:09:03 2003
 * 
 * @brief This file contains a skeleton predator implementation that
 * handles all communication with the pursuit server. Only four
 * specific methods have to be implemented in which the policy is
 * specified.
 * 
 */
#include "Predator.h"    // all header files
#include <string.h>      // needed for strncmp
#include <stdio.h>       // needed for printf
#include <stdlib.h>      // needed for drand48
#include <sys/time.h>    // needed for gettimeofday

/**
 * Maximum message size
 */
#define MAX_MSG 128

/**
 * This method receives the incoming information from the pursuit
 * server. Nothing special is currently done with this information.
 * @param incoming string message
 */
void  Predator::processVisualInformation( char* str )
{
  // current implementation just parses the incoming message and prints the
  // relative position of the perceived predator/prey.
  
  bool bPred = false;
  bool bPrey = false;
  int nr, x, y, ret, iClosingBrackets = 1;
  str = str+4; // to after 'see'

  seenPreys=0;
  seenPreds=0;

  while( str && str[0] != ')' )
  {
    bPred = false;
    bPrey = false;
    ret = sscanf( str, " (predator %d %d)", &x, &y );
    if( ret == 0 )
      ret = sscanf( str, " ((predator %d) %d %d)", &nr, &x, &y );
    else
      bPred = true;
    if( ret == 0 )
      ret = sscanf( str, " (prey %d %d)", &x, &y );  
    else
      bPred = true;
    if( ret == 0 )
      ret = sscanf( str, " ((prey %d) %d %d)", &nr, &x, &y );
    else bPrey=true;

    if( ret == 0 )
      cerr << "(Predator::processVisualInformation) something wrong in msg " <<
        str << endl;
    else bPrey = true;
    
//    printf( "%s seen at (%d, %d)\n",(bPred==true) ? "predator" : "prey",x, y);

    if(bPred) {
	posPred[seenPreds].x=x;
	posPred[seenPreds].y=y;
        seenPreds++;
    }
    else if(bPrey) {
	posPrey[seenPreys].x=x;
	posPrey[seenPreys].y=y;
        seenPreys++;
//	cerr << "Seen Prey at " << x << " , " << y << endl;
    }

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
 * This method is called to determine the actual movement command of
 * the predator. The actual command string should be entered in the
 * specified string 'str'. 
 * @param str should be filled with the actual command string
 */
void Predator::determineMovementCommand( char* str )
{
  // this can be done better ;-)
  // don't forget it is also possible to execute (move none)!
  
  switch( (int)(drand48()*5) )
  {
    case 0: sprintf( str, "(move north)" ); break;
    case 1: sprintf( str, "(move south)" ); break;
    case 2: sprintf( str, "(move east)" );  break;
    case 3: sprintf( str, "(move west)" );  break;
    case 4: sprintf( str, "(move none)" );  break;
    default: break; 
  }                  
}

/**
 * This method receives the incoming communication from the other
 * predators (only when communication is turned on in the server
 * configuration file). Nothing special is currently done with this
 * information.  
 * @param incoming communication message
 */
void  Predator::processCommunicationInformation( char* str )
{

}

/** 
 * This method is called to determine the actual communication command
 * of the predator. The actual command string should be entered in the
 * specified string 'str'. 
 * @param str should be filled with the actual command string
 */
void Predator::determineCommunicationCommand( char* str )
{
  // example:  sprintf( str, "(say hello)" );
  
  sprintf( str, "(say hello)" );
}


/**
 * This method is called when an episode is ended and can be used to
 * reset some variables. 
 */
void Predator::episodeEnded( )
{
  // this method is called when an episode has ended and can be used to 
  // reinitialize some variables
  printf( "EPISODE ENDED\n" );
}
    

/**
 * This method is called when this predator is involved in a
 * collision.
 */
void Predator::collisionOccured( char * str  )
{
  // this method is called when a collision occured and can be used to 
  // reinitialize some variables
  cout << "collisionOccured: " << str << endl;
  printf( "COLLISION OCCURED\n" );
}

/**
 * This method is called when this predator is involved in a
 * penalization.  
 */
void Predator::penalizeOccured( char * str )
{
  // this method is called when a predator is penalized and can be used to 
  // reinitialize some variables
  cout << "penalizeOccured: " << str << endl;
  printf( "PENALIZED\n" );
}
    
void Predator::preyCaught( char * str )
{
  // this method is called when a prey is caught and can be used to 
  // reinitialize some variables
  cout << "preyCaught: " << str << endl;
  printf( "PREY CAUGHT\n" );

}
/***************************************************************************/
/********** CODE BELOW CAN BE KEPT UNCHANGED *******************************/
/***************************************************************************/

/** 
 * This constructor creates a socket connection with the port number on the 
 * specified host and sends the initialization string.
 *
 * \param strHost host to connect to
 * \param iPort port to connect to 
 */
Predator::Predator( char *strHost, int iPort )
{
  m_connection.connect( strHost, iPort );         // create connection
  m_connection.sendMessage( "(init predator)" );  // initialize predator
}

/**
 * This is the mainloop of the predator. Visual messages are received and 
 * movement commands can be send that will be executed by the server. 
 */
void Predator::mainLoop( )
{
  char strBuf [MAX_MSG];
  int  ret = 1;
  ret = m_connection.receiveMessage( strBuf, MAX_MSG );

  while( ret > 0 )
  {
    ret = m_connection.receiveMessage( strBuf, MAX_MSG );  // receive a message
    if( strncmp( strBuf, "(quit)", 6 ) == 0 )       // if message == quit
       ret = -1;                                    //   stop
    else if( strncmp( strBuf, "(hear", 4 ) == 0 )   // if message == hear
      processCommunicationInformation( strBuf );    //   process communication
    else if( strncmp( strBuf, "(see", 4 ) == 0 )    // if message == see
    {
      processVisualInformation( strBuf );           //  process visual msg
      determineCommunicationCommand( strBuf );      //  determine communication
      if( strBuf[0] != '\0' )                       //  and if not empty
      {
        m_connection.sendMessage( strBuf );         //   send it
      }
    }
    else if( strncmp( strBuf, "(send_action", 12 ) == 0 )// if msg==send_action
    {
      determineMovementCommand( strBuf );           //   determine action
      m_connection.sendMessage( strBuf );            //   and send it
    }
    else if( strncmp( strBuf, "(referee episode_ended", 22 ) == 0 )
    {
      episodeEnded();
    }
    else if( strncmp( strBuf, "(referee prey_caught", 20) == 0 )
    {
      preyCaught( strBuf );
    }
    else if( strncmp( strBuf, "(referee collision", 18 ) == 0 )
    {
      collisionOccured( strBuf );
    }
    else if( strncmp( strBuf, "(referee penalize", 17 ) == 0 )
    {
      penalizeOccured( strBuf );
    }
    else
      printf( "message not understood: %s\n", strBuf );
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
int main( int argc, char * argv[] ) { 
  // initialize the random seed
  timeval tv_now; 
  gettimeofday( &tv_now, NULL ); 
  srand48( tv_now.tv_usec );

  // read in the command arguments format: exectuable [[host] [port]]
  char strHost[MAX_MSG];
  int  iPort = 4001;

  if( argc == 1 )  
    strcpy( strHost, "localhost" );   // no extra argument use localhost
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
    
  Predator pred( strHost, iPort );                      // create the predator
  pred.mainLoop( );
      
}
