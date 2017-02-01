# Multi-agent-Double-Deep-Q-Networks
A multi-agent version of the Double DQN algorithm, with Foraging Task and Pursuit Game test scenarios.

## Forager Task

The Forager Task is in Java 8 and pre-compiled. If you want/need to recompile it, just compile the `.java` files in `Forager/src` including the libraries on `Forager/include`. 

To run, just

    java -jar Forager/Forager.jar

## Pursuit Game

The Pursuit Game is in C, and originally by Kok et al. (https://staff.fnwi.uva.nl/a.visser/research/ias/trilearn/publications/b2hd-IAS-UVA-03-03.html). It is also pre-compiled for Linux x64 systems, but you can compile it after installing GL and glut libraries (http://www.mesa3d.org) with

    cd Pursuit
    ./configure
    make

To run, adjust configurations on `Pursuit/pursuit/src/pursuit.conf`, and just

    ./Pursuit/start.sh

## Multi-agent Double DQN

The Multi-agent Double DQN algorithm is in the `MaDDQN` folder. To run, you will need Python3 (https://www.python.org/download/releases/3.0/) and TensorFlow (https://www.tensorflow.org/). Adjust configuration on `MaDDQN/src/config.py`, and just

    python3 MaDDQN/src/main.py
    
## Results
    
The algorithm works almost out of the box with the two testing scenarios. Below are some results for the Foraging Task

![Example](https://www.dropbox.com/s/mba4xp9rwnad5y1/ff-gen-2.GIF)

And for the Pursuit Game

![Example](https://www.dropbox.com/s/ptlwd4ii81n2zur/prst-2a-2p.GIF)
![Example](https://www.dropbox.com/s/pdl8as0mnyqytpx/prst-2a-2p-15x15.GIF)
