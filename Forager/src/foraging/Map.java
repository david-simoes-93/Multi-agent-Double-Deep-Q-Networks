/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package foraging;

import berryTypes.BerryType;
import mapTypes.MapType;

/**
 * @author bluemoon
 */
public class Map {

    public final static byte EMPTY = 0x00, OBSTACLE = 0x01, BERRY = 0x02, AGENT = 0x04,
            UNKNOWN = 0x08, BASE = 0x10, CARRYING = 0x20, OTHERAGENT = 0x40, NUMBEROFMAPTYPES = 8;
    private final byte[][] map;
    private final byte[] agentOutputs, agentOutputsByPosition;
    private final int[][] agentPositions;
    private final int numberOfAgents, numberOfBerries;
    private final boolean localPerspective;


    public Map(int x, int y, int n, int b, boolean l, MapType obstacles, BerryType berries) {
        map = new byte[x][y];
        numberOfAgents = n;
        numberOfBerries = b;
        agentOutputs = new byte[numberOfAgents];
        agentOutputsByPosition = new byte[numberOfAgents];
        agentPositions = new int[numberOfAgents][2];
        localPerspective = l;

        obstacles.fillObstacles(map);
        berries.fillBerries(map, b);
    }

    public static String printMapCell(byte b) {

        if ((b & Map.OBSTACLE) != 0) {
            return "X";
        } else if ((b & Map.OTHERAGENT) != 0) {
            return "=";
        } else if ((b & Map.AGENT) != 0 && (b & Map.OTHERAGENT) != 0 && (b & Map.BERRY) != 0) {
            return "Ü";
        } else if ((b & Map.AGENT) != 0 && (b & Map.BERRY) != 0) {
            return "Ö";
        } else if ((b & Map.CARRYING) != 0 && (b & Map.OTHERAGENT) != 0 && (b & Map.BERRY) != 0) {
            return "ü";
        } else if ((b & Map.CARRYING) != 0 && (b & Map.BERRY) != 0) {
            return "ö";
        } else if ((b & Map.BERRY) != 0) {
            return "O";
        } else if ((b & Map.UNKNOWN) != 0) {
            return " ";
        } else if ((b & Map.BASE) != 0 && (b & Map.AGENT) != 0 && (b & Map.OTHERAGENT) != 0) {
            return "Ä";
        } else if ((b & Map.BASE) != 0 && (b & Map.AGENT) != 0) {
            return "Ë";
        } else if ((b & Map.BASE) != 0 && (b & Map.CARRYING) != 0 && (b & Map.OTHERAGENT) != 0) {
            return "ä";
        } else if ((b & Map.BASE) != 0 && (b & Map.CARRYING) != 0) {
            return "ë";
        } else if ((b & Map.BASE) != 0) {
            return "E";
        } else if ((b & Map.CARRYING) != 0 && (b & Map.OTHERAGENT) != 0) {
            return "u";
        } else if ((b & Map.CARRYING) != 0) {
            return "h";
        } else if ((b & Map.AGENT) != 0 && (b & Map.OTHERAGENT) != 0) {
            return "U";
        } else if ((b & Map.AGENT) != 0) {
            return "H";
        } else {
            return "_";
        }
    }

    public static byte printMapCell2(char c) {
        switch (c) {
            case 'X':
                return Map.OBSTACLE;
            case 'H':
                return Map.AGENT;
            case 'O':
                return Map.BERRY;
            case ' ':
                return Map.UNKNOWN;
            case 'E':
                return Map.BASE;
            case 'h':
                return Map.CARRYING;
            case 'Ë':
                return Map.BASE | Map.AGENT;
            case 'ë':
                return Map.BASE | Map.CARRYING;
            case 'Ö':
                return Map.BERRY | Map.AGENT;
            case 'ö':
                return Map.BERRY | Map.CARRYING;
            default: // '_'
                return Map.EMPTY;
        }
    }

    public int numberOfAgentsMovingToCell(int x, int y) {
        int counter = 0;
        for (int i = 0; i < numberOfAgents; i++) {
            if (agentPositions[i][0] == x && agentPositions[i][1] == y + 1 && agentOutputs[i] == ForagerScenario.UP) {
                counter++;
            } else if (agentPositions[i][0] == x && agentPositions[i][1] == y - 1 && agentOutputs[i] == ForagerScenario.DOWN) {
                counter++;
            } else if (agentPositions[i][0] == x - 1 && agentPositions[i][1] == y && agentOutputs[i] == ForagerScenario.RIGHT) {
                counter++;
            } else if (agentPositions[i][0] == x + 1 && agentPositions[i][1] == y && agentOutputs[i] == ForagerScenario.LEFT) {
                counter++;
            }
        }
        return counter;
    }

    public int getTotalNumberOfBerries() {
        return numberOfBerries;
    }

    // returns what the agent sees
    public byte[] getState(int n) {

        byte[] state = new byte[numberOfAgents + ForagerScenario.readX * ForagerScenario.readY];
        if(localPerspective) {
            System.arraycopy(agentOutputs, 0, state, 0, numberOfAgents);
        }else{
            System.arraycopy(agentOutputsByPosition, 0, state, 0, numberOfAgents);
        }
        int counter = 0;

        if (localPerspective) {
            int[] pos = getAgentPosition(n);
            for (int j = pos[1] - ForagerScenario.readY / 2; j <= pos[1] + ForagerScenario.readY / 2; j++) {
                for (int i = pos[0] - ForagerScenario.readX / 2; i <= pos[0] + ForagerScenario.readX / 2; i++) {
                    if (i < 0 || i >= ForagerScenario.mapX || j < 0 || j >= ForagerScenario.mapY) {
                        state[counter + numberOfAgents] = UNKNOWN;
                    } else {
                        state[counter + numberOfAgents] = map[i][j];
                    }
                    counter++;
                }
            }
        } else {
            for (int j = 0; j < ForagerScenario.readY; j++) {
                for (int i = 0; i < ForagerScenario.readX; i++) {
                    state[counter + numberOfAgents] = map[i][j];
                    counter++;
                }
            }

            for (int i = 0; i < numberOfAgents; i++) {
                if (i == n) continue;
                state[numberOfAgents+agentPositions[i][0] + agentPositions[i][1] * ForagerScenario.readX] |= OTHERAGENT;
            }
        }

        /*synchronized(map) {
            System.out.print("Agent " + n + ": ");
            for (int i = 0; i < numberOfAgents; i++)
                System.out.print(ForagerScenario.actionName(state[i])+" ");
            System.out.println();
        }*/

        return state;
    }

    public String getAgentNumberAtPosition(int x, int y){
        for(int i=0; i<agentPositions.length; i++){
            if(x==agentPositions[i][0]&&y==agentPositions[i][1])
                return ""+i;
        }
        return ""+9;
    }

    public void printMap() {
        for (int j = 0; j < ForagerScenario.mapY; j++) {
            String temp = "";
            for (int i = 0; i < ForagerScenario.mapX; i++) {
                String s = printMapCell(map[i][j]);
                temp += s.equals("H") ? getAgentNumberAtPosition(i,j) : s;
            }
            ForagerScenario.logger.debug(temp);
        }
    }

    public boolean placeAgent(int n, int x, int y) {
        if (map[x][y] == EMPTY || map[x][y] == BERRY) {
            map[x][y] |= AGENT;
            agentPositions[n][0] = x;
            agentPositions[n][1] = y;
            return true;
        }
        return false;
    }

    public void moveAgent(int n, int x, int y) {
        int[] currPos = getAgentPosition(n);

        if ((map[currPos[0]][currPos[1]] & AGENT) != 0) {
            map[currPos[0]][currPos[1]] &= ~AGENT;
            map[x][y] |= AGENT;
        } else if ((map[currPos[0]][currPos[1]] & CARRYING) != 0) {
            map[currPos[0]][currPos[1]] &= ~CARRYING;
            map[x][y] |= CARRYING;
        }
        agentPositions[n][0] = x;
        agentPositions[n][1] = y;
    }

    public boolean placeAgent(int n) {
        int x = (int) (Math.random() * ForagerScenario.mapX);
        int y = (int) (Math.random() * (4 * ForagerScenario.mapY / 10.0) + (6 * ForagerScenario.mapY / 10.0));

        if (map[x][y] == EMPTY || map[x][y] == BERRY) {
            map[x][y] |= AGENT;
            agentPositions[n][0] = x;
            agentPositions[n][1] = y;
            return true;
        }
        return false;
    }


    public byte[][] getMap() {
        return map;
    }

    public byte[] getAgentOutputs() {
        return agentOutputs;
    }

    public void calculateAgentOutputsByPosition() {
        int counter=0;
        for (int j = 0; j < ForagerScenario.readY; j++) {
            for (int i = 0; i < ForagerScenario.readX; i++) {
                if((map[i][j]&AGENT)!=0 || (map[i][j]&CARRYING)!=0){
                    for(int agentNumber=0; agentNumber<numberOfAgents; agentNumber++){
                        if(agentPositions[agentNumber][0]==i && agentPositions[agentNumber][1]==j){
                            agentOutputsByPosition[counter]=agentOutputs[agentNumber];
                            counter++;
                            break;
                        }
                    }
                }
            }
        }
    }

    public int[] getAgentPosition(int n) {
        return agentPositions[n];
    }

    public int getNumberOfAgents() {
        return numberOfAgents;
    }

    public boolean isTraversable(int x, int y) {
        if (x < 0 || x >= ForagerScenario.mapX || y < 0 || y >= ForagerScenario.mapY) {
            return false;
        }

        return (map[x][y] == EMPTY || map[x][y] == BERRY || map[x][y] == BASE);
    }
}
