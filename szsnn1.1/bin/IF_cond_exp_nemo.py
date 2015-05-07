from brian import *

myclock=Clock(dt=1*ms)
v_rest = -49 * mV
tau_m = 20 * ms
cm = 1 * nF
Io = 0 * nA
tau_syn_E = 5 * ms
tau_syn_I = 10 * ms

wei_E = 100#1.62/200*nA
wei_I = -9.0/200*nA

eqs = '''
dv/dt = (Ie+Ii+Io)/cm + (v_rest - v)/(tau_m) : volt
dIe/dt = -Ie/(tau_syn_E) : amp
dIi/dt = -Ii/(tau_syn_I) : amp
'''
n = 100;
P = NeuronGroup(n, eqs, threshold=-50*mV, reset=-60*mV)
P.v = -60*mV
Pe = P.subgroup(int(0.8*n))
Pi = P.subgroup(int(0.2*n))
Ce = Connection(Pe, P, 'Ie', weight=wei_E, sparseness=1, delay = 1* ms)
Ci = Connection(Pi, P, 'Ii', weight=wei_I, sparseness=1, delay = 1* ms)

Mv = StateMonitor(Pi[0], 'v', record=True)
MIe = StateMonitor(Pe[0], 'Ie', record=True)
MIi = StateMonitor(Pe[0], 'Ii', record=True)
M = SpikeMonitor(P)
run(1*second)
#plot(Mv.times/ms, Mv[0]/mV)
plot(MIe.times/ms, MIi[0]/nA)
#raster_plot(M)
show()
