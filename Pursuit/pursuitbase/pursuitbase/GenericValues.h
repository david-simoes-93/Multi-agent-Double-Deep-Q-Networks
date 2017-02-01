/**
 * @file   GenericValues.h
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Fri Aug  1 14:31:56 2003
 * 
 * @brief This file contains the GenericValues class which is a
 * container for variable types with associated values. It can be used
 * to read in a configuration file.
 * 
 */
#ifndef _GENERIC_VALUES_
#define _GENERIC_VALUES_

#include <iostream>    // needed for output stream

using namespace std;

/**
 * This enumeration contains all generic values types used throughout
 * the code.
 */
enum GenericValueKind
{
  GENERIC_VALUE_DOUBLE  = 0,
  GENERIC_VALUE_STRING  = 1,
  GENERIC_VALUE_BOOLEAN = 2,
  GENERIC_VALUE_INTEGER = 3,
};

/******************************************************************************/
/********************   CLASS GENERICVALUET   *********************************/
/******************************************************************************/

/**
 * This class contains a pointer to a variable of a generic type
 * (double, char*, bool, int) and this pointer is associated with a
 * string by which the variable can be reached. Several methods are
 * defined which enable one to access the name and value of the
 * variable.
 */
class GenericValueT
{
  // private member data
private:

  const char*      m_strName;  /**< the name associated with the variable to
                                    which the class pointer points            */
  void*            m_vAddress; /**< a pointer to a variable of a generic type */
  GenericValueKind m_type;     /**< the (generic) type of the variable to which
                                    the class pointer points                  */

  // public methods
public:

  // constructor for the GenericValueT class
  GenericValueT( const char *strName, void *vAddress, GenericValueKind type );

  // destructor for the GenericValueT class
  ~GenericValueT( );

  // get methods for private member variables
  const char* getName ( );

  // methods to set/get the value of this generic variable
  bool  setValue( char *strValue );
  char* getValue( char *strValue );

  // display method
  void show( ostream& out, const char *strSeparator );
};

/******************************************************************************/
/********************   CLASS GENERICVALUES   *********************************/
/******************************************************************************/

/**
 * This class contains a collection of GenericValueT objects. This
 * makes it possible to reference variables using string names. The
 * class is an abstract class which should not be instantiated. It is
 * the subclass of this class which contains the actual variables. In
 * order to add a reference to a variable the method 'addSetting' must
 * be used which associates the variables in the subclass with string
 * names. The GenericValues class is used to read in configuration
 * files. This now becomes very easy as long as one makes sure that
 * the names in the configuration file match the string names
 * associated with the corresponding variables 
 */
class GenericValues
{
  // private member data
private:

  char  *m_strClassName;    /**< the name associated with this group of generic
                                 values; this is usually the name of the
                                 subclass which contains the actual variables */
  GenericValueT ** m_values;/**< a pointer to an array containing all generic
                                 value pointers (GenericValueT objects)       */
  int   m_iValuesTotal;     /**< the total number of generic values stored in
                                 the collection so far                        */
  int   m_iMaxGenericValues;/**< the number of generic values in the current
                                 collection, i.e. the maximum number of values
                                 that can be stored                           */

  GenericValueT* getValuePtr( const char *strName                             );

public:

  // constructor for the GenericValues class
  GenericValues           ( char *strName,              int iMaxValues        );

  // destructor for the GenericValues class
  virtual ~GenericValues  (                                                   );

  // get methods for private member variables
  char* getClassName      (                                                   );
  int   getValuesTotal    (                                                   );

  // method for adding a generic value to the collection
  bool addSetting( const char *strName, void *vAddress, GenericValueKind t );

  // methods for reading and writing generic values and collections of values
  virtual char* getValue  ( const char *strName,       char *strValue         );
  virtual bool  setValue  ( const char *strName,       char *strValue         );
  virtual bool  readValues( const char *strFile, const char *strSeparator = 0 );
  virtual bool  saveValues( const char *strFile, const char *strSeparator = 0,
                                                       bool bAppend = true    );

  // display method
  virtual void  show      ( ostream& out,        const char *strSeparator     );
};

#endif
