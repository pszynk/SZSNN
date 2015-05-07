from brian import *

myclock=Clock(dt=1*ms)
v_rest = -49 * mV
tau_m = 20 * ms
cm = 1 * nF
Io = 0 * nA
tau_syn_E = 5 * ms
tau_syn_I = 10 * ms

wei_E = 100*nA#1.62/200*nA
#wei_I = -9.0/200*nA

eqs = '''
dv/dt = (Ie+Ii+Io)/cm + (v_rest - v)/(tau_m) : volt
dIe/dt = -Ie/(tau_syn_E) : amp
dIi/dt = -Ii/(tau_syn_I) : amp
'''
n = 2;
P = NeuronGroup(n, eqs, threshold=-50*mV, reset=-200*mV)
P.v = -70*mV
P1 = P.subgroup(1)
P2 = P.subgroup(1)
C = Connection(P1, P2, 'Ie', weight=wei_E, sparseness=1, delay = 0* ms)


Mv = StateMonitor(P2, 'v', record=True)
MIe = StateMonitor(P2, 'Ie', record=True)
#MIi = StateMonitor(P2, 'Ii', record=True)
#M = SpikeMonitor(P)
run(80*ms)
plot(Mv.times/ms, Mv[0]/mV)
plot(MIe.times/ms, MIe[0]/nA)
#raster_plot(M)
show()
