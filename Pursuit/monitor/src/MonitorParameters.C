/**
 * @file   MonitorParameters.C
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Thu Aug  7 17:53:12 2003
 * 
 * @brief  This class contains the methods related to the monitor parameters.
 */

#include "MonitorParameters.h"
#include <string.h>

/**
 * This constructors sets all the defaults values of the parameters
 * and associates text strings with all variables. 
 */
MonitorParameters::MonitorParameters( ): GenericValues("MonitorParameters", 15)
{
  m_iColumns               = 20;
  m_iRows                  = 20;
  m_iWindowWidth           = 400;
  m_iWindowHeight          = 400;      
  strcpy( m_strHost, "localhost" );
  m_iPort                  = 4001;
  m_iVisibleDistance       = 2;
  m_bShowNumber            = false;
  m_bShowVisibleRange      = true;  
  m_bDisplayStatistics     = true;
  strcpy( m_strBackgroundColor, "ffffff" ); // white
  strcpy( m_strGridColor,       "000000" ); // black
  strcpy( m_strPreyColor,       "ff6633" ); // lightred
  strcpy( m_strPredatorColor,   "33ccff" ); // lightblue
  strcpy( m_strCollisionColor,  "66cccc" ); // 

  // add all the settings and link text string to variable
  addSetting( "columns",         &m_iColumns,          GENERIC_VALUE_INTEGER );
  addSetting( "rows",            &m_iRows,             GENERIC_VALUE_INTEGER );
  addSetting( "window_width",    &m_iWindowWidth,      GENERIC_VALUE_INTEGER );
  addSetting( "window_height",   &m_iWindowHeight,     GENERIC_VALUE_INTEGER );
  addSetting( "port",            &m_iPort,             GENERIC_VALUE_INTEGER );  
  addSetting( "host",            &m_strHost,           GENERIC_VALUE_STRING  );    
  addSetting( "visible_distance",&m_iVisibleDistance,  GENERIC_VALUE_INTEGER );
  addSetting( "show_number",     &m_bShowNumber,       GENERIC_VALUE_BOOLEAN );  
  addSetting( "show_visible_range",&m_bShowVisibleRange,GENERIC_VALUE_BOOLEAN );    
  addSetting( "display_statistics",&m_bDisplayStatistics,GENERIC_VALUE_BOOLEAN );      
  addSetting( "background_color",&m_strBackgroundColor,GENERIC_VALUE_STRING  );
  addSetting( "grid_color",      &m_strGridColor,      GENERIC_VALUE_STRING  );
  addSetting( "prey_color",      &m_strPreyColor,      GENERIC_VALUE_STRING  );
  addSetting( "predator_color",  &m_strPredatorColor,  GENERIC_VALUE_STRING  );
  addSetting( "collision_color", &m_strCollisionColor, GENERIC_VALUE_STRING  );
}

/**
 * Returns the number of columns of the grid. 
 *
 * @return number of columns
 */
int MonitorParameters::getColumns( ) const
{
  return m_iColumns;
}

/**
 * Sets the number of columns of the grid. 
 *
 * @param i set number of columns
 */
bool MonitorParameters::setColumns( int    i )
{
  m_iColumns = i;
  return true;
}

/**
 *  Returns the number of rows of the grid. 
 *
 *  @return number of rows
 */
int MonitorParameters::getRows( ) const
{
  return m_iRows;
}

/**
 *  Sets the number of rows of the grid. 
 *
 * @param i set number of rows
 */
bool MonitorParameters::setRows( int    i )
{
  m_iRows = i;
  return true;
}

/**
 * Return the width of the screen (in pixels). 
 *
 * @return width of window
 */
int MonitorParameters::getWindowWidth( )
{
  return m_iWindowWidth;
}

/**
 * Set the width of the screen (in pixels). 
 *
 * @param i set width of window
 */
bool MonitorParameters::setWindowWidth( int i )
{
  m_iWindowWidth = i;
  return true;
}

/**
 * Return the height of the screen (in pixels). 
 * 
 * @return height of window
 */
int MonitorParameters::getWindowHeight( )
{
  return m_iWindowWidth;
}

/**
 * Set the height of the screen (in pixels). 
 *
 * @param i set height of window
 */
bool MonitorParameters::setWindowHeight( int i )  
{
  m_iWindowHeight = i;
  return true;
}

/**
 * Return the port number of the server with which a client can connect. 
 *
 * @return port number
 */
int MonitorParameters::getPort( )
{
  return m_iPort;
}

/**
 * Set the host to which the client must connect. 
 *
 * @param str set host name
 */
bool MonitorParameters::setHost( char *str )  
{
  strcpy( m_strHost, str );
  return true;
}

/**
 * Return the host to which the client must connect. 
 *
 * @return host name
 */
char* MonitorParameters::getHost( )
{
  return m_strHost;
}

/**
 * Set the port number of the server with which a client can connect. 
 *
 * @param i set port number
 */
bool MonitorParameters::setPort( int i )  
{
  m_iPort = i;
  return true;
}


/**
 *  Return the distance in which objects are returned in the visual
 *  message.  When the relative distance in number of cells is smaller
 *  than this value, the object is added to the visual message,
 *  otherwise it is discarded.  A value of -1 indicates that all
 *  objects are listed. 
 *
 * @return visible distance
 */
int MonitorParameters::getVisibleDistance( )
{
  return m_iVisibleDistance;
}

/**
 * Set the distance in which objects are returned in the visual
 * message.  When the relative distance in number of cells is smaller
 * than this value, the object is added to the visual message,
 * otherwise it is discarded.  A value of -1 indicates that all
 * objects are listed. 
 *
  * @param i set visible distance
 */
bool MonitorParameters::setVisibleDistance( int i )  
{
  m_iVisibleDistance = i;
  return true;
}

/**
 * Returns whether the agents' number should be displayed. 
 *
 * @return bool whether numbers are shown
 */
bool MonitorParameters::getShowNumber( )
{
  return m_bShowNumber;
}

/**
 * Set the variable that indicates whether the agents' number should
 * be displayed. 
 *
 * @param b set whether numbers should be displayed
 */
bool MonitorParameters::setShowNumber( bool b )  
{
  m_bShowNumber = b;
  return true;
}

/**
 * Returns whether the agents' visible range should be displayed.
 *
 * @return visible range
 */
bool MonitorParameters::getShowVisibleRange( )
{
  return m_bShowVisibleRange;
}

/**
 * Set the variable that indicates whether the agents' visible range
 * should be displayed. 
 *
 * @param b set whether visible range is displayed
 */
bool MonitorParameters::setShowVisibleRange( bool b )  
{
  m_bShowVisibleRange = b;
  return true;
}

/**
 * Returns whether the statistics should be displayed. 
 *
 * @return bool whether statistics are displayed
 */
bool MonitorParameters::getDisplayStatistics( )
{
  return m_bDisplayStatistics;
}

/**
 * Set the variable that indicates whether statistics should be displayed. 
 *
 * @param b set whether statistics are displayed.
 */
bool MonitorParameters::setDisplayStatistics( bool b )  
{
  m_bDisplayStatistics = b;
  return true;
}

/**
 * Return the background color of the grid. A color is specified by a
 * string of 6 characters. Each pair of colors specifies the color in
 * the RGB spectrum. 
 *
 * @return background color
 */
char* MonitorParameters::getBackgroundColor( )
{
  return m_strBackgroundColor;
}

/** 
 * Set the background color of the grid. A color is specified by a
 * string of 6 characters. Each pair of colors specifies the color in
 * the RGB spectrum. 
 *
 * @param str set background color
 */
bool MonitorParameters::setBackgroundColor( char*  str )
{
  if( strlen( str ) > 6 )
    return false;
  
  strcpy( m_strBackgroundColor, str );
  return true;
}

/**
 * Return the color of the gridlines. A color is specified by a string
 * of 6 characters. Each pair of colors specifies the color in the RGB
 * spectrum. 
 *
 * @return grid color
 */
char* MonitorParameters::getGridColor( )
{
  return m_strGridColor;
}

/**
 * Set the color of the gridlines. A color is specified by a string of
 * 6 characters. Each pair of colors specifies the color in the RGB
 * spectrum. 
 *
 * @param str set gridcolor
 */
bool MonitorParameters::setGridColor( char*  str )
{
  if( strlen( str ) > 6 )
    return false;
  
  strcpy( m_strGridColor, str );
  return true;
}

/**
 * Return the color of the preys. A color is specified by a string of
 * 6 characters. Each pair of colors specifies the color in the RGB
 * spectrum. 
 *
 * @return prey color
 */
char* MonitorParameters::getPreyColor( )
{
  return m_strPreyColor;
}

/**
 * Set the color of the preys. A color is specified by a string of 6
 * characters. Each pair of colors specifies the color in the RGB
 * spectrum. 
 *
 * @param str set prey color
 */
bool MonitorParameters::setPreyColor( char*  str )
{
  if( strlen( str ) > 6 )
    return false;
  
  strcpy( m_strPreyColor, str );
  return true;
}

/**
 * Return the color of the predators. A color is specified by a string
 * of 6 characters. Each pair of colors specifies the color in the RGB
 * spectrum. 
 *
 * @return predator color
 */
char* MonitorParameters::getPredatorColor( )
{
  return m_strPredatorColor;
}

/**
 * Set the color of the predators. A color is specified by a string of
 * 6 characters. Each pair of colors specifies the color in the RGB
 * spectrum. 
 *
 * @param str set predator color
 */
bool MonitorParameters::setPredatorColor( char*  str )
{
  if( strlen( str ) > 6 )
    return false;
  
  strcpy( m_strPredatorColor, str );
  return true;
}

/**
 * Return the color of a collided animal. A color is specified by a
 * string of 6 characters. Each pair of colors specifies the color in
 * the RGB spectrum. 
 *
 * @return collision color
 */
char* MonitorParameters::getCollisionColor( )
{
  return m_strCollisionColor;
}

/**
 * Set the color of a collided animal. A color is specified by a
 * string of 6 characters. Each pair of colors specifies the color in
 * the RGB spectrum. 
 *
 * @param str set collision color
 */
bool MonitorParameters::setCollisionColor( char*  str )
{
  if( strlen( str ) > 6 )
    return false;
  
  strcpy( m_strCollisionColor, str );
  return true;
}


