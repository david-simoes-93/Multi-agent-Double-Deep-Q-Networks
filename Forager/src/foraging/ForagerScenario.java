/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package foraging;

import agents.AgentThread;
import berryTypes.BerryType;
import berryTypes.RandomBerries;
import mapTypes.HType;
import mapTypes.MapType;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Random;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * @author bluemoon
 */
public class ForagerScenario implements Runnable {

    public static final byte NOTHING = 0, UP = 1, DOWN = 2, LEFT = 3, RIGHT = 4, GRAB = 5, RELEASE = 6, NUMBEROFACTIONS = 7;
    public static final double moveChance = 1;
    public static final boolean debug = false;
    public static final Logger logger = LogManager.getLogger("FS");
    public static int readX = 11, readY = 11, mapX = 20, mapY = 20;
    //public static boolean jal=true;
    public static boolean independent_rewards=false;
    public final MapType obs;
    public final BerryType ber;
    public static AtomicInteger berryGrabbedCounter, berryReleasedCounter;
    public static volatile int retVal, turnCounter, simulationOver;
    public static volatile int[] retVals;
    public static int turnMax = 205, numberOfBerries = 10;
    public final boolean localPerspective;

    private final String ip;
    public final int port;

    private final ArrayList<AgentThread> agents;
    private final int presetNumberOfAgents;
    private final ArrayList<ServerThread> serverThreads;
    private final ServerAcceptThread server;

    private boolean doneInit;
    private final Random r;

    public ArrayList<ServerThread> getServerThreads(){
        return serverThreads;
    }

    public ForagerScenario() throws IOException {
        doneInit=false;

        r=new Random();
        agents = new ArrayList();
        presetNumberOfAgents=-1;
        serverThreads = new ArrayList();
        server = new ServerAcceptThread(serverThreads, r);
        berryGrabbedCounter = new AtomicInteger();
        berryReleasedCounter = new AtomicInteger();

        ip = "localhost";
        port = 3000;

        retVal = 0;
        turnCounter = 0;
        simulationOver = 0;
        localPerspective = true;

        this.obs = new HType();
        this.ber = new RandomBerries();
    }

    public boolean isInitDone(){
        return doneInit;
    }

    public ForagerScenario(String i, int p, boolean local, int rX, int rY, int mX, int mY, int nOB,
                           MapType obs, BerryType ber, Random r) throws IOException {
        readX = rX;
        readY = rY;
        mapX = mX;
        mapY = mY;
        numberOfBerries = nOB;

        this.r=r;

        agents = new ArrayList();
        presetNumberOfAgents=-1;
        serverThreads = new ArrayList();
        server = new ServerAcceptThread(p, serverThreads, r);
        berryGrabbedCounter = new AtomicInteger();
        berryReleasedCounter = new AtomicInteger();

        ip = i;
        port = p;

        retVal = 0;
        turnCounter = 0;
        simulationOver = 0;

        localPerspective = local;

        this.obs=obs;
        this.ber=ber;
    }

    // random port, specify number of agents
    public ForagerScenario(String i, boolean local, int rX, int rY, int mX, int mY, int nOB,
                           MapType obs, BerryType ber, Random r, int nOA) throws IOException {
        readX = rX;
        readY = rY;
        mapX = mX;
        mapY = mY;
        numberOfBerries = nOB;

        this.r=r;

        agents = null;
        presetNumberOfAgents=nOA;
        serverThreads = new ArrayList();
        server = new ServerAcceptThread(0, serverThreads, r);
        berryGrabbedCounter = new AtomicInteger();
        berryReleasedCounter = new AtomicInteger();

        ip = i;
        port = server.port;

        retVal = 0;
        turnCounter = 0;
        simulationOver = 0;

        localPerspective = local;

        this.obs=obs;
        this.ber=ber;
    }

    public static String actionName(int a) {
        switch (a) {
            case NOTHING:
                return "Nothing";
            case UP:
                return "Up";
            case DOWN:
                return "Down";
            case LEFT:
                return "Left";
            case RIGHT:
                return "Right";
            case GRAB:
                return "Grab";
            case RELEASE:
                return "Release";
            default:
                return "Unknown";
        }
    }

    public void addAgent(AgentThread t) {
        agents.add(t);
    }

    public boolean remAgent(AgentThread t) {
        return agents.remove(t);
    }

    public void run() {
        try {
            if (agents != null) {
                retVals = new int[agents.size()];
                server.prepareMap(agents.size(), numberOfBerries, localPerspective, obs, ber);
            }else {
                retVals = new int[presetNumberOfAgents];
                server.prepareMap(presetNumberOfAgents, numberOfBerries, localPerspective, obs, ber);
            }

            server.start();
            logger.info("Started server");
            Thread.sleep(100);

            if (agents != null) {
                for (AgentThread t : agents) {
                    t.setParams(ip, port);
                    t.connect();
                }
                logger.info("Connected agents");
                Thread.sleep(100);

                for (AgentThread t : agents) {
                    t.start();
                }
            }
            doneInit=true;

            if (agents != null) {
                logger.info("Started agents");
                Thread.sleep(100);

                for (AgentThread t : agents) {
                    t.join();
                }
            }

        }catch(Exception ex){
            ex.printStackTrace();
        }
    }
}
