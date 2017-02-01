/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package agents;

import foraging.ForagerScenario;
import foraging.Map;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.SocketException;

/**
 * @author bluemoon
 */
public class AgentThread extends Thread {

    protected final int agentNumber;
    protected final int numberOfAgents;
    private String ip = "localhost";
    private int port = 3000;
    private OutputStream os;
    private InputStream is;

    public AgentThread(int n, int agents) {
        agentNumber = n;
        numberOfAgents = agents;
    }

    public void setParams(String i, int p) {
        ip = i;
        port = p;
    }

    public void connect() throws IOException {
        Socket sock = new Socket(ip, port);
        os = sock.getOutputStream();
        is = sock.getInputStream();
    }

    public void moveUp() throws IOException {
        os.write(ForagerScenario.UP);
        os.flush();
    }

    public void moveDown() throws IOException {
        os.write(ForagerScenario.DOWN);
        os.flush();
    }

    public void moveLeft() throws IOException {
        os.write(ForagerScenario.LEFT);
        os.flush();
    }

    public void moveRight() throws IOException {
        os.write(ForagerScenario.RIGHT);
        os.flush();
    }

    public void grab() throws IOException {
        os.write(ForagerScenario.GRAB);
        os.flush();
    }

    public void release() throws IOException {
        os.write(ForagerScenario.RELEASE);
        os.flush();
    }

    public void doNothing() throws IOException {
        os.write(ForagerScenario.NOTHING);
        os.flush();
    }

    public void doSomething(byte b) throws IOException {
        os.write(b);
        os.flush();
    }

    public byte[] readState() throws IOException {
        byte[] bytes = new byte[numberOfAgents + ForagerScenario.readX * ForagerScenario.readY];
        int r = is.read(bytes);
        if (r == -1) throw new SocketException("Read -1 bytes from server");

        return bytes;
    }

    public void close(){
        //System.out.println("Agent "+agentNumber+" closing conn");
        try {
            os.close();
            is.close();
        }catch(IOException ex){
            //System.out.println("Agent "+agentNumber+" closed conn");
            ex.printStackTrace();
        }
    }

    public void printState(byte[] bytes) {
        for (int j = 0; j < ForagerScenario.readY; j++) {
            String temp = "";
            for (int i = 0; i < ForagerScenario.readX; i++) {
                temp += Map.printMapCell(bytes[numberOfAgents + i + j * ForagerScenario.readX]);
            }
            System.out.println(temp);
        }
    }
}
