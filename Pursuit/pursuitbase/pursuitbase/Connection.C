/**
 * @file   Connection.C
 * @author Jelle Kok <jellekok@science.uva.nl>
 * @date   Fri Aug  1 15:24:25 2003
 * 
 * @brief This file contains all the class definitions of the
 * Connection class which makes it possible to create connections over
 * UDP sockets.
 * 
 * 
 */
#include <errno.h>       // EWOULDBLOCK
#include <unistd.h>      // close
#include <string.h>      // strlen
#include <sys/types.h>   // socket
#include <sys/socket.h>  // socket

#ifndef INADDR_NONE
/** 
 * If not specified (i.e. Solaris), INADDR_NONE is set to ((in_addr_t)-1) 
 */
#define INADDR_NONE ((in_addr_t)-1)
#endif

#include "Connection.h"


/*****************************************************************************/
/********************** CONNECTION *******************************************/
/*****************************************************************************/

/**
 * Default constructor. Only sets the maximum message size. 
 */
Connection::Connection( )
{
  m_iMaxMsgSize = 1024;
  m_sock.socketfd = -1;    
}

/**
 * Constructor makes a connection with the server using the connect
 * method.  
 * 
 * @param hostname string representation of host machine (string or IP number) 
 * 
 * @param port port number for connection of the host machine 
 * @param iMaxSize maximum message size that can be sent or received 
 */
Connection::Connection(const char *hostname, int port, int iMaxSize)
{
  m_iMaxMsgSize = iMaxSize;
  if(  connect( hostname, port ) )
    printf( "(Connection:connection) Socket connection made with %s:%d",
                                                             hostname, port );
  else
    printf( "(Connection:Connection) Could not create connection with %s:%d"
                                                          , hostname, port );
}

/**
 * Deconstructor closes the connection with the server 
 */
Connection::~Connection()
{
  disconnect();
}

/**
 * This method binds a new socket on the specified port number. 
 * @param port port number for this end of the connection 
 */
void Connection::bind ( const int port ) 
{
  memset ( (char *)&m_sock.serv_addr, 0, sizeof ( m_sock.serv_addr ) ) ;
  
  // make a socket
  m_sock.socketfd = socket(AF_INET, SOCK_DGRAM, 0);
  if ( m_sock.socketfd < 0 )
  {
    cerr << "Error creating socket" << endl;
    m_sock.socketfd = -1;    
    return;
  }

  // name the socket
  struct sockaddr_in listen_addr;

  memset ( (char *)&listen_addr, 0, sizeof ( listen_addr ) ) ;
  listen_addr.sin_family            = AF_INET ;
  listen_addr.sin_addr.s_addr       = htonl ( INADDR_ANY );
  listen_addr.sin_port              = htons ( port );

  int err = ::bind ( m_sock.socketfd, (struct sockaddr *) &listen_addr,
                     sizeof ( listen_addr) );
  if ( err < 0 )
  {
    cerr << "Error binding socket" << endl;
    cerr << errno << " " 
                  << (EBADF==errno) 
                  << (EINVAL ==errno) 
                  << (EACCES ==errno)                   
                  << (ENOTSOCK ==errno)                                     
                  << (EROFS ==errno)
                  << (EFAULT ==errno)                  
                  << (ENAMETOOLONG ==errno)                  
                  << (ENOENT ==errno)
                  << (ENOMEM ==errno)                  
                  << (ENOTDIR ==errno)                                    
                  << (ELOOP ==errno)
                  << endl;
    printf( "%d, %d\n", m_sock.socketfd, port );
  }
}

/**
 * This method sets up a connection with the server.
 *
 * @param hostname string representation of host machine (string or IP number)
 * @param port port number for connection of the host machine
 *
 * @return bool indicating whether connection was made 
 */
bool Connection::connect(const char *host, int port )
{
  struct hostent *host_ent;
  struct in_addr *addr_ptr;
  struct sockaddr_in  cli_addr ;
  int    sockfd ;  

  m_sock.socketfd = -1 ;

  if( (host_ent = (struct hostent*)gethostbyname(host)) == NULL)
  {
    // if not a string, get information from IP adress.
    if( inet_addr(host) == INADDR_NONE )
    {
      cerr << "(Connection::connect) Cannot find host " << host << endl;
      return false ;
    }
  }
  else   // get the necessary information from the hostname (string)
  {
    addr_ptr = (struct in_addr *) *host_ent->h_addr_list;
    host = inet_ntoa(*addr_ptr);
  }

  //  Open UDP socket.
  if( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    cerr << "(Connection::connect) Cannot create socket " << host << endl;
    return false ;
  }

  // insert the information of the client
  cli_addr.sin_family      = AF_INET ;
  cli_addr.sin_addr.s_addr = htonl(INADDR_ANY) ;
  cli_addr.sin_port        = htons(0) ;

  // bind the client to the server socket
  if(::bind(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0)
  {
    cerr << "(Connection::connect) Cannot bind local address " << host << endl;
    return false ;
  }

  //  Fill in the structure with the address of the server.
  m_sock.socketfd = sockfd ;

  m_sock.serv_addr.sin_family       = AF_INET ;
  m_sock.serv_addr.sin_addr.s_addr  = inet_addr(host);
  m_sock.serv_addr.sin_port         = htons(port) ;

  return true;
}

/**
 * This method connects to the socket descriptor associated with this
 * class to the specified socket adress. 
 * @param dest destination socket to connect to
 */
void Connection::connect ( struct sockaddr_in& dest ) 
{
  int err = ::connect ( m_sock.socketfd, (struct sockaddr *) &dest,
                        sizeof ( dest ) );
  if ( err < 0 ) 
  {
    perror( "Error connecting socket" );
    m_sock.socketfd = -1;
  }
  else
	{
    m_sock.serv_addr = dest;
	}
}


/**
 * This method connect to the specified host with the specified port number.
 * @param port port number to connect to 
 * @param host host to connect to 
 */
void Connection::connect ( const port_type& port, 
                           const host_type& host ) 
{
  struct sockaddr_in dest;
  memset( (char *) &dest, 0, sizeof ( dest ) ) ;
  dest.sin_family            = AF_INET ;
  dest.sin_addr.s_addr       = htonl ( host );
  dest.sin_port              = htons ( port );

  connect ( dest );
}


/**
 * This method closes the current socket connection.
 */
void Connection::disconnect( void )
{
  if (isConnected() )
  {
    close(m_sock.socketfd) ;
    m_sock.socketfd = -1; // This also 'sets' isConnected() to false
  }
}

/**
 * This method determines whether the socket connection is connected.
 * @return bool indicating whether socket connection is available 
 */
bool Connection::isConnected(void) const
{
  return(m_sock.socketfd != -1);
}

/**
 * This method reads a message from the connection. When there is no
 * message available, it blocks till it receives a message.
 * @param msg string in which message is stored
 * @param maxsize maximum size of the message.
 * @return -1: error, 0: 0 bytes were read, 1 when read was succesfull 
 */
int Connection::receiveMessage( char *msg, int maxsize )
{
  socklen_t servlen ;

  int n;
  struct sockaddr_in serv_addr ;

  servlen = sizeof(serv_addr) ;

  // receive message from server
  n = recvfrom(m_sock.socketfd, msg, maxsize, 0,
                              (struct sockaddr *)&serv_addr, &servlen);

  if(n < 0)                                   // error
  {    
//    cerr << m_sock.serv_addr << endl;
//    perror( "(Connection::receiveMessage) " );
    m_sock.socketfd = -1;
    if( n == -1 && errno == EWOULDBLOCK)
    {
      msg[0] = '\0' ;
      return 0 ;
    }
    else
      return -1;
  }
  else                                       // succesfull, set new server info
  {                                          // next message will go to there
    m_sock.serv_addr.sin_port = serv_addr.sin_port ;
    msg[n] = '\0' ;

    return ( n == 0 ) ? 0 : 1 ;
  }
}

/**
 * This method waits for a new connection. The received message is put
 * in 'msg' and its maximum size is 'maxsize'. The method returns the
 * information from the sending socket.
 * @param msg received message
 * @param maxsize maximum size of 'msg'
 * @return information from sending socket. 
 */
struct sockaddr_in Connection::waitForNewConnection( char *msg, int maxsize )
{
  socklen_t servlen ;

  int n;
  struct sockaddr_in serv_addr ;

  servlen = sizeof(serv_addr) ;

  // receive message from server
  n = recvfrom(m_sock.socketfd, msg, maxsize, 0,
                              (struct sockaddr *)&serv_addr, &servlen);

  if( n < 0 )    // error
  {
    msg[0] = '\0' ;
    perror( "waitForNewConnection" );
  }
  return serv_addr;
}


/**
 * This method sends a message to the server using the current
 * connection.
 *
 * @param msg string which contains message
 * @return true on succes, false in case of failure 
 */
bool Connection::sendMessage( const char *msg )
{
  int n;

  n = strlen(msg) + 1;  

  if( sendto(m_sock.socketfd, msg, n, 0, 
     (struct sockaddr *)&m_sock.serv_addr, sizeof(m_sock.serv_addr)) != n )
  {
    if( sendto(m_sock.socketfd, msg, n, 0, NULL, 0 ) != n )
    {
      perror( "(Connection::sendMessage)" );
      m_sock.socketfd = -1;
      return false ;
    }
  }
  return true ;
}

/**
 * This method returns the file descriptor of this connection.
 * @return file descriptor of this connection.
 */
int Connection::getSocketFd( )
{
  return m_sock.socketfd;
}
  
/**
 * This method always loops and waits for input. When input is
 * received from fpin then this input is send to the server using the
 * current connection.  When message is received from the server, this
 * message is sent to fpout.
 *
 * @param fpin file pointer from which input is read (i.e. stdin )
 * @param fpout file pointer to which output should be directed (i.e. stdout)
 * @return 0 but only when error occured in reading input 
 */
int Connection::message_loop( FILE *fpin, FILE *fpout )
{
  fd_set readfds, readfds_bak;
  int in, max_fd, n, ret;
  char buf[m_iMaxMsgSize];

  in = fileno( fpin );
  FD_ZERO( &readfds );
  FD_SET( in, &readfds );
  readfds_bak = readfds;
  max_fd = ((in > m_sock.socketfd) ? in : m_sock.socketfd) = 1;

  while( 1 )
  {
    readfds = readfds_bak;
    // wait for message from socket or fpin
    if(( ret = select( max_fd, &readfds, NULL, NULL, NULL )) < 0 )
    {
//      perror("select");
      break;
    }
    else if( ret != 0 )
    {
      if( FD_ISSET(in, &readfds))              // file descriptor fpin was set
      {
        fgets(buf, m_iMaxMsgSize, fpin);             // get the message
        if( sendMessage(buf ) == false )       // and send it
          break;
      }
      if( FD_ISSET(m_sock.socketfd, &readfds ) ) // file descriptor socket was set
      {
        n = receiveMessage(buf, m_iMaxMsgSize);      // receive the message
        if( n == -1 )
          break;
        else if( n > 0 )                       // and put it to fpout
        {
          fputs(buf, fpout);
          fputc( '\n', fpout);
        }
        fflush(stdout);
      }
    }
  }
  return 0;
}


/**
 * This method prints whether the connection is up or not.
 * @param os output stream to which output should be directed 
 */
void Connection::show( ostream os )
{
  if( ! isConnected() )
    os << "Not connected" << endl;
  else
    os << "Connected" << endl;

}

/********************** TESTING PURPOSES *************************************/

/*
int main( void )
{
  Connection server;      
  char       message[1024];
  

  server.bind( 4001 );   // open the port
  
  struct sockaddr_in cli_addr = server.waitForNewConnection(message,1024);

  // received new connection, use host and port number to connect 
  unsigned long host = ntohl ( cli_addr.sin_addr.s_addr );
  int           port = ntohs ( cli_addr.sin_port );  

  // for two-way communication also create client for sending info
  Connection client;
  client.bind();
  client.connect( port, host );
  printf( "Connect to %d %d\n" , port, cli_addr.sin_addr.s_addr);  
  while( 1 )
  {
    printf( "succeeded sending: %d\n", client.sendMessage( "hoi\n" ) );
    usleep( 100 * 1000 );
  }    
  return 0;
}



#define MAX_SIZE 1024

int main( int argc, char * argv [ ] )
{

  char       message[MAX_SIZE];
  char       host[128];
  if( argc == 1 || argc > 3 )
  {
    cerr << "usage as client: " << argv[0] << " port host " << endl;
    cerr << "usage as server: " << argv[0] << " port      " << endl;
    exit(0);
  }
  
  if( argc == 3 )
  {
    strcpy( host, argv[2] );
    cerr << "host: " << host << endl;
  }
  int port = atoi( argv[1] );

  cerr << "port: " << port << endl;

  if( argc == 2 )
  {
    Connection server;      
    server.bind( port );   // open the port  

    cerr << "Start server at port " << port << endl;
    server.waitForNewConnection(message,MAX_SIZE);      // wait for a client
    while( server.receiveMessage( message,MAX_SIZE ) )  // recv messages
    {
      cerr << "recv: " << message << endl;              // and print them
    }
    cerr << "end server" << endl;                       // never reached
  }
  else
  {
    Connection client;
    int ret = client.connect( host, port );             // connect
    int i = 0;
    while( 1 )
    {
      sprintf( message ,"message nr %d", i++ );         // send messages
      printf( "send: %s, return %d\n", message, client.sendMessage(message));
      usleep( 100 * 1000 );
    }    
  }

  return 0;
}



*/
