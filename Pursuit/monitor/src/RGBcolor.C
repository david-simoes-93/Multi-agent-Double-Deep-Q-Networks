/**
 * @file   RGBcolor.C
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Fri Aug  1 15:39:02 2003
 * 
 * @brief This file contains the definitions for the RGBColori which
 * contains a color specified by its RGB values and contains several
 * conversion methods (e.g., to and from HEX decimal).
 * 
 * 
 */
#include "RGBcolor.h"

#include <stdlib.h>   // needed for strol 
#include <iostream>   // needed for cerr

/**
 * This constructor initializes a color using three double values
 * representing the red, green and blue portion respectively.  
 */
RGBcolor::RGBcolor( double red, double green, double blue ) 
{ 
  setColor( red, green, blue );
}

/** 
 * This constructor sets it to the default color (black).
 */
RGBcolor::RGBcolor( )
{
  RGBcolor( 0, 0, 0 ); 
}

/**
 * This constructor initializes a color using the hexadecimal value,
 * where each two hexadecimal values represent the red, green and blue
 * portion respectively.
 */
RGBcolor::RGBcolor( char *str ) 
{ 
  setColor( str );
}

/**
 * This method sets the color using three double values representing
 * the red, green and blue portion respectively.  
 */
void RGBcolor::setColor( double red, double green, double blue )
{
  m_dRed   = red ; 
  m_dGreen = green; 
  m_dBlue  = blue;
}

/**
 * This method sets the color using the hexadecimal value, where each
 * two hexadecimal values represent the red, green and blue portion
 * respectively.
 */
void RGBcolor::setColor( char *str ) 
{
  RGBcolor( );
  while( *str == ' ' )         // walk to first none digit
    str++;
  if( str == '\0' )
    return;
    
  double dColor[3];
  char   strChar[3], *cTmp;
  long   lTmp;
  bool   bReturn = false;
  
  for( int i = 0; i < 3; i++ )   // find three hex digits each of length 2
  {
    if( str[0] != '\0' && str[1] != '\0' )       // if we have two characters
    {  
      strChar[0] = str[0]; strChar[1] = str[1]; strChar[2] = '\0';
      lTmp = strtol( strChar, &cTmp, 16 );      // copy them and convert
      if( cTmp != strChar+2 || lTmp < 0 || lTmp > 255 ) // in range
      {
        cerr << "reading hex digit too long: " << str << endl;
        break;
      }
      dColor[i] = (double) lTmp / 255.0;        // calculate associated double
      str += 2;                                 // skip the characters
      if( i == 2 )                              // have converted third digit
        bReturn = true;
    }
  }
  if( bReturn == true )
  {  
    m_dRed   = dColor[0];
    m_dGreen = dColor[1];
    m_dBlue  = dColor[2];
  }
}

/** 
 * This method returns the red component of this RGB color.
 * 
 * 
 * @return red component of this RGBColor
 */
double RGBcolor::getRed  () const 
{
  return m_dRed; 
}

/** 
 * This method returns the green component of this RGB color.
 * 
 * 
 * @return green component of this RGBColor
 */
double RGBcolor::getGreen() const 
{
  return m_dGreen; 
}

/** 
 * This method returns the blue component of this RGB color.
 * 
 * 
 * @return blue component of this RGBColor
 */
double RGBcolor::getBlue () const 
{ 
  return m_dBlue; 
}
