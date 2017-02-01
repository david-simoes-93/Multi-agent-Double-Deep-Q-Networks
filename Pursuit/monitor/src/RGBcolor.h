/**
 * @file   RGBcolor.h
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Fri Aug  1 14:35:24 2003
 * 
 * @brief  This class holds one color specified by its RGB values.
 * 
 */
#ifndef _RGBCOLOR_H_
#define _RGBCOLOR_H_

using namespace std;

/**
 * This class holds the information of a color using its RGB values.
 * It is possible specify a color using three doubles or through a
 * string which represent the hexadecimal value of this color
 * (e.g. 00ff33). */
class RGBcolor 
{
 private:
  double m_dRed;    /**< red portion of the current color   */
  double m_dGreen;  /**< green portion of the current color */
  double m_dBlue;   /**< blue portion of the current color  */
  
 public:
 
  RGBcolor( double red, double green, double blue          );
  RGBcolor(                                                ); 
  RGBcolor( char *str                                      );
  
  void     setColor( double red, double green, double blue );
  void     setColor( char * str                            );
  
  double   getRed  (                                       ) const;
  double   getGreen(                                       ) const;
  double   getBlue (                                       ) const;
};

#endif
