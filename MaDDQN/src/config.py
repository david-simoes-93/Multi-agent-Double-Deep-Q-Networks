class AgentConfig(object):
    display = False                 # GUI

    max_step = 200000               # total training steps
    memory_size = 5000              # replay memory size

    batch_size = 32                 # mini-batch size
    random_start = 0                # steps to randomly wait
    # cnn_format = 'NCHW'             # batch size, history size, y, x
    discount = 0.9                  # future rewards discount (gamma)
    target_q_update_step = 500      # steps to update networks (tau)

    # alpha
    learning_rate =         0.001             # learning rate (alpha) 0.001
    learning_rate_minimum = 0.001     # learning rate min val 0.0001
    learning_rate_decay = 1             # dont decay
    learning_rate_decay_step = 1    # dont decay

    # epsilon
    ep_end = 0.1                    # exploration rate min val
    ep_start = 1.                   # exploration rate
    ep_end_t = 150000               # anneal steps

    history_length = 1              # history
    train_frequency = 1             # skip frames
    learn_start = 500               # when to start learning

    min_delta = -10000                  # target clipping (delta)
    max_delta = 10000                   # target clipping (delta)

    double_q = True                # Double DQN
    dueling = False                 # Dueling DQN

    _test_step = 2000               # steps to test learning
    _save_step = _test_step * 100   # steps to save

    sampling = 'uni'                # sampling type: uni, aups, ups, ps


class EnvironmentConfig(object):
    env_name = 'FF'                 # Forager Framework
    noa = 2                         # number of agents
    nob = 2                         # number of berries

    jal = False                     # JAL or IL
    independent_rewards = False     # split rewards

    gui = True

    load = False
    load_suffix = ''
    save_suffix = ''
    load_prefix = ''
    save_prefix = ''

    screen_width = 7                # max x
    screen_height = 7               # map y
    max_reward = 10000.             # reward clipping
    min_reward = -10000.            # reward clipping


class DQNConfig(AgentConfig, EnvironmentConfig):
    model = ''                      # ?
    pass


class M1(DQNConfig):
    backend = 'tf'                  # tensor flow
    env_type = 'ff'                 # ff (Foraging), or prst (pursuit)
    action_repeat = 1               # repeat action

def get_config(FLAGS):
    if FLAGS.model == 'm1':
        config = M1

    for k, v in FLAGS.__dict__['__flags'].items():
        if k == 'gpu':
            if v == False:
                config.cnn_format = 'NHWC' # Number_of_samples, Height, Width, Context
            else:
                config.cnn_format = 'NCHW' # Number_of_samples, Context, Height, Width

        if hasattr(config, k):
            setattr(config, k, v)

    return config
