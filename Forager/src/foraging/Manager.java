package foraging;

import berryTypes.LeftRightBerries;
import berryTypes.RandomBerries;
import mapTypes.HType;

import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.util.Random;

/**
 * Created by david on 15-11-2016.
 */
public class Manager {
    public static void main(String [] args) throws IOException, InterruptedException {
        int port=2000;
        for(int i=0; i<args.length; i++){
            switch(args[i]){
                case "-p":
                case "-port":
                    port=Integer.parseInt(args[i+1]);
                    i++;
                    break;
                case "-help":
                    System.out.println("java -jar ForagerFramework.jar -p 2000");
                    return;
                default:
                    System.out.println("Unrecognized parameter: "+args[i]);
                    break;
            }
        }

        int size=7;
        ForagerScenario.readX=size;
        ForagerScenario.readY=size;
        ForagerScenario.mapX=size;
        ForagerScenario.mapY=size;
        ForagerScenario.numberOfBerries=2;
        //ForagerScenario.jal=false;
        //ForagerScenario.independent_rewards=true;
        int noa = 2;

        ServerSocket server = new ServerSocket(port);
        while(true){
            System.out.println("Awaiting connection");
            Socket sock = server.accept();

            InputStreamReader reader = new InputStreamReader(sock.getInputStream());
            //PrintWriter writer = new PrintWriter(sock.getOutputStream(), true);
            OutputStream os = sock.getOutputStream();
            char[] command = new char[1024];
            ForagerScenario fs = null;
            Random testRandom = new Random();
            while(!sock.isClosed() && sock.isConnected()){
                int bytes_read = reader.read(command);
                String command2 = new String(command).substring(0,3);


                //System.out.println("Read: "+command2);
                switch(command2){
                    case "rwd": // reward
                        if(fs!=null) {
                            if(!ForagerScenario.independent_rewards)
                                os.write(ByteBuffer.allocate(4).putInt(ForagerScenario.retVal).array());
                            else{
                                ByteBuffer bb = ByteBuffer.allocate(4*noa);
                                for(int i=0; i<noa; i++)
                                    bb.putInt(ForagerScenario.retVals[i]);
                                //System.out.println(bb.array()+" "+bb.getInt(0)+" "+bb.getInt(4));
                                os.write(bb.array());
                            }
                            //System.out.println(fs.retVal);
                        }else {
                            os.write(ByteBuffer.allocate(4).putInt(-1).array());
                            //System.out.println(-1);
                        }
                        break;
                    case "trm": // is terminal
                        if(fs!=null) {
                            if (ForagerScenario.simulationOver == 0) {
                                os.write(0);
                                //System.out.println(0);
                            }else {
                                //if(!testMode)
                                //    System.out.println("1 - "+ForagerScenario.berryReleasedCounter.get());
                                os.write(1);
                                //System.out.println(1);
                            }
                        }else {
                            os.write(-1);
                            //System.out.println(-1);
                        }
                        break;
                    case "new": // Create new scenario with 2 agents
                        if(bytes_read>3) {
                            String [] command_args = new String(command).substring(3).trim().split("-");
                            ForagerScenario.numberOfBerries = Integer.parseInt(command_args[0]);
                            if(command_args.length>1){
                                noa = Integer.parseInt(command_args[1]);
                            }
                        }

                        fs = new ForagerScenario("localhost", false, ForagerScenario.readX, ForagerScenario.readY,
                                ForagerScenario.mapX, ForagerScenario.mapY, ForagerScenario.numberOfBerries,
                                new HType(), new RandomBerries(), new Random(), noa);
                        fs.run();
                        os.write(ByteBuffer.allocate(4).putInt(fs.port).array());
                        System.out.println("New @ "+fs.port+" with "+noa+" agents and "+ForagerScenario.numberOfBerries+" berries");
                        break;
                    case "tst": // Create new test scenario with 2 agents
                        if(bytes_read>3) {
                            String [] command_args = new String(command).substring(3).trim().split("-");
                            ForagerScenario.numberOfBerries = Integer.parseInt(command_args[0]);
                            if(command_args.length>1){
                                noa = Integer.parseInt(command_args[1]);
                            }
                        }

                        fs = new ForagerScenario("localhost", false, ForagerScenario.readX, ForagerScenario.readY,
                                ForagerScenario.mapX, ForagerScenario.mapY, ForagerScenario.numberOfBerries,
                                new HType(), new LeftRightBerries(), testRandom, noa);
                        fs.run();
                        os.write(ByteBuffer.allocate(4).putInt(fs.port).array());
                        System.out.println("Test @ "+fs.port+" with "+noa+" agents and "+ForagerScenario.numberOfBerries+" berries");
                        break;
                    case "end":
                        sock.close();
                        break;
                    default:
                        System.out.println("Got: "+command2);
                        os.write(-1);
                        break;
                }
                os.flush();
            }
        }
    }
}
