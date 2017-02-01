import random
import tensorflow as tf


from dqn.agent import Agent
from ff.ffEnvironment import FfEnvironment
from prst.prstEnvironment import PrstEnvironment
from config import get_config

flags = tf.app.flags

# Model
flags.DEFINE_string('model', 'm1', 'Type of model')
flags.DEFINE_boolean('dueling', False, 'Whether to use dueling deep q-network')
flags.DEFINE_boolean('double_q', True, 'Whether to use double q-learning')

# Etc
flags.DEFINE_boolean('use_gpu', False, 'Whether to use gpu or not')
flags.DEFINE_string('gpu_fraction', '1/1', 'idx / # of gpu fraction e.g. 1/3, 2/3, 3/3')
flags.DEFINE_boolean('display', False, 'Whether to do display the game screen or not')
flags.DEFINE_boolean('is_train', True, 'Whether to do training or testing')
flags.DEFINE_integer('random_seed', 123, 'Value of random seed')

FLAGS = flags.FLAGS

# Set random seed
tf.set_random_seed(FLAGS.random_seed)
random.seed(FLAGS.random_seed)

if FLAGS.gpu_fraction == '':
    raise ValueError("--gpu_fraction should be defined")


def calc_gpu_fraction(fraction_string):
    idx, num = fraction_string.split('/')
    idx, num = float(idx), float(num)

    fraction = 1 / (num - idx + 1)
    print(" [*] GPU : %.4f" % fraction)
    return fraction


def main(_):
    gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=calc_gpu_fraction(FLAGS.gpu_fraction))
    config = get_config(FLAGS) or FLAGS

    steps=[]
    q_vals=[]
    with tf.Session(config=tf.ConfigProto(gpu_options=gpu_options)) as sess:
        if len(_) > 1:
            # print('train', _[1], 'gui',_[2],'gen_test',_[3],'gen_frac',_[4],'nob',_[5], 'load_prefix',_[6])
            FLAGS.is_train = _[1] == 'True'     # True
            config.gui = _[2] == 'True'         # False
            config.gen_test = _[3] == 'True'    # True
            config.max_step = int(_[4])         # 200 000
            config.ep_end_t = int(_[5])         # 150 000
            config.gen_frac = float(_[6])       # 0.001
            config.nob = int(_[7])              # 2
            config.noa = int(_[8])              # 2
            config.load_prefix = _[9]           # ./
            config.save_prefix = _[10]          # ./
            config.sampling = _[11]             # aups

        # save values for each NoB and NoA
        config.save_suffix = str(config.nob)+'+'+str(config.noa)
        config.load_suffix = str(config.nob)+'+'+str(config.noa)

        # if gen Testing, then load values is true
        if hasattr(config, 'gen_test') and config.gen_test:
            config.load = True
            if FLAGS.is_train:
                config.max_step = int(config.max_step*config.gen_frac)
                config.ep_end_t = int(config.ep_end_t*config.gen_frac)
                # if generalizing berries
                config.load_suffix = str(config.nob-1)+'+'+str(config.noa)
                # else if generalizing agents
                #config.load_suffix = str(config.nob)+'+'+str(config.noa-1)

        # load proper environment and agent
        if config.env_type is 'ff':
            env = FfEnvironment(config)
        elif config.env_type is 'prst':
            env = PrstEnvironment(config)
        # the name Agent is unfortunate... it is more of a manager
        agent = Agent(config, env, sess)

        # train or play
        if FLAGS.is_train:
            steps, q_vals, avg_ep_rewards, games_per_epoch = agent.train()
            with open("output.txt", "a") as myfile:
                myfile.write(str([_,config.max_step,config.ep_end_t,config.jal,config.env_type]))
                myfile.write(str(steps))
                myfile.write(str(q_vals))
                myfile.write(str(avg_ep_rewards))
                myfile.write(str(games_per_epoch))
                myfile.write('\n')
        else:
            best, ratio, mean, steps, steps_std, rewards, steps_list = agent.play()
            with open("output.txt", "a") as myfile:
                myfile.write(str(_))
                myfile.write(str(rewards))
                myfile.write(str(steps_list))
                myfile.write(str((best,ratio,mean, steps, steps_std)))
                myfile.write('\n')

    print('Done')

if __name__ == '__main__':
    tf.app.run()
