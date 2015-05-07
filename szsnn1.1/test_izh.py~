from brian import *
	
defaultclock.dt = 1*ms              # Clock is set to 1 ms to match SpiNNaker's timer interrupt
#a = 0.02/ms
#b = 0.2/ms
#c = -65*mV
#d = 2*mV/ms #8

stim = 20*nA
N = 1000

eqs = Equations('''
a : 1/second
b : 1/second
c : volt
d : volt/second
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

#v_rest = -49 * mV
#tau_m = 20 * ms
#cm = 1 * nF
#Io = 0 * nA
#tau_syn_E = 5 * ms
#tau_syn_I = 10 * ms

wei_E = 100#1.62/200*nA
wei_I = -9.0/200*nA

eqsIF= Equations('''
        dv/dt  = (Ie + Ii + Io)/c_m + (v_rest-v)/tau_m : mV
        dIe/dt = -Ie/tau_syn_E                : nA
        dIi/dt = -Ii/tau_syn_I                : nA
        tau_syn_E                             : ms
        tau_syn_I                             : ms
        tau_m                                 : ms
        c_m                                   : nF
        v_rest                                : mV
        Io                              : nA
	v_thresh		: mV
	v_reset		 	: mV
        '''
        )
n = 100
th = StringThreshold('v > v_thresh')
P = NeuronGroup(n, eqsIF, threshold='v > v_thresh', reset='v = v_reset')
P.v_rest = -49 * mV
P.tau_m = 20 * ms
P.c_m = 1 * nF
P.Io = 0 * nA
P.tau_syn_E = 5 * ms
P.tau_syn_I = 10 * ms
P.v_thresh = -50 * mV
P.v_reset = -60 * mV
#P.rest()
wei_E = 1.62/20* nA#1.62/200*nA
wei_I = -9.0/20*nA
print P.reset

P.v = -60*mV
Pe = P.subgroup(int(0.8*n))
Pi = P.subgroup(int(0.2*n))
#Ce = Connection(Pe, P, 'Ie', weight=wei_E, sparseness=0.01, delay = 1* ms)
#Ci = Connection(Pi, P, 'Ii', weight=wei_I, sparseness=0.01, delay = 1* ms)

Mv = StateMonitor(Pi[0], 'v', record=True)
MIe = StateMonitor(Pe[0], 'Ie', record=True)
MIi = StateMonitor(Pe[0], 'Ii', record=True)
M = SpikeMonitor(P)
run(1*second)
plot(Mv.times/ms, Mv[0]/mV)
#plot(MIe.times/ms, MIi[0]/nA)
raster_plot(M)
show()
