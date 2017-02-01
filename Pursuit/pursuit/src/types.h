/**
 * @file   types.h
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Fri Aug  1 14:29:53 2003
 * 
 * @brief This file contains the different enumerations used in the
 * rest of the program.
 */

#ifndef _TYPES_
#define _TYPES_

#include "Parameters.h"

// extern Parameters PS;

/**
 * An ObjectT holds all the possible objects that can make a
 * connection with the server. This are not only the objects (preys
 * and predators) on the field, but also the monitors. Furthermore
 * also an object OBJ_AGENTS is specified, that can be used when both
 * the predators and prey are meant and an object OBJ_ALL that denotes
 * all objects (useful in an iteration).
 */
typedef enum _objectT {
  OBJ_ILLEGAL,
  OBJ_PREDATOR,
  OBJ_PREY,
  OBJ_MONITOR,
  OBJ_AGENTS,
  OBJ_ALL
} ObjectT ; 

/**
 * The ActionT enumeration specifies all different actions that can be
 * executed. This are the movement commands to any of the adjacent
 * cells of the agent.
 */
typedef enum _actionT {
  ACT_ILLEGAL,
  ACT_MOVE_NORTH,
  ACT_MOVE_SOUTH,
  ACT_MOVE_EAST,
  ACT_MOVE_WEST,
  ACT_MOVE_NORTHEAST,
  ACT_MOVE_NORTHWEST,
  ACT_MOVE_SOUTHEAST,
  ACT_MOVE_SOUTHWEST,
  ACT_MOVE_NONE
} ActionT ;

#endif
