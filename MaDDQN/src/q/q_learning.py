from ff.ffEnvironment import FfEnvironment
from prst.prstEnvironment import PrstEnvironment
import numpy as np
from tqdm import tqdm
import random
import matplotlib.pyplot as plt
import time
import os
import signal
import subprocess

class q_config(object):
    def __init__(self):
        self.jal = True
        self.noa = 2
        self.nob = 2
        self.independent_rewards = False
        self.screen_width = 7
        self.screen_height = 7
        self.max_training_steps = 100000
        self.anneal_training_steps = self.max_training_steps*0.75

def get_list_of_actions_from_index(index, env):
    actions = [0]*env.noa
    for x in range(len(actions)):
        actions[x] = int(index % env.numberOfActions)
        index = int((index - actions[x]) / env.numberOfActions)
    return actions

def get_index_from_list_of_actions(actions, env):
    return int(sum(env.numberOfActions ** i * j for i, j in enumerate(actions, 0)))

def predict(s_t, q_table, env, step, end_steps, test_ep=None):
    start_eps = 1.0
    end_eps = 0.1
    learn_start_steps = 500
    ep = test_ep or (end_eps + max(0., (start_eps - end_eps) * (end_steps - max(0., step - learn_start_steps)) / end_steps))

    actions = []
    if random.random() < ep:
        for _ in range(env.noa):
            actions.append(random.randrange(env.action_size))
        #print(":",actions, env.action_size)
    else:
        state_key = add_state_if_necessary(s_t, q_table, env)

        if env.jal:
            index = np.argmax(q_table[state_key])
            actions = get_list_of_actions_from_index(index, env)
            #print("+",actions)
        else:
            actions = [0]*env.noa
            for i in range(env.noa):
                actions[i] = np.argmax(q_table[state_key[i]])
            #print("-",actions)

    #if step>end_steps:
    #    print(names[actions[0]], names[actions[1]])
    return actions

def turn_state_to_key(screen, config, env):
    if config.jal:
        size = env.numberOfMapCells*config.screen_width*config.screen_height
        ret_val = ""

        for agent in range(int(len(screen)/size)):
            part_screen = screen[agent*size:(agent+1)*size]
            ret_val += '\n'.join(' '.join(''.join( \
                str(int(part_screen[i+j*env.numberOfMapCells+x*env.numberOfMapCells*config.screen_width])) \
                    for i in range(env.numberOfMapCells) ) \
                               for j in range(config.screen_width) ) \
                                for x in range(config.screen_height) ) + '\n'

        return ret_val
    else:
        #print(screen)
        return '\n'.join(' '.join(''.join( \
            str(int(screen[i+j*env.numberOfMapCells+x*env.numberOfMapCells*config.screen_width])) \
                for i in range(env.numberOfMapCells) ) \
                    for j in range(config.screen_width) ) \
                         for x in range(config.screen_height) )

def add_state_if_necessary(screen, q_table, env):
    if env.jal:
        currstate_key = turn_state_to_key(screen, config, env)
        if currstate_key not in q_table.keys():
            q_table[currstate_key]=[0]*env.numberOfActions**env.noa
        return currstate_key
    else:
        currstate_keys = []
        for i in range(env.noa):
            currstate_key = turn_state_to_key(screen[i], config, env)
            if currstate_key not in q_table.keys():
                q_table[currstate_key]=[0]*env.numberOfActions**env.noa
            currstate_keys.append(currstate_key)
        return currstate_keys

def debug_run_prst(jal, step, gui, plt, q, learning_test_steps, learning_test_values, env):
    states_text = ["(see ((prey 1) 3 3) ((prey 2) 3 2) ((predator 1) 0 0) ((predator 2) 0 1)) ",
                   "(see ((prey 1) 3 -3) ((prey 2) 3 3) ((predator 1) 0 0) ((predator 2) 0 1)) ",
                   "(see ((prey 1) 3 3) ((prey 2) 3 -3) ((predator 1) 0 0) ((predator 2) 0 1)) ",
                   "(see ((prey 1) 3 -2) ((prey 2) 3 -3) ((predator 1) 0 0) ((predator 2) -1 2)) ",
                   "(see ((prey 1) 3 -3) ((prey 2) 3 -2) ((predator 1) 0 0) ((predator 2) -2 2)) ",
                   "(see ((prey 1) 2 -2) ((prey 2) 3 -2) ((predator 1) 0 0) ((predator 2) -3 3)) ",
                   "(see ((prey 1) 2 -2) ((prey 2) 2 -1) ((predator 1) 0 0) ((predator 2) 3 -3)) ",
                   "(see ((prey 1) 1 -3) ((prey 2) 1 -2) ((predator 1) 0 0) ((predator 2) 2 -2)) ",
                   "(see ((prey 1) 3 -3) ((predator 1) 0 0) ((predator 2) 2 -2)) ",
                   "(see ((prey 1) -3 -2) ((predator 1) 0 0) ((predator 2) 3 -3)) ",
                   "(see ((prey 1) -2 -1) ((predator 1) 0 0) ((predator 2) -3 -3)) ",
                   "(see ((prey 1) -2 -1) ((predator 1) 0 0) ((predator 2) -2 -2)) ",
                   "(see ((prey 1) -2 -1) ((predator 1) 0 0) ((predator 2) -2 -2)) ",
                   "(see ((prey 1) -2 -1) ((predator 1) 0 0) ((predator 2) -2 -2)) "]
    states_text1 = ["(see ((prey 1) 3 2) ((prey 2) 3 1) ((predator 1) 0 -1) ((predator 2) 0 0)) ",
                    "(see ((prey 1) 3 3) ((prey 2) 3 2) ((predator 1) 0 -1) ((predator 2) 0 0)) ",
                    "(see ((prey 1) 3 2) ((prey 2) 3 3) ((predator 1) 0 -1) ((predator 2) 0 0)) ",
                    "(see ((prey 1) -3 3) ((prey 2) -3 2) ((predator 1) 1 -2) ((predator 2) 0 0)) ",
                    "(see ((prey 1) -2 2) ((prey 2) -2 3) ((predator 1) 2 -2) ((predator 2) 0 0)) ",
                    "(see ((prey 1) -2 2) ((prey 2) -1 2) ((predator 1) 3 -3) ((predator 2) 0 0)) ",
                    "(see ((prey 1) -1 1) ((prey 2) -1 2) ((predator 1) -3 3) ((predator 2) 0 0)) ",
                    "(see ((prey 1) -1 -1) ((prey 2) -1 0) ((predator 1) -2 2) ((predator 2) 0 0)) ",
                    "(see ((prey 1) 1 -1) ((predator 1) -2 2) ((predator 2) 0 0)) ",
                    "(see ((prey 1) 1 1) ((predator 1) -3 3) ((predator 2) 0 0)) ",
                    "(see ((prey 1) 1 2) ((predator 1) 3 3) ((predator 2) 0 0)) ",
                    "(see ((prey 1) 0 1) ((predator 1) 2 2) ((predator 2) 0 0)) ",
                    "(see ((prey 1) 0 1) ((predator 1) 2 2) ((predator 2) 0 0)) ",
                    "(see ((prey 1) 0 1) ((predator 1) 2 2) ((predator 2) 0 0)) "]

    states = []
    for s in states_text:
        states.append(PrstEnvironment.convert_state_to_1_out_of_n(s, env.width, env.height, env.numberOfMapCells))
    states1 = []
    for s in states_text1:
        states1.append(PrstEnvironment.convert_state_to_1_out_of_n(s, env.width, env.height, env.numberOfMapCells))

    avg_max = 0
    for x in range(len(states_text)):
        #print("....")
        #print(add_state_if_necessary(states[x]+states1[x], q, env))
        #exit()
        if jal:
            avg_max += max(q[add_state_if_necessary(states[x]+states1[x], q, env)])
        else:
            avg_max += max(q[add_state_if_necessary([states[x], states1[x]], q, env)[0]])

    learning_test_steps.append(step)
    learning_test_values.append(avg_max/len(states_text))
    if gui:
        plt.clf()
        plt.plot(learning_test_steps, learning_test_values)
        plt.pause(0.001)

def debug_run_ff(config, jal, step, gui, plt, q, learning_test_steps, learning_test_values, env):
    screen, reward, action, terminal = env.new_game('tst')
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

    avg_max = 0
    for x in range(len(list_of_actions)):
        if jal:
            avg_max += max(q[add_state_if_necessary(screen, q, env)])
        else:
            avg_max += max(q[add_state_if_necessary(screen, q, env)[0]])

        action = list_of_actions[counter]
        counter += 1
        screen, reward, terminal = env.act(action)

        if terminal:
            break

    learning_test_steps.append(step)
    learning_test_values.append(avg_max/counter)
    if gui:
        plt.clf()
        plt.plot(learning_test_steps, learning_test_values)
        plt.pause(0.001)


def train_several(n_epochs):
    for runs in range(n_epochs):
        p = subprocess.Popen('../../../Pursuit/start2.sh',
                             stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
        time.sleep(5)
        train_once({}, runs)
        os.killpg(os.getpgid(p.pid), signal.SIGTERM)
        time.sleep(1)

def train_once(q_table, runs):
    #env = FfEnvironment(config)
    env = PrstEnvironment(config)
    screen, reward, action, terminal = env.new_random_game()
    prevstate_key = add_state_if_necessary(screen, q_table, env)
    term = False
    iteration_steps = 0

    steps_per_game = []
    last_debug = 0

    history = np.empty([5000,5], dtype=object)
    history_index = 0

    learning_test_steps=[0]
    learning_test_values=[0]
    if gui:
        plt.ion()
        plt.show()
        plt.plot(learning_test_steps, learning_test_values)
        plt.pause(0.001)

    for step in tqdm(range(0, config.max_training_steps), ncols=70, initial=0):
        actions = predict(screen, q_table, env, step, config.anneal_training_steps)

        env.act(actions)
        screen, reward, term = env.state
        currstate_key = add_state_if_necessary(screen, q_table, env)
        #print("----")
        #print(screen, currstate_key)
        if config.jal:
            history[history_index]=[prevstate_key, currstate_key, reward, term, get_index_from_list_of_actions(actions, env)]
        else:
            history[history_index]=[prevstate_key, currstate_key, reward, term, actions]
        #print(history[0])
        history_index = (history_index+1)%len(history)
        prevstate_key = currstate_key

        if step>500:
            indexes = random.sample(range(0, min(len(history),step)), 32)

            for i in indexes:
                prevstate_key_i = history[i][0]
                currstate_key_i = history[i][1]
                reward_i = history[i][2]
                term_i = history[i][3]
                actions_index_i = history[i][4]

                if config.jal:
                    if term_i:
                        q_table[prevstate_key_i][actions_index_i] = q_table[prevstate_key_i][actions_index_i]*(1.0-learning_rate) + learning_rate*reward_i
                    else:
                        q_table[prevstate_key_i][actions_index_i] = q_table[prevstate_key_i][actions_index_i]*(1.0-learning_rate) + \
                                                      learning_rate*(reward_i+future_reward_rate*np.max(q_table[currstate_key_i]))
                else:
                    for agent in range(config.noa):
                        if term_i:
                            q_table[prevstate_key_i[agent]][actions_index_i[agent]] = q_table[prevstate_key_i[agent]][actions_index_i[agent]]*(1.0-learning_rate) + learning_rate*reward_i
                        else:
                            q_table[prevstate_key_i[agent]][actions_index_i[agent]] = q_table[prevstate_key_i[agent]][actions_index_i[agent]]*(1.0-learning_rate) + \
                                                                        learning_rate*(reward_i+future_reward_rate*np.max(q_table[currstate_key_i[agent]]))

        iteration_steps += 1
        if term or iteration_steps > env.max_steps:
            if step > last_debug + 2000:
                #debug_run_ff(config, config.jal, step, gui, plt, q_table, learning_test_steps, learning_test_values, env)
                debug_run_prst(config.jal, step, gui, plt, q_table, learning_test_steps, learning_test_values, env)
                last_debug = step

            screen, reward, action, terminal = env.new_random_game()
            prevstate_key = add_state_if_necessary(screen, q_table, env)
            steps_per_game.append(iteration_steps)
            iteration_steps = 0
        if step % 2000 == 2000-1:
            if len(steps_per_game)>0:
                print(np.mean(steps_per_game), len(q_table.keys()))
            steps_per_game = []

    env.close()
    file_name = 'q_table'+str(config.screen_height)+'x'+str(config.screen_width)+'-'+str(runs)+'x'+str(step)+'.npy'
    np.save(file_name, q_table)
    print(file_name, learning_test_steps, learning_test_values)

def play(file_name, n_step=200, n_episode=1000, test_ep=0.05):
    # Load
    q_table = np.load(file_name).item()

    best_reward, best_idx = 0, 0
    rewards = []
    steps = []
    #env = FfEnvironment(config)
    env = PrstEnvironment(config)

    for idx in range(n_episode):
        screen, reward, action, terminal = env.new_random_game()
        current_reward = 0

        for _ in tqdm(range(n_step), ncols=70):
            # 1. predict
            action = predict(screen, q_table, env, 0, 0, test_ep)
            # 2. act
            screen, reward, terminal = env.act(action)

            current_reward += reward
            if terminal:
                steps.append(_)
                break

        rewards.append(current_reward)
        if current_reward > best_reward:
            best_reward = current_reward

    print('Best',best_reward,', ratio of',rewards.count(best_reward)/len(rewards),', avg of', np.mean(rewards))
    env.close()
    return best_reward,rewards.count(best_reward)/len(rewards), np.mean(rewards), np.mean(steps), np.std(steps), rewards, steps


#           0       1       2       3       4     5       6
names = ['NOTHING', 'UP','DOWN','LEFT','RIGHT','GRAB','RELEASE']
config = q_config()
gui = True
future_reward_rate = 0.9
learning_rate = 0.5

# JAL average reward
train_several(5)

p = subprocess.Popen('../../../Pursuit/start2.sh',
                     stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
time.sleep(5)
play('q_table7x7-0x199999.npy')
os.killpg(os.getpgid(p.pid), signal.SIGTERM)
time.sleep(1)

# jal train once
#train_once({}, 0)
#play('q_table7x7-0x199999.npy')

# jal generalize to 1 more berry
config.nob = 3
config.max_training_steps = 20000

p = subprocess.Popen('../../../Pursuit/start3.sh',
                     stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
time.sleep(5)
train_once(np.load('q_table7x7-0x199999.npy').item(),10)
os.killpg(os.getpgid(p.pid), signal.SIGTERM)
time.sleep(1)

p = subprocess.Popen('../../../Pursuit/start3.sh',
                     stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
time.sleep(5)
play('q_table7x7-10x19999.npy')
os.killpg(os.getpgid(p.pid), signal.SIGTERM)
time.sleep(1)

# jal generalize to 1 more berry
config.nob = 4
config.max_training_steps = 20000
p = subprocess.Popen('../../../Pursuit/start4.sh',
                     stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
time.sleep(5)
train_once(np.load('q_table7x7-10x19999.npy').item(),11)
os.killpg(os.getpgid(p.pid), signal.SIGTERM)
time.sleep(1)

p = subprocess.Popen('../../../Pursuit/start4.sh',
                     stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
time.sleep(5)
play('q_table7x7-11x19999.npy')
os.killpg(os.getpgid(p.pid), signal.SIGTERM)
time.sleep(1)

# il
config.nob=2
config.max_training_steps = 200000
config.jal = False
train_several(5)

p = subprocess.Popen('../../../Pursuit/start2.sh',
                     stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
time.sleep(5)
play('q_table7x7-0x199999.npy')
os.killpg(os.getpgid(p.pid), signal.SIGTERM)
time.sleep(1)

#train_once({}, 0)
#play('q_table7x7-0x199999.npy')

# il generalize to 1 more berry
config.nob = 3
config.max_training_steps = 20000

p = subprocess.Popen('../../../Pursuit/start3.sh',
                     stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
time.sleep(5)
train_once(np.load('q_table7x7-0x199999.npy').item(),10)
os.killpg(os.getpgid(p.pid), signal.SIGTERM)
time.sleep(1)

p = subprocess.Popen('../../../Pursuit/start3.sh',
                     stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
time.sleep(5)
play('q_table7x7-10x19999.npy')
os.killpg(os.getpgid(p.pid), signal.SIGTERM)
time.sleep(1)

# il generalize to 1 more berry
config.nob = 4
config.max_training_steps = 20000
p = subprocess.Popen('../../../Pursuit/start4.sh',
                     stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
time.sleep(5)
train_once(np.load('q_table7x7-10x19999.npy').item(),11)
os.killpg(os.getpgid(p.pid), signal.SIGTERM)
time.sleep(1)

p = subprocess.Popen('../../../Pursuit/start4.sh',
                     stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
time.sleep(5)
play('q_table7x7-11x19999.npy')
os.killpg(os.getpgid(p.pid), signal.SIGTERM)
time.sleep(1)
