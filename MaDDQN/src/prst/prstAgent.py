#!/usr/bin/python3

numberOfMapCells = 4
moves = ["(move none)", "(move south)", "(move north)", "(move west)", "(move east)"]


def get_position(state, width, height):
    #(see ((prey 1) 0 -2) ((prey 2) -2 -1) ((predator 1) 0 0) ((predator 2) 3 3))  0
    #(see ((prey 1) -3 2) ((prey 2) 2 3) ((predator 1) -3 -3) ((predator 2) 0 0))  0

    rel_pos = []
    pos = 0

    main_pred = []
    my_index = 0
    observations = state[6:-3].split(') (')

    # find main_pred rel coords and self index
    index = 0
    for i in range(len(observations)):
        fields = observations[i].split(' ')
        if '(predator' in observations[i]:
            rel_pos.append([int(fields[2]), int(fields[3])])
            if '(predator 1' in observations[i]:
                main_pred = [int(fields[2]), int(fields[3])]
            if '(predator' in observations[i] and ' 0 0' in observations[i]:
                my_index = index
            index += 1

    for i in range(len(rel_pos)):
        rel_pos[i][0] = rel_pos[i][0]-main_pred[0]+int(width/2)
        rel_pos[i][1] = rel_pos[i][1]-main_pred[1]+int(height/2)
        rel_pos[i] = rel_pos[i][0]+rel_pos[i][1]*width

    for p in rel_pos:
        if p < rel_pos[my_index]:
            pos += 1

    #print(state, pos)
    return pos


class PrstAgent(object):
    def __init__(self, id_, noa, agentSocket, sem, mainSem, jal=True):
        self.id_ = id_
        # self.agentPositions = agentPositions
        self.noa = noa
        self.position = id_
        self.agentSocket = agentSocket
        self.terminal = False
        self.reward = 0
        self.actions = [0]*noa
        self.statePacked = None
        self.terminal = False
        self.semaphore = sem
        self.mainSem = mainSem
        self.jal = jal

        self.state = ''

    def agentStep(self):
        self.semaphore.acquire()
        turn = 0
        self.quit = False
        #cycle = 0
        while not self.quit:
            #print("Sent: ", moves[self.actions[self.position]])
            if not self.noop:
                self.agentSocket.send(moves[self.actions[self.position]].encode('utf-8'))
            #print(self.id_, '--', turn)

            #if self.terminal:
            #    print(self.id_, 'terminal - resetting')
            self.reward = 0
            self.terminal = False
            msg = self.agentSocket.recv(1024).decode('UTF-8')
            #if self.noop:
            #print(self.id_, self.noop, 'Got: ', msg)
            while '(send_action' not in msg and not self.terminal:
                if '(quit' in msg:
                    self.terminal = True
                    quit = True
                elif '(hear' in msg :
                    # self.processCommunicationInformation( msg )
                    pass
                elif '(referee prey_caught' in msg :
                    self.reward += 200
                    #print(self.id_, 'Got: ', msg)
                elif '(see' in msg:
                    self.state = msg
                    #self.agentSocket.send(''.encode('utf-8')) # comms message
                    #("Sent: <empty>")
                elif '(referee episode_ended' in msg:
                    self.terminal = True
                    #print(self.id_, 'Got: ', msg)
                    #print(self.id_, "terminal")
                elif '(referee collision' in msg:
                    self.reward += -50
                    #print(self.id_, 'Got: ', msg)
                    # msg = self.processReposition( )
                msg = self.agentSocket.recv(1024).decode('UTF-8')
                #if self.noop:
                #print(self.id_, self.noop, 'Got: ', msg)
            #print(msg)
            #print(msg.split(')'))
            #print(msg.split(')')[-2].split(' '))
            #print(msg.split(')')[-2].split(' ')[-1])
            #if not self.terminal:
            #    newcycle = int(msg.split(')')[-2].split(' ')[-1])
            #    if newcycle-cycle > 1:
            #        print(self.id_, "RIP", cycle, newcycle, msg)
            #        while True:
            #            print(self.id_, "---",self.agentSocket.recv(1024).decode('UTF-8'))
            #        exit()
            #    cycle=newcycle
            #else:
            #    cycle=0
            # if (self.jal and self.id_ == 0) or not self.jal:
            #    # wake up main
            self.mainSem.release()

            #if self.jal:
            #    self.position = get_position(self.state, 7, 7) # todo

            #print(self.id_, self.position)

            #print(self.id_, '##', turn)

            #if self.terminal:
            #    print(self.id_, 'terminal - Gonna wait')
            # sleep
            self.semaphore.acquire()
            turn += 1

