#!/usr/bin/python3

import socket
import random
import numpy as np
from struct import pack, unpack
import threading

from ff.ffAgent import FfAgent

NOTHING = 0
#numberOfActions = 7
#numberOfMapCells = 8  # since EMPTY = 0 does not count atm


class FfEnvironment(object):
    def __init__(self, config):
        self.debug = False

        self.max_steps = 200
        self.numberOfMapCells = 8
        self.numberOfActions = 7

        port = 2000                         # FF server port
        self.ip = '127.0.0.1'               # FF server ip
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect((self.ip, port))     # socket to connect to FF server

        self.agentSockets = []              # list of agent sockets
        self.noa = config.noa               # number of agents
        self.agentPositions = [0]*self.noa  # positions of the agents
        self.waitsteps = 3                  # how many steps to NoOp

        self.jal = config.jal
        self.nob = config.nob
        self.independent_rewards = config.independent_rewards
        self.input_size = config.screen_width*config.screen_height*self.numberOfMapCells
        if config.jal:
            self._screen = np.zeros([self.input_size])
        else:
            self._screen = np.zeros([config.noa, self.input_size])
        self.reward = 0                     # current processed reward
        self.lastReward = 0                 # last step's basic reward
        self.terminal = True                # whether simulation is on-going
        self.display = False

        # self.prevState = []
        self.main_semaphore = threading.Semaphore(0)
        self.semaphoreForAgents = []
        self.agentObjects = []

        self.noop = False

    def debug_run(self, config, jal, step, gui, plt, q, s_t, learning_test_steps, learning_test_values):
        # print("Debug")
        # self.env.debug = True

        screen, reward, action, terminal = self.new_game('tst')
        # print('\n')
        # if self.env.debug:
        #    print('\n'.join(' '.join(''.join( \
        #        str(int(screen[i+j*numberOfMapCells+x*numberOfMapCells*self.config.screen_width])) \
        #                for i in range(numberOfMapCells) ) \
        #                    for j in range(self.config.screen_width) ) \
        #                        for x in range(self.config.screen_height) ))
        counter = 0

        NOTHING = 0
        UP = 1
        DOWN = 2
        LEFT = 3
        RIGHT = 4
        GRAB = 5
        RELEASE = 6
        dist = config.screen_width-2
        half_dist = int(dist/2)
        if jal:
            list_of_actions = [] + [[LEFT, RIGHT]]*dist + [[UP, DOWN]]*half_dist + [[RIGHT, NOTHING]]*half_dist + \
                              [[UP, NOTHING]]*(half_dist+1) + [[RIGHT, NOTHING]]*half_dist + [[GRAB, NOTHING]] + \
                              [[LEFT, NOTHING]]*half_dist + [[DOWN, NOTHING]]*dist + [[RELEASE, NOTHING]] + \
                              [[UP, NOTHING]]*dist + [[LEFT, NOTHING]]*half_dist + [[GRAB, NOTHING]] + \
                              [[RIGHT, NOTHING]]*half_dist + [[DOWN, NOTHING]]*dist + [[RELEASE, NOTHING]] + \
                              [[NOTHING, NOTHING]]*10
        else:
            list_of_actions = [] + [[UP, DOWN]]*half_dist + [[LEFT, RIGHT]]*dist + [[RIGHT, NOTHING]]*half_dist + \
                              [[UP, NOTHING]]*(half_dist+1) + [[RIGHT, NOTHING]]*half_dist + [[GRAB, NOTHING]] + \
                              [[LEFT, NOTHING]]*half_dist + [[DOWN, NOTHING]]*dist + [[RELEASE, NOTHING]] + \
                              [[UP, NOTHING]]*dist + [[LEFT, NOTHING]]*half_dist + [[GRAB, NOTHING]] + \
                              [[RIGHT, NOTHING]]*half_dist + [[DOWN, NOTHING]]*dist + [[RELEASE, NOTHING]] + \
                              [[NOTHING, NOTHING]]*10
        # print(list_of_actions)
        # list_of_actions = [[RIGHT,NOTHING],[UP, LEFT],[UP,UP],[LEFT,UP],[GRAB,RIGHT],[RIGHT,GRAB],[DOWN,LEFT], [DOWN,DOWN], [RELEASE,DOWN],[LEFT,DOWN],[,]]

        avg_max = 0
        # match = 0
        for x in range(len(list_of_actions)):
            # 1. predict
            # q_vals, index = self.sess.run([self.q, self.q_action],{self.s_t: [screen]})
            if jal:
                avg_max += max(q.eval({s_t: [screen]})[0])
            else:
                avg_max += max(q.eval({s_t: [screen[0]]})[0])

            action = list_of_actions[counter]
            counter += 1
            # print(action)

            # 2. act
            screen, reward, terminal = self.act(action)

            if terminal:
                break

        if self.debug:
            print(len(list_of_actions),'<',counter, avg_max)
        # print(len(list_of_actions), x)
        # self.env.debug = False

        learning_test_steps.append(step)
        learning_test_values.append(avg_max/counter)
        if gui:
            plt.clf()
            plt.plot(learning_test_steps, learning_test_values)
            plt.pause(0.001)

    def new_game(self, command='new'):
        # close old game
        for agent, agentSem in zip(self.agentObjects, self.semaphoreForAgents):
            agent.terminal = True
            agentSem.release()
        for agentSocket in self.agentSockets:
            agentSocket.close()

        # request new game
        self.s.sendall((command+str(self.nob)+"-"+str(self.noa)).encode('utf-8'))
        port = unpack('!i', self.s.recv(4))[0]

        self.semaphoreForAgents = []
        self.agentObjects = []
        agentThreads = []

        # start agent sockets
        self.agentSockets = []
        for x in range(self.noa):
            self.agentSockets.append(socket.socket(socket.AF_INET, socket.SOCK_STREAM))
            self.agentSockets[x].connect((self.ip, port))
        self.terminal = False
        self.lastReward = 0

        for i in range(self.noa):
            sem = threading.Semaphore(0)
            self.semaphoreForAgents.append(sem)
            agent = FfAgent(i, self.noa, self.agentSockets[i], sem, self.main_semaphore, self.jal)
            self.agentObjects.append(agent)
            thread = threading.Thread(target=agent.agentStep)
            agentThreads.append(thread)
            thread.start()

        # do NoOps
        self.noop = True
        for x in range(self.waitsteps):
            self._step([NOTHING] * self.noa)
        self.noop = False

        self.s.sendall('rwd'.encode('utf-8'))
        if not self.independent_rewards:
            reward = unpack('!i', self.s.recv(4))[0]
        else:
            reward = unpack('!' + 'i'*self.noa, self.s.recv(4*self.noa))
        # print(reward)
        self.reward = reward
        # print("starting reward ", self.reward)

        # return screen, reward, action, terminal
        # print(self.screen)

        return self.screen, 0, [0]*self.noa, self.terminal

    def new_random_game(self):
        return self.new_game()

    def _step(self, actions):
        for i in range(self.noa):
            self.agentObjects[i].actions = actions
            self.semaphoreForAgents[i].release()
        if self.jal:
            self.main_semaphore.acquire()
            state_packed = self.agentObjects[0].statePacked

            state = unpack('!'+'b'*len(state_packed), state_packed)[self.noa:]

            # self._screen = FfEnvironment.clearAgentSpecificInfo(states)
            # self._screen[0:] = FfEnvironment.clearAgentSpecificInfo(states)
            # if self.debug:
            #    print("map ", state)
            self._screen[0:] = FfEnvironment.clearAgentSpecificInfo(state, self.numberOfMapCells)
        else:
            for i in range(self.noa):
                self.main_semaphore.acquire()
            for i in range(self.noa):
                state_packed = self.agentObjects[i].statePacked

                state = unpack('!'+'b'*len(state_packed), state_packed)[self.noa:]

                # self._screen = FfEnvironment.clearAgentSpecificInfo(states)
                # self._screen[0:] = FfEnvironment.clearAgentSpecificInfo(states)
                # if self.debug:
                #    print("map ", state)
                self._screen[i,0:] = FfEnvironment.convert_state_to_1_out_of_n(state, self.numberOfMapCells)

        self.s.sendall('rwd'.encode('utf-8'))
        if not self.independent_rewards:
            reward = unpack('!i', self.s.recv(4))[0]
            self.reward = -(reward - self.lastReward - 1)
        else:
            reward = unpack('!' + 'i'*self.noa, self.s.recv(4*self.noa))
            self.reward = np.subtract(reward, self.lastReward)
        # reward = unpack('!i', self.s.recv(4))[0]
        # print(reward)
        self.lastReward = reward

        # if self.reward != 0 and not self.noop:
        #    print(self.prevState)
        #    print(state)
        #    print("--")
        # self.prevState = state

        self.s.sendall('trm'.encode('utf-8'))
        self.terminal = unpack('!b', self.s.recv(1))[0]
        # print(self.terminal)

    def _random_step(self):
        actions = random.sample(range(self.numberOfActions), self.noa)
        self._step(actions)

    @property
    def screen(self):
        return self._screen

    @property
    def action_size(self):
        return self.numberOfActions

    @property
    def lives(self):
        return 1

    @property
    def state(self):
        #print(self.reward, self.terminal, self._screen)
        return self._screen, self.reward, self.terminal

    def render(self):
        if self.display:
            self.env.render()

    def after_act(self, action):
        self.render()

    # repeat same action for some steps
    def act(self, action, is_training=True):
        self._step(action)
        self.after_act(action)
        return self.state

    @staticmethod
    def convert_state_to_1_out_of_n(state, numberOfMapCells):
        # http://stackoverflow.com/questions/40680411/extending-integer-list-to-1-out-of-n-binary-list

        # convert to binary
        print(state)
        cleanstate = list(map(int, ''.join(bin(i)[2:].zfill(numberOfMapCells) for i in state)))

        # add bit for EMPTY
        for i in range(len(state)):
            if state[i] == 0:
                cleanstate[i*numberOfMapCells] = 1

        return cleanstate

    @staticmethod
    def clearAgentSpecificInfo(state, numberOfMapCells):
        # http://stackoverflow.com/questions/40680411/extending-integer-list-to-1-out-of-n-binary-list

        # convert to binary
        cleanstate = list(map(int, ''.join(bin(i)[2:].zfill(numberOfMapCells) for i in state)))

        # add bit for EMPTY
        # clear OTHERAGENT bit
        for i in range(len(state)):
            if state[i] == 0:
                cleanstate[i*numberOfMapCells] = 1
            cleanstate[i*numberOfMapCells+1] = 0

        return cleanstate

    def close(self):
        for agent, agentSem in zip(self.agentObjects, self.semaphoreForAgents):
            agent.terminal = True
            agentSem.release()
        for agentSocket in self.agentSockets:
            agentSocket.close()

        self.s.sendall('end'.encode('utf-8'))
        self.s.close()
