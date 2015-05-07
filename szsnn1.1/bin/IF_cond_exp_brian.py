from brian import *

myclock=Clock(dt=1*ms)
#dv/dt = (ge+gi-(v+49*mV))/(20*ms) : volt
eqs = '''
dv/dt = (ge+gi-(v+49*mV))/(20*ms) : volt
dge/dt = -ge/(5*ms) : volt
dgi/dt = -gi/(10*ms) : volt
'''
P = NeuronGroup(1000, eqs, threshold=-50*mV, reset=-70*mV)
P.v = -70*mV
Pe = P.subgroup(800)
Pi = P.subgroup(200)
#Ce = Connection(Pe, P, 'ge', weight=1.62*mV, sparseness=0.02, delay = 1* ms)
#Ci = Connection(Pi, P, 'gi', weight=-9*mV, sparseness=0.02, delay = 1* ms)
Ce = Connection(Pe, P, 'ge', delay = 1* ms)
Ci = Connection(Pi, P, 'gi',delay = 1* ms)
Ce.connect_full(Pe, P, weight=1.62*mV)
Ci.connect_full(Pi, P, weight=-9*mV)
M = SpikeMonitor(P)
run(1*second)
with open("brianSpikes", 'w') as f:
	for spike in M.spikes:
		f.write(str(spike[1])+"\t"+str(spike[0]))

f.close()
raster_plot(M)
show()
