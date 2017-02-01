/**
 * @file   main.C
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Mon Aug 11 13:42:33 2003
 * 
 * @brief This file contains the main of the logplayer. It initalizes
 * all classes and calls the appropriate mainloops.
 */
#include <signal.h>     // needed for sigaction
#include <unistd.h>     // needed for getpid
#include <stdlib.h>     // needed for atexit
#include <string.h>     // needed for strlen
#include <pthread.h>    // needed for pthread_t
#include "types.h"      // needed for STATUS_STOP

#include "Controller.h"
#include "LogPlayer.h"

bool showOptions( char *argv[] );
bool readOptions( int argc, char *argv[], int *iPort, int *iArgFile );
void exitHandler( int i, void * );  // called when program exits
		
LogPlayer g_player;                    /**< Instantiation of the logplayer. */

#ifndef HAVE_ON_EXIT
/**
 * This function is defined as a wrapper function for
 * 'exitHandler'. This function is used as the callback function for
 * the redefinition of on_exit on systems where on_exit is not
 * available. 
 */
void exitHandlerSolaris( )
{
  exitHandler( 0, NULL );
}

/** 
 * This function is a redefinition for on_exit on machines where *
 * on_exit is not available.
 */
int on_exit( void(*function)(int, void*),void *arg)
{
  atexit( &exitHandlerSolaris );
  return 0;
}
#endif

/**
 * This function is called just before the program terminates. It
 * sends all the monitors an exit message and then kills itself. 
 * @param i not used
 */
void exitHandler( int i, void * )
{
  g_player.sendMonitorExitMessages( );
  signal(SIGINT, SIG_DFL);
  kill(getpid(), SIGINT);
}


/**
 *  This is the main function. It creates the controller (display and
 *  callback functions) and the logplayer (holds all possible world
 *  states and deals with socket communication with the monitors). 
 *
 * @param argc number of arguments to command line options
 * @param argv different supplied string
 */
int main(int argc, char *argv[])
{
  //  char *id="$Id: main.C,v 1.1 2005/01/06 12:34:54 jellekok Exp $";
  //  cerr << id << endl;
  cerr << "Starting logplayer" << endl;

  pthread_t thrControl, thrListen, thrReceive;
  int iPort = 4001, iArgFile = 0;
	
  // read the options from the command line
  if( readOptions( argc, argv, &iPort, &iArgFile ) == false )
    return 1;

  // not set, no argument specified which contains logfile
  if( iArgFile == 0 )
  {
    cerr <<  "Error: need logfile as input; exit." << endl; 
    return 1;
  }
  
  g_player.init( iPort, argv[iArgFile], STATUS_STOP );  // initialize logplayer
  Controller * controller = new Controller( &g_player, argv[0] );
  if( controller == NULL )
  {
    cerr << "(main)  Could not instantiate controller" << endl;
    exit( 1 );
  }
  controller->init(argc, argv) ;
  controller->makeWindow();

  // create threads for listening for new connected monitors, visualization by
  // controller and receiving messages from monitors
  pthread_create( &thrListen,  NULL, LogPlayer::thrAccept, &g_player );
  pthread_create( &thrControl, NULL, Controller::loop, controller );
  pthread_create( &thrReceive, NULL, LogPlayer::thrReceive, &g_player );  

  // specify what to do when the program is interrupted. 
  struct sigaction sig_action;
  sig_action.sa_handler = (void (*)(int))exitHandler;
  sig_action.sa_flags   = 0;
  sigaction( SIGINT, &sig_action, NULL );

  // call same exitHandler when program ends normal
  on_exit( exitHandler, NULL);
	
  // start the logplayer
  g_player.mainLoop(); 

  return 0 ;
}

/** 
 * This function shows the possible options for this program.  
 * @param argv options supplied to the command prompt
 * 
 * @return bool indicating whether was printed sucessfully.
 */
bool showOptions( char *argv[] )
{
  cerr << argv[0] << " [-port port_nr] logfile" << endl;
  return false;
}



/** 
 * This function reads the supplied options and sets the associated
 * variables when set. Two arguments are specified. iPort will be set
 * with the port number to which the monitors have to connect to
 * (default 4001). This is parsed from the '-port nr'
 * argument. iArgFile will be filled with the index of argv that
 * contains the logfile with all the data that should be displayed.
 *
 * @param argc number of arguments
 * @param argv strings of arguments
 * @param iPort will be filled with port number
 * @param iArgFile indicates where logfile was supplied as argument
 * @return bool indicating whether options were read successfully
 */
bool readOptions( int argc, char *argv[], int *iPort, int *iArgFile )
{
  if( argc == 1 )
    return showOptions( argv );
  
  int i = 1 ; 
  while( i < argc )                                    // for all options
  {
    if( argv[i][0] == '-' && strlen( argv[i] ) > 1)    // if option found
    {
      if( strncmp( argv[i] , "-h" , 2 ) == 0 )         // -h shows options
        return showOptions( argv );
      else if( strncmp( argv[i], "-p", 2 ) == 0 )      // -p indicates port
       {
        if( argc <= i + 1 || sscanf( argv[i+1], "%d", iPort ) != 1 )
        {
          cerr << "Error: reading port from argument."<< endl;
          return false;     
        }
        i += 2;
      }
    }
    else                                               // no specific '-'option
    {
      *iArgFile = i;                                   // assume is logfile
      i++;
    }
  }
  return true;
}

