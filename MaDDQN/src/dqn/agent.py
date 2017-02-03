import os
import random
import numpy as np
from tqdm import tqdm
import tensorflow as tf
import matplotlib.pyplot as plt

from .base import BaseModel
from .history import History
from .ops import linear, conv2d
from .replay_memory import ReplayMemory
from .utils import save_pkl, load_pkl

class Agent(BaseModel):
    def __init__(self, config, environment, sess):
        super(Agent, self).__init__(config)
        self.sess = sess
        self.weight_dir = 'weights'

        self.env = environment
        self.history = History(self.config, self.env.input_size)
        self.memory = ReplayMemory(self.config, self.model_dir, self.env.input_size, self.env.numberOfActions)
        self.gui = config.gui

        self.learning_test_steps = []
        self.learning_test_values = []
        self.games_per_epoch = []

        with tf.variable_scope('step'):
            self.step_op = tf.Variable(0, trainable=False, name='step')
            self.step_input = tf.placeholder('int32', None, name='step_input')
            self.step_assign_op = self.step_op.assign(self.step_input)
        self.build_dqn()

    def train(self):

        start_step = 0 #self.step_op.eval()

        num_game, self.update_count, ep_reward = 0, 0, 0.
        total_reward, self.total_loss, self.total_q = 0., 0., 0.
        max_avg_ep_reward = 0
        ep_rewards, actions = [], []

        self.games_per_epoch.append(0)
        self.learning_test_steps.append(0)
        self.learning_test_values.append(0)
        self.avg_ep_rewards = [0]
        if self.gui:
            plt.ion()
            plt.show()
            plt.plot(self.learning_test_steps, self.learning_test_values)
            plt.pause(0.001)

        screen, reward, action, terminal = self.env.new_random_game()
        # for i in range(self.noa):
        #    print("Start\n",self.print_screen(screen[i]), reward, action[i], terminal)

        for _ in range(self.history_length):
            if self.jal:
                self.history.add(screen)
            else:
                for i in range(self.noa):
                    self.history.add(screen[i])

        last_debug = 0

        iteration_steps = 0
        for self.step in tqdm(range(0, self.max_step), ncols=70, initial=start_step):
            if self.step == self.learn_start and False:
                num_game, self.update_count, ep_reward = 0, 0, 0.
                total_reward, self.total_loss, self.total_q = 0., 0., 0.
                ep_rewards, actions = [], []

            #self.env.debug_run(self.q, self.gui, plt, self.learning_test_steps,
            #                   self.learning_test_values, self.step, self.s_t)


            # 1. predict
            action = self.predict(self.history.get())
            # 2. act
            screen, reward, terminal = self.env.act(action, is_training=True)
            # 3. observe
            self.observe(screen, reward, action, terminal)

            if not self.config.independent_rewards:
                ep_reward += reward
                #if reward!=0:
                #    print("agent knows reward, @ ",ep_reward, reward)
                #if reward != 0:
                #    print("ep_reward ",ep_reward, reward)
            else:
                ep_reward += np.sum(reward)

            iteration_steps += 1
            if terminal or iteration_steps > self.env.max_steps:
                #print("boom")
                iteration_steps = 0
                if self.step > last_debug + 2000:
                    if self.config.env_type == 'ff' and self.config.noa == 2:
                        self.env.debug_run(self.config, self.jal, self.step, self.gui, plt, self.q, self.s_t,
                                           self.learning_test_steps, self.learning_test_values)
                    elif self.config.env_type == 'prst' and self.config.noa == 2:
                        self.env.debug_run(self.q, self.gui, plt, self.learning_test_steps,
                                           self.learning_test_values, self.step, self.s_t)
                    last_debug = self.step

                screen, reward, action, terminal = self.env.new_random_game()
                # self.env.debug = self.step > 20000

                num_game += 1
                ep_rewards.append(ep_reward)
                #print("rewards ",str(ep_rewards))
                ep_reward = 0.

            # print(action)
            # add action index to list of actions
            actions.append(self.get_index_from_list_of_actions(action))
            if not self.config.independent_rewards:
                total_reward += reward
            else:
                total_reward += np.sum(reward)

            if self.step >= self.learn_start:
                if self.step % self.test_step == self.test_step - 1:
                    avg_reward = total_reward / self.test_step
                    avg_loss = self.total_loss / self.update_count
                    avg_q = self.total_q / self.update_count

                    try:
                        max_ep_reward = np.max(ep_rewards)
                        min_ep_reward = np.min(ep_rewards)
                        avg_ep_reward = np.mean(ep_rewards)
                    except:
                        max_ep_reward, min_ep_reward, avg_ep_reward = 0, 0, 0

                    #print(avg_reward, avg_loss, avg_q, avg_ep_reward, max_ep_reward, min_ep_reward, num_game)
                    print('\navg_r: %.4f, avg_l: %.6f, avg_q: %3.6f, avg_ep_r: %.4f, max_ep_r: %.4f, min_ep_r: %.4f, # game: %d' \
                        % (avg_reward, avg_loss, avg_q, avg_ep_reward, max_ep_reward, min_ep_reward, num_game))
                    self.avg_ep_rewards.append(avg_ep_reward)
                    self.games_per_epoch.append(num_game)

                    #if max_avg_ep_reward * 0.9 <= avg_ep_reward:
                    #    self.step_assign_op.eval({self.step_input: self.step + 1})
                    #    self.save_model(self.step + 1)

                    #    max_avg_ep_reward = max(max_avg_ep_reward, avg_ep_reward)

                    if self.step > 180 and False:
                        self.inject_summary({
                            'average.reward': avg_reward,
                            'average.loss': avg_loss,
                            'average.q': avg_q,
                            'episode.max reward': max_ep_reward,
                            'episode.min reward': min_ep_reward,
                            'episode.avg reward': avg_ep_reward,
                            'episode.num of game': num_game,
                            'episode.rewards': ep_rewards,
                            'episode.actions': actions,
                            'training.learning_rate': self.learning_rate_op.eval({self.learning_rate_step: self.step}),
                        }, self.step)

                    num_game = 0
                    total_reward = 0.
                    self.total_loss = 0.
                    self.total_q = 0.
                    self.update_count = 0
                    #ep_reward = 0.
                    ep_rewards = []
                    actions = []

        self.save_model()
        self.env.close()
        print("Done!")
        print(self.learning_test_steps)
        print(self.learning_test_values)
        return self.learning_test_steps, self.learning_test_values, self.avg_ep_rewards, self.games_per_epoch

    def print_screen(self, screen):
        return '\n'.join(' '.join(''.join(
            str(int(screen[i+j*self.env.numberOfMapCells+x*self.env.numberOfMapCells*self.config.screen_width]))
            for i in range(self.env.numberOfMapCells))
                        for j in range(self.config.screen_width))
                            for x in range(self.config.screen_height))

    def get_epsilon(self):
        return (self.ep_end + max(0., (self.ep_start - self.ep_end)
                                    * (self.ep_end_t - max(0., self.step - self.learn_start)) / self.ep_end_t))

    # return a list of actions to be executed based on state and exploration rate
    def predict(self, s_t, test_ep=None):
        ep = test_ep or self.get_epsilon()

        actions = []
        if random.random() < ep:
            for _ in range(self.env.noa):
                actions.append(random.randrange(self.env.action_size))
        else:
            if self.jal:
                index = self.q_action.eval({self.s_t: [s_t]})[0]
                if self.env.debug:
                    print("state being evaluated: ", s_t)
                    print("reward: ", self.q.eval({self.s_t: [s_t]})[0])
                actions = self.get_list_of_actions_from_index(index)
            else:
                actions = [0]*self.noa
                for i in range(self.noa):
                    if self.env.debug:
                        print("state being evaluated: ", self.print_screen(s_t[i]))
                        print("reward: ", self.q.eval({self.s_t: [s_t[i]]})[0])
                    actions[i] = self.q_action.eval({self.s_t: [s_t[i]]})[0]

        if self.env.debug:
            print("actions: ", actions)

        return actions

    def get_list_of_actions_from_index(self, index):
        actions = [0]*self.env.noa
        for x in range(len(actions)):
            actions[x] = int(index % self.env.numberOfActions)
            index = int((index - actions[x]) / self.env.numberOfActions)
        return actions

    def get_index_from_list_of_actions(self, actions):
        return int(sum(self.env.numberOfActions ** i * j for i, j in enumerate(actions, 0)))

    def observe(self, screen, reward, action, terminal):
        #reward = max(self.min_reward, min(self.max_reward, reward))

        if self.jal:
            self.history.add(screen)
            self.memory.add(screen, reward, action, terminal)
            #if terminal:
            #    print('ADDING TERMINAL TRANSITION')
        else:
            for i in range(self.noa):
                #if reward[i]!=0:
                #    print("batch", i,'\n', self.print_screen(screen[i]), reward[i], action[i], terminal)
                self.history.add(screen[i])
                if not self.config.independent_rewards:
                    self.memory.add(screen[i], reward, action[i], terminal)
                else:
                    self.memory.add(screen[i], reward[i], action[i], terminal)

        if self.step > self.learn_start:
            if self.step % self.train_frequency == 0:
                self.q_learning_mini_batch()

            if self.step % self.target_q_update_step == self.target_q_update_step - 1:
                self.update_target_q_network()

    def q_learning_mini_batch(self):
        if self.memory.count < self.history_length:
            return
        else:
            s_t, action, reward, s_t_plus_1, terminal = self.memory.sample()

        #if reward[0] != 0:
        #    print(str(s_t[0]), action[0], reward[0], str(s_t_plus_1[0]))

        #for i in range(32):
        #    print("batch\n", self.print_screen(s_t[i]), '\n', action_names[action[i]], reward[i], terminal[i], '\n', self.print_screen(s_t_plus_1[i]))

        if self.double_q:
            # Double Q-learning
            # get best action for current NN and state(t+1)
            pred_action = self.q_action.eval({self.s_t: s_t_plus_1})

            # get predicted Q(t+1) with action pred_action for target NN
            q_t_plus_1_with_pred_action = self.target_q_with_idx.eval({
                self.target_s_t: s_t_plus_1,
                self.target_q_idx: [[idx, pred_a] for idx, pred_a in enumerate(pred_action)]    # wtf is this
            })

            # targetQ(t) = terminal ? reward : gamma*Q(t+1)+reward
            target_q_t = (1. - terminal) * self.discount * q_t_plus_1_with_pred_action + reward
        else:
            # get predicted Q(t+1) values for target NN
            q_t_plus_1 = self.target_q.eval({self.target_s_t: s_t_plus_1})

            # select best Q values (the ones with the best action)
            terminal = np.array(terminal) + 0.
            max_q_t_plus_1 = np.max(q_t_plus_1, axis=1)

            # targetQ(t) = terminal ? reward : gamma*Q(t+1)+reward
            target_q_t = (1. - terminal) * self.discount * max_q_t_plus_1 + reward

        #if self.env.debug:
        #    action1hot, q_acted, delta = self.sess.run([self.action_one_hot, self.q_acted, self.delta], {
        #        self.target_q_t: target_q_t,
        #        self.action: action,
        #        self.s_t: s_t,
        #        self.learning_rate_step: self.step
        #    })
        #    for i in range(len(s_t)):
        #        print(reward[i], terminal[i], self.get_list_of_actions_from_index(action[i]), target_q_t[i], " - ", ''.join(str(int(j)) for j in s_t[i]), " - ", ''.join(str(int(j)) for j in s_t_plus_1[i]))
        #        q_vals_plus_1 = self.q.eval({self.s_t: [s_t_plus_1[i]]})
        #        q_vals = self.q.eval({self.s_t: [s_t[i]]})
        #        epsilon = self.learning_rate_op.eval({self.learning_rate_step: self.step})
        #        print('Q_vals(t)', q_vals)
        #        print('Q_vals(t+1)', q_vals_plus_1)
        #        print('----')
        #        print('delta', delta[i])
        #        print(epsilon)


        if self.config.sampling == 'ups':
            q_t = self.sess.run([self.q], {self.s_t: s_t})[0]
            #print(len(q_t), q_t)
            #q_t=q_t[0]
            #print(len(q_t), q_t, q_t.shape)
            weights = [0]*len(target_q_t)

            for i in range(len(target_q_t)):
                weights[i] = abs(target_q_t[i]-q_t[i,action[i]])
                #if self.step % 2000 == 0 or terminal[i] or reward[i] != 0: # or reward[i] != 0
                #    print(terminal[i], reward[i], '    ', target_q_t[i],' - ',q_t[i,action[i]],' = ',weights[i])

            #print(weights)
            #print(abs(target_q_t))
            #weights = abs(target_q_t)
            weights /= sum(weights)
            resample_indexes = np.random.choice(self.config.batch_size, self.config.batch_size, p=weights)

            learning_rates = np.empty(target_q_t.shape)
            s_t_resampled = np.empty(s_t.shape)
            action_resampled = np.empty(action.shape)
            target_q_t_resampled = np.empty(target_q_t.shape)
            for i in range(self.config.batch_size):
                learning_rates[i] = (self.config.learning_rate/self.config.batch_size) / weights[resample_indexes[i]]
                s_t_resampled[i] = s_t[resample_indexes[i]]
                action_resampled[i] = action[resample_indexes[i]]
                target_q_t_resampled[i] = target_q_t[resample_indexes[i]]
        elif self.config.sampling == 'aups':
            epsilon = self.get_epsilon()

            q_t = self.sess.run([self.q], {self.s_t: s_t})[0]
            weights = [0]*len(target_q_t)
            for i in range(len(target_q_t)):
                weights[i] = abs(target_q_t[i]-q_t[i,action[i]])**epsilon

            #weights = weights**epsilon
            #weights = abs(target_q_t)**epsilon
            weights /= sum(weights)
            resample_indexes = np.random.choice(self.config.batch_size, self.config.batch_size, p=weights)

            learning_rates = np.empty(target_q_t.shape)
            s_t_resampled = np.empty(s_t.shape)
            action_resampled = np.empty(action.shape)
            target_q_t_resampled = np.empty(target_q_t.shape)
            for i in range(self.config.batch_size):
                learning_rates[i] = (self.config.learning_rate/self.config.batch_size) / (1.0/weights[resample_indexes[i]])
                s_t_resampled[i] = s_t[resample_indexes[i]]
                action_resampled[i] = action[resample_indexes[i]]
                target_q_t_resampled[i] = target_q_t[resample_indexes[i]]
        elif self.config.sampling == 'ps':
            print('not yet implemented: PS')
        else:
            learning_rates = [self.config.learning_rate]*self.config.batch_size
            s_t_resampled = s_t
            action_resampled = action
            target_q_t_resampled = target_q_t

        if self.env.debug:
            for i in range(len(s_t)):
                print(self.get_list_of_actions_from_index(action_resampled[i]), target_q_t_resampled[i], " - ", ''.join(str(int(j)) for j in s_t_resampled[i]))

        _, q_t, loss, summary_str = self.sess.run([self.optim, self.q, self.loss, self.q_summary], {
            self.target_q_t: target_q_t_resampled,
            self.action: action_resampled,
            self.s_t: s_t_resampled,
            self.learning_rate_step: self.step,
            self.tuned_learning_rate: learning_rates,
        })

        #self.writer.add_summary(summary_str, self.step)
        self.total_loss += loss
        self.total_q += q_t.mean()
        self.update_count += 1

    def build_dqn(self):
        self.w = {}     # weights
        self.t_w = {}   # target weights

        hl_size = 50
        if self.jal:
            output_size = self.env.action_size**self.config.noa
        else:
            output_size = self.env.action_size

        # training network
        with tf.variable_scope('prediction'):
            self.s_t = tf.placeholder('float32', [None, self.env.input_size], name='s_t')
            self.l1, self.w['l1_w'], self.w['l1_b'] = linear(self.s_t, hl_size, activation_fn=tf.nn.relu, name='l1')
            self.l2, self.w['l2_w'], self.w['l2_b'] = linear(self.l1, hl_size, activation_fn=tf.nn.relu, name='l2')
            #self.l3, self.w['l3_w'], self.w['l3_b'] = linear(self.l2, hl_size, activation_fn=tf.nn.relu, name='l3')
            #self.l4, self.w['l4_w'], self.w['l4_b'] = linear(self.l3, hl_size, activation_fn=tf.nn.relu, name='l4')
            self.q, self.w['q_w'], self.w['q_b'] = linear(self.l2, output_size, name='q')

            self.q_action = tf.argmax(self.q, dimension=1)

            q_summary = []
            avg_q = tf.reduce_mean(self.q, 0)
            for idx in range(output_size):
                q_summary.append(tf.histogram_summary('q/%s' % idx, avg_q[idx]))
            self.q_summary = tf.merge_summary(q_summary, 'q_summary')

        # target network
        with tf.variable_scope('target'):
            self.target_s_t = tf.placeholder('float32', [None, self.env.input_size], name='target_s_t')
            self.target_l1, self.t_w['l1_w'], self.t_w['l1_b'] = linear(self.target_s_t, hl_size, activation_fn=tf.nn.relu, name='target_l1')
            self.target_l2, self.t_w['l2_w'], self.t_w['l2_b'] = linear(self.target_l1, hl_size, activation_fn=tf.nn.relu, name='target_l2')
            #self.target_l3, self.t_w['l3_w'], self.t_w['l3_b'] = linear(self.target_l2, hl_size, activation_fn=tf.nn.relu, name='target_l3')
            #self.target_l4, self.t_w['l4_w'], self.t_w['l4_b'] = linear(self.target_l3, hl_size, activation_fn=tf.nn.relu, name='target_l4')
            self.target_q, self.t_w['q_w'], self.t_w['q_b'] = linear(self.target_l2, output_size, name='target_q')

            self.target_q_idx = tf.placeholder('int32', [None, None], 'outputs_idx')
            self.target_q_with_idx = tf.gather_nd(self.target_q, self.target_q_idx)

        # to copy training to target networks
        with tf.variable_scope('pred_to_target'):
            self.t_w_input = {}
            self.t_w_assign_op = {}

            for name in self.w.keys():
                self.t_w_input[name] = tf.placeholder('float32', self.t_w[name].get_shape().as_list(), name=name)
                self.t_w_assign_op[name] = self.t_w[name].assign(self.t_w_input[name])

        # optimizer
        with tf.variable_scope('optimizer'):
            # tensor to hold target Q-value
            self.target_q_t = tf.placeholder('float32', [None], name='target_q_t')

            # tensors for action, for action matrix and for Q-value deltas
            # eg, action='2', action_one_hot=[0,0,1,0], q_acted=[0,0,Q,0]
            self.action = tf.placeholder('int64', [None], name='action')
            self.action_one_hot = tf.one_hot(self.action, output_size, 1.0, 0.0, name='action_one_hot')
            self.q_acted = tf.reduce_sum(self.q * self.action_one_hot, reduction_indices=1, name='q_acted')

            # delta
            self.delta = self.target_q_t - self.q_acted
            # print(self.delta)
            # self.clipped_delta = tf.clip_by_value(self.delta, self.min_delta, self.max_delta, name='clipped_delta')

            # self.global_step = tf.Variable(0, trainable=False)

            # clip delta. quadratic if [-1, 1] and linear otherwise
            # self.loss = tf.reduce_mean(tf.select(tf.logical_and(tf.less(self.delta, 1), tf.greater(self.delta, -1)),
            #                                      tf.square(self.delta), tf.abs(self.delta)), name='loss')
            self.loss = tf.reduce_mean(tf.square(self.delta), name='loss')

            self.tuned_learning_rate = tf.placeholder('float32', [None], name='tuned_learning_rate')
            self.tuned_lr_min = tf.reduce_mean(self.tuned_learning_rate)
            self.learning_rate_step = tf.placeholder('int64', None, name='learning_rate_step')
            self.learning_rate_op = tf.maximum(self.learning_rate_minimum,
                                               tf.train.exponential_decay(
                                                   self.learning_rate,
                                                   self.learning_rate_step,
                                                   self.learning_rate_decay_step,
                                                   self.learning_rate_decay,
                                                   staircase=True))

            # self.opt = tf.train.AdamOptimizer(self.learning_rate_op)
            # self.gradients = self.opt.compute_gradients(self.loss)

            # clip gradient tensors
            # self.clipped_grad = []
            # for gv in self.gradients:
            #    if gv[0] is not None:
            #        self.clipped_grad.append((tf.clip_by_value(gv[0], self.min_delta, self.max_delta), gv[1]))

            # self.clipped_grad = tf.clip_by_value(self.gradients, self.min_delta, self.max_delta, name='clipped_grad')
            # self.optim = self.opt.apply_gradients(self.clipped_grad)

            #self.optim = tf.train.AdamOptimizer(self.tuned_lr_min).minimize(self.loss)
            self.optim = tf.train.AdamOptimizer(self.learning_rate_op).minimize(self.loss)

        # probably statistics?
        with tf.variable_scope('summary'):
            scalar_summary_tags = ['average.reward', 'average.loss', 'average.q',
                                   'episode.max reward', 'episode.min reward', 'episode.avg reward',
                                   'episode.num of game', 'training.learning_rate']

            self.summary_placeholders = {}
            self.summary_ops = {}

            for tag in scalar_summary_tags:
                self.summary_placeholders[tag] = tf.placeholder('float32', None, name=tag.replace(' ', '_'))
                self.summary_ops[tag] = tf.scalar_summary("%s-%s/%s" % (self.env_name, self.env_type, tag),
                                                          self.summary_placeholders[tag])

            histogram_summary_tags = ['episode.rewards', 'episode.actions']

            for tag in histogram_summary_tags:
                self.summary_placeholders[tag] = tf.placeholder('float32', None, name=tag.replace(' ', '_'))
                self.summary_ops[tag] = tf.histogram_summary(tag, self.summary_placeholders[tag])

            #self.writer = tf.train.SummaryWriter('./logs/%s' % self.model_dir, self.sess.graph)

        tf.initialize_all_variables().run()

        #self._saver = tf.train.Saver(list(self.w.values()) + [self.step_op], max_to_keep=30)
        self._saver = tf.train.Saver(list(self.w.values()), max_to_keep=30)

        self.load_model()
        self.update_target_q_network()

    def update_target_q_network(self):
        for name in self.w.keys():
            self.t_w_assign_op[name].eval({self.t_w_input[name]: self.w[name].eval()})

    def save_weight_to_pkl(self):
        if not os.path.exists(self.weight_dir):
            os.makedirs(self.weight_dir)

        for name in self.w.keys():
            save_pkl(self.w[name].eval(), os.path.join(self.weight_dir, "%s.pkl" % name))

    def load_weight_from_pkl(self, cpu_mode=False):
        with tf.variable_scope('load_pred_from_pkl'):
            self.w_input = {}
            self.w_assign_op = {}

            for name in self.w.keys():
                self.w_input[name] = tf.placeholder('float32', self.w[name].get_shape().as_list(), name=name)
                self.w_assign_op[name] = self.w[name].assign(self.w_input[name])

        for name in self.w.keys():
            self.w_assign_op[name].eval({self.w_input[name]: load_pkl(os.path.join(self.weight_dir, "%s.pkl" % name))})

        self.update_target_q_network()

    def inject_summary(self, tag_dict, step):
        # print(tag_dict["episode.actions"])
        summary_str_lists = self.sess.run([self.summary_ops[tag] for tag in tag_dict.keys()], {
            self.summary_placeholders[tag]: value for tag, value in tag_dict.items()
            })
        for summary_str in summary_str_lists:
            #self.writer.add_summary(summary_str, self.step)
            pass

    def play(self, n_step=200, n_episode=10, test_ep=None, render=False):
        #self.env.debug = True
        #n_episode = 1
        if not self.config.load: # override loading settings. we are playing, so we load
            self.config.load = True
            self.load_model()

        # self.debug_run()

        if test_ep is None:
            test_ep = self.ep_end


        test_history = History(self.config, self.env.input_size)

        best_reward, best_idx = 0, 0
        rewards = []
        steps = []
        for idx in range(n_episode):
            screen, reward, action, terminal = self.env.new_random_game()
            current_reward = 0

            for _ in range(self.history_length):
                if self.jal:
                    test_history.add(screen)
                else:
                    for i in range(self.config.noa):
                        test_history.add(screen[i])

            for _ in tqdm(range(n_step), ncols=70):
                # 1. predict
                action = self.predict(test_history.get(), test_ep)
                # 2. act
                screen, reward, terminal = self.env.act(action, is_training=False)
                # 3. observe
                if self.jal:
                    test_history.add(screen)
                else:
                    for i in range(self.config.noa):
                        test_history.add(screen[i])

                current_reward += reward
                if terminal:
                    steps.append(_)
                    break

            rewards.append(current_reward)
            if current_reward > best_reward:
                best_reward = current_reward
                best_idx = idx

            #print("=" * 30)
            #print(" [%d] Best reward : %d" % (best_idx, best_reward))
            #print("=" * 30)

        print('Best',best_reward,', ratio of',rewards.count(best_reward)/len(rewards),', avg of', np.mean(rewards))
        self.env.close()
        return best_reward,rewards.count(best_reward)/len(rewards), np.mean(rewards), np.mean(steps), np.std(steps), rewards, steps






