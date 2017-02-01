import java.net.*;
import java.io.*;

public class StringDatagramSocket
{
  private DatagramSocket g_socket      = null;
  private DatagramPacket g_packetSend  = null;
  private DatagramPacket g_packetRecv  = null; 
  private InetAddress    g_inetAddress = null;
  private int            g_iPort       = -1;
  private int            g_buf_size;
  
  // constructor
  public StringDatagramSocket( String host, int port )
    throws Exception
  {
    this( host, port, 1024 );
  }
  
  /** This constructor intializes all the datagram packages. */
  public StringDatagramSocket( String host, int port, int buffer_size )
    throws Exception
  {
    g_buf_size    = buffer_size;
    g_inetAddress = InetAddress.getByName( host );
    g_iPort       = port;
    g_packetSend  = new DatagramPacket( new byte[buffer_size],
                                        buffer_size,
                                        g_inetAddress,
                                        g_iPort );
    g_packetRecv  = new DatagramPacket( new byte[buffer_size],
                                             buffer_size );
    g_socket      = new DatagramSocket();
  }
  
  /** This method blocks till a message is received from the socket and
      returns it. */
  public synchronized String receive()
    throws IOException
  {
    g_socket.receive( g_packetRecv );          // get the message
    int len = g_packetRecv.getLength() ;       // get length msg
    g_packetRecv.setLength( g_buf_size );      // set length (is adjusted?!)
    return new String( g_packetRecv.getData(), 0, len ) ; // make string
  }
  
  /** This method sends a message. */
  public synchronized void send( String message )
    throws IOException
  {
    System.arraycopy( message.getBytes(), 0,             // copy msg to packet
                      g_packetSend.getData(), 0, message.length() );
    g_packetSend.setLength( message.length() );          // set the length
    g_socket.send( g_packetSend );                       // and send it
  }
  
  /** This method returns the total last received datagram package. */
  public DatagramPacket getReceivedPacket()
  {
    return( g_packetRecv );
  }
  
  /** This method sets the port of the datagram packet that is sent. */
  public void setPort( int port )
  {
    g_iPort = port;
    g_packetSend.setPort( port );
  }
  
  /** This method closes the socket connection. */
  public void close()
    throws IOException
  {
    g_socket.close();
  }
  
}  
