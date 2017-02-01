/**
 * @file   Parameters.C
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Fri Aug  1 15:34:27 2003
 * 
 * @brief This file contains the specific Parameters to this
 * program. Alle textual strings are linked to their corresponding
 * variables using the GenericValues class.
 * 
 * 
 */
#include "Parameters.h"
#include <string.h>

/**
 * This constructors sets all the defaults values of the parameters
 * and associates text strings with all variables. 
 */
Parameters::Parameters( ): GenericValues("Parameters", 29)
{
  m_iColumns               = 15;
  m_iRows                  = 15;
  m_iPort                  = 4001;
  m_iTimeStep              = 100;
  m_iNrEpisodes            = 50;
  m_iNextEpisodeWait       = 1000;
  m_iVisibleDistance       = 2;
  m_iCaptureMethod         = 1;
  m_bAllowComm             = false;  
  m_bAllowDiagonalPrey     = false;
  m_bAllowDiagonalPred     = false;  
  m_bLogObjectInfo         = true;
  strcpy( m_strLogObjectFile, "./game.log" );
  m_bAutomaticStart        = false;  
  m_bPenalizeAll           = false;  
  m_bUseIds                = false;
  strcpy( m_strOutputFile, "cycles.txt" );
  m_iAgent1PredPosX        = -1;
  m_iAgent1PredPosY        = -1;
  m_iAgent2PredPosX        = -1;
  m_iAgent2PredPosY        = -1;
  m_iAgent3PredPosX        = -1;
  m_iAgent3PredPosY        = -1;
  m_iAgent4PredPosX        = -1;
  m_iAgent4PredPosY        = -1;
  m_iAgent1PreyPosX        = -1;
  m_iAgent1PreyPosY        = -1;
  m_iAgent2PreyPosX        = -1;
  m_iAgent2PreyPosY        = -1;

  
  // add all the settings and link text string to variable
  addSetting( "columns",         &m_iColumns,          GENERIC_VALUE_INTEGER );
  addSetting( "rows",            &m_iRows,             GENERIC_VALUE_INTEGER );
  addSetting( "port",            &m_iPort,             GENERIC_VALUE_INTEGER );
  addSetting( "time_step",       &m_iTimeStep,         GENERIC_VALUE_INTEGER );
  addSetting( "nr_episodes",     &m_iNrEpisodes,       GENERIC_VALUE_INTEGER );
  addSetting( "next_episode_wait",&m_iNextEpisodeWait, GENERIC_VALUE_INTEGER );
  addSetting( "visible_distance",&m_iVisibleDistance,  GENERIC_VALUE_INTEGER );
  addSetting( "capture_method",  &m_iCaptureMethod,    GENERIC_VALUE_INTEGER );
  addSetting( "allow_comm",      &m_bAllowComm,        GENERIC_VALUE_BOOLEAN );
  addSetting( "allow_diagonal_prey", 
                                 &m_bAllowDiagonalPrey,GENERIC_VALUE_BOOLEAN );
  addSetting( "allow_diagonal_pred", 
                                 &m_bAllowDiagonalPred,GENERIC_VALUE_BOOLEAN );
  addSetting( "log_object_info", &m_bLogObjectInfo,    GENERIC_VALUE_BOOLEAN );
  addSetting( "log_object_file", &m_strLogObjectFile,  GENERIC_VALUE_STRING  );
  addSetting( "automatic_start", &m_bAutomaticStart,   GENERIC_VALUE_BOOLEAN );
  addSetting( "output_file"     ,&m_strOutputFile,     GENERIC_VALUE_STRING  );

  addSetting( "penalize_all"    ,&m_bPenalizeAll,      GENERIC_VALUE_BOOLEAN );
  addSetting( "use_ids"         ,&m_bUseIds,           GENERIC_VALUE_BOOLEAN );
  addSetting( "agent1_predpos_x",&m_iAgent1PredPosX,   GENERIC_VALUE_INTEGER );
  addSetting( "agent1_predpos_y",&m_iAgent1PredPosY,   GENERIC_VALUE_INTEGER );
  addSetting( "agent2_predpos_x",&m_iAgent2PredPosX,   GENERIC_VALUE_INTEGER );
  addSetting( "agent2_predpos_y",&m_iAgent2PredPosY,   GENERIC_VALUE_INTEGER );
  addSetting( "agent3_predpos_x",&m_iAgent3PredPosX,   GENERIC_VALUE_INTEGER );
  addSetting( "agent3_predpos_y",&m_iAgent3PredPosY,   GENERIC_VALUE_INTEGER );
  addSetting( "agent4_predpos_x",&m_iAgent4PredPosX,   GENERIC_VALUE_INTEGER );
  addSetting( "agent4_predpos_y",&m_iAgent4PredPosY,   GENERIC_VALUE_INTEGER );
  addSetting( "agent1_preypos_x",&m_iAgent1PreyPosX,   GENERIC_VALUE_INTEGER );
  addSetting( "agent1_preypos_y",&m_iAgent1PreyPosY,   GENERIC_VALUE_INTEGER );
  addSetting( "agent2_preypos_x",&m_iAgent2PreyPosX,   GENERIC_VALUE_INTEGER );
  addSetting( "agent2_preypos_y",&m_iAgent2PreyPosY,   GENERIC_VALUE_INTEGER );

}

/**
 * Returns the number of columns of the grid. 
 */
int Parameters::getColumns( ) const
{
  return m_iColumns;
}

/**
 * Sets the number of columns of the grid. 
 */
bool Parameters::setColumns( int    i )
{
  m_iColumns = i;
  return true;
}

/**
 * Returns the number of rows of the grid. 
 */
int Parameters::getRows( ) const
{
  return m_iRows;
}

/**
 * Sets the number of rows of the grid.
 */
bool Parameters::setRows( int    i )
{
  m_iRows = i;
  return true;
}

/**
 * Return the port number of the server with which a client can connect. 
 */
int Parameters::getPort( )
{
  return m_iPort;
}

/**
 * Set the port number of the server with which a client can connect. 
 */
bool Parameters::setPort( int i )  
{
  m_iPort = i;
  return true;
}

/**
 * Get the time (in msec.) of one turn.
 */
int Parameters::getTimeStep( )
{
  return m_iTimeStep;
}

/**
 * Set the time (in msec.) of one turn.
 */
bool Parameters::setTimeStep( int i )  
{
  m_iTimeStep = i;
  return true;
}

/**
 * Get the number of episodes (prey captures) before the program is halted. 
 */
int Parameters::getNrEpisodes( )
{
  return m_iNrEpisodes;
}

/**
 * Set the number of episodes (prey captures) before the program is halted.
 */
bool Parameters::setNrEpisodes( int i )  
{
  m_iNrEpisodes = i;
  return true;
}

/**
 * Return the time (in msec.) that is waited after a prey is captured
 * and the next episode starts. 
 */
int Parameters::getNextEpisodeWait( )
{
  return m_iNextEpisodeWait;
}

/**
 * Set the time (in msec.) that is waited after a prey is captured and
 * the next episode starts. 
 */
bool Parameters::setNextEpisodeWait( int i )  
{
  m_iNextEpisodeWait = i;
  return true;
}

/**
 * Return the distance in which objects are returned in the visual
 * message.  When the relative distance in number of cells is smaller
 * than this value, the object is added to the visual message,
 * otherwise it is discarded.  A value of -1 indicates that all
 * objects are listed. 
 */
int Parameters::getVisibleDistance( )
{
  return m_iVisibleDistance;
}

/**
 * Set the distance in which objects are returned in the visual
 * message.  When the relative distance in number of cells is smaller
 * than this value, the object is added to the visual message,
 * otherwise it is discarded.  A value of -1 indicates that all
 * objects are listed. 
 */
bool Parameters::setVisibleDistance( int i )  
{
  m_iVisibleDistance = i;
  return true;
}

/**
 * Return the capture method which specifies the situation in which a
 * prey is captured. 
 */
int Parameters::getCaptureMethod( )
{
  return m_iCaptureMethod;
}

/**
 * Set the capture method denoted by an integer. There are different capture
 *    methods:
 *  - A predator and a prey share the same cell.
 *  - A prey is surrounded by four predators. The predators are thus 
 *    located north, south, east and west of the prey.
 *  - A prey is surrounded by two predators. 
 *  - In the previous turn two predators were surrounding the prey, but only 
 *    one moved to the location of the prey. When a predator attempts to capture 
 *    a prey while the above conditions are violated, the predator is 
 *    penalized and placed on a random position on the field. 
 */
bool Parameters::setCaptureMethod( int i )  
{
  m_iCaptureMethod = i;
  return true;
}

/**
 * Returns whether the preys are allowed to move diagonally or not. 
 */
bool Parameters::getAllowDiagonalPrey( )
{
  return m_bAllowDiagonalPrey;
}

/**
 * Set the variable that indicates whether the preys are allowed to
 * move diagonal. 
 */
bool Parameters::setAllowDiagonalPrey( bool b )  
{
  m_bAllowDiagonalPrey = b;
  return true;
}

/**
 * Returns whether the predators are allowed to move diagonally or not. 
 */
bool Parameters::getAllowDiagonalPred( )
{
  return m_bAllowDiagonalPred;
}

/**
 * Set the variable that indicates whether the predators are allowed
 * to move diagonal. 
 */
bool Parameters::setAllowDiagonalPred( bool b )  
{
  m_bAllowDiagonalPred = b;
  return true;
}


/**
 * Returns whether the agents are allowed to communicate. 
 */
bool Parameters::getAllowComm( )
{
  return m_bAllowComm;
}

/**
 * Set the variable that indicates whether the agents are allowed to
 * communicate .
 */
bool Parameters::setAllowComm( bool b )  
{
  m_bAllowComm = b;
  return true;
}

/**
 * Returns whether the object information should be logged.
 */
bool Parameters::getLogObjectInfo( )
{
  return m_bLogObjectInfo;
}

/**
 * Set the variable that indicates whether the server should start
 * automatically.
 */
bool Parameters::setLogObjectInfo( bool b   )      
{
  m_bLogObjectInfo = b;
  return true;
}

/**
 * Return the file to which the log information is written. The
 * received actions and the positions of all preys/predators are
 * written to this file.
 */
char* Parameters::getLogObjectFile( )
{
  return m_strLogObjectFile;
}

/**
 * Set the file to which the log information is written. The received
 * actions and the positions of all preys/predators are written to
 * this file.*/
bool Parameters::setLogObjectFile( char*  str )
{
  strcpy( m_strLogObjectFile, str );
  return true;
}


/**
 * Returns whether the server should start automatically 
 */
bool Parameters::getAutomaticStart( )
{
  return m_bAutomaticStart;
}

/**
 * Set the variable that indicates whether the server should start
 * automatically. 
 * @param bool indicating whether the server should start automatically  
 */
bool Parameters::setAutomaticStart( bool b   )      
{
  m_bAutomaticStart = b;
  return true;
}


/**
 * Return the file to which the output is written. The number of
 * cycles for each episode is written to this file. If no file is
 * specified, the output is directed to standard output. 
 */
char* Parameters::getOutputFile( )
{
  return m_strOutputFile;
}

/**
 * Set the file to which the output is written. The number of cycles
 * for each episode is written to this file. If no file is specified,
 * the output is directed to standard output. 
 */
bool Parameters::setOutputFile( char*  str )
{
  strcpy( m_strOutputFile, str );
  return true;
}

/**
 * Returns whether the server should sent a penalty or collision
 * message to all agents or only to the agents that were involved in
 * the penalty or collison.
 */
bool Parameters::getPenalizeAll( )
{
  return m_bPenalizeAll;
}

/**
 * Set the variable that indicates whether the server should sent a
 * penalty or collision message to all agents or only to the involved
 * agents. 
 *
 * @param bool indicating whether the server should sent penalty and
 * collision message to all agents or only the involved ones.
 */
bool Parameters::setPenalizeAll( bool b   )      
{
  m_bPenalizeAll = b;
  return true;
}

/**
 * Return the variable that indicates whether the server should sent
 * the messages containing the ids of the other agents. When this
 * option is turned on in combination with the penalize_all set to
 * false. Each predator also receives the referee messages of the
 * other predators (including their id).
 *
 * @return bool indicating whether the server should sent the ids of
 * the other agents in the messages.
 */
bool Parameters::getUseIds( )
{
  return m_bUseIds;
}

/**
 * Set the variable that indicates whether the server should sent the
 * messages containing the ids of the other agents. When this option
 * is turned on in combination with the penalize_all set to
 * false. Each predator also receives the referee messages of the
 * other predators (including their id). 
 *
 * @param bool indicating whether the server should sent the ids of
 * the other agents in the messages.
 */
bool Parameters::setUseIds( bool b   )      
{
  m_bUseIds = b;
  return true;
}

/* 
 * This method returns the position of this agent. When -1 its
 * position is assigned randomly, otherwise it is set to the specified
 * value.
 */
int  Parameters::getAgent1PredPosX( )
{
  return m_iAgent1PredPosX;
}

/* 
 * This method sets the position of this agent. When -1 its position
 * is assigned randomly, otherwise it is set to the specified value.
 */
bool Parameters::setAgent1PredPosX( int i )
{
  m_iAgent1PredPosX = i;
}

/* 
 * This method returns the position of this agent. When -1 its
 * position is assigned randomly, otherwise it is set to the specified
 * value.
 */
int  Parameters::getAgent1PredPosY( )
{
  return m_iAgent1PredPosY;
}

/* 
 * This method sets the position of this agent. When -1 its position
 * is assigned randomly, otherwise it is set to the specified
 * value. 
 */
bool Parameters::setAgent1PredPosY( int i )
{
  m_iAgent1PredPosY = i;
}

/* 
 * This method returns the position of this agent. When -1 its
 * position is assigned randomly, otherwise it is set to the specified
 * value.
 */
int  Parameters::getAgent2PredPosX( )
{
  return m_iAgent2PredPosX;
}

/* 
 * This method sets the position of this agent. When -1 its position
 * is assigned randomly, otherwise it is set to the specified value.
 */
bool Parameters::setAgent2PredPosX( int i )
{
  m_iAgent2PredPosX = i;
}

/* 
 * This method returns the position of this agent. When -1 its
 * position is assigned randomly, otherwise it is set to the specified
 * value.
 */
int  Parameters::getAgent2PredPosY( )
{
  return m_iAgent2PredPosY;
}

/* 
 * This method sets the position of this agent. When -1 its position
 * is assigned randomly, otherwise it is set to the specified
 * value. 
 */
bool Parameters::setAgent2PredPosY( int i )
{
  m_iAgent2PredPosY = i;
}

/* 
 * This method returns the position of this agent. When -1 its
 * position is assigned randomly, otherwise it is set to the specified
 * value.
 */
int  Parameters::getAgent3PredPosX( )
{
  return m_iAgent3PredPosX;
}

/* 
 * This method sets the position of this agent. When -1 its position
 * is assigned randomly, otherwise it is set to the specified value.
 */
bool Parameters::setAgent3PredPosX( int i )
{
  m_iAgent3PredPosX = i;
}

/* 
 * This method returns the position of this agent. When -1 its
 * position is assigned randomly, otherwise it is set to the specified
 * value.
 */
int  Parameters::getAgent3PredPosY( )
{
  return m_iAgent3PredPosY;
}

/* 
 * This method sets the position of this agent. When -1 its position
 * is assigned randomly, otherwise it is set to the specified
 * value. 
 */
bool Parameters::setAgent3PredPosY( int i )
{
  m_iAgent3PredPosY = i;
}

/* 
 * This method returns the position of this agent. When -1 its
 * position is assigned randomly, otherwise it is set to the specified
 * value.
 */
int  Parameters::getAgent4PredPosX( )
{
  return m_iAgent4PredPosX;
}

/* 
 * This method sets the position of this agent. When -1 its position
 * is assigned randomly, otherwise it is set to the specified value.
 */
bool Parameters::setAgent4PredPosX( int i )
{
  m_iAgent4PredPosX = i;
}

/* 
 * This method returns the position of this agent. When -1 its
 * position is assigned randomly, otherwise it is set to the specified
 * value.
 */
int  Parameters::getAgent4PredPosY( )
{
  return m_iAgent4PredPosY;
}

/* 
 * This method sets the position of this agent. When -1 its position
 * is assigned randomly, otherwise it is set to the specified
 * value. 
 */
bool Parameters::setAgent4PredPosY( int i )
{
  m_iAgent4PredPosY = i;
}

/* 
 * This method returns the position of this agent. When -1 its
 * position is assigned randomly, otherwise it is set to the specified
 * value.
 */
int  Parameters::getAgent1PreyPosX( )
{
  return m_iAgent1PreyPosX;
}

/* 
 * This method sets the position of this agent. When -1 its position
 * is assigned randomly, otherwise it is set to the specified value.
 */
bool Parameters::setAgent1PreyPosX( int i )
{
  m_iAgent1PreyPosX = i;
}

/* 
 * This method returns the position of this agent. When -1 its
 * position is assigned randomly, otherwise it is set to the specified
 * value.
 */
int  Parameters::getAgent1PreyPosY( )
{
  return m_iAgent1PreyPosY;
}

/* 
 * This method sets the position of this agent. When -1 its position
 * is assigned randomly, otherwise it is set to the specified
 * value. 
 */
bool Parameters::setAgent1PreyPosY( int i )
{
  m_iAgent1PreyPosY = i;
}

/* 
 * This method returns the position of this agent. When -1 its
 * position is assigned randomly, otherwise it is set to the specified
 * value.
 */
int  Parameters::getAgent2PreyPosX( )
{
  return m_iAgent2PreyPosX;
}

/* 
 * This method sets the position of this agent. When -1 its position
 * is assigned randomly, otherwise it is set to the specified value.
 */
bool Parameters::setAgent2PreyPosX( int i )
{
  m_iAgent2PreyPosX = i;
}

/* 
 * This method returns the position of this agent. When -1 its
 * position is assigned randomly, otherwise it is set to the specified
 * value.
 */
int  Parameters::getAgent2PreyPosY( )
{
  return m_iAgent2PreyPosY;
}

/* 
 * This method sets the position of this agent. When -1 its position
 * is assigned randomly, otherwise it is set to the specified
 * value. 
 */
bool Parameters::setAgent2PreyPosY( int i )
{
  m_iAgent2PreyPosY = i;
}
