/**
 * @file   LogPlayer.C
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Mon Aug 11 11:40:51 2003
 * 
 * @brief This file contains the declaration of the LogPlayer
 * class. This class stores all the world information and methods to
 * iterate over this information. Furthermore, it also handles the
 * interaction with the monitor(s).
 */
#include "LogPlayer.h"

#include <iostream>       // needed for cerr
#include <stdlib.h>       // needed for exit
#include <fstream>        // needed for 'ifstream'
#include <stdio.h>        // needed for sscanf
#include <unistd.h>       // needed for usleep
#include <string.h>       // needed for strlen

using namespace std;

/**
 *  This is the constructor and sets all member variables to default
 *  values. */
LogPlayer::LogPlayer()
{
  m_iPort = 4001;
  m_status = STATUS_ILLEGAL;
}

/**
 * This is the constructor and reads in all the world information and
 * sets all the member variables.
 *
 * @param iPort port number monitor clients can connect to 
 * @param strFile filename which contains all the world state information
 * @param status current status 
 */
void LogPlayer::init( int iPort, char * strFile, StatusT status )
{
  readFileContents( strFile );       // read all world contents (in m_World)
  m_iPort = iPort;                   // set port number
  setStatus( status );
  m_iter_world = m_World.begin();    // set iterator to first cycle
  m_bHalt = false;
  m_iTimeStep = 100;                 // set time_step between 2 cycles (msec)
}

/**
 *  This is the destructor of the LogPlayer. It deletes all world
 *  state information stored in a list from memory.
 */
LogPlayer::~LogPlayer( )
{
  list<LogWorld*>::iterator iter ;  
  iter = m_World.begin();
  while( ! m_World.empty() )
  {
    delete *iter;                             
    iter = m_World.erase( iter );
  }  
}

/**
 * This method sets the status of the logplayer. This indicates
 * whether the logplayer is for example playing. stopped or playing
 * one frame at a time. 
 */
void LogPlayer::setStatus( StatusT status )
{
  m_status = status;

  // set the status, when status is either step forward or backward,
  // we also advance one cycle in the corresponding direction
  switch( m_status )
  {
    case STATUS_STEP:   
      goToNextFrame();    
      sendInfoToMonitors();  
      break;
    case STATUS_REV_STEP:   
      goToPrevFrame();    
      sendInfoToMonitors();  
      break;
    default:
      break;
  }
}

/**
 *  This method reads all the world state information from the
 *  specified file and stores it in the world datastructures. 
 * 
 * @param strFile file with all world states information
 */
void LogPlayer::readFileContents( char *strFile )
{ 
  char strLine[MAX_MSG];
  ifstream in( strFile );
  int iLineNr = 0;
  
  if( !in )
  {
    cerr << "(LogPlayer::readFileContents) Could not open file '" 
         << strFile << "'" << endl;
    exit( 1 );
  }
  
  int    iEpisode, iCycle, iPenalties, iCollisions;
  double dAvgCaptTime;
  
  while( in.getline( strLine, sizeof( strLine ) ) )
  {
    iLineNr++;
    // if information is information about server parameters, store line
    if( strncmp( strLine, "(server_param", 13 ) == 0 )
      strcpy( m_strServerParam, strLine );
    // if information is information about a world state, process
    else if( strncmp( strLine, "(world (stats", 13 ) == 0 )
    {
      // each LogWorld class holds one state of the world
      LogWorld * logWorld = new LogWorld();

      // for all world state lines, first read in the statistics      
      int ret = sscanf( strLine, "(world (stats %d %d %lf %d %d)",  
                              &iEpisode, 
                              &iCycle, 
                              &dAvgCaptTime, 
                              &iPenalties,    
                              &iCollisions );

      if( ret !=  5 )
      {
        cerr << "(LogPlayer::readFileContents) Error in reading world " <<
          "statistics (line " << iLineNr << "): " << strLine << endl;
        exit( 1 );
      }

      // add the statistics 
      logWorld->addStats(iEpisode,iCycle,iPenalties,iCollisions,dAvgCaptTime);
  
      // then read in all agent information and store it in class
      int  i = 0, j = 0 ;
      int  x, y, nr;
      char c;
      bool isPred = true;
      
      // and then all agent information
      while( strLine[i] != '\0' && j == 0 )  // go to the end of the statistics
      {
        if( strLine[i] == ')' )              // this is after first closing ')'
        j++;
        i++;
      }

      while( strLine[i] != '\0' && strLine[i] != ')' ) // while new agent info
      {
        // first try to read predator information
        ret = sscanf( &strLine[i]," ((predator %d%c) %d %d)", &nr, &c, &x, &y);
        if( ret == 4 )
          isPred = true;
        else                                   // try to read prey information.
        {
          ret = sscanf( &strLine[i], " ((prey %d%c) %d %d)", &nr, &c, &x, &y);
          if( ret == 4 )
            isPred = false;
          else
          {
            cerr << "(LogPlayer::readFileContents) error in line " << iLineNr
                 << ": " <<  &strLine[i] << endl;
            exit( 1 );
          }
        }
        logWorld->addAgent( isPred, nr, x, y, (c == 'c'), (c=='x' ) );        

        // now advance to second closing bracket after agent information.
        // (world (stats 1 12 31.3 3 3) ((predator 1 ) 4 4))
        j = 0;
        while( strLine[i] != '\0' && j < 2 ) // didn't find closing bracket yet
        {
          if( strLine[i] == ')' ) 
            j++;
          i++;
        }          
      }
      // add this state to the list of all world states (m_World).
      m_World.push_back( logWorld );  
    }
  }

  if( m_World.size() == 0 )
  {
    cerr << "(LogPlayer::readFileContents) no date in file " 
         << strFile << endl;
    exit( 1 );
  }
}

/**
 *  This method is called in a separate thread and waits for incoming
 *  connection requests. When such a request arrives, it is checked
 *  whether it is a monitor. When this is the case, it is added to the
 *  monitor list. 
 */
void LogPlayer::acceptNewConnection( )
{
  Connection server;      
  char       message[MAX_MSG];
  
  server.bind( m_iPort );   // open the port
  
  while( m_bHalt == false )
  {
    // wait for new connection
    struct sockaddr_in cli_addr = server.waitForNewConnection(message,MAX_MSG);

    // received new connection, use host and port number to connect 
    unsigned long host = ntohl ( cli_addr.sin_addr.s_addr );
    int           port = ntohs ( cli_addr.sin_port );  

    // if it is a monitor, create a new clinet and add it to list
    if( strncmp( message, "(init monitor)", 14 ) == 0 )
    {
      Connection *client = new Connection();
      client->bind( );
      client->connect( port, host );
      client->sendMessage( "(init ok)" );
      client->sendMessage( m_strServerParam );
      cerr << "monitor connected" << endl;
      m_Monitors.push_back( client );      
      sendInfoToMonitors();        
    }
    else
      cout << "wrong init messages: " << message <<  endl;
  }
}

/**
 *  This method is called in a separate thread an handles the incoming
 *  messages of all the monitors.  
 */ 
void LogPlayer::receiveMessages()
{
  struct timeval tv;
  int ret, iMax = -1;
  char str[MAX_MSG];
  list<Connection*>::iterator iter ;  
      
  // as long as we do not halt yet
  while( m_bHalt == false )
  {
    tv.tv_sec   = 2;     // reset after two seconds to be able to also add
    tv.tv_usec  = 0;     // newly connected monitors
    iMax        = -1;
    fd_set readfds;
    FD_ZERO( &readfds );
    iter = m_Monitors.begin();
    // add all monitor file descriptors
    while( iter != m_Monitors.end() )    
     {
      // if connected, add them
      if( (*iter)->isConnected() )
      { 
        int fd = (*iter)->getSocketFd();
        FD_SET( fd, &readfds );
        if( fd > iMax ) 
          iMax = fd;
        iter++;                
      }
      // otherwise delete 
      else
      {
        delete *iter;   
        iter = m_Monitors.erase( iter );
        cerr << "monitor disconnected" << endl;
      }
    }  
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
      iter = m_Monitors.begin();
      while( iter != m_Monitors.end() )
      {
        if( FD_ISSET( (*iter)->getSocketFd(), &readfds ) )
        {
          (*iter)->receiveMessage( str, MAX_MSG );
          processMonitorMessage( str );
        }
        iter++;
      }
    }
  }
}

/**
 *  This method processes a message from a monitor. This message can
 *  change the status of the logplayer (step to next frame or start
 *  playing) or the time that is waited between two successive
 *  frames. 
 */
void LogPlayer::processMonitorMessage( char * str )
{
  // change status based on user input as follows:
  // continue -> play
  // pause -> stop
  // quit -> halt
  // step -> steo
  // speed_up -> lower time_step between cycles
  // speed_down -> increase time_step between cycles
  if( strncmp( str, "(monitor continue)", 18 ) == 0 )
    setStatus( STATUS_PLAY );
  else if( strncmp( str, "(monitor pause)", 15 ) == 0 )
    setStatus( STATUS_STOP );  
  else if( strncmp( str, "(monitor quit)", 14 ) == 0 )
    m_bHalt = true;
  else if( strncmp( str, "(monitor step)", 14 )  == 0 )
   setStatus( STATUS_STEP );
  else if( strncmp( str, "(monitor speed_up)", 18 )  == 0 )    
    m_iTimeStep = ( m_iTimeStep > 20 ) ? m_iTimeStep - 10 : m_iTimeStep;
  else if( strncmp( str, "(monitor speed_down)", 20 )  == 0 )    
    m_iTimeStep = ( m_iTimeStep < 1000 ) ? m_iTimeStep + 10 : m_iTimeStep;
  else if( strlen( str ) > 0 )
    cerr << "(LogPlayer::processMonitorMessage) unknown msg: " << str << endl;

}

/**
 *  This method sends the information of the current frame to all
 *  monitors. 
 */
void LogPlayer::sendInfoToMonitors( )
{
  static char str[MAX_MSG];
  (*m_iter_world)->createInfoString( str );   // make string of current frame
  list<Connection*>::iterator iter ;          // sent it to all monitors
  iter = m_Monitors.begin();
  while( iter != m_Monitors.end() )
   {
    (*iter)->sendMessage( str );
    iter++;
  }  
}

/**
 *  This methods send an exit message to all connected monitors. 
 */
void LogPlayer::sendMonitorExitMessages( )
{
  list<Connection*>::iterator iter ;  
  iter = m_Monitors.begin();
  while( iter != m_Monitors.end() )
   {
    (*iter)->sendMessage( "(quit)" );
    iter++;
  }  
}

/**
 *  This is the mainloop. Depending on the current status it either
 *  does nothign or advances to the next or previous frame. This
 *  information of the new frame is then send to all monitors. 
 */
void LogPlayer::mainLoop()
{
  while( ! m_bHalt )
  {
    // if status is play or reverse play, go to the next frame
    // do not do anything in case of step, this is done in setStatus
    if( m_status == STATUS_PLAY || m_status == STATUS_REV_PLAY )
    {
      if( m_status == STATUS_PLAY )
        goToNextFrame();
      else
        goToPrevFrame();
      sendInfoToMonitors();
    }
    // sleep for the specified time_step msecs. 
    usleep( m_iTimeStep * 1000 );
  }
  sendMonitorExitMessages( );
}

/**
 *  This method advanced to the next frame. 
 */
void LogPlayer::goToNextFrame( )
{
  m_iter_world++; // advance one frame and set status to stop when at end
  if( m_iter_world == m_World.end() )
  {
    m_iter_world--;
    m_status = STATUS_STOP;
  }
}

/**
 *  This method sets the frame to the previous frame. 
 */
void LogPlayer::goToPrevFrame( )
{
  // at first frame -> stop, else decrease frame with one
  if( m_iter_world != m_World.begin() )
    m_iter_world--;
  else
    m_status = STATUS_STOP;
}

/**
 *  This method jumps to the specified frame. 
 */
void LogPlayer::jumpToFrame( int iEpisode, int iCycle )
{
  bool bForward;
  list<LogWorld*>::iterator iter_tmp = m_iter_world;
  
  if( (*iter_tmp)->getNrEpisode() == iEpisode )           // equal episode
    bForward = ( (*iter_tmp)->getNrCycle() <= iCycle );   // forward if cycle >
  else                                                    // else
    bForward = ( (*iter_tmp)->getNrEpisode() < iEpisode );// forward if episod>
  
  // if we have to move forward, step forward one by one till we're thre
  if( bForward == true )
  {
    while( iter_tmp != m_World.end() &&                // go forward until end
          (*iter_tmp)->getNrEpisode() <= iEpisode )    // reached or too far.
    {
      if( (*iter_tmp)->getNrEpisode() == iEpisode && 
          (*iter_tmp)->getNrCycle() == iCycle )
      {
        m_iter_world = iter_tmp;                       // return when found
        sendInfoToMonitors();
        return;
      }
      iter_tmp++;
    }
  }
  else
  {
    // go backward till we are there
    while( iter_tmp != m_World.begin() &&             // go backward 
           (*iter_tmp)->getNrEpisode() >= iEpisode )  // reached or too far.
    {
      if( (*iter_tmp)->getNrEpisode() == iEpisode && 
          (*iter_tmp)->getNrCycle() == iCycle )
      {
        m_iter_world = iter_tmp;                       // return when found
        sendInfoToMonitors();        
        return;
      }
      iter_tmp--;
    }
  }      
  cerr << "Error: frame (" << iEpisode<< "," << iCycle << ") does not exist\n";

}

/**
 *  This method exits the program and sends the exit messages to all
 *  the monitors. It is called after an interrupt has arrived. 
 */
void LogPlayer::exitHandler(  )
{
  if( m_bHalt == false ) // only do this once after halted. 
  {
    m_bHalt = true;  
    sendMonitorExitMessages();          
  }
}

/**
 * This method is called by a thread with a void pointer to a
 * logplayer class as its argument. It calls the acceptNewConnection
 * method of this class. 
 * @param v void pointer to logplayer instanation
 */
void* LogPlayer::thrAccept( void * v)
{
  ((LogPlayer*)v)->acceptNewConnection(); 
  return NULL;
}

/**
 * This method is called by a thread with a void pointer to a
 * logplayer class as its argument. It calls the receiveMessages
 * method of this class. 
 * @param v void pointer to logplayer instanation
 */
void* LogPlayer::thrReceive( void * v)
{
  ((LogPlayer*)v)->receiveMessages(); 
  return NULL;
}

/**
 * This method is called by a signal with a void pointer to a
 * logplayer class as its argument. It calls the exitHandler method of
 * this class. 
 * @param i not used
 * @param v void pointer to logplayer instanation
 */
void LogPlayer::exitHandler( int i, void * v)
{   
  ((LogPlayer*)v)->exitHandler(); 
}

/**
 * This method halts the logplayer. As a result, the logplayer will
 * send an exit message to all monitors and quit. 
 */
void LogPlayer::setHalt( bool b )
{ 
  cerr << "halt" << endl; 
  fflush(stderr);
  m_bHalt = b;
}


/**
 * This method returns the status of the logplayer. 
 * @return status of the logplayer. 
 */
StatusT LogPlayer::getStatus( )
{ 
  return m_status; 
}	
