/**
 * @file   main.C
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Fri Aug  1 11:39:57 2003
 * 
 * @brief This file contains the main of the program that is used to
 * start the different threads that handle the new connections,
 * existing connections and the update of the world.
 */


#include <string.h>        // needed for strlen
#include <pthread.h>       // needed for pthread
#include "World.h"         // needed for World
#include <signal.h>        // needed for sigaction
#include <unistd.h>        // needed for getpid
#include <stdlib.h>        // needed for atexit


/**
 * The global container for the current world state 
 */
World WORLD;               

void  exitHandler( int i, void * );  // called when program exits
void* acceptNewConnection( void * ); // thread method to accept new connections
void* receiveMessages( void* );      // thread method to receive messages
void* mainLoop( void* );             // main program thread 
void  readOptions( int argc, char ** argv ); // read command options
void  showOptions();                 // show the command options

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

/** This function is a redefinition for on_exit on machines where
 * on_exit is not available. 
 */
int on_exit( void(*function)(int, void*),void *arg)
{
  atexit( &exitHandlerSolaris );
  return 0;
}
#endif

/** 
 * This function is called when the program exits. It sends all the clients
 * an exit message and then kills itself. 
 */
void exitHandler( int i, void * )
{
  WORLD.setHalted( true );  
  WORLD.sendExitMessages();          
  signal(SIGINT, SIG_DFL);
  kill(getpid(), SIGINT);
}

/**
 * This method calls the accepNewConnection method from the World class. Since
 * a thread cannot be started with a class method, this wrapper function is
 * created. 
 * @param void never used
 */
void* acceptNewConnection( void * )
{
  WORLD.acceptNewConnection();
  return NULL;
}

/** 
 * This method calls the receiveMessages method from the World class. Since
 * a thread cannot be started with a class method, this wrapper function is
 * created. 
 */
void* receiveMessages( void* )
{
  WORLD.receiveMessages();
  return NULL;
}

/**
 * This method calls the mainLoop method from the World class. Since
 * a thread cannot be started with a class method, this wrapper function is
 * created. 
 * @param void never used
 */
void* mainLoop( void* )
{
  WORLD.mainLoop();
  return NULL;
}

/**
 * This function reads and processes all the parameters supplied at  
 * the command prompt. 
 * @param argc number of arguments
 * @param argv string representations of the arguments
 */
void readOptions( int argc, char **argv )
{
  for( int i = 1 ; i < argc ; i = i + 2  )
  {
    // read a command option
    if( argv[i][0] == '-' && strlen( argv[i] ) > 1)
    {
      switch( argv[i][1] )
      {
        case 'c':                                  // read configuration file
          if( WORLD.readValues( argv[i+1], ":" ) == false )
            cerr << "Error reading configuration file: " << argv[i+1] << endl;
          break;                                   // display help
        case 'h':
           showOptions();
           exit(0);
        default:
          break;
      }
    }
  }
}

/**
 * This function prints the possible command options to the screen. 
 */
void showOptions( )
{
  cout << " -h     :    print this help message"         << endl 
       << " -c file:    reads in parameters from 'file'" << endl  ;
}

/**
 * This function starts all the threads that are needed for the program. 
 * @param argc number of arguments
 * @param argv string representations of the arguments
 */
int main(int argc, char **argv)
{
  int i;
  pthread_t thrListen, thrReceive;
  
  readOptions( argc, argv );               // read the command line options

  // start the different threads
  pthread_create( &thrListen,  NULL, acceptNewConnection,  NULL );
  pthread_create( &thrReceive, NULL, receiveMessages,      NULL );  

  // specify what to do when the program is interrupted. 
  struct sigaction sig_action;
  sig_action.sa_handler = (void (*)(int))exitHandler;
  sig_action.sa_flags   = 0;
  sigaction( SIGINT, &sig_action, NULL );

  on_exit( exitHandler, &i );              // on exit, call exitHandler
  WORLD.mainLoop( );

  return 0;
}
