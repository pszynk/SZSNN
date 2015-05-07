from brian import *
from brian.library.IF import * 

myclock=Clock(dt=1*ms)

a = 0.02 / ms
b = 0.2 / ms
c = -65 * mV
d = 2 * nA
#I = 10 * nA
eqs = '''
dv/dt = (0.04/ms/mV)*v**2 + (5/ms)*v + 140*mV/ms - u  : volt
du/dt = a*(b*v-u) : volt/second
I : volt/second
'''


resetRS = AdaptiveReset(Vr=-65*mV, b=2*nA)
group = NeuronGroup(1, model=eqs, threshold=-30*mV, reset=resetRS) 


#M = StateMonitor(group, 'v', record=True)
#N = StateMonitor(group, 'u', record=True)

run(1*second)
#plot(M.times / ms, M[0] / mV)
#plot(N.times / ms, N[0] / mV)
show()

