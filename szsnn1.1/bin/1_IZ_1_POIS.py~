'''
Izhikevich Neuron Model
Simple Model of Spiking Neurons
Eugene M. Izhikevich
IEEE Transactions on Neural Networks, 2003
'''
from brian import *
	
defaultclock.dt = 1*ms              # Clock is set to 1 ms to match SpiNNaker's timer interrupt

# Neuron Parameters
a = 0.02/ms
b = 0.2/ms
c = -65*mV
d = 8*mV/ms

stim = 20*nA
N = 1

eqs = Equations('''
dv/dt = (0.04/ms/mV)*v**2+(5/ms)*v+140*mV/ms-u+I/nF  : volt
du/dt = a*(b*v-u)                                    : volt/second
I                                                    : amp
''')
reset = '''
v = c
u += d
'''
threshold = 30*mV
G = NeuronGroup(N,eqs,threshold=threshold,reset=reset)

G.v = -80*mV
G.u = 0
	
v = StateMonitor(G,'v',record=0)
u = StateMonitor(G,'u',record=0)
p = SpikeMonitor(G)

	
G.I = 0*nA
run(100*ms)
G.I = stim
run(300*ms)
G.I = 0*nA
run(99*ms)
	
figure(1)
title('traces')
plot(v.times/ms,v[0]/mV,'b',u.times/ms,u[0]*ms/mV,'r')
xlabel('time [ms]')
ylabel('v [mV], u [mV/ms]')
legend(('v','u'),loc='upper right')

print G.I
	
show()
