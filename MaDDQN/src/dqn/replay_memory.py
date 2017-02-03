"""Code from https://github.com/tambetm/simple_dqn/blob/master/src/replay_memory.py"""

import os
import random

import numpy as np

from .utils import save_npy, load_npy

#numberOfActions = 7
#numberOfMapCells = 8

class ReplayMemory:
    def __init__(self, config, model_dir, input_size, numberOfActions):
        self.model_dir = model_dir
        self.numberOfActions = numberOfActions

        self.jal = config.jal
        self.noa = config.noa

        #self.cnn_format = config.cnn_format
        self.memory_size = config.memory_size
        self.actions = np.empty(self.memory_size, dtype=np.uint8) # why was this here: **config.noa
        self.rewards = np.empty(self.memory_size, dtype=np.integer)
        self.terminals = np.empty(self.memory_size, dtype=np.bool)
        self.history_length = config.history_length
        self.batch_size = config.batch_size
        #if config.env_type == 'ff':
        self.screens = np.empty((self.memory_size, input_size), dtype=np.float16)
        #if config.jal:
        self.dims = (input_size,)
        #else:
        #    self.dims = (config.noa, config.screen_height * config.screen_width*numberOfMapCells)
        self.prestates = np.empty((self.batch_size,) + self.dims, dtype=np.float16)
        self.poststates = np.empty((self.batch_size,) + self.dims, dtype=np.float16)
        #else:
        #    self.screens = np.empty((self.memory_size, config.screen_height, config.screen_width), dtype=np.float16)
        #    self.dims = (config.screen_height, config.screen_width)

            # pre-allocate prestates and poststates for minibatch
        #    self.prestates = np.empty((self.batch_size, self.history_length) + self.dims, dtype=np.float16)
        #    self.poststates = np.empty((self.batch_size, self.history_length) + self.dims, dtype=np.float16)

        self.count = 0
        self.current = 0
        self.env_type = config.env_type

    def add(self, screen, reward, action, terminal):
        assert screen.shape == self.dims
        # NB! screen is post-state, after action and reward
        if self.jal:
            self.actions[self.current] = sum(self.numberOfActions ** i * j for i, j in enumerate(action, 0))
        else:
            self.actions[self.current] = action
        self.rewards[self.current] = reward
        self.screens[self.current, ...] = screen
        self.terminals[self.current] = terminal
        self.count = max(self.count, self.current + 1)
        self.current = (self.current + 1) % self.memory_size

    def getState(self, index):
        assert self.count > 0, "replay memory is empty, use at least --random_steps 1"
        # normalize index to expected range, allows negative indexes
        index %= self.count
        # if is not in the beginning of matrix
        if index >= self.history_length - 1:
            # use faster slicing
            return self.screens[(index - (self.history_length - 1)):(index + 1), ...]
        else:
            # otherwise normalize indexes and use slower list based access
            indexes = [(index - i) % self.count for i in reversed(range(self.history_length))]
            return self.screens[indexes, ...]

    def sample(self):
        # memory must include poststate, prestate and history
        assert self.count > self.history_length
        # sample random indexes
        indexes = []
        while len(indexes) < self.batch_size:
            # find random index
            while True:
                # sample one index (ignore states wraping over
                index = random.randint(self.history_length, self.count - 1)
                # if wraps over current pointer, then get new one
                if index >= self.current and index - self.history_length < self.current:
                    continue
                # if wraps over episode end, then get new one
                # NB! poststate (last screen) can be terminal state!
                if self.jal:
                    if self.terminals[(index - self.history_length):index].any():
                        #print(index)
                        #print(self.terminals[(index - self.history_length):index])
                        continue
                else:
                    if self.terminals[(index - self.history_length*self.noa):index].any():
                        continue
                # otherwise use this index
                break

            # NB! having index first is fastest in C-order matrices
            #print(self.getState(index - 1).shape)
            if self.jal:
                self.prestates[len(indexes), ...] = self.getState(index - 1)
            else:
                self.prestates[len(indexes), ...] = self.getState(index - self.noa)
            self.poststates[len(indexes), ...] = self.getState(index)
            indexes.append(index)

        actions = self.actions[indexes]
        rewards = self.rewards[indexes]
        terminals = self.terminals[indexes]

        #if self.cnn_format == 'NHWC':
        #    return np.transpose(self.prestates, (0, 2, 3, 1)), actions, \
        #           rewards, np.transpose(self.poststates, (0, 2, 3, 1)), terminals
        #else:
        return self.prestates, actions, rewards, self.poststates, terminals

    def save(self):
        for idx, (name, array) in enumerate(
                zip(['actions', 'rewards', 'screens', 'terminals', 'prestates', 'poststates'],
                    [self.actions, self.rewards, self.screens, self.terminals, self.prestates, self.poststates])):
            save_npy(array, os.path.join(self.model_dir, name))

    def load(self):
        for idx, (name, array) in enumerate(
                zip(['actions', 'rewards', 'screens', 'terminals', 'prestates', 'poststates'],
                    [self.actions, self.rewards, self.screens, self.terminals, self.prestates, self.poststates])):
            array = load_npy(os.path.join(self.model_dir, name))