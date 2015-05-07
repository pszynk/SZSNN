from pyNN.brian import *
#from pyNN.nest import *
import pylab as lab
from numpy import array

setup(timestep=1, max_delay=1, min_delay=1)

n = 100;
EXC = 80
INH = 20
wei_E = 2#1.62/200 #* 100#1#0.5#1.62/40
wei_I = -1#-9.0/100 #* 100#-0.5#-9.0/10

P = Population(n, IF_curr_exp, {'v_rest':-49, 'tau_syn_I': 10,'tau_syn_E': 5, 'v_reset':-200, 'v_thresh': -50, 'cm': 1.0, 'tau_m':20.0})
P.initialize('v', -70)
P.initialize('Ie', 10)
Pe = P[:EXC]
Pi = P[EXC:]

Ce_list = []
for i in range(0, EXC):
	for j in range(i+1, i+5):
		if j < n:
			Ce_list.append((i, j, wei_E, 2))

print Ce_list

Ci_list = []
for i in range(EXC, n):
	for j in range(i+1, i+5):
		if j < n:
			Ci_list.append((i-EXC, j, wei_I, 2))

print Ci_list
			
Ce = Projection(Pe, P, FromListConnector(Ce_list), target='excitatory')
Ci = Projection(Pi, P, FromListConnector(Ci_list), target='inhibitory')			



Ci.saveConnections("inhibCH.conn")
Ce.saveConnections("excitCH.conn")
P.record('spikes')
#Pe1 = Pe[0]
run(1000)
P.printSpikes("pyNNspikesCH.spikes")
spikes = P.getSpikes()
spike_array = array(spikes)
lab.scatter(spike_array[:,1], spike_array[:,0], c='b', marker='s', faceted=False)
#lab.plot(Pe1.get_gsyn(), range(0,1000))
lab.xlim(0, 1000)
lab.ylim(0, n)
lab.show()

