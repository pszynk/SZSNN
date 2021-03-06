from pyNN.brian import *
#from pyNN.nest import *
import pylab as lab
from numpy import array

setup(timestep=1, max_delay=3, min_delay=1)

n = 2;

wei_E = 100 #* 100#1#0.5#1.62/40
wei_I = -9.0/100 #* 100#-0.5#-9.0/10

P = Population(n, IF_curr_exp, {'v_rest':-49, 'tau_syn_I': 10,'tau_syn_E': 5, 'v_reset':-200, 'v_thresh': -50, 'cm': 1.0, 'tau_m':20.0})
P.initialize('v', -70)

P1 = P[:1]
P2 = P[1:]
C = Projection(P1, P2, OneToOneConnector(weights=wei_E, delays=0), target='excitatory')



C.saveConnections("2neurony.conn")

P.record('spikes')
P2.record_v()

run(1000)
P.printSpikes("pyNN2neurony.spikes")
spikes = P2.getSpikes()
vies = P2.get_v()
P2.print_v("pyNN2neurony.vvv")

print vies
vies_array = array(vies)
#spike_array = array(spikes)
#lab.scatter(spike_array[:,1], spike_array[:,0], c='b', marker='s', faceted=False)
lab.plot(vies_array[:,1], vies_array[:,2], c='b')
lab.xlim(0, 100)

#lab.ylim(0, n)
lab.show()

