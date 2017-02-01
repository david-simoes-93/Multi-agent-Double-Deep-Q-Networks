#!/usr/bin/python3

from struct import pack, unpack

agent_mask = 0b00100100
other_agent_mask = 0b01000000


def get_position(packed_state, noa):

    pos = 0
    for b in packed_state[noa:]: # maybe we want to skip agent action info
        if b & other_agent_mask != 0:
            pos += 1
        if b & agent_mask != 0:
            #print(packed_state, noa, pos)
            return pos
    #print(packed_state, noa, -1)
    return -1


class FfAgent(object):
    def __init__(self, id_, noa, agentSocket, sem, mainSem, jal=True):
        self.id_ = id_
        # self.agentPositions = agentPositions
        self.noa = noa
        self.position = id_
        self.agentSocket = agentSocket
        self.terminal = False
        self.actions = [0]*noa
        self.statePacked = None
        self.terminal = False
        self.semaphore = sem
        self.mainSem = mainSem
        self.jal = jal

    def agentStep(self):
        self.semaphore.acquire()

        while not self.terminal:
            self.agentSocket.sendall(pack('!b', self.actions[self.position]))     # send action as byte

            self.statePacked = self.agentSocket.recv(1024)   # read and store state for 1st agent

            if (self.jal and self.id_ == 0) or not self.jal:
                # wake up main
                self.mainSem.release()

            if self.jal:
                self.position = get_position(self.statePacked, self.noa)
            # self.agentPositions[self.id] = FfEnvironment.get_position(statePacked, self.noa)

            # sleep
            self.semaphore.acquire()

