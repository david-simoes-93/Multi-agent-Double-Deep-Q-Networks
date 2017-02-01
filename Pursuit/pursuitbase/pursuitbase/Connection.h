/**
 * @file   Connection.h
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Fri Aug  1 14:18:55 2003
 * 
 * @brief This file contains the Connection class whicbh holds all
 * functionality related to a connection over UDP sockets.
 * 
 * 
 */
#ifndef _CONNECTION_
#define _CONNECTION_

#include <stdio.h>     // printf, FILE
#include <iostream>    // ostream
#include <netdb.h>     // sockaddr_in
#include <arpa/inet.h> // sockaddr_in

using namespace std;

#ifdef HAVE_UINT16_T
/** 
 * If specified port_type is set to uint16_t
 */
typedef uint16_t port_type;
#else // default to unsigned short
/** 
 * If uint16_t is not specified port_type is set to an unsigned short
 */
typedef unsigned short port_type;
#endif

#ifdef HAVE_UINT32_T
/** 
 * If specified host_type is set to uint32_t
 */
typedef uint32_t host_type;
#else // default to unsigned long
/** 
 * If uint32_t is not specified host_type is set to an unsigned long
 */
typedef unsigned long host_type;
#endif

/**
 * Socket is a combination of a filedescriptor with a server adress.
 */
typedef struct _socket{
    int    socketfd  ;                /**< File descriptor of the socket.    */
    struct sockaddr_in  serv_addr ;   /**< Server information of the socket. */
} Socket ;

/********************** CONNECTION ********************************************/

/**
 * This class creates creates a (socket) connection using a hostname
 * and a port number. After the connection is created it is possible
 * to send and receive messages from this connection.
 */
class Connection {

  Socket         m_sock;        /**< communication protocol with the server.  */
  int            m_iMaxMsgSize; /**< maximum message size for send and receive*/

public:

  // constructors
  Connection            (                                               );
  Connection            ( const char *        hostname, 
                          int                 port,   
                          int                 iSize                     );
  ~Connection           (                                               );

  // methods that deal with the connection itself
  void bind             ( int                 port = 0                  );
  bool connect          ( const char *        host, 
                          int                 port                      );
  void connect          ( struct sockaddr_in& dest                      ) ;
  void connect          ( const port_type&    port,
                          const host_type&    host                      ) ;
  struct sockaddr_in waitForNewConnection( 
                          char *              msg, 
                          int                 maxsize                   );
  void disconnect       ( void                                          );
   bool isConnected      ( void                                         ) const;

  // methods that deal with the communication over the connection
  int  message_loop     ( FILE *              in,       
                          FILE *              out                       );
  int  receiveMessage   ( char *              msg,
                          int                 maxsize                   );
  bool sendMessage      ( const char *        msg                       );
  int  getSocketFd      (                                               );  
  void show             ( ostream             os                        );

};

#endif
