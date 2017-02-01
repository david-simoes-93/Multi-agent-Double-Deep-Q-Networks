/**
 * @file   Monitor.C
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Thu Aug  7 17:41:34 2003
 * 
 * @brief This file contains the declerations of all the methods that
 * store and visualize the current world state using the GLUT program.
 */

#include "RGBcolor.h"          // needed for color representation
#include <math.h>              // needed for cos and sin
#include "Monitor.h"           // needed for function definitions
#include "MonitorParameters.h" // needed for parameters
#include "Connection.h"        // needed for connection with server
#include <stdio.h>             // needed for printf
#include <pthread.h>           // needed for pthread
#include <unistd.h>            // needed for usleep
#include <string.h>            // needed for strlen
#include <sys/time.h>

/** 
 * Maximum length of one message. 
 */
#define MAX_MSG 1024

/** 
 * Are we in debug method or not.
 */
#define DEBUG 0

// global variables
int               WIN;              /**< This is the window variable.        */
int               WIN2;             /**< This is the 2nd window variable.    */
MonitorWorld      WORLD;            /**< All the world information           */
MonitorParameters PS;               /**< All the parameters                  */
Connection        CONNECTION;       /**< Connection with the server          */
bool              bSTART = false;   /**< Indicates whether ready to display  */


/**
 * This constructor resets all variables for the Agent class. 
 */
Agent::Agent()
{
  m_iX        = 0;
  m_iY        = 0;
  m_iNr       = 0;
  m_bCollided = false;
  m_bCaught   = 0;
}

/**
 * This constructor for the MonitorWorld class resets all variables.
 */
MonitorWorld::MonitorWorld( )
{
  m_iNrPredators        = 0;
  m_iNrPreys            = 0;
  m_iNrCollisions       = 0;
  m_iNrCycle            = 0;
  m_iNrEpisode          = 0;
  m_dAverageCaptureTime = 0;
  m_bRedisplay          = true;
  
}


/**
 * This function redisplays the lower part of the window where the
 * statistics information is printed.
 */
void display2(void)
{
  char     strNr[128];  
  glutSetWindow( WIN2 );                           // set to stats window
  glClear(GL_COLOR_BUFFER_BIT);                    // clear the current buffer

#if DEBUG
  struct timeval tv_old, tv_new;
  double time_diff;
  gettimeofday( &tv_old, NULL );
#endif

  // display game statistics 
  glColor3f(0,0,0);                                 // make writing color=black
  glRasterPos2f( 0.5, 0.5 );                        // go to lower left corner
  sprintf( strNr,                                   // make statistics
    "episode %3d  cycle %4d  average capture time %.2f  penalties %3d  collisions %3d", 
               WORLD.m_iNrEpisode,
	       WORLD.m_iNrCycle, 
	       WORLD.m_dAverageCaptureTime,    
	       WORLD.m_iNrPenalties, 
	       WORLD.m_iNrCollisions );
#if DEBUG
  gettimeofday( &tv_old, NULL );
  cerr << "strNr: " << strlen( strNr ) << endl; 
#endif

  if( PS.getDisplayStatistics() == true )
    for( size_t i = 0 ; i < strlen( strNr ) ; i ++ )  // and write them
    {
      glutBitmapCharacter( GLUT_BITMAP_HELVETICA_10, strNr[i] );      
    }

#if DEBUG
  gettimeofday( &tv_new, NULL );
  time_diff = ((double)tv_new.tv_sec + (double)tv_new.tv_usec/1000000)-
                      ((double)tv_old.tv_sec + (double)tv_old.tv_usec/1000000);
  printf( "statistics time_diff: %f\n", time_diff * 1000 );
  tv_old = tv_new;
#endif
 
  glutSwapBuffers();                  // swap buffer (use double buffering)
}

/**
 * This function is called each time the world should be
 * redisplayed. This happens after the window is refocused, or it is
 * called explicitly when an update of the world has been done. This
 * method displays the grid and all agents located in it.
 */
void display(void)
{
#if DEBUG
  static struct timeval tv_old;
  struct timeval tv_new;

  gettimeofday( &tv_old, NULL );
  cerr << "redisplay " << endl;  
  tv_new = tv_old;
#endif
  glClear(GL_COLOR_BUFFER_BIT);                    // clear the current buffer
  int      x=0, y=0, nr=0;                         // initialize some variables
  bool     bCollided, bCaught; 
  RGBcolor rgb;
  char     strNr[128];  
  
  if( PS.getShowVisibleRange() == true )           // if visible range has to
  {                                                // be displayed
    static int iArrayVisual[MAX_CELLS][MAX_CELLS];
    int        iVis = PS.getVisibleDistance();
    
    for( int i = 0 ; i < PS.getRows(); i ++ )      // first create empty array
      for( int j = 0; j < PS.getColumns() ; j++ )
        iArrayVisual[i][j] = 0;

    for( int i = 0 ; i < WORLD.m_iNrPredators ; i ++  )
    {
      x  = WORLD.m_Predators[i].m_iX;              // and add one to each cell
      y  = WORLD.m_Predators[i].m_iY;              // that is located in visible
                                                   // distance of a predator
      for( int i = x-iVis; i <= x+iVis; i++ )
        for( int j = y - iVis; j <= y + iVis ; j++ )
          iArrayVisual
                 [(i+PS.getRows())   %PS.getRows()]
                 [(j+PS.getColumns())%PS.getColumns()]++;
    }

    for( int i = 0 ; i < PS.getRows(); i ++ )      // finally display cells
      for( int j = 0; j < PS.getColumns() ; j++ )  // which are visible, when
      {                                            // cell is seen by multiple
        if( iArrayVisual[i][j] > 0 )               // predators, make it darker
        {
          glColor3f( 0.90 - (iArrayVisual[i][j]-1)*0.07,   // set color 
	             0.96 - (iArrayVisual[i][j]-1)*0.07, 
		     1                                  );
          glRecti( i, j, i+1, j+1 );               // and color cell
        }
      }      
  } 

#if DEBUG
  gettimeofday( &tv_new, NULL );
  double time_diff = ((double)tv_new.tv_sec + (double)tv_new.tv_usec/1000000)-
                      ((double)tv_old.tv_sec + (double)tv_old.tv_usec/1000000);
  printf( "visible range time_diff: %f\n", time_diff * 1000 );
  tv_old = tv_new;
#endif
  
  // iterate over all predators in the world and display them
  for( int i = 0 ; i < WORLD.m_iNrPredators ; i ++  )
  {
    x         = WORLD.m_Predators[i].m_iX;
    y         = WORLD.m_Predators[i].m_iY;
    nr        = WORLD.m_Predators[i].m_iNr;
    bCollided = WORLD.m_Predators[i].m_bCollided;
  
    if( bCollided )                                 // if agent collided
      rgb.setColor( PS.getCollisionColor() );       //   set collision color
    else                                            // else
      rgb.setColor( PS.getPredatorColor() );        //   set predator color
      
    glColor3f(rgb.getRed(), rgb.getGreen(), rgb.getBlue() );
                                                    // draw the predator
    glBegin(GL_POLYGON);                            // make a circle 
    int iSlices = 128;                              // which is a polygon
    for (int i = 0; i < iSlices; i++)               // with a lot of points
    {
      // lines are on whole digits, so center circle is 0.5 further 
      // 0.4 is to make sure circle does not touch the grid edges 
      glVertex2f( x + 0.5 + 0.4 * cos(  (float) i * 2*M_PI / (float) iSlices ),
                  y + 0.5 + 0.4 * sin(  (float) i * 2*M_PI / (float) iSlices ));
    }
    glEnd();

    if( PS.getShowNumber() == true )                // if number should be shown
    {
      glColor3f(0,0,0);                             // set color to black
      glRasterPos2f( x + 0.5, y + 0.5);             // go to middle of cell
      sprintf( strNr, "%d", nr );                   // make the number
      for( size_t i = 0 ; i < strlen( strNr ) ; i ++ ) // write it
        glutBitmapCharacter( GLUT_BITMAP_HELVETICA_10, strNr[i] );      
    }
  }

#if DEBUG
  gettimeofday( &tv_new, NULL );
  time_diff = ((double)tv_new.tv_sec + (double)tv_new.tv_usec/1000000)-
                      ((double)tv_old.tv_sec + (double)tv_old.tv_usec/1000000);
  printf( "predators time_diff: %f\n", time_diff * 1000 );
  tv_old = tv_new;
#endif

  // now display all the preys
  for( int i = 0 ; i < WORLD.m_iNrPreys ; i ++  )
  {
    x         = WORLD.m_Preys[i].m_iX;
    y         = WORLD.m_Preys[i].m_iY;
    nr        = WORLD.m_Preys[i].m_iNr;
    bCollided = WORLD.m_Preys[i].m_bCollided;
    bCaught   = WORLD.m_Preys[i].m_bCaught;
  
    if( bCaught )                                   // if prey caught
      continue;                                     //   don't display it
    else if( bCollided )                            // else if agent collided
      rgb.setColor( PS.getCollisionColor() );       //   set collision color
    else 					    // else
      rgb.setColor( PS.getPreyColor() );            //   set prey color 
      
    glColor3f(rgb.getRed(), rgb.getGreen(), rgb.getBlue() );

    glBegin( GL_POLYGON );                          // draw the prey:
     glVertex2f( x + 0.1, y + 0.1 );                // make a triangle
     glVertex2f( x + 0.9, y + 0.1 );                // with edge points just
     glVertex2f( x + 0.5, y + 0.9 );                // inside the cell
    glEnd();
      
    if( PS.getShowNumber() == true )                // if number should be shown
    {
      glColor3f(0,0,0);      			    // set color to black
      glRasterPos2f( x + 0.5, y + 0.5); 	    // go to middle of cell
      sprintf( strNr, "%d", nr );                   // make the number and write
      for( size_t i = 0 ; i < strlen( strNr ) ; i ++ )
        glutBitmapCharacter( GLUT_BITMAP_HELVETICA_10, strNr[i] );      
    }
  }
  
#if DEBUG
  gettimeofday( &tv_new, NULL );
  time_diff = ((double)tv_new.tv_sec + (double)tv_new.tv_usec/1000000)-
                      ((double)tv_old.tv_sec + (double)tv_old.tv_usec/1000000);
  printf( "prey display time_diff: %f\n", time_diff * 1000 );
  tv_old = tv_new;
#endif

  // draw the grid
  rgb.setColor( PS.getGridColor() );                // set grid color
  glColor3f(rgb.getRed(), rgb.getGreen(),rgb.getBlue());

  for( int i = 0 ; i <= PS.getColumns() ; i++ )     // draw all the columns
  {
    glBegin(GL_LINES);
      glVertex2d( i, 0     );
      glVertex2d( i, PS.getRows() );
    glEnd();
  }

  for( int i = 0; i <= PS.getRows() ; i++ )         // draw all the rows
  {
    glBegin(GL_LINES);
      glVertex2d( 0,                i );
      glVertex2d( PS.getColumns() , i );    
    glEnd();
  }  

#if DEBUG
  gettimeofday( &tv_new, NULL );
  time_diff = ((double)tv_new.tv_sec + (double)tv_new.tv_usec/1000000)-
                      ((double)tv_old.tv_sec + (double)tv_old.tv_usec/1000000);
  printf( "game display time_diff: %f\n", time_diff * 1000 );
  tv_old = tv_new;
#endif

  glutSwapBuffers();                  // swap buffer (use double buffering)

#if DEBUG
  gettimeofday( &tv_new, NULL );
  time_diff = ((double)tv_new.tv_sec + (double)tv_new.tv_usec/1000000)-
                      ((double)tv_old.tv_sec + (double)tv_old.tv_usec/1000000);
  printf( "swap buffers time_diff: %f\n", time_diff * 1000 );
  tv_old = tv_new;
  cerr << "end display" << endl;
#endif   
}


/**
 *  This function is called when a request arrives to reshape the main
 *  window. Since the height and width of each cell should be equal,
 *  the new shape of the window cannot take any arbitrary
 *  form. Therefore we set the new width depending on the new height
 *  by making sure that the width and height of each cell stays the
 *  same.
 */
void reshape( int width, int height )         
{
  width  = (int)( (height * (double)PS.getColumns()) / (double)PS.getRows() );  
  glViewport(0,0,width,height);                      // change the viewport

  glutSetWindow( WIN2 ); // set height of stats window equal to one row
  glutReshapeWindow( width, height/(PS.getRows()+1) );
  glutPositionWindow( 0, height - height/PS.getRows() + 5 ); // small border
  
  glutSetWindow( WIN );
  glutReshapeWindow( width, height );              // reshape the window 
}

/**
 *  This function is called when a request arrives to reshape the
 *  stats window. However, this is already done in reshape.
 */
void reshape2( int width, int height )         
{
  glViewport( 0, 0, width, height );
}


/** This function is called 20 times a second and checks whether the
 *  world should be redisplayed depending on the m_bRedisplay variable
 *  in the WORLD variable.  When this is the case the display method
 *  is called.
 */
void idle( int i )
{
#if DEBUG
  static struct timeval tv_old;
  struct timeval tv_new;
  gettimeofday( &tv_new, NULL );
  double time_diff = ((double)tv_new.tv_sec + (double)tv_new.tv_usec/1000000)-
                      ((double)tv_old.tv_sec + (double)tv_old.tv_usec/1000000);
  printf( "time_diff: %f\n", time_diff * 1000 );
#endif

  if( WORLD.m_bRedisplay == true )
  {
    glutSetWindow( WIN );
    glutPostRedisplay();
    glutSetWindow( WIN2 );
    glutPostRedisplay();
    WORLD.m_bRedisplay = false;
  }    
  glutTimerFunc( (unsigned int ) 50, idle, 0 );    // call method again in 50 ms

#if DEBUG
  tv_old = tv_new;
#endif
}

/**
 *  This function processes a selection in the mouse menu. The
 *  specified integer denotes which menu item was selected. The
 *  associated message is send to the server to be processed there. 
 */
void menu( int i)
{
  switch( i )
  {
    case 1: CONNECTION.sendMessage( "(monitor continue)" ); break;
    case 2: CONNECTION.sendMessage( "(monitor pause)" );    break;
    case 3: CONNECTION.sendMessage( "(monitor quit)" );    break;
    case 4: CONNECTION.sendMessage( "(monitor step)" );    break;
    default: 
            break;
  }
}

/**
 *  This method processes a press of a key on the keyboard. The
 *  specified unsigned char denotes which character was pressed and is
 *  handled accordingly.
 */
void callBackKeyBoard( unsigned char ch, int x, int y )
{
  switch( ch )
  {
    case 'c': menu( 1 ); break;
    case 'p': menu( 2 ); break;
    case 'h': 
    case 'q': menu( 3 ); break;
    case 's': menu( 4 ); break;
    case '+': CONNECTION.sendMessage( "(monitor speed_up)" ); break;
    case '-': CONNECTION.sendMessage( "(monitor speed_down)" ); break;
    default: 
            break;
  }
}

/**
 *  This function is called by a separate thread and handles all
 *  message from the server. These messages are among other the state
 *  information of the world, new server parameters or an indication
 *  that the monitor should be shut down. 
 */ 
void* receiveMessages( void* )      
{
  char strBuf[MAX_MSG];
  bool bCont = true;
  while( bCont )
  {
    int ret = CONNECTION.receiveMessage( strBuf, MAX_MSG );  // receive msg
    if( ret > 0 )                                            // ok
      bCont = processIncomingMessge( strBuf );               //  process it
    else
    {
      perror( "Cannot receive message: " );
      bCont = false;                                         // else quit
    }
  }
  exit(0);
  return NULL;
}


/**
 * This function processes an incoming message from the server and
 * takes the appropriate action (in most cases this is filling the
 * information in MonitorWorld).
 */
bool processIncomingMessge( char *str )
{
// cerr << "process: " << str ;

  if( strncmp( str, "(init ok)", 9 ) == 0 )               // init ok rec.
    return true;                                          // do nothing
  else if( strncmp( str, "(quit)", 6 ) == 0 )             // quit received
  {
    cerr << "Server wants me to exit!" << endl;
    return false;                                         // exit
  }
  else if( strncmp( str, "(server_param", 13 ) == 0)      // server parameters
  {
    cerr << "Reading server parameters" << endl;          // read them
    readServerParam( "visible_distance", str );
    readServerParam( "rows", str );
    readServerParam( "columns", str );
    bSTART = true;                                        // ready to start now
    return true;
  }
  else if( strncmp( str, "(world", 6 ) != 0 )
  {
    cerr << "Unknown message received: " << str << endl;  
    return true;
  }

  // now read world information.
  WORLD.m_iNrPredators = 0;                           // reset the information
  WORLD.m_iNrPreys     = 0;

  int ret = sscanf( str, "(world (stats %d %d %lf %d %d)",  // read stats
                              &WORLD.m_iNrEpisode, 
                              &WORLD.m_iNrCycle, 
                              &WORLD.m_dAverageCaptureTime, 
                              &WORLD.m_iNrPenalties,    
                              &WORLD.m_iNrCollisions );

  if( ret !=  5 )
  {
    cerr << "Error in reading world statistics: " << str << endl;
    return true;
  }

  Agent * agent = NULL;                      // some needed variables
  int  x, y, nr;
  int  i = 0;
  int  j = 0;
  char c;
  
  while( str[i] != '\0' && j == 0 )           // go to the end of the statistics
  {
    if( str[i] == ')' )                       // this is after first closing ')'
      j++;
    i++;
  }
  while( str[i] != '\0' && str[i] != ')' )    // while new agent information
  {
    // first try to read predator information
    ret = sscanf( &str[i], " ((predator %d%c) %d %d)", &nr, &c, &x, &y );
    if( ret == 4 )
    {
      if( WORLD.m_iNrPredators >= MAX_AGENTS )
      {
        cerr << "predator index too high: " << WORLD.m_iNrPredators << endl;
        return true;
      }       
      agent = &WORLD.m_Predators[WORLD.m_iNrPredators++];    
    }
    else                                    // try to read prey information.
    {
      ret = sscanf( &str[i], " ((prey %d%c) %d %d)", &nr, &c, &x, &y );    
      if( ret == 4 )
      {
        if( WORLD.m_iNrPreys >= MAX_AGENTS )
        {
          cerr << "prey index too high: " << WORLD.m_iNrPreys << endl;
          return true;
        }       
        agent = &WORLD.m_Preys[WORLD.m_iNrPreys++];
      }
      else
      {
        fprintf( stderr, "corrupted message: %s", &str[i] );
        return true;
      }
    }

    // now advance to second closing bracket after agent information.
    // (world (stats 1 12 31.3 3 3) ((predator 1 ) 4 4))
    j = 0;
    while( str[i] != '\0' && j < 2 ) // didn't found closing bracket yet
    {
      if( str[i] == ')' ) 
        j++;
      i++;
    }

    agent->m_iX = x;                    // fill agent info with parsed values
    agent->m_iY = y;
    agent->m_iNr = nr;
    agent->m_bCollided = (c == 'c' );
    agent->m_bCaught   = (c == 'x' );  
  }

  WORLD.m_bRedisplay = true;  
  return true;
}

/**
 *  This function reads the server parameters from the server_param
 *  message and stores them in the Parameters instance.
 */
bool readServerParam( char *strParam, char *strMsg )
{
  char strFormat[128];
  char strValue[128] = "";
  sprintf( strValue, "none" );

  sprintf( strFormat, "%s ", strParam );        // add space after parameters
  char *str = strstr( strMsg, strFormat );      // and find param definition
  sprintf( strFormat, "%s %%[^)]", strParam );  // read till closing bracket

  if( str == NULL )
  {
    cerr << "(WorldMonitor::readServerParam) error finding " << strParam <<endl;
    return false;
  }
  int ret = sscanf( str, strFormat, strValue ); // read in values

  if( ret == 1 )
    PS.setValue( strParam, strValue );          // succeeded -> change value
  else
    cerr << "(Monitor::readServerParam) error reading " << strParam <<endl;

  return (ret == 1 ) ? true : false ;
}

/**
 * This function reads and processes all the parameters supplied at
 * the command prompt.
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
          if( PS.readValues( argv[i+1], ":" ) == false )
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
 *  This function prints the possible command options to the screen.
 */
void showOptions( )
{
  cout << " -h     :    print this help message"         << endl 
       << " -c file:    reads in parameters from 'file'" << endl  ;
}


/**
 *  This is the main call of the glut program and never exits. It
 *  creates and initializes the window and the mouse menu. At the end
 *  'glutMainLoop' is called, which processes and handles all callback
 *  functions. 
 */
int main( int argc, char** argv )
{
  //  char *id="$Id: Monitor.C,v 1.1 2005/01/06 12:34:58 jellekok Exp $";
  //  cerr << id << endl;
  cerr << "Starting monitor" << endl;

  readOptions( argc, argv );                        // read command line options  

  CONNECTION.connect( PS.getHost(), PS.getPort() ); // create connection
  CONNECTION.sendMessage( "(init monitor)" );       // initialize monitor
    
  pthread_t thrListen;                              // start thread
  pthread_create( &thrListen,  NULL, receiveMessages,  NULL );  

  int iAttempts = 0;                                // don't start visualization
  while( bSTART == false && iAttempts < 50 )        // before server_param have
  {                                                 // arrived, if they aren't
    usleep( 100 * 1000 );                           // received after 5 seconds
    iAttempts++;                   
  }
  if( iAttempts == 50 )                             // assume server not alive
  {
    cerr << "Could not contact server (port and host ok?)" << endl;
    return -1;
  }
    
  glutInit(&argc, argv);                          // initialize GLUT
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);    // use double buffering
  glutInitWindowSize(PS.getWindowWidth(), PS.getWindowHeight() );                  

  WIN = glutCreateWindow("Pursuit domain");       // create the window
  
  glMatrixMode(GL_PROJECTION);                    // use projection
  glLoadIdentity();
  glOrtho((GLdouble)0, (GLdouble)PS.getColumns(), // set the "virtual" size
          (GLdouble)-1, (GLdouble)PS.getRows(),   // independent of pixel size
          -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  
  RGBcolor rgbBG( PS.getBackgroundColor() );      // set background color
  glClearColor(rgbBG.getRed(), rgbBG.getGreen(), rgbBG.getBlue(), 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
                
  glutTimerFunc( (unsigned int )50, idle, 0 );    // call idle after 500 ms
  glutDisplayFunc(display);                       // redefine redisplay
  glutReshapeFunc(reshape);                       // redefine reshaping window

  glutCreateMenu( menu );                         // create a menu
  glutAddMenuEntry( "Start | Continue ",  1 );    // with four items
  glutAddMenuEntry( "Pause  ",            2 );  
  glutAddMenuEntry( "Step-by-step",       4 );    
  glutAddMenuEntry( "Quit   ",            3 );
  glutAttachMenu( GLUT_LEFT_BUTTON );             // open menu after pressing
  glutAttachMenu( GLUT_RIGHT_BUTTON );            // left or right mouse button
  glutKeyboardFunc( callBackKeyBoard );           // call method after key-press
    
  WIN2 = glutCreateSubWindow( WIN, 0, 
         PS.getWindowHeight() - PS.getWindowHeight()/(PS.getRows()+1) + 5 , 
         PS.getWindowWidth(), PS.getWindowHeight()/(PS.getRows()+1) );
//  WIN2 = glutCreateSubWindow( WIN, 0, (int)PS.getWindowHeight()*( 1 - 1.0 / (double)PS.getRows() ), 
//                              PS.getWindowWidth(), PS.getWindowHeight()/PS.getRows() ) ;
  glMatrixMode(GL_PROJECTION);                    // use projection
  glLoadIdentity();
  glOrtho((GLdouble)0, (GLdouble)PS.getColumns(), // set the "virtual" size
          (GLdouble)0, 1 ,   // independent of pixel size
          -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glClearColor(0.98, 1.0, 0.92 , 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glutDisplayFunc( display2 );
  glutReshapeFunc( reshape2 );  
  
  glutKeyboardFunc( callBackKeyBoard );           // call method after key-press

  glutMainLoop();                                 // start GLUT
}
