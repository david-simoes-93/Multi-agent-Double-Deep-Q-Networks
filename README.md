# Multi-agent Double Deep Q-Networks
A multi-agent version of the Double DQN algorithm, with Foraging Task and Pursuit Game test scenarios.

## Forager Task

The Forager Task is in Java 8 and pre-compiled. If you want/need to recompile it, just compile the `.java` files in `Forager/src` including the libraries on `Forager/include`. 

To run, just

    java -jar Forager/Forager.jar

## Pursuit Game

The Pursuit Game is in C, and originally by [Kok et al.](https://staff.fnwi.uva.nl/a.visser/research/ias/trilearn/publications/b2hd-IAS-UVA-03-03.html). It is also pre-compiled for Linux x64 systems, but you can compile it after installing [GL and glut libraries](http://www.mesa3d.org) with

    cd Pursuit
    ./configure
    make

To run, adjust configurations on `Pursuit/pursuit/src/pursuit.conf`, and just

    ./Pursuit/start.sh

## Multi-agent Double DQN

The Multi-agent Double DQN algorithm is in the `MaDDQN` folder. Original code by [devsisters](https://github.com/devsisters/DQN-tensorflow), you will need [Python3.3+](https://www.python.org/download/releases/3.0/), [TDQM](https://github.com/tqdm/tqdm) and [TensorFlow 0.13+](https://www.tensorflow.org/). Adjust configuration on `MaDDQN/src/config.py`, and just

    python3 MaDDQN/src/main.py
    
## Results
    
The algorithm works almost out of the box with the two testing scenarios. Below are some results for the Foraging Task. It shows the results of several stages of learning.

![ff-gen-0](https://cloud.githubusercontent.com/assets/9117323/22506905/c872a3b2-e87a-11e6-8b23-7fa81c620d1c.GIF)
![ff-gen-1](https://cloud.githubusercontent.com/assets/9117323/22506906/c88da176-e87a-11e6-89a0-abcd50b1c9fe.GIF)
![ff-gen-2](https://cloud.githubusercontent.com/assets/9117323/22506907/c89adf26-e87a-11e6-9ac2-51749c5a7445.GIF)

For the Pursuit Game, there is an example of a 1v2 situation, where it is impossible for the agent to catch the prey, and then a 2v2 on a small and a large map.

![prst-1a-2p](https://cloud.githubusercontent.com/assets/9117323/22506910/c89fc19e-e87a-11e6-9c11-87c9065f2c89.GIF)
![prst-2a-2p](https://cloud.githubusercontent.com/assets/9117323/22506908/c89cfeaa-e87a-11e6-82d7-1abca7c8c9a5.GIF)
![prst-2a-2p-15x15](https://cloud.githubusercontent.com/assets/9117323/22506909/c89f27d4-e87a-11e6-8819-4922b1772133.GIF)
