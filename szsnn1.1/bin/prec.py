from numpy import float32

v_rest = -49.0
v_thresh = -50.0
tau_m = 20.0
cm = 1.0
one = 1.0
tau_syn_E = 5.0
tau_syn_I = 10.0

I = 3.0
v = -70.0
delat = 0.0
counter = 0
while v < v_thresh:
	counter += 1
	for i in range(0,8):
		delta = 0.125 * (I/cm + (v_rest - v) / tau_m)
		print "to jest delta", delta
		v = v + delta
		print "to jest v", v
	I = ((one - one/tau_syn_E) * I)
	print "to jest I", I

print counter
