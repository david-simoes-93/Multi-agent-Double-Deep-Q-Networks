#!/usr/bin/python3

from socket import *
import random
import numpy as np
from struct import pack, unpack
import threading

from prst.prstAgent import PrstAgent

class PrstEnvironment(object):
    def __init__(self, config):
        self.numberOfActions = 5
        self.numberOfMapCells = 2           # empty, agent, prey, otheragent

        self.debug = False

        self.port = 4001                    # FF server port
        self.ip = '127.0.0.1'               # FF server ip

        self.agentSockets = []              # list of agent sockets
        self.noa = config.noa               # number of agents
        self.agentPositions = [0]*self.noa  # positions of the agents
        self.waitsteps = 3                  # how many steps to NoOp

        self.width = config.screen_width
        self.height = config.screen_height

        self.jal = config.jal
        self.nob = config.nob
        self.independent_rewards = config.independent_rewards

        self.gridless_input = False
        if self.gridless_input and self.jal:
            self.input_size = (config.noa+config.nob)*2*2#+config.noa
        elif self.gridless_input and not self.jal:
            self.input_size = (config.noa-1+config.nob)*2#+config.noa
        elif self.jal:
            self.input_size = config.screen_width*config.screen_height*self.numberOfMapCells*self.noa
        else:
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

        self.started = False

        self.max_steps = 50000

    def new_game(self, command='new'):
        while self.started and not self.terminal:
            #print("bboom")
            self._random_step()

        if not self.started:
            self.semaphoreForAgents = []
            self.agentObjects = []
            agentThreads = []

            # start agent sockets
            self.agentSockets = []
            for x in range(self.noa):
                self.agentSockets.append(socket(AF_INET, SOCK_DGRAM))
                self.agentSockets[x].bind(('', 0))
                self.agentSockets[x].sendto("(init predator)".encode('utf-8'), ('', self.port))
                #print("Sent: (init predator)")
                msg, addr = self.agentSockets[x].recvfrom( 1024 )
                self.agentSockets[x].connect( addr )
            self.terminal = False
            #self.lastReward = 0

            for i in range(self.noa):
                sem = threading.Semaphore(0)
                self.semaphoreForAgents.append(sem)
                agent = PrstAgent(i, self.noa, self.agentSockets[i], sem, self.main_semaphore, self.jal)
                self.agentObjects.append(agent)
                thread = threading.Thread(target=agent.agentStep)
                agentThreads.append(thread)
                thread.start()

            self.started = True
        #print("Done")

        # do NoOps
        self.noop=True
        for x in range(self.waitsteps):
            self._step([0] * self.noa)
            self.noop=False

        self.reward = 0
        return self.screen, 0, [0]*self.noa, self.terminal

    def new_random_game(self):
        return self.new_game()

    def _step(self, actions):
        for i in range(self.noa):
            self.agentObjects[i].noop=self.noop
            self.agentObjects[i].actions = actions
            self.semaphoreForAgents[i].release()

        for i in range(self.noa):
            self.main_semaphore.acquire()

        if not self.gridless_input:
            if self.jal:
                input_size_per_agent = int(self.input_size/self.noa)
                self._screen[0:input_size_per_agent] = PrstEnvironment.convert_state_to_1_out_of_n(self.agentObjects[0].state,
                                                            self.width, self.height, self.numberOfMapCells)
                for i in range(1, self.noa):
                    self._screen[input_size_per_agent*i:input_size_per_agent*(i+1)] = PrstEnvironment.convert_state_to_1_out_of_n(self.agentObjects[i].state,
                                                            self.width, self.height, self.numberOfMapCells)
            else:
                for i in range(self.noa):
                    self._screen[i, 0:] = PrstEnvironment.convert_state_to_1_out_of_n(self.agentObjects[i].state,
                                                            self.width, self.height, self.numberOfMapCells)
        else:
            if self.jal:
                input_size_per_agent = int(self.input_size/self.noa)
                self._screen[0:input_size_per_agent] = PrstEnvironment.convert_state_to_gridless(self.agentObjects[0].state, self.nob, self.noa, self.jal)
                for i in range(1, self.noa):
                    self._screen[input_size_per_agent*i:input_size_per_agent*(i+1)] = PrstEnvironment.convert_state_to_gridless(self.agentObjects[i].state, self.nob, self.noa, self.jal)
            else:
                for i in range(self.noa):
                    self._screen[i, 0:] = PrstEnvironment.convert_state_to_gridless(self.agentObjects[i].state, self.nob, self.noa, self.jal)

        if not self.independent_rewards:
            self.reward = 0
            for agent in self.agentObjects:
                #self.reward += (agent.reward/2)
                if abs(agent.reward) > abs(self.reward):
                    self.reward = agent.reward
        else:
            self.reward = [0]*self.noa
            for i in range(self.noa):
                self.reward[i] = self.agentObjects[i].reward

        self.terminal = False
        for agent in self.agentObjects:
            self.terminal = self.terminal or agent.terminal

        #if self.reward!=0:
        #    print("Reward = ",self.reward)
        #if self.terminal:
        #    print("Environment terminal!")

        #if self.terminal or self.reward!=0:
        #    print(self.terminal, self.reward)

        if False:
            print(actions, '\n', '\n'.join(' '.join(''.join(
                str(int(self._screen[i+j*self.numberOfMapCells + x*self.numberOfMapCells*self.width]))
                for i in range(self.numberOfMapCells-2))
                    for j in range(self.width))
                        for x in range(self.height)))
            print(self.reward, self.terminal)

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
        return self._screen, self.reward, self.terminal

    def render(self):
        if self.display:
            self.env.render()

    def after_act(self, action):
        self.render()

    def debug_run(self, q, gui, plt, learning_test_steps, learning_test_values, step, s_t):
        list_of_actions = [["(move west)", "(move none)"], ["(move west)", "(move none)"],
                               ["(move west)", "(move none)"], ["(move west)", "(move none)"],
                               ["(move north)", "(move none)"], ["(move north)", "(move none)"],
                               ["(move north)", "(move none)"], ["(move north)", "(move none)"]]
        states_text = ["(see ((predator 1) 0 0) ((predator 2) 0 -1) ((prey 1) 0 3) ((prey 2) -3 3) ) ",
                       "(see ((predator 1) 0 0) ((predator 2) 0 -3) ((prey 1) 0 3) ((prey 2) -2 3) ) ",
                       "(see ((predator 1) 0 0) ((predator 2) 0 3) ((prey 1) 0 2) ((prey 2) -2 3) ) ",
                       "(see ((predator 1) 0 0) ((predator 2) 0 2) ((prey 1) 0 1) ((prey 1) -1 3) ) ",
                       "(see ((predator 1) 0 0) ((predator 2) 0 1) ((prey 2) 0 3) ) ",
                       "(see ((predator 1) 0 0) ((predator 2) -1 1) ((prey 2) 1 3) ) ",
                       "(see ((predator 1) 0 0) ((predator 2) -1 1) ((prey 2) 0 2) ) ",
                       "(see ((predator 1) 0 0) ((predator 2) -1 1) ((prey 2) 0 1) ) ",
                       "(see ((predator 1) 0 0) ((predator 2) 0 1) ) "]
        states_text1 = ["(see ((predator 1) 0 1) ((predator 2) 0 0) ((prey 1) 0 -3) ((prey 2) -3 -3) ) ",
                        "(see ((predator 1) 0 3) ((predator 2) 0 0) ((prey 1) 0 -1) ((prey 2) -2 -1) ) ",
                        "(see ((predator 1) 0 -3) ((predator 2) 0 0) ((prey 1) 0 -1) ((prey 2) -2 0) ) ",
                        "(see ((predator 1) 0 -2) ((predator 2) 0 0) ((prey 1) 0 -1) ((prey 1) -1 1) ) ",
                        "(see ((predator 1) 0 -1) ((predator 2) 0 0) ((prey 2) 0 2) ) ",
                        "(see ((predator 1) 1 -1) ((predator 2) 0 0) ((prey 2) 1 2) ) ",
                        "(see ((predator 1) 1 -1) ((predator 2) 0 0) ((prey 2) 1 1) ) ",
                        "(see ((predator 1) 1 -1) ((predator 2) 0 0) ((prey 2) 1 0) ) ",
                        "(see ((predator 1) 0 -1) ((predator 2) 0 0) ) "]
        '''states_text = ["(see ((predator 1) 0 0) ((predator 2) 0 3) ((prey 1) -3 0) ((prey 2) -3 3) ) ",
                       "(see ((predator 1) 0 0) ((predator 2) 0 3) ((prey 1) -2 0) ((prey 2) -2 -3) ) ",
                       "(see ((predator 1) 0 0) ((predator 2) 0 3) ((prey 1) -2 0) ((prey 2) -3 3) ) ",
                       "(see ((predator 1) 0 0) ((predator 2) 0 3) ((prey 1) -1 0) ((prey 2) -1 -3) ) ",
                       "(see ((predator 1) 0 0) ((predator 2) 0 3) ((prey 2) 0 3) ) ",
                       "(see ((predator 1) 0 0) ((predator 2) 0 3) ((prey 2) 1 3) ) ",
                       "(see ((predator 1) 0 0) ((predator 2) 0 3) ((prey 2) 0 2) ) ",
                       "(see ((predator 1) 0 0) ((predator 2) 0 3) ((prey 2) 0 1) ) ",
                       "(see ((predator 1) 0 0) ((predator 2) 0 3) ) "]
        states_text1 = ["(see ((predator 1) 0 -3) ((predator 2) 0 0) ((prey 1) -3 -3) ((prey 2) -3 0) ) ",
                        "(see ((predator 1) 0 -3) ((predator 2) 0 0) ((prey 1) -2 -3) ((prey 2) -2 3) ) ",
                        "(see ((predator 1) 0 -3) ((predator 2) 0 0) ((prey 1) -2 -3) ((prey 2) -3 0) ) ",
                        "(see ((predator 1) 0 -3) ((predator 2) 0 0) ((prey 1) -1 -3) ((prey 2) -1 3) ) ",
                        "(see ((predator 1) 0 -3) ((predator 2) 0 0) ((prey 2) 0 0) ) ",
                        "(see ((predator 1) 0 -3) ((predator 2) 0 0) ((prey 2) 1 0) ) ",
                        "(see ((predator 1) 0 -3) ((predator 2) 0 0) ((prey 2) 0 -1) ) ",
                        "(see ((predator 1) 0 -3) ((predator 2) 0 0) ((prey 2) 0 -2) ) ",
                        "(see ((predator 1) 0 -3) ((predator 2) 0 0) ) "]'''
        states = []
        for s in states_text:
            if self.gridless_input:
                states.append(PrstEnvironment.convert_state_to_gridless(s, self.nob, self.noa, self.jal))
            else:
                states.append(PrstEnvironment.convert_state_to_1_out_of_n(s, self.width, self.height, self.numberOfMapCells))
        states1 = []
        for s in states_text1:
            if self.gridless_input:
                states1.append(PrstEnvironment.convert_state_to_gridless(s, self.nob, self.noa, self.jal))
            else:
                states1.append(PrstEnvironment.convert_state_to_1_out_of_n(s, self.width, self.height, self.numberOfMapCells))


        avg_max = 0
        # match = 0
        for x in range(len(list_of_actions)):
            # 1. predict
            # q_vals, index = self.sess.run([self.q, self.q_action],{self.s_t: [screen]})
            #avg_max += max(q.eval({s_t: [states[x]]})[0])
            if self.jal:
                avg_max += max(q.eval({s_t: [states[x]+states1[x]]})[0])
            else:
                avg_max += max(q.eval({s_t: [states[x]]})[0])

        learning_test_steps.append(step)
        learning_test_values.append(avg_max/len(list_of_actions))
        if gui:
            plt.clf()
            plt.plot(learning_test_steps, learning_test_values)
            plt.pause(0.001)

    # repeat same action for some steps
    def act(self, action, is_training=True):
        self._step(action)
        self.after_act(action)
        return self.state

    @staticmethod
    def convert_state_to_gridless(state, nob, noa, jal):
        if jal:
            cleanstate = [0]*((nob+noa)*2)#+nob*noa
        else:
            cleanstate = [0]*((nob+noa-1)*2)#+nob*noa

        observations = state[6:-3].split(') (')
        skip = noa # if independent, skip self agent info
        if not jal:
            for o in observations:
                fields = o.strip().split(' ')
                fields[1] = fields[1].replace(')','')
                fields[3] = fields[3].replace(')','')

                if int(fields[2])==0 and int(fields[3])==0:
                    skip = int(fields[1])

        for o in observations:
            fields = o.strip().split(' ')
            fields[1] = fields[1].replace(')','')
            fields[3] = fields[3].replace(')','')
            skip_mod=0
            if int(fields[1])>skip:
                skip_mod=-2

            if int(fields[1])==skip:
                pass
            elif '(predator' in o:
                #print(skip, skip_mod, nob*2+int(fields[1])*2-2+skip_mod+0, fields[1])
                cleanstate[nob*2+int(fields[1])*2-2+skip_mod+0] = int(fields[2])
                cleanstate[nob*2+int(fields[1])*2-2+skip_mod+1] = int(fields[3])
            elif '(prey' in o:
                cleanstate[int(fields[1])*2-2+0] = int(fields[2])
                cleanstate[int(fields[1])*2-2+1] = int(fields[3])
            else:
                print('observation: ', o)
        #print(state, cleanstate)
        return cleanstate

    @staticmethod
    def convert_state_to_1_out_of_n(state, width, height, number_of_map_cells):
        cleanstate = [0]*(width*height*number_of_map_cells) #subtract empty and otheragent types

        observations = state[6:-3].split(') (')
        for o in observations:
            fields = o.strip().split(' ')
            fields[3] = fields[3].replace(')','')

            if '(predator' in o:
                cleanstate[int((int(fields[2])+int(width/2))*number_of_map_cells +
                               (-int(fields[3])+int(height/2))*width*number_of_map_cells +
                               0)] = 1
            elif '(prey' in o:
                cleanstate[int((int(fields[2])+int(width/2))*number_of_map_cells +
                               (-int(fields[3])+int(height/2))*width*number_of_map_cells +
                               1)] = 1
            else:
                print('observation: ', o)

        return cleanstate

    def close(self):
        for agent, agentSem in zip(self.agentObjects, self.semaphoreForAgents):
            agent.quit = True
            agentSem.release()
        for agentSocket in self.agentSockets:
            agentSocket.close()

        #self.s.sendall('end'.encode('utf-8'))
        #self.s.close()
