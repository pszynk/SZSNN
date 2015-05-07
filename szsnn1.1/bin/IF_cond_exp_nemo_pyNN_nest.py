#from pyNN.brian import *
from pyNN.nest import *
import pylab as lab
from numpy import array

setup(timestep=1.0, max_delay=3.0, min_delay=1.0)

n = 4000;

wei_E = 1.62/20 #* 100#1#0.5#1.62/40
wei_I = -9.0/20 #* 100#-0.5#-9.0/10

P = Population(n, IF_curr_exp, {'v_rest':-49.0, 'tau_syn_I': 10.0,'tau_syn_E': 5.0, 'v_reset':-100.0, 'v_thresh': -50.0, 'cm': 1.0, 'tau_m':30.0})
P.initialize('v', -70.0)
#P.initialize('Ie', 10)
Pe = P[:(n*0.8)]
Pi = P[(n*0.8):]
Ce = Projection(Pe, P, FixedProbabilityConnector(p_connect=0.01, allow_self_connections=False, weights=wei_E, delays=2.0), target='excitatory')
#Ci = Projection(Pi, P, FixedProbabilityConnector(p_connect=0.01, allow_self_connections=False, weights=wei_E, delays=1), target='excitatory')

#Ce = Projection(Pe, P, FixedProbabilityConnector(p_connect=0.02, allow_self_connections=False, weights=wei_I, delays=1), target='inhibitory') #
Ci = Projection(Pi, P, FixedProbabilityConnector(p_connect=0.01, allow_self_connections=False, weights=wei_I, delays=2.0), target='inhibitory') #

#Ce = Projection(Pe, P, AllToAllConnector(allow_self_connections=False, weights=wei_E, delays=1), target='excitatory')
#Ci = Projection(Pi, P, AllToAllConnector(allow_self_connections=False, weights=wei_I, delays=1), target='inhibitory')

Ci.saveConnections("inhib.conn")
Ce.saveConnections("excit.conn")
P.record('spikes')
Pe1 = Pe[0]
run(1000)
P.printSpikes("pyNNspikes.spikes")
spikes = P.getSpikes()
spike_array = array(spikes)
lab.scatter(spike_array[:,1], spike_array[:,0], c='b', marker='s', faceted=False)
#lab.plot(Pe1.get_gsyn(), range(0,1000))
lab.xlim(0, 1000)
lab.ylim(0, n)
lab.show()

