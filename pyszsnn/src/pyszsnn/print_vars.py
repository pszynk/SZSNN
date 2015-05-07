import sys
import pylab as lab

argn = (sys.argv)
argv = sys.argv

#tytul = str(argv[1])
secondsC = int(argv[2])
varsC = int(argv[3])
varsNames = argv[4:4+varsC]
nexta = 4+varsC
simNames = [argv[nexta], argv[nexta+1]]
files = argv[nexta+1:]
print varsC, varsNames

vars_in_files = {}
for filename in files:
	with open(filename, 'r') as f:
		vars_in_files[filename] = dict(zip(varsNames, [[] for i in range(0,2)]))
		print vars_in_files
		for line in f:
			i = 0
			if (line[0] != '#'):
				print line
				vars_in_files[filename][vars_in_files[filename].keys()[i%varsC]].append(float(line))
				i += 1
				

fig1 = lab.figure(facecolor='white')
ax1 = lab.axes(frameon=False)
ax1.set_frame_on(False)
ax = lab.subplot(111)
facecolors = ['none', 'r']
edgecolors = ['k', 'none', 'b']
sizes= [50, 10, 30]
markers= ['o', 'o', 'd']
lab.title("tytul")
lab.xlabel("czas [ms]")
lab.ylabel("indeks neuronu")

i = 0
plots = {}
for filename in vars_in_files.keys():
	for var in vars_in_files[filename].keys():
		lab.plot(range(0, len(vars_in_files[filename][var]))[0:50], vars_in_files[filename][var][0:50])
	
#lab.legend(scatters.values(), scatters.keys())
#lab.xlim(0, secondsC)
#lab.ylim(0, neuronsC)		
lab.show()