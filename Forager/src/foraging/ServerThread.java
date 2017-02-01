/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package foraging;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * @author bluemoon
 */
public class ServerThread extends Thread {

    private final AtomicInteger semaphore;
    private final OutputStream os;
    private final InputStream is;
    private final Map map;
    private final int agentNumber;

    public ServerThread(Socket s, AtomicInteger i, Map m) throws IOException {
        os = s.getOutputStream();
        is = s.getInputStream();
        semaphore = i;
        map = m;
        agentNumber = i.get();
    }

    public void close(){
        //System.out.println("Closing server thread for agent "+agentNumber);
        try {
            os.close();
            is.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run() {
        try {
            while (true) {
                // read 1 damn byte
                int b = is.read(map.getAgentOutputs(), agentNumber, 1);
                if (b == -1) {
                    ForagerScenario.logger.error("Agent " + agentNumber + " read -1");
                    break;
                }
                ForagerScenario.logger.debug("Agent " + agentNumber + " sent output");

                synchronized (semaphore) {
                    semaphore.addAndGet(-1);

                    if (semaphore.get() == 0) {
                        if (ForagerScenario.debug) {
                            map.printMap();
                            Thread.sleep(100);
                        }

                        map.calculateAgentOutputsByPosition();

                        ForagerScenario.logger.debug("Server thread for agent " + agentNumber + " notified everyone");
                        semaphore.notifyAll();
                        semaphore.addAndGet(map.getNumberOfAgents());
                    } else {
                        semaphore.wait();
                    }
                    ForagerScenario.logger.debug("Server thread for agent " + agentNumber + " woke up at turn " + ForagerScenario.turnCounter);
                }

                int[] pos = map.getAgentPosition(agentNumber);
                switch (map.getAgentOutputs()[agentNumber]) {
                    case ForagerScenario.UP:
                        if (map.isTraversable(pos[0], pos[1] - 1) && Math.random() < ForagerScenario.moveChance
                                && map.numberOfAgentsMovingToCell(pos[0], pos[1] - 1) == 1) {
                            map.moveAgent(agentNumber, pos[0], pos[1] - 1);
                        }
                        break;
                    case ForagerScenario.DOWN:
                        if (map.isTraversable(pos[0], pos[1] + 1) && Math.random() < ForagerScenario.moveChance
                                && map.numberOfAgentsMovingToCell(pos[0], pos[1] + 1) == 1) {
                            map.moveAgent(agentNumber, pos[0], pos[1] + 1);
                        }
                        break;
                    case ForagerScenario.LEFT:
                        if (map.isTraversable(pos[0] - 1, pos[1]) && Math.random() < ForagerScenario.moveChance
                                && map.numberOfAgentsMovingToCell(pos[0] - 1, pos[1]) == 1) {
                            map.moveAgent(agentNumber, pos[0] - 1, pos[1]);
                        }
                        break;
                    case ForagerScenario.RIGHT:
                        if (map.isTraversable(pos[0] + 1, pos[1]) && Math.random() < ForagerScenario.moveChance
                                && map.numberOfAgentsMovingToCell(pos[0] + 1, pos[1]) == 1) {
                            map.moveAgent(agentNumber, pos[0] + 1, pos[1]);
                        }
                        break;
                    case ForagerScenario.GRAB:
                        if ((map.getMap()[pos[0]][pos[1]] & Map.BERRY) != 0 &&
                                (map.getMap()[pos[0]][pos[1]] & Map.CARRYING) == 0) {
                            map.getMap()[pos[0]][pos[1]] = Map.CARRYING;
                            ForagerScenario.berryGrabbedCounter.addAndGet(1);
                            ForagerScenario.retVals[agentNumber]+=ForagerScenario.turnMax;
                        }
                        break;
                    case ForagerScenario.RELEASE:
                        if ((map.getMap()[pos[0]][pos[1]] & Map.CARRYING) != 0 &&
                                (map.getMap()[pos[0]][pos[1]] & Map.BASE) != 0) {
                            map.getMap()[pos[0]][pos[1]] = Map.AGENT | Map.BASE;
                            ForagerScenario.berryReleasedCounter.addAndGet(1);
                            ForagerScenario.retVals[agentNumber]+=ForagerScenario.turnMax*2;
                        }
                        break;
                    case ForagerScenario.NOTHING:
                        break;
                    default:
                        ForagerScenario.logger.info("Agent " + agentNumber + " sent trash: " + map.getAgentOutputs()[agentNumber]);
                        break;
                }
                ForagerScenario.logger.debug("Agent " + agentNumber + " moved");

                synchronized (semaphore) {
                    semaphore.addAndGet(-1);

                    if (semaphore.get() == 0) {
                        if (ForagerScenario.debug) {
                            map.printMap();
                            Thread.sleep(100);
                        }
                        ForagerScenario.turnCounter++;

                        ForagerScenario.logger.debug("Server thread for agent " + agentNumber + " notified everyone");
                        semaphore.notifyAll();
                        semaphore.addAndGet(map.getNumberOfAgents());

                        // assume 10k turns: penalty starts at 310k and goes down to 10k (if all berries were caught in the last turn) or less (if faster)
                        ForagerScenario.retVal = ForagerScenario.turnCounter + 3 * map.getTotalNumberOfBerries() * ForagerScenario.turnMax;
                        ForagerScenario.retVal -= (ForagerScenario.berryReleasedCounter.get() * 2 + ForagerScenario.berryGrabbedCounter.get()) * ForagerScenario.turnMax;

                        if (ForagerScenario.berryReleasedCounter.get() == map.getTotalNumberOfBerries() || ForagerScenario.turnCounter > ForagerScenario.turnMax) {
                            ForagerScenario.simulationOver = 1;
                        }
                        //System.out.println("retval:"+ForagerScenario.retVal);

                    } else {
                        semaphore.wait();
                    }
                    ForagerScenario.logger.debug("Server thread for agent " + agentNumber + " woke up at turn " + ForagerScenario.turnCounter);
                }

                os.write(map.getState(agentNumber));
                os.flush();
                ForagerScenario.logger.debug("Agent " + agentNumber + " got input");

                if (ForagerScenario.simulationOver != 0) {
                    ForagerScenario.logger.info("Closing server thread for agent " + agentNumber + " since " + ForagerScenario.berryReleasedCounter.get() + "==" + map.getTotalNumberOfBerries()
                            + " or " + ForagerScenario.turnCounter + ">" + ForagerScenario.turnMax);
                    close();

                    break;
                } else {
                    ForagerScenario.logger.debug("Server thread for agent " + agentNumber + " compared " + ForagerScenario.berryReleasedCounter.get() + " with " + map.getTotalNumberOfBerries());
                }
            }
        } catch (IOException ex) {
            ForagerScenario.logger.error("Server thread for agent " + agentNumber + " was terminated. " + ex.getMessage());
        } catch (InterruptedException ex) {
            ForagerScenario.logger.error("Server thread for agent " + agentNumber + " was terminated. " + ex.getMessage());
        }
        //System.out.println("Server thread closed");
    }
}
