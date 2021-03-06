from brian import *
from pyszsnn import *
	
defaultclock.dt = 1*ms              # Clock is set to 1 ms to match SpiNNaker's timer interrupt
a = 0.02/ms
b = 0.2/ms
c = -65*mV
d = 2*mV/ms #8

stim = 20*nA
N = 100

eqs = Equations('''
dv/dt = (0.04/ms/mV)*v**2+(5/ms)*v+140*mV/ms-u+I/nF  : volt
du/dt = a*(b*v-u)                                    : volt/second
I                                                    : amp
''')
#dv/dt = (0.04/ms/mV)*v**2+(5/ms)*v+140*mV/ms-u+I/nF  : volt

reset = '''
v = c
u += d
'''
threshold = 30*mV
G = NeuronGroup(N,eqs,threshold=threshold,reset=reset)

#G.v = -80*mV
#G.u = 0
print G.v
print G.u

sg1 = G.subgroup(int(N*0.8))
sg2 = G.subgroup(int(N*0.2))	

we = 10.2
wi = -3

S1=Synapses(sg1,G,model='we:1',pre='v+=we', max_delay=14*ms)
S2=Synapses(sg2,G,model='wi:1',pre='v+=wi', max_delay=10*ms)

S1.connect_random(sg1,G,sparseness=0.5)
S2.connect_random(sg2,G,sparseness=0.5)
S1.we[sg1, G]=we
S2.wi[sg2, G]=wi
#S1.delays[sg1, G] = 3*ms
#S2.delays[sg2, G] = 3*ms

#C1 = Connection(sg1, G, 'I', weight=10.2*nA, delay=1*ms, sparseness=0.5)
#C2 = Connection(sg2, G, 'I', weight=-0.03*nA, delay=1*ms, sparseness=0.5)

#brianConnectionToFile(C1, 10**9, 'izi1.conn')
#brianConnectionToFile(C2, 10**9, 'izi2.conn')

v = StateMonitor(G,'v',record=3)
u = StateMonitor(G,'u',record=3)
p = SpikeMonitor(G)


print str(S1.delay)
	
#G.I = 0*nA
run(1*ms)
#G.v = 30*mV
G.I = stim
run(10*ms)
G.I = 0
run(40*ms)
#G.v = 30*mV
#G.v = c
#G.I = 0*nA
run(300*ms)
#plot(v.times/ms,v[3]/mV,'b')
with open("ZRZT_ONE_IZI.log", 'w') as f:
	for i in range(0, 341):
		f.write(str(i)+"\t"+str(v[3][i])+"\t"+str(u[3][i])+"\n")

f.close()

raster_plot(p)
show()

	
