/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package agents;

import foraging.ForagerScenario;
import foraging.Map;

import java.io.IOException;
import java.net.SocketException;

/**
 * @author bluemoon
 */
public class RandomAgent extends AgentThread {

    private static Object flag;

    public RandomAgent(int n, int agents) {
        super(n, agents);
        flag = new Object();
    }

    @Override
    public void run() {
        try {
            byte[] state;
            doNothing();

            while (true) {
                ForagerScenario.logger.debug("\tAgent " + agentNumber + " starting cycle");

                state = readState();
                if (ForagerScenario.debug) {
                    synchronized (flag) {
                        System.out.println("Agent " + agentNumber + ": ");
                        printState(state);
                        System.out.println();
                    }
                }
                ForagerScenario.logger.debug("\tAgent " + agentNumber + " readState");

                int minBerryDistance = Integer.MAX_VALUE;
                int[] closestBerryPosition = {0, 0};
                for (int j = 0; j < ForagerScenario.readY; j++) {
                    for (int i = 0; i < ForagerScenario.readX; i++) {
                        if ((state[numberOfAgents + i + j * ForagerScenario.readX] & Map.BERRY) != 0) {
                            int dist = Math.abs(j - ForagerScenario.readY / 2) + Math.abs(i - ForagerScenario.readX / 2);
                            if (dist < minBerryDistance) {
                                minBerryDistance = dist;
                                closestBerryPosition[0] = i - ForagerScenario.readX / 2;
                                closestBerryPosition[1] = j - ForagerScenario.readY / 2;
                            }
                        }
                    }
                }

                byte myCell = state[numberOfAgents + ForagerScenario.readX / 2 + (ForagerScenario.readY / 2) * ForagerScenario.readX];
                if ((myCell & Map.BASE) != 0 && (myCell & Map.CARRYING) != 0) {
                    //at base
                    release();
                } else if ((myCell & Map.CARRYING) != 0) {
                    //move to base
                    double r = Math.random();
                    if ((state[numberOfAgents + ForagerScenario.readX / 2 + ((ForagerScenario.readY / 2) + 1) * ForagerScenario.readX] & Map.OBSTACLE) == 0) {
                        moveDown();
                    } else if (r < 0.5) {
                        moveLeft();
                    } else {
                        moveRight();
                    }
                } else if (minBerryDistance == 0) {
                    grab();
                } else if (minBerryDistance != Integer.MAX_VALUE) {
                    //move towards berry
                    if (Math.abs(closestBerryPosition[0]) > Math.abs(closestBerryPosition[1])) {
                        if (closestBerryPosition[0] < 0) {
                            if ((state[numberOfAgents + (ForagerScenario.readX / 2) - 1 + (ForagerScenario.readY / 2) * ForagerScenario.readX] & Map.OBSTACLE) == 0) {
                                moveLeft();
                            } else {
                                moveUp();
                            }
                        } else {
                            if ((state[numberOfAgents + (ForagerScenario.readX / 2) + 1 + (ForagerScenario.readY / 2) * ForagerScenario.readX] & Map.OBSTACLE) == 0) {
                                moveRight();
                            } else {
                                moveUp();
                            }
                        }
                    } else {
                        if (closestBerryPosition[1] < 0) {
                            moveUp();
                        } else {
                            moveDown();
                        }
                    }
                } else {
                    double r = Math.random();
                    if (r < 0.25) {
                        moveUp();
                    } else if (r < 0.5) {
                        moveDown();
                    } else if (r < 0.75) {
                        moveLeft();
                    } else {
                        moveRight();
                    }
                }

                ForagerScenario.logger.debug("\tAgent " + agentNumber + " moved");
            }
        } catch (IOException ex) {
            if (ex instanceof SocketException) {
                ForagerScenario.logger.debug("Agent " + agentNumber + " was terminated. " + ex.getMessage());
            } else {
                ForagerScenario.logger.error("Agent " + agentNumber + " was terminated. " + ex.getMessage());
            }
        }
    }
}
