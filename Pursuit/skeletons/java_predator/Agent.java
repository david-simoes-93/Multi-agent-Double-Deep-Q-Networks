import java.net.*;
import java.io.*;
import java.util.*;

/** This class contains the interaction of the agent with the server. Depending
    on the message from the server the appropriate abstract methods are called.
    These should be defined in the subclasses. */
public abstract class Agent
{
  protected StringDatagramSocket g_socket    = null;
  protected boolean              g_connected = false;
  
  /** This method connects to the default host and port number. */
  public void connect()
  {
    connect( "localhost", 4001 );
  }

  /** This method connects to the specified host and port number. */  
  public void connect( String host, int port )
  {
    try
    {
      g_socket = new StringDatagramSocket( host, port ); 
      g_connected = true;
      mainLoop();
    }
    catch( Exception e )
    {
      e.printStackTrace();
    }
  }
  
  /** This is the main loop and handles the interaction with the server. It will
      not exit before the connection has been broken by the server. */
  protected void mainLoop()
    throws Exception
  {
    String strMessage = null;
    
    initialize();                        // initialize (abstract method)
    
    while( g_connected )
    {
      strMessage = g_socket.receive();

      if( strMessage.startsWith( "(init ok)" ) )
      {
        // server returns message from other port that should be just afterwards
        g_socket.setPort( g_socket.getReceivedPacket().getPort());
        System.out.println( "Connected to server..." );
      }
      else if( strMessage.startsWith( "(quit)" ) )
      {
        g_connected = false;
        System.out.println( "Server was stopped..." );
      }
      else if( strMessage.startsWith( "(send_action" ) )
      {
        String strMove = determineMovementCommand();       // abstract method
        if( !strMove.equals( "" ) )
        {
          g_socket.send( strMove );
        }
        else
          System.out.println( "Not moving..." );
      }
      else if( strMessage.startsWith( "(see" ) )
      {
        processVisualInformation( strMessage );
        String strComm = determineCommunicationCommand  ( );
        if( !strComm.equals( "" ) )
          g_socket.send( strComm );
        
      }
      else if( strMessage.startsWith( "(referee episode_ended" ) )
      {
        episodeEnded();
      }
      else if( strMessage.startsWith( "(referee collision)" ) )
      {
        collisionOccured();
      }
      else if( strMessage.startsWith(  "(referee penalize)" ) )
      {
        penalizeOccured();
      }
      else if( strMessage.startsWith(  "(referee prey_caught" ) )
      {
        preyCaught();
      }   
      else if( strMessage.startsWith( "(hear" ) )  
      {
        processCommunicationInformation( strMessage );  
      }    
    }
    
    g_socket.send( "(quit)" );
    g_socket.close();
  }
  
  // below are methods in which the functionality should be defined
  public abstract void   initialize() throws IOException;
  public abstract String determineMovementCommand       ( );
  public abstract void   processVisualInformation       ( String strMessage );
  public abstract void   processCommunicationInformation( String strComm );
  public abstract String determineCommunicationCommand  ( );
  public abstract void   episodeEnded                   ( );
  public abstract void   collisionOccured               ( );
  public abstract void   penalizeOccured                ( );
  public abstract void   preyCaught                     ( );

}
