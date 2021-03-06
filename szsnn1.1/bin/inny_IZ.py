
from brian import *
from brian.library.IF import *

a=0.02/ms
b=0.2/ms
eqs = '''
dvm/dt=(0.04/ms/mV)*vm**2+(5/ms)*vm+140*mV/ms-w+I : volt
dw/dt=a*(b*vm-w) : volt/second
I : amp
''' 


resetRS = AdaptiveReset(Vr=-65*mV, b=2.0*nA)
group = NeuronGroup(1000, model=eqs, threshold=-30*mV, reset=resetRS)
sg1 = group.subgroup(800)
sg2 = group.subgroup(200)

group.v = -80*mV
group.u = 0

C1 = Connection(sg1, group, 'I', weight=-20*nA, sparseness=0.01)
C2 = Connection(sg2, group, 'I', weight=20*mV/ms, sparseness=0.02)



SP = SpikeMonitor(group)

#M = StateMonitor(group, 'vm', record=True)
#N = StateMonitor(group, 'w', record=True)

run(250*ms)
#plot(M.times / ms, M[0] / mV)
#plot(N.times / ms, N[0] / mV)
raster_plot(SP)
show()

