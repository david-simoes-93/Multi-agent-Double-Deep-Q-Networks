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

/**
 * @author bluemoon
 */
public class SmartLinearWeightedAgent extends AgentThread {

    private static int inputLayerSize, hiddenLayer1Size, hiddenLayer2Size, hiddenInputSize, outputLayerSize;
    // a binary, 1-of-k encoding.
    Matrix weightsOutputLayer; //weightsHiddenLayer1, weightsHiddenLayer2,
    double[] weightsH1, weightsH2;

    public SmartLinearWeightedAgent(int n, int agents, double[] weightsH1, double[] weightsH2, double[] weightsO,
                                    int inputSize, int hiddenSize1, int hiddenSize2, int hiddenISize, int outputSize) {
        // We will get as inputs: berries (x*y), bases (x*y), obstacles (4), carry (1);
        // there will be 2 hidden layers that convert berries, bases and carry to targets (2*(x*y))
        // the output (#actions) is measured by obstacles + targets (3*(x*y))
        super(n, agents);

        outputLayerSize = outputSize;
        inputLayerSize = inputSize;
        hiddenLayer1Size = hiddenSize1;
        hiddenLayer2Size = hiddenSize2;
        hiddenInputSize = hiddenISize;

        this.weightsH1 = weightsH1;
        this.weightsH2 = weightsH2;

        ForagerScenario.logger.debug("Creating  agent with a " + hiddenLayer1Size + "/" + hiddenLayer2Size + " by (x*y)+1"
                + " hidden weight matrix and a " + outputLayerSize + " by " + (hiddenLayer1Size + hiddenLayer2Size) + "+something output weight matrix");

        //weightsHiddenLayer1 = new Matrix(weightsH1, hiddenLayer1Size);  //3 * 121
        //weightsHiddenLayer2 = new Matrix(weightsH2, hiddenLayer2Size);  //3 * 121
        weightsOutputLayer = new Matrix(weightsO, outputLayerSize).transpose();     //(121+121+4) * #actions
    }

    public static double sigmoid(double x) {
        return 1 / (1 + Math.exp(-x));
    }

    // returns an array of size inputLayerSize + 1 (which is the bias, always 1)
    // it represents the unfolded state input (preprocessed readings)
   /* public double[] copyFromByteArray(byte[] source) {
     double[] dest = new double[inputLayerSize + 1];

     // split the other agents' actions into binary info
     for (int i = 0; i < ForagerScenario.NUMBEROFACTIONS; i++) {
     for (int j = 0; j < numberOfAgents; j++) {
     if (j == agentNumber) {
     continue;
     }
     // if source == NOTHING, source == UP, etc
     dest[i * (numberOfAgents - 1) + j] = source[j] == i ? 1 : 0;
     }
     }

     // split cell types into binary info
     int counter = (numberOfAgents - 1) * ForagerScenario.NUMBEROFACTIONS;

     //check for EMPTY; has to be separate cuz there are no 1's in this type
     /*for (int j = 0; j < ForagerScenario.readX * ForagerScenario.readY; j++) {
     dest[counter + j] = source[numberOfAgents + j] == Map.EMPTY ? 1 : 0;
     }
     counter += ForagerScenario.readX * ForagerScenario.readY;* /
     dest[counter] = (source[numberOfAgents + ForagerScenario.readX / 2 - 1
     + ForagerScenario.readY / 2 * ForagerScenario.readX]
     & Map.OBSTACLE) != 0 ? 1 : 0;
     dest[counter + 1] = (source[numberOfAgents + ForagerScenario.readX / 2 + 1
     + ForagerScenario.readY / 2 * ForagerScenario.readX]
     & Map.OBSTACLE) != 0 ? 1 : 0;
     dest[counter + 2] = (source[numberOfAgents + ForagerScenario.readX / 2
     + (ForagerScenario.readY / 2 - 1) * ForagerScenario.readX]
     & Map.OBSTACLE) != 0 ? 1 : 0;
     dest[counter + 3] = (source[numberOfAgents + ForagerScenario.readX / 2
     + (ForagerScenario.readY / 2 + 1) * ForagerScenario.readX]
     & Map.OBSTACLE) != 0 ? 1 : 0;
     counter += 4;

     for (int j = 0; j < ForagerScenario.readX * ForagerScenario.readY; j++) {
     if ((source[numberOfAgents + j] & Map.BASE) != 0) {
     dest[counter + j] = 1;
     } else if ((source[numberOfAgents + j] & Map.BERRY) != 0) {
     dest[counter + j] = -1;
     }
     //dest[counter + j] = (source[numberOfAgents + j] & Map.OBSTACLE) != 0 ? 1 : 0;
     }
     counter += ForagerScenario.readX * ForagerScenario.readY;
     /* for (int j = 0; j < ForagerScenario.readX * ForagerScenario.readY; j++) {
     dest[counter + j] = (source[numberOfAgents + j] & Map.BERRY) != 0 ? 1 : 0;
     }
     counter += ForagerScenario.readX * ForagerScenario.readY;* /
     for (int j = 0; j < ForagerScenario.readX * ForagerScenario.readY; j++) {
     dest[counter + j] = (source[numberOfAgents + j] & Map.OBSTACLE) != 0 ? 1 : 0;
     }
     counter += ForagerScenario.readX * ForagerScenario.readY;
     /*for (int j = 0; j < ForagerScenario.readX * ForagerScenario.readY; j++) {
     dest[counter + j] = (source[numberOfAgents + j] & Map.AGENT) != 0 ? 1 : 0;
     }
     counter += ForagerScenario.readX * ForagerScenario.readY;* /
     dest[counter] = (source[numberOfAgents + ForagerScenario.readX * (ForagerScenario.readY / 2)
     + ForagerScenario.readX / 2] & Map.CARRYING) != 0 ? 1 : -1; // if carrying, 1 or -1

     counter++;
     dest[counter] = 1; // bias

     return dest;
     }*/
    public double[] getInputGroup1(byte[] source) {
        double[] dest = new double[hiddenLayer1Size * 3];

        int carry = (source[numberOfAgents + ForagerScenario.readX * (ForagerScenario.readY / 2)
                + ForagerScenario.readX / 2] & Map.CARRYING) != 0 ? 1 : -1;
        for (int j = 0; j < ForagerScenario.readX * ForagerScenario.readY; j++) {
            if ((source[numberOfAgents + j] & Map.BERRY) != 0) {
                dest[j * 3] = 1;
            }
            dest[j * 3 + 1] = carry;
            dest[j * 3 + 2] = 1; //bias
        }

        return dest;
    }

    public double[] getInputGroup2(byte[] source) {
        double[] dest = new double[hiddenLayer2Size * 3];

        int carry = (source[numberOfAgents + ForagerScenario.readX * (ForagerScenario.readY / 2)
                + ForagerScenario.readX / 2] & Map.CARRYING) != 0 ? 1 : -1;
        for (int j = 0; j < ForagerScenario.readX * ForagerScenario.readY; j++) {
            if ((source[numberOfAgents + j] & Map.BASE) != 0) {
                dest[j * 3] = 1;
            }
            dest[j * 3 + 1] = carry;
            dest[j * 3 + 2] = 1; //bias
        }

        return dest;
    }

    public double[] getInputGroup3(byte[] source) {
        double[] dest = new double[hiddenInputSize];

        dest[0] = (source[numberOfAgents + ForagerScenario.readX / 2 - 1
                + ForagerScenario.readY / 2 * ForagerScenario.readX]
                & Map.OBSTACLE) != 0 ? 1 : 0;
        dest[1] = (source[numberOfAgents + ForagerScenario.readX / 2 + 1
                + ForagerScenario.readY / 2 * ForagerScenario.readX]
                & Map.OBSTACLE) != 0 ? 1 : 0;
        dest[2] = (source[numberOfAgents + ForagerScenario.readX / 2
                + (ForagerScenario.readY / 2 - 1) * ForagerScenario.readX]
                & Map.OBSTACLE) != 0 ? 1 : 0;
        dest[3] = (source[numberOfAgents + ForagerScenario.readX / 2
                + (ForagerScenario.readY / 2 + 1) * ForagerScenario.readX]
                & Map.OBSTACLE) != 0 ? 1 : 0;

        return dest;
    }

    @Override
    public void run() {
        try {
            byte[] state;
            doNothing();

            while (true) {
                state = readState();
                ForagerScenario.logger.debug("Agent " + agentNumber + " read input");

                //int l = state.length;
                int a = ForagerScenario.NUMBEROFACTIONS;

                //Matrix stateM1 = new Matrix(getInputGroup1(state), 3); // 121 * 3
                double[] stateM1 = getInputGroup1(state);
                //ForagerScenario.logger.debug("1State: " + stateM1.getRowDimension() + "*" + stateM1.getColumnDimension());
                //ForagerScenario.logger.debug("1Weights: " + weightsHiddenLayer1.getRowDimension() + "*" + weightsHiddenLayer1.getColumnDimension());
                //Matrix hiddenLayer1 = weightsHiddenLayer1.times(stateM1);
                //ForagerScenario.logger.debug("1State (hidden): " + hiddenLayer1.getRowDimension() + "*" + hiddenLayer1.getColumnDimension());
                /*Matrix hiddenLayer1WithBias = new Matrix(hiddenLayer1.getRowDimension(), hiddenLayer1.getColumnDimension() + 1);
                 hiddenLayer1WithBias.setMatrix(0, 0, 0, hiddenLayer1Size - 1, hiddenLayer1);
                 hiddenLayer1WithBias.set(0, hiddenLayer1Size, 1);*/

                //Matrix stateM2 = new Matrix(getInputGroup2(state), 3);
                double[] stateM2 = getInputGroup2(state);
                //ForagerScenario.logger.debug("2State: " + stateM2.getRowDimension() + "*" + stateM2.getColumnDimension());
                //ForagerScenario.logger.debug("2Weights: " + weightsHiddenLayer2.getRowDimension() + "*" + weightsHiddenLayer2.getColumnDimension());
                //Matrix hiddenLayer2 = weightsHiddenLayer2.times(stateM2);
                //ForagerScenario.logger.debug("2State (hidden): " + hiddenLayer2.getRowDimension() + "*" + hiddenLayer2.getColumnDimension());
                /*Matrix hiddenLayer2WithBias = new Matrix(hiddenLayer2.getRowDimension(), hiddenLayer2.getColumnDimension() + 1);
                 hiddenLayer2WithBias.setMatrix(0, 0, 0, hiddenLayer2Size - 1, hiddenLayer2);
                 hiddenLayer2WithBias.set(0, hiddenLayer2Size, 1);*/

                Matrix stateM3 = new Matrix(getInputGroup3(state), hiddenInputSize);
                Matrix hiddenLayerWithBias = new Matrix(1, hiddenLayer1Size + hiddenLayer2Size + hiddenInputSize + 1);
                /*for (int i = 0; i < hiddenLayer1Size; i++) {
                    hiddenLayerWithBias.set(0, i, sigmoid(hiddenLayer1.get(i, i)));
                }
                for (int i = 0; i < hiddenLayer2Size; i++) {
                    hiddenLayerWithBias.set(0, hiddenLayer1Size + i, sigmoid(hiddenLayer2.get(i, i)));
                }*/

                for (int i = 0; i < hiddenLayer1Size; i++) {
                    double val = weightsH1[i * 3] * stateM1[i * 3] +
                            weightsH1[i * 3 + 1] * stateM1[i * 3 + 1] +
                            weightsH1[i * 3 + 2] * stateM1[i * 3 + 2];

                    hiddenLayerWithBias.set(0, i, sigmoid(val));
                }
                for (int i = 0; i < hiddenLayer2Size; i++) {
                    double val = weightsH2[i * 3] * stateM2[i * 3] +
                            weightsH2[i * 3 + 1] * stateM2[i * 3 + 1] +
                            weightsH2[i * 3 + 2] * stateM2[i * 3 + 2];

                    hiddenLayerWithBias.set(0, hiddenLayer1Size + i, sigmoid(val));
                }

                for (int i = 0; i < hiddenInputSize; i++) {
                    hiddenLayerWithBias.set(0, hiddenLayer1Size + hiddenLayer2Size + i, stateM3.get(i, 0));
                }
                //copy hidden1, hidden2 and state3

                ForagerScenario.logger.debug("OState: " + hiddenLayerWithBias.getRowDimension() + "*" + hiddenLayerWithBias.getColumnDimension());
                ForagerScenario.logger.debug("OWeights: " + weightsOutputLayer.getRowDimension() + "*" + weightsOutputLayer.getColumnDimension());
                Matrix resultsM = hiddenLayerWithBias.times(weightsOutputLayer).transpose();
                ForagerScenario.logger.debug("Results: " + resultsM.getRowDimension() + "*" + resultsM.getColumnDimension());
                double[] results = resultsM.getColumnPackedCopy();

                // make sure everything is positive
                for (int i = 0; i < a; i++) {
                    results[i] = sigmoid(results[i]);
                } // not sure if...

                // normalize and build cumulative probability array
                double maxSum = 0;
                double[] sumResults = new double[a];
                for (int i = 0; i < a; i++) {
                    maxSum += results[i];
                }
                if (maxSum != 0) {
                    for (int i = 0; i < a; i++) {
                        results[i] /= maxSum;
                        sumResults[i] = results[i];
                        if (i > 0) {
                            sumResults[i] += sumResults[i - 1];
                        }
                    }
                } else {
                    for (int i = 0; i < a; i++) {
                        sumResults[i] = (i + 1) / a;
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
                ForagerScenario.logger.debug("Agent " + agentNumber + " crashed. " + ex.getMessage());
            }
        }
    }
}
