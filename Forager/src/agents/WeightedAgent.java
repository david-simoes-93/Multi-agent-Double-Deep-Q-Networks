/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package agents;

import Jama.Matrix;
import foraging.ForagerScenario;
import foraging.Map;

import java.io.IOException;
import java.net.SocketException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * @author bluemoon
 */
public class WeightedAgent extends AgentThread {

    private static Object flag;
    private static int matrixRows;
    //double[] weights;
    Matrix weightsM;

    public WeightedAgent(int n, int agents, double[] m) {
        super(n, agents);
        matrixRows = numberOfAgents * ForagerScenario.NUMBEROFACTIONS + ForagerScenario.readX * ForagerScenario.readY * Map.NUMBEROFMAPTYPES;
        // agents + ForagerScenario.readX * ForagerScenario.readY

        ForagerScenario.logger.debug("Creating  agent with a " + m.length / matrixRows + " by " + matrixRows + " weight matrix");

        weightsM = new Matrix(m, matrixRows);
        flag = new Object();
    }

    public double[] copyFromByteArray(byte[] source) {
        double[] dest = new double[matrixRows];

        // split the actions into binary info
        for (int i = 0; i < ForagerScenario.NUMBEROFACTIONS; i++) {
            for (int j = 0; j < numberOfAgents; j++) {
                // if source == NOTHING, source == UP, etc
                dest[i * numberOfAgents + j] = source[j] == i ? 1 : 0;
            }
        }

        // split cell types into binary info
        int counter = numberOfAgents * ForagerScenario.NUMBEROFACTIONS;

        //check for EMPTY; has to be separate cuz there are no 1's in this type
        for (int j = 0; j < ForagerScenario.readX * ForagerScenario.readY; j++) {
            dest[counter + j] = source[numberOfAgents + j] == Map.EMPTY ? 1 : 0;
        }
        for (int i = 1; i < Map.NUMBEROFMAPTYPES; i++) {
            for (int j = 0; j < ForagerScenario.readX * ForagerScenario.readY; j++) {
                // if source & OBSTACLE !=0, source & BERRY != 0, etc etc
                dest[counter + i * ForagerScenario.readX * ForagerScenario.readY + j] = (source[numberOfAgents + j] & (int) Math.pow(2, i - 1)) != 0 ? 1 : 0;
            }
        }

        return dest;

        /*double[] dest = new double[matrixRows];
         for (int i = 0; i < source.length; i++) {
         dest[i] = source[i];
         }
         return dest;
         */
    }

    @Override
    public void run() {
        try {
            byte[] state;
            doNothing();

            while (true) {
                state = readState();
                ForagerScenario.logger.debug("Agent " + agentNumber + " read input");
                /*if (true) {
                 synchronized (flag) {
                 System.out.println("Agent " + agentNumber + ": ");
                 printState(state);
                 System.out.println();
                 }
                 }*/

                //int l = state.length;
                int a = ForagerScenario.NUMBEROFACTIONS;

                Matrix stateM = new Matrix(copyFromByteArray(state), 1);
                ForagerScenario.logger.debug("State: " + stateM.getRowDimension() + "*" + stateM.getColumnDimension());
                ForagerScenario.logger.debug("Weights: " + weightsM.getRowDimension() + "*" + weightsM.getColumnDimension());
                Matrix resultsM = stateM.times(weightsM);
                ForagerScenario.logger.debug("Results: " + resultsM.getRowDimension() + "*" + resultsM.getColumnDimension());
                double[] results = resultsM.getColumnPackedCopy();

                // make sure everything is positive
                double minVal = results[0];
                for (int i = 1; i < a; i++) {
                    if (results[i] < minVal) {
                        minVal = results[i];
                    }
                }
                if (minVal < 0) {
                    for (int i = 0; i < a; i++) {
                        results[i] -= minVal;
                    }
                }

                // normalize and build cumulative probability array
                double maxSum = 0;
                double[] sumResults = new double[a];
                for (int i = 0; i < a; i++) {
                    maxSum += results[i];
                }
                for (int i = 0; i < a; i++) {
                    results[i] /= maxSum;
                    sumResults[i] = results[i];
                    if (i > 0) {
                        sumResults[i] += sumResults[i - 1];
                    }
                }

                // pick action
                double choice = Math.random();
                for (byte i = 0; i < a; i++) {
                    if (choice < sumResults[i]) {
                        ForagerScenario.logger.debug("Agent " + agentNumber + " chose action " + i);
                        doSomething(i);
                        break;
                    }
                }
                ForagerScenario.logger.debug("Agent " + agentNumber + " ended cycle");
            }
        } catch (IOException ex) {
            //ex.printStackTrace();
            if (ex instanceof SocketException) {
                if (ForagerScenario.debug) {
                    ForagerScenario.logger.info("Agent " + agentNumber + " was terminated. " + ex.getMessage());
                }
            } else {
                Logger.getLogger(RandomAgent.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
}
