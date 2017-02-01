#!/usr/bin/python

from socket import *
import string
import random
import math

# MAIN CLASS
class Predator:

    sock = None

    # processes the incoming visualization messages from the server
    def processVisualInformation( self, msg ):
        
        if '(see)' in msg:
            return
        # strip the '(see ' and the ')'
        msg = msg[6:-3]
        observations = msg.split(') (')
        for o in observations:
            print('observation: ', o)
            #(obj, x, y) = string.split(o, " ")
            #print obj + " seen at (" + x + ", " + y + ")"
            # implementation should be done by students            
            # TODO: process these relative x and y coordinates

    # determines the next movement command for this agent
    def determineMovementCommand( self ):

        rand = random.randint(0, 4)                   
        if(rand == 0):
          msg = "(move south)"
        elif(rand == 1):
          msg = "(move north)"
        elif(rand == 2):
          msg = "(move west)"
        elif(rand == 3):
          msg = "(move east)"
        elif(rand == 4):
          msg = "(move none)"

        return msg

    # determine a communication message 
    def determineCommunicationCommand( self ):
        # TODO: Assignment 3
        return ""

    # process the incoming visualization messages from the server   
    def processCommunicationInformation( self, str ):
        # TODO: Assignment 3
        pass

    def processEpisodeEnded( self ):
       # TODO: initialize used variables (if any)
       pass
       
    def processReposition( self ):
       # TODO: is called when predator collided or penalized
       pass

    # BELOW ARE METODS TO CALL APPROPRIATE METHODS; CAN BE KEPT UNCHANGED
    def connect( self, host='', port=4001 ):
        self.sock = socket( AF_INET, SOCK_DGRAM)                  
        self.sock.bind( ( '', 0 ) )                               
        self.sock.sendto( "(init predator)".encode('utf-8') , (host, port ) )       
        pass
  
    def mainLoop( self ):
        msg, addr = self.sock.recvfrom( 1024 )                    
        self.sock.connect( addr )                                 
        ret = 1
        while ret:
            #print('msg:',msg)
            msg = self.sock.recv( 1024 )  
            #msg = bytes(msg, 'utf-8') 
            msg = str(msg) 
            print('msg:',msg,type(msg))                        
            if '(quit' in msg:
                # quit message
                ret = 0                                       

            elif '(hear' in msg :
                # process audio
                self.processCommunicationInformation( msg )

            elif '(see' in msg :
                # process visual
                self.processVisualInformation( msg )

                msg = self.determineCommunicationCommand( )
                #if len(msg) > 0:
                self.sock.send( msg.encode('utf-8') )
        
            elif '(send_action' in msg :  
                msg = self.determineMovementCommand( )
                self.sock.send( msg.encode('utf-8') )           

            elif '(referee episode_ended)' in msg:  
                msg = self.processEpisodeEnded( )
         
            elif '(referee collision)' in msg:  
                msg = self.processReposition( )
                
            else:
                print("msg not understood " + msg)
            print('msg end')
        self.sock.close()                                         
        pass
  

if __name__ == "__main__":

    predator = Predator()
    predator.connect()
    predator.mainLoop()
