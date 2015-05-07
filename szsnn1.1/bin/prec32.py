from numpy import float32

v_rest = float32(-49.0)
v_thresh = float32(-50.0)
tau_m = float32(20.0)
cm = float32(1.0)
one = float32(1.0)
tau_syn_E = float32(5.0)
tau_syn_I = float32(10.0)

I = float32(3.0)
v = float32(-70.0)
delat =float32(0.0)
counter = float32(0)

while v < v_thresh:
	counter += 1

	delta = I/cm + (v_rest - v) / tau_m
	print "to jest delta", delta
	v = v + delta
	print "to jest v", v
	I = ((one - one/tau_syn_E) * I)
	print "to jest I", I

print counter
