from time import time
from brian import *

import pyszsnn.brian.dumper as dpr
defaultclock.dt = 1*ms  


v_rest = -49 * mV
tau_m = 20 * ms
cm = 1 * nF
Io = 0 * nA
tau_syn_E = 5 * ms
tau_syn_I = 10 * ms


eqs = '''
dv/dt = (Ie+Ii+Io)/cm + (v_rest - v)/(tau_m) : volt
dIe/dt = -Ie/(tau_syn_E) : amp
dIi/dt = -Ii/(tau_syn_I) : amp
'''
N =20000;


wei_E = 1.62/20*nA
wei_I = -9.0/20*nA

P = NeuronGroup(N, eqs, threshold=-50*mV, reset=-100*mV, method="Euler")
P.v = -70*mV
P.Ie = 0
P.Ii = 0
print "saving template population"
dpr.saveIF_curr_expTemplateNGToFile(N, 0, "IFPOP.tpop", 
        v_rest=-49, cm=1, tau_m=20, tau_refrac=0, tau_syn_e=5, 
        tau_syn_i=10,  i_offset=0, v_reset=-100, v_thresh=-50, v=-70, label="IF_curr_exp")
print "template population saved"
Pe = P.subgroup(int(0.8*N))
Pi = P.subgroup(int(0.2*N))
Ce = Connection(Pe, P, 'Ie', weight=wei_E, sparseness=0.01, delay = 1* ms)
Ci = Connection(Pi, P, 'Ii', weight=wei_I, sparseness=0.01, delay = 1* ms)
print "saving inhibitory connections"
dpr.saveConnectionToFile(Ci, amp/nA, 0,0, "inhib.conn")
print "inhibitory connections saved"
print "saving excitatory connections"
dpr.saveConnectionToFile(Ce, amp/nA, 0,0, "excit.conn")
print "excitatory connections saved"
#recIdx = 0
M = SpikeMonitor(P)
#Mv = StateMonitor(Pe, 'v', record=recIdx)
start = time()
run(1*second)
elapsed = time() - start

#dpr.saveSpikesToFile(M, "IF_rys1.spikes", "rysunek1-IF")
#with open("IF_rys1_v.vars", 'w') as f:
#    varray = Mv[recIdx]
#    for i in range(0,len(varray)):
#        f.write(str(varray[i]*1000)+"\n")
#    f.close()

#figure(1)
#    title('traces')
#subplot(211)
#plot(Mv.times/ms,Mv[recIdx]/mV,'b')
#    xlabel('time [ms]')
#    ylabel('v [mV], u [mV/ms]')
#    legend(('v','u'),loc='upper right')
#    raster_plot(MS)
#subplot(212)
#raster_plot(M)
print elapsed;
#show()


