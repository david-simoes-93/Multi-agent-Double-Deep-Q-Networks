/**
 * @file   Controller.C
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Mon Aug 11 10:44:52 2003
 * 
 * @brief This file contains the definitions of the controller widget
 * that holds the buttons to control the logplayer.
 */

#include <signal.h>
#include <string.h>
#include "Controller.h"
#include "play.xpm"       // different xpm pictures
#include "rev.xpm"
#include "minus.xpm"
#include "stop.xpm"
#include "plus.xpm"


/** 
 * This function creates a widget with a pixmap inside it. The pixmap is
 * an xpm picture that is stored in the file 'xpm_filename'.
 * @param parent parent widget in which labeled box is placed
 * @param xpm_filename name of the xpm picture that will be displayed 
 * @return GtkWidget labeled box widget
 */
GtkWidget *xpm_label_box( GtkWidget *parent,
                          gchar     *xpm_filename )
{
  GdkPixmap *pixmap;
  GdkBitmap *mask;
  GtkStyle  *style;
  
  style = gtk_widget_get_style(parent);  // get background from parent's style
  // create pixmap from file
  pixmap = gdk_pixmap_create_from_xpm (parent->window, &mask,
                                       &style->bg[GTK_STATE_NORMAL], 
                                       xpm_filename);   
  // returned the widget with this pixmap
  return gtk_pixmap_new (pixmap, mask);
}

/**
 * This function creates a widget with a pixmap inside it. The pixmap
 * is an xpm picture that is stored in the data 'xpm_data'. 
 * @param parent parent widget in which labeled box is placed
 * @param xpm_data data of the xpm picture that will be displayed 
 * @return GtkWidget labeled box widget
*/

GtkWidget *xpm_label_box_d( GtkWidget *parent,
                            gchar     **xpm_data )
{
  GdkPixmap *pixmap;
  GdkBitmap *mask;
  GtkStyle  *style;

  style = gtk_widget_get_style(parent);  // get background from parent's style
  // create the pixmap from the data 
  pixmap = gdk_pixmap_create_from_xpm_d (parent->window, &mask,
            &style->bg[GTK_STATE_NORMAL], xpm_data);
  // return the widget with this pixmap
  return gtk_pixmap_new (pixmap, mask);
}

/**
 * This is the constructor of the Controller class and initializes the
 * member variable that holds an instance of the logplayer. This
 * variable is used to pass through events that are triggered by the
 * callback functions from the controller. The strCommand is the
 * command that is used to start the program.  It is currently not
 * used. It was used to check from which directory the program was
 * started and to checked from which directory the xpm pictures should
 * be extracted. 
 * @param player reference to instantiated logplayer class
 * @param strCommand string representing how progrm was started (argv[0])
 */
Controller::Controller( LogPlayer *player, char *strCommand )
{
  char *tmp = strrchr( strCommand, '/' );

  if( tmp == NULL )        // add current directory to command when no dir. 
    strcpy( strXpmDir, "./" );
  else
  {
    tmp++;
    *tmp = '\0';           // create reference to xpm picture dir. 
    strcpy( strXpmDir, strCommand );
  }
  m_player = player;       // create referenec to logplayer instantiation
}

/**
 * This method initializes the controller using the command line
 * options. 
 * @param argc number of command options
 * @param argv string values of command options 
 */
void Controller::init( int argc, char *argv[] )
{
  gtk_init(&argc, &argv);          // pass info to gtk_init
  gtk_rc_parse( "logplayer.rc" );
}

/**
 *  This method creates the windows. It initializes all buttons with
 *  assoicated pictures and places them on the window. The callback
 *  functions are also initialized in this function. 
 * */
void Controller::makeWindow()
{
  
  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);            // create the window
  gtk_widget_realize(win);

  toggleRevPlay = gtk_toggle_button_new();              // create all buttons
  buttonRevStep = gtk_button_new();  
  buttonStop    = gtk_button_new();      
  buttonStep    = gtk_button_new();  
  togglePlay    = gtk_toggle_button_new(); 
  buttonGo      = gtk_button_new_with_label( "go" );  

#if 0 // get pictures from files from dir. in which program was started
  boxRevPlay = xpm_label_box(win,strcat(strcpy(strTmp,strXpmDir),"rev.xpm"  ));
  boxRevStep = xpm_label_box(win,strcat(strcpy(strTmp,strXpmDir),"minus.xpm"));
  boxStop    = xpm_label_box(win,strcat(strcpy(strTmp,strXpmDir),"stop.xpm" ));
  boxStep    = xpm_label_box(win,strcat(strcpy(strTmp,strXpmDir),"plus.xpm" ));
  boxPlay    = xpm_label_box(win,strcat(strcpy(strTmp,strXpmDir),"play.xpm" ));
#else // get pictures from include files
  boxRevPlay = xpm_label_box_d(win, rev_xpm   );   
  boxRevStep = xpm_label_box_d(win, minus_xpm ); 
  boxStop    = xpm_label_box_d(win, stop_xpm  ); 
  boxStep    = xpm_label_box_d(win, plus_xpm  );
  boxPlay    = xpm_label_box_d(win, play_xpm  );
#endif           
  main = gtk_vbox_new( 1, 0 );                      // create main box  
  buttons = gtk_hbox_new (1, 0);                    // create hor. buttonsbox
                                                   // and add all buttons
                                    
  // reverse play
  gtk_container_add(GTK_CONTAINER(toggleRevPlay), boxRevPlay);
  gtk_box_pack_start (GTK_BOX (buttons), toggleRevPlay, 0, 1, 0);
  gtk_widget_show( toggleRevPlay );

  // reverse stepwise play
  gtk_container_add(GTK_CONTAINER(buttonRevStep), boxRevStep);
  gtk_box_pack_start (GTK_BOX (buttons), buttonRevStep, 0, 1, 0);
  gtk_widget_show( buttonRevStep );

  // stop 
  gtk_container_add(GTK_CONTAINER(buttonStop), boxStop);
  gtk_box_pack_start (GTK_BOX (buttons), buttonStop, 0, 1, 0);
  gtk_widget_show( buttonStop );
  
  // forward stepwise play
  gtk_container_add(GTK_CONTAINER(buttonStep), boxStep);
  gtk_box_pack_start (GTK_BOX (buttons), buttonStep, 0, 1, 0);
  gtk_widget_show( buttonStep );  

  // forward play
  gtk_container_add(GTK_CONTAINER(togglePlay), boxPlay);
  gtk_box_pack_start (GTK_BOX (buttons), togglePlay, 0, 1, 0);
  gtk_widget_show( togglePlay );

  // jump box (jump to cycle)
  jump = gtk_hbox_new (0, 1);                           // create the jumpbox

  // label to which episode, cycle number to jump
  labelEpisode = gtk_label_new( " episode:" );  
  gtk_box_pack_start (GTK_BOX (jump), labelEpisode, 0, 1, 0);
  gtk_widget_show( labelEpisode );
  
  inputEpisode = gtk_entry_new_with_max_length( 6 );
  gtk_widget_set_usize( inputEpisode, 50, 20 );
  gtk_box_pack_start (GTK_BOX (jump), inputEpisode, 0, 1, 0);
  gtk_widget_show( inputEpisode );
    
  labelCycle = gtk_label_new( " cycle:" );  
  gtk_box_pack_start (GTK_BOX (jump), labelCycle, 0, 1, 0);
  gtk_widget_show( labelCycle );

  inputCycle = gtk_entry_new_with_max_length( 6 );
  gtk_widget_set_usize( inputCycle, 50, 20 );
  gtk_box_pack_start (GTK_BOX (jump), inputCycle, 0, 1, 0);
  gtk_widget_show( inputCycle );

  gtk_box_pack_start (GTK_BOX (jump), buttonGo, 0, 1, 0);
  gtk_widget_show( buttonGo );
      
  gtk_box_pack_start (GTK_BOX (main), buttons, 0, 1, 0);      // add both to
  gtk_box_pack_start (GTK_BOX (main), jump, 0, 1, 0 );        // main frame
  gtk_container_add(GTK_CONTAINER(win), main);    

  gtk_window_set_title(GTK_WINDOW(win), "logplayer");

  gtk_signal_connect(GTK_OBJECT(toggleRevPlay),           // add all callback
                     "clicked",
                     GTK_SIGNAL_FUNC(com_rev_play),
                     this);

  gtk_signal_connect(GTK_OBJECT(buttonRevStep),
                     "clicked",
                     GTK_SIGNAL_FUNC(com_rev_step),
                     this);

  gtk_signal_connect(GTK_OBJECT(buttonStop),
                     "clicked",
                     GTK_SIGNAL_FUNC(com_stop),
                     this);

  gtk_signal_connect(GTK_OBJECT(buttonStep),
                     "clicked",
                     GTK_SIGNAL_FUNC(com_step),
                     this);

  gtk_signal_connect(GTK_OBJECT(togglePlay),
                     "clicked",
                     GTK_SIGNAL_FUNC(com_play),
                     this);

  gtk_signal_connect(GTK_OBJECT(buttonGo),
                     "clicked",
                     GTK_SIGNAL_FUNC(com_jump),
                     this);
 
  gtk_signal_connect(GTK_OBJECT (win), 
                     "delete_event",
                     GTK_SIGNAL_FUNC (com_destroy), 
                     this);
 
  gtk_signal_connect(GTK_OBJECT (win), 
                     "destroy",
                     GTK_SIGNAL_FUNC (com_destroy), 
                     this);

  gtk_widget_show_all(win);
}

/**
 * This method starts the mainloop of the controller. 
 */
void Controller::mainLoop( )
{
  gtk_main();
}

/**
 * This method sets the buttons. Basically it only changes the toggle
 * buttons that are used for playing forward or playing
 * backward. Based on the current state and the state of the buttons,
 * they are toggled or not.
 * @param status status of the logplayer
 */
void Controller::setButtons( StatusT status )
{
  GtkToggleButton *makeInactive1 = NULL, *makeInactive2 = NULL;

  // in play, reverse play should not be active and vice versa
  // in all other cases they both should be inactive
  switch( status )
  {
    case STATUS_PLAY:     
      makeInactive1 = GTK_TOGGLE_BUTTON(toggleRevPlay);
      break;
    case STATUS_REV_PLAY: 
      makeInactive1 = GTK_TOGGLE_BUTTON(togglePlay);    
      break;
    case STATUS_ILLEGAL : 
        break;
    default:                     
      makeInactive1 = GTK_TOGGLE_BUTTON(toggleRevPlay);
      makeInactive2 = GTK_TOGGLE_BUTTON(togglePlay);
      break;
  }
  
  // if either rev_play or play is active but should be inactive, change it
  if( makeInactive1 != NULL && 
      gtk_toggle_button_get_active(makeInactive1)==TRUE)
    gtk_toggle_button_set_active( makeInactive1, FALSE );
  if( makeInactive2 != NULL && 
      gtk_toggle_button_get_active(makeInactive2)==TRUE)
    gtk_toggle_button_set_active( makeInactive2, FALSE );
}

/**
 *  This method is the callback for reverse play. If the toggle button
 *  is activated it sets the status to STATUS_REV_PLAY, if it is not
 *  active, it sets it to STATUS_STOP. 
 */
void Controller::com_rev_play(GtkWidget* w, gpointer data)
{
  StatusT status = STATUS_ILLEGAL;
  if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( w ) ) == TRUE )
    status = STATUS_REV_PLAY;

  // exception when changed to play, since then we don't want to go to stop.
  else if( ((Controller*)data)->m_player->getStatus() != STATUS_PLAY )
    status = STATUS_STOP;

  // set the status
  if( status != STATUS_ILLEGAL )
    ((Controller*)data)->setStatus( status );  
  return; 
}

/** 
 * This method sets the new status of the logplayer.
 * 
 * @param status new status of the logplayer
 */
void Controller::setStatus( StatusT status )
{
  m_player->setStatus( status );
  setButtons( status );
}

/**
 * This method is the callback for stepwize play. It sets the state to
 * STATUS_REV_STEP.  
 * @param w pointer to button widget
 * @param data reference to controller widget
 */
void Controller::com_rev_step( GtkWidget* w, gpointer data)
{
  ((Controller*)data)->setStatus( STATUS_REV_STEP);   
}

/**
 * This method is the callback for stepwize play. It sets the state
 * to STATUS_STOP.  
 * @param w pointer to button widget
 * @param data reference to controller widget
 */
void Controller::com_stop( GtkWidget* w, gpointer data)
{
  ((Controller*)data)->setStatus( STATUS_STOP );  
}

/** This method is the callback for stepwize play. It sets the state
 *  to STATUS_STEP.
 * @param w pointer to button widget
 * @param data reference to controller widget
 */
void Controller::com_step    ( GtkWidget* w, gpointer data)
{
  ((Controller*)data)->setStatus( STATUS_STEP );  
}

/**
 * This method is the callback for normal play. If the toggle button
 * is activated it sets the status to STATUS_PLAY, if it is not
 * active, it sets it to STATUS_STOP.
 * @param w pointer to button widget
 * @param data reference to controller widget
 */
void Controller::com_play( GtkWidget* w, gpointer data)
{
  StatusT status = STATUS_ILLEGAL;
  if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( w ) ) == TRUE )
    status = STATUS_PLAY;
  // exception when changed to revplay, since then we don't want to go to stop.
  else if( ((Controller*)data)->m_player->getStatus() != STATUS_REV_PLAY )
    status = STATUS_STOP;

  if( status != STATUS_ILLEGAL )
    ((Controller*)data)->setStatus( status );  
  return; 
}

/**
 * This method is the callback for the go button. It reads in the
 * values of the two input boxes and then calls the method from the
 * Logplayer instance to jump to the specified cycle in that episode
 * number.
 * @param w pointer to button widget
 * @param data reference to controller widget
 */
void Controller::com_jump( GtkWidget* w, gpointer data)
{
  const gchar        *str;
  int          iEpisode, iCycle;
  Controller   *controller = (Controller*)data;
  
  // get input from episode box
  void *hack1 = (void *) (controller->inputEpisode);
  str = gtk_entry_get_text(  GTK_ENTRY( (controller->inputEpisode) ) );
  //str = gtk_entry_get_text(  GTK_ENTRY( hack1 ) );
  int ret = sscanf( str, "%d", &iEpisode );

  // get input from cycle box  
  str = gtk_entry_get_text( GTK_ENTRY( controller->inputCycle ) );
  ret += sscanf( str, "%d", &iCycle );
  
  // error when it is not possible to get the 2 digits, else jump to frame
  if( ret != 2 )
    cerr << "Error: please enter two integer values in text boxes" << endl;
  else
    controller->jumpToFrame( iEpisode, iCycle );
}

/**
 * This method destroys the current window and terminates the
 * program. 
 * @param w pointer to button widget
 * @param data reference to controller widget
 */
void Controller::com_destroy( GtkWidget *widget, gpointer   data )
{
  gtk_main_quit();
  raise( SIGINT );  // ugly: but signal is caught
}

/** 
 *  This method starts the mainloop of the controller pointed to by
 *  the void reference v.
 * 
 * @param v reference to controller window
 * 
 */
void * Controller::loop( void *v ) 
{  
  ((Controller*)v)->mainLoop(); return NULL; 
}
  
/** 
 * This method jumps to the specified episode, cycle number (if it
 * exists). This is done by calling the same function from the
 * instantiated logplayer class.
 * 
 * @param iEpisode desired episode number
 * @param iCycle desired cycle number
 */
void Controller::jumpToFrame( int iEpisode, int iCycle ) 
{ 
  m_player->jumpToFrame( iEpisode,iCycle );  
}
