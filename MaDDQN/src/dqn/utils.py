import time
import pickle
import tensorflow as tf

# executes 'f', prints how long it took, returns f's return values
def timeit(f):
  def timed(*args, **kwargs):
    start_time = time.time()
    result = f(*args, **kwargs)
    end_time = time.time()

    print("   [-] %s : %2.5f sec" % (f.__name__, end_time - start_time))
    return result
  return timed

# returns a string with current time
def get_time():
  return time.strftime("%Y-%m-%d_%H:%M:%S", time.gmtime())

#
@timeit
def save_pkl(obj, path):
  with open(path, 'w') as f:
    pickle.dump(obj, f)
    print("  [*] save %s" % path)

@timeit
def load_pkl(path):
  with open(path) as f:
    obj = pickle.load(f)
    print("  [*] load %s" % path)
    return obj

@timeit
def save_npy(obj, path):
  np.save(path, obj)
  print("  [*] save %s" % path)

@timeit
def load_npy(path):
  obj = np.load(path)
  print("  [*] load %s" % path)
  return obj
