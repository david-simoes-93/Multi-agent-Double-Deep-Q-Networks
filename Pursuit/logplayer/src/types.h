/**
 * @file   types.h
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Mon Aug 11 13:41:47 2003
 * 
 * @brief This file contains the types used throughout the logplayer
 * sources and defines the status possibilities for the logplayer.
 * 
 */
#ifndef _TYPES_
#define _TYPES_

/**
 * Maximum number of characters in message over sockets. 
 */
#define MAX_MSG 1024

/**
 *  This enumerator contains all the possible states of the **
 *  logplaye. This is for instance the status of playing, stopping or
 *  advancing step by step. 
 */
typedef enum _statusT {
  STATUS_ILLEGAL,
  STATUS_STOP,
  STATUS_REV_STEP,
  STATUS_REV_PLAY,
  STATUS_STEP,
  STATUS_PLAY
} StatusT ; 


#endif
