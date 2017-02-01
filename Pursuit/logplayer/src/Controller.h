/**
 * @file   Controller.h
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Mon Aug 11 11:18:47 2003
 * 
 * @brief This file contains the definitions for the Controller class
 * which contains the widgets and their associated callback functions.
 */
#ifndef _CONTROLLER_
#define _CONTROLLER_

#include <gtk/gtk.h>
#include "types.h"
#include "LogPlayer.h"

/**
 * This class contains the controller which contains the widgets to
 * control the functionality of the log player. win = main = buttons +
 * jump buttons = toggleRevPlay, buttonRevStep, buttonStop,
 * buttonStep, togglePlay jump = labelEpisode, inputEpisode,
 * labelCycle, inputCycle, buttonGo
*/
class Controller {
  LogPlayer* m_player;         /**< reference to the logplayer              */

  GtkWidget* win;              /**< reference to controller window          */
  GtkWidget* main;             /**< reference to main controller            */
  GtkWidget* buttons;          /**< controller part related to play buttons */
  GtkWidget* jump;             /**< controller part related to jump buttons */
  
  GtkWidget* toggleRevPlay;    /**< reverse play button                     */
  GtkWidget* buttonRevStep;    /**< reverse stepwise play button            */
  GtkWidget* buttonStop;       /**< stop button                             */
  GtkWidget* buttonStep;       /**< stepwise button                         */
  GtkWidget* togglePlay;       /**< play button                             */
  GtkWidget* buttonGo;         /**< jump button                             */

  GtkWidget* boxRevPlay;       /**< label reverse play button               */
  GtkWidget* boxRevStep;       /**< label reverse stepwise play button      */
  GtkWidget* boxStop;          /**< label stop button                       */
  GtkWidget* boxStep;          /**< label stepwise button                   */
  GtkWidget* boxPlay;          /**< label play button                       */

  GtkWidget* labelEpisode;     /**< label episode description               */
  GtkWidget* inputEpisode;     /**< label input episode                     */
  GtkWidget* labelCycle;       /**< label cycle number                      */
  GtkWidget* inputCycle;       /**< label input cycle number                */
	
  char       strXpmDir[128];   /**< dir of xpm pictures (same as program)   */

public:
  Controller( LogPlayer *player, char *strCommand );

  // initialization and starting methods  
  void       init      ( int argc, char *argv[] );
  void       makeWindow( void                   );
  void       mainLoop  ( void                   );
  
  // get and set methods
  void       setStatus ( StatusT                );
  void       setButtons( StatusT                );

  // all callback functions
  static void com_rev_play( GtkWidget* w, gpointer data );
  static void com_rev_step( GtkWidget* w, gpointer data );
  static void com_stop    ( GtkWidget* w, gpointer data );
  static void com_step    ( GtkWidget* w, gpointer data );
  static void com_play    ( GtkWidget* w, gpointer data );
  static void com_jump    ( GtkWidget* w, gpointer data );
  static void com_destroy ( GtkWidget* w, gpointer data );

  static void * loop      ( void     * v                );
  
  // misc. methods
  void        jumpToFrame ( int iEpisode, int iCycle    );       
} ;

#endif
