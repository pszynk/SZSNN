from brian import *
from pyszsnn import *
	
defaultclock.dt = 1*ms              # Clock is set to 1 ms to match SpiNNaker's timer interrupt
a = 0.02/ms
b = 0.2/ms
c = -65*mV
d = 2*mV/ms #8

stim = 20*nA
N = 1000

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

sg1 = G.subgroup(800)
sg2 = G.subgroup(200)	



C1 = Connection(sg1, G, 'I', weight=10.2*nA, delay=1*ms, sparseness=0.01)
C2 = Connection(sg2, G, 'I', weight=-0.03*nA, deley=1*ms, sparseness=0.01)

brianConnectionToFile(C1, 10**9, 'izi1.conn')
brianConnectionToFile(C2, 10**9, 'izi2.conn')

v = StateMonitor(G,'v',record=0)
u = StateMonitor(G,'u',record=0)
I = StateMonitor(G, 'I', record=0)
p = SpikeMonitor(G)

	
G.I = 0*nA
run(1*ms)
G.v = 30*mV
#G.I = stim
run(40*ms)
G.v = 30*mV
#G.v = c
#G.I = 0*nA
run(300*ms)
#plot(v.times/ms,v[0]/mV,'b')
raster_plot(p)
show()

	
