import time
import os
import signal
import subprocess

python_str = '/home/david/.virtualenvs/cv/bin/python main.py'
#python_str = '/root/.virtualenvs/tf/bin/python3 main.py'

# is_train, gui,    gen_test,   max_step,   ep_end_t,   gen_frac,   nob,    noa,    load_prefix     save_prefix,    sampling
# True      False   True        200000      150000      0.10        2       2       ./              gen0100/        us

def call_pursuit(cmd, pursuit_mod=''):
    p = subprocess.Popen('../../pursuit_package-0.9-ua3/start'+pursuit_mod+'.sh',
                         stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
    time.sleep(5)
    os.system(cmd)
    os.killpg(os.getpgid(p.pid), signal.SIGTERM)

# compare samplng
if False:
    for i in range(3):
        os.system(python_str+' True False False 150000 100000 0 2 2 ./ ./ us')

    for i in range(3):
        os.system(python_str+' True False False 150000 100000 0 2 2 ./ ./ ups')

    for i in range(3):
        os.system(python_str+' True False False 150000 100000 0 2 2 ./ ./ aups')
# train gen of agents for prst
elif False:
    for _ in range(3):
        # get baseline
        call_pursuit(python_str+' True True False 200000 150000 0 2 2 ./ ./ us')

        # train from baseline
        i = 3
        #os.system(python_str+' True False True 200000 150000 0.20 6 '+str(i)+' ./ genA0200/ us')   # 20% of training
        call_pursuit(python_str+' True False True 200000 150000 0.10 6 '+str(i)+' ./ genA0100/ us')   # 10% of training
        #call_pursuit(python_str+' True False True 200000 150000 0.075 6 '+str(i)+' ./ genA0075/ us')  # 7.5% of training
        call_pursuit(python_str+' True False True 200000 150000 0.05 6 '+str(i)+' ./ genA0050/ us')   # 5% of training
        call_pursuit(python_str+' True False True 200000 150000 0.025 6 '+str(i)+' ./ genA0025/ us')  # 2.5% of training

        for i in range(4, 6):
            #os.system(python_str+' True False True 200000 150000 0.20 6 '+str(i)+' genA0200/ genA0200/ us')   # 20% of training
            call_pursuit(python_str+' True False True 200000 150000 0.10 6 '+str(i)+' genA0100/ genA0100/ us')   # 10% of training
            #os.system(python_str+' True False True 200000 150000 0.075 6 '+str(i)+' genA0075/ genA0075/ us')  # 7.5% of training
            call_pursuit(python_str+' True False True 200000 150000 0.05 6 '+str(i)+' genA0050/ genA0050/ us')   # 5% of training
            call_pursuit(python_str+' True False True 200000 150000 0.025 6 '+str(i)+' genA0025/ genA0025/ us')  # 2.5% of training

        # test
        #for i in range(3, 6):
            #os.system(python_str+' False False True 1 1 1 6 '+str(i)+' genA0200/ ./ us')

        for i in range(3, 6):
            call_pursuit(python_str+' False False True 1 1 1 6 '+str(i)+' genA0100/ ./ us')

        #for i in range(3, 6):
        #    call_pursuit(python_str+' False False True 1 1 1 6 '+str(i)+' genA0075/ ./ us')

        for i in range(3, 6):
            call_pursuit(python_str+' False False True 1 1 1 6 '+str(i)+' genA0050/ ./ us')

        for i in range(3, 6):
            call_pursuit(python_str+' False False True 1 1 1 6 '+str(i)+' genA0025/ ./ us')
# compare IL or JAL
elif True:
    os.system(python_str+' True True False 200000 150000 0 2 2 ./ ./ us')
    os.system(python_str+' True False False 200000 150000 0 2 2 ./ ./ us')
    os.system(python_str+' True False False 200000 150000 0 2 2 ./ ./ us')
# compare IL or JAL for prst
elif True:
    for _ in range(1):
        call_pursuit(python_str+' True True False 500000 400000 0 2 2 ./ ./ us', pursuit_mod='2')
        call_pursuit(python_str+' False False True 1 1 1 2 2 ./ ./ us', pursuit_mod='2')
elif True: #gen tests for prst
    # get baseline
    call_pursuit(python_str+' True True False 200000 150000 0 2 2 ./ ./ us', pursuit_mod='2')

# train from baseline
    i = 3
    #call_pursuit(python_str+' True False True 200000 150000 0.20 '+str(i)+' 2 ./ gen0200/ us')   # 20% of training
    call_pursuit(python_str+' True False True 200000 150000 0.10 '+str(i)+' 2 ./ gen0100/ us', pursuit_mod=str(i))   # 10% of training
    #call_pursuit(python_str+' True False True 200000 150000 0.075 '+str(i)+' 2 ./ gen0075/ us')  # 7.5% of training
    call_pursuit(python_str+' True False True 200000 150000 0.05 '+str(i)+' 2 ./ gen0050/ us', pursuit_mod=str(i))   # 5% of training
    call_pursuit(python_str+' True False True 200000 150000 0.025 '+str(i)+' 2 ./ gen0025/ us', pursuit_mod=str(i))  # 2.5% of training

    for i in range(4, 11):
        #call_pursuit(python_str+' True False True 200000 150000 0.20 '+str(i)+' 2 gen0200/ gen0200/ us')   # 20% of training
        call_pursuit(python_str+' True False True 200000 150000 0.10 '+str(i)+' 2 gen0100/ gen0100/ us', pursuit_mod=str(i))   # 10% of training
        #call_pursuit(python_str+' True False True 200000 150000 0.075 '+str(i)+' 2 gen0075/ gen0075/ us')  # 7.5% of training
        call_pursuit(python_str+' True False True 200000 150000 0.05 '+str(i)+' 2 gen0050/ gen0050/ us', pursuit_mod=str(i))   # 5% of training
        call_pursuit(python_str+' True False True 200000 150000 0.025 '+str(i)+' 2 gen0025/ gen0025/ us', pursuit_mod=str(i))  # 2.5% of training

    # test
    #for i in range(3, 11):
    #    os.system(python_str+' False False True 1 1 1 '+str(i)+' 2 gen0200/ ./ us')

    call_pursuit(python_str+' False False True 1 1 1 2 2 ./ ./ us', pursuit_mod='2')

    for i in range(3, 11):
        call_pursuit(python_str+' False False True 1 1 1 '+str(i)+' 2 gen0100/ ./ us', pursuit_mod=str(i))

    #for i in range(3, 11):
    #    os.system(python_str+' False False True 1 1 1 '+str(i)+' 2 gen0075/ ./ us')

    for i in range(3, 11):
        call_pursuit(python_str+' False False True 1 1 1 '+str(i)+' 2 gen0050/ ./ us', pursuit_mod=str(i))

    for i in range(3, 11):
        call_pursuit(python_str+' False False True 1 1 1 '+str(i)+' 2 gen0025/ ./ us', pursuit_mod=str(i))
else: #gen tests for FF
    # get baseline
    os.system(python_str+' True False False 200000 150000 0 2 2 ./ ./ us')

    # train from baseline
    i = 3
    os.system(python_str+' True False True 200000 150000 0.20 '+str(i)+' 2 ./ gen0200/ us')   # 20% of training
    os.system(python_str+' True False True 200000 150000 0.10 '+str(i)+' 2 ./ gen0100/ us')   # 10% of training
    os.system(python_str+' True False True 200000 150000 0.075 '+str(i)+' 2 ./ gen0075/ us')  # 7.5% of training
    os.system(python_str+' True False True 200000 150000 0.05 '+str(i)+' 2 ./ gen0050/ us')   # 5% of training
    os.system(python_str+' True False True 200000 150000 0.025 '+str(i)+' 2 ./ gen0025/ us')  # 2.5% of training

    for i in range(4, 11):
        os.system(python_str+' True False True 200000 150000 0.20 '+str(i)+' 2 gen0200/ gen0200/ us')   # 20% of training
        os.system(python_str+' True False True 200000 150000 0.10 '+str(i)+' 2 gen0100/ gen0100/ us')   # 10% of training
        os.system(python_str+' True False True 200000 150000 0.075 '+str(i)+' 2 gen0075/ gen0075/ us')  # 7.5% of training
        os.system(python_str+' True False True 200000 150000 0.05 '+str(i)+' 2 gen0050/ gen0050/ us')   # 5% of training
        os.system(python_str+' True False True 200000 150000 0.025 '+str(i)+' 2 gen0025/ gen0025/ us')  # 2.5% of training

    # test
    for i in range(3, 11):
        os.system(python_str+' False False True 1 1 1 '+str(i)+' 2 gen0200/ ./ us')

    for i in range(3, 11):
        os.system(python_str+' False False True 1 1 1 '+str(i)+' 2 gen0100/ ./ us')

    for i in range(3, 11):
        os.system(python_str+' False False True 1 1 1 '+str(i)+' 2 gen0075/ ./ us')

    for i in range(3, 11):
        os.system(python_str+' False False True 1 1 1 '+str(i)+' 2 gen0050/ ./ us')

    for i in range(3, 11):
        os.system(python_str+' False False True 1 1 1 '+str(i)+' 2 gen0025/ ./ us')
