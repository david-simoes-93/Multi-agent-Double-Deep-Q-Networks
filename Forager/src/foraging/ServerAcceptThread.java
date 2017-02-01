/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package foraging;

import berryTypes.BerryType;
import mapTypes.MapType;

import java.io.IOException;
import java.net.ServerSocket;
import java.util.ArrayList;
import java.util.Random;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * @author bluemoon
 */
public class ServerAcceptThread extends Thread {

    private final ArrayList<ServerThread> serverThreads;
    private final ServerSocket sock;
    AtomicInteger semaphore;
    Map map;
    public final int port;
    private final Random r;

    public ServerAcceptThread(int n, ArrayList<ServerThread> serverThreads, Random r) throws IOException {
        sock = new ServerSocket(n);
        port = sock.getLocalPort();

        this.serverThreads=serverThreads;
        semaphore = new AtomicInteger();
        this.r=r;
    }

    public ServerAcceptThread(ArrayList<ServerThread> serverThreads, Random r) throws IOException {
        port = 3000;
        sock = new ServerSocket(port);
        this.serverThreads=serverThreads;
        semaphore = new AtomicInteger();
        this.r=r;
    }

    public void prepareMap(int n, int b, boolean l, MapType obs, BerryType ber) {
        map = new Map(ForagerScenario.mapX, ForagerScenario.mapY, n, b, l, obs, ber);
    }

    @Override
    public void run() {
        synchronized(semaphore) {
            while (semaphore.get() < map.getNumberOfAgents()) {
                while (!map.placeAgent(semaphore.get(), r.nextInt(ForagerScenario.mapX),
                        r.nextInt((ForagerScenario.readY-2)/2) + ForagerScenario.readY/2+1));
                try {
                    ServerThread st = new ServerThread(sock.accept(), semaphore, map);
                    serverThreads.add(st);
                    st.start();
                    semaphore.addAndGet(1);
                } catch (IOException ex) {
                    ForagerScenario.logger.error(ex.getMessage());
                    System.gc();
                }

            }
        }
        try {
            sock.close();
        } catch (IOException ex) {
            ForagerScenario.logger.error(ex.getMessage());
        }
    }
}
