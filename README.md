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

The Multi-agent Double DQN algorithm is in the `MaDDQN` folder. Original DQN code by [devsisters](https://github.com/devsisters/DQN-tensorflow), you will need [Python3.3+](https://www.python.org/download/releases/3.0/), [TDQM](https://github.com/tqdm/tqdm), [matplotlib](http://matplotlib.org/), [python-tk](https://wiki.python.org/moin/TkInter) and [TensorFlow 0.13+](https://www.tensorflow.org/). Adjust configuration on `MaDDQN/src/config.py`, and just

    python3 MaDDQN/src/main.py
    
## Results
    
The algorithm works almost out of the box with the two testing scenarios. Below are some results for the Foraging Task. It shows the results of several stages of learning. For 7x7 maps, we used a neural network with 2 hidden layers of 50 nodes each, RelU activation. For 15x15 maps, we used a neural network with 2 hidden layers, 250 nodes each, and RelU activation.

Low-training (left) and high-training (right), 2 agents, 10 berries
</p>
<img src="https://cloud.githubusercontent.com/assets/9117323/22506905/c872a3b2-e87a-11e6-8b23-7fa81c620d1c.GIF" width="200">
<img src="https://cloud.githubusercontent.com/assets/9117323/22506907/c89adf26-e87a-11e6-9ac2-51749c5a7445.GIF" width="200">

----

For the Pursuit Game, there is an example of a 1v2 situation, where it is impossible for the agent to catch the prey, and then a 2v2 on a small and a large map.

</p>
<img src="https://cloud.githubusercontent.com/assets/9117323/22506910/c89fc19e-e87a-11e6-9c11-87c9065f2c89.GIF" width="200">
<img src="https://cloud.githubusercontent.com/assets/9117323/22506908/c89cfeaa-e87a-11e6-82d7-1abca7c8c9a5.GIF" width="200">

----

15x15 map, 4 agents, 10 berries
<img src="https://cloud.githubusercontent.com/assets/9117323/25333859/edd922c6-28e3-11e7-8d71-fee13deab4c6.GIF" width="400">

----

15x15 map, 2 predator, 2 prey

<img src="https://cloud.githubusercontent.com/assets/9117323/22506909/c89f27d4-e87a-11e6-8819-4922b1772133.GIF" width="400">

----

15x15 map, 2 predators, 3-6 prey, 3 obstacles (generalization, 10%)
<img src="https://cloud.githubusercontent.com/assets/9117323/24868699/c51d58a6-1e08-11e7-8cc6-8cd328652339.gif" width="400">
<img src="https://cloud.githubusercontent.com/assets/9117323/24874701/815f2c9c-1e1d-11e7-90be-151a46e4f925.gif" width="400">
<img src="https://cloud.githubusercontent.com/assets/9117323/24874896/120f54f6-1e1e-11e7-8912-4190101b556d.gif" width="400">
----

15x15 map, 4 predators, 3 prey, 3 obstacles
<img src="https://cloud.githubusercontent.com/assets/9117323/22506909/c89f27d4-e87a-11e6-8819-4922b1772133.GIF" width="400">
