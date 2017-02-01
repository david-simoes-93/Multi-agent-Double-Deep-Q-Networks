import numpy as np

#numberOfMapCells = 8

class History:
    def __init__(self, config, input_size):
        #self.cnn_format = config.cnn_format

        batch_size, history_length, screen_height, screen_width = \
            config.batch_size, config.history_length, config.screen_height, config.screen_width

        # dimension with noa!
        #if config.env_type == 'ff':
        if config.jal:
            self.history = np.zeros([input_size], dtype=np.float32)
        else:
            self.history = np.zeros([config.noa, input_size], dtype=np.float32)
        #else:
        #    self.history = np.zeros([history_length, screen_height, screen_width], dtype=np.float32)

    def add(self, screen):
        #print(len(screen), screen.shape, len(self.history), self.history.shape)
        #self.history = screen
        if len(self.history.shape) == 1:    # this may not be the best way to do this.. maybe a flag?
            self.history[0:] = screen
        else:
            self.history[:-1] = self.history[1:]
            self.history[-1] = screen

    def reset(self):
        self.history *= 0

    def get(self):
        #if self.cnn_format == 'NHWC':
        #    return np.transpose(self.history, (1, 2, 0))
        #else:
        return self.history
