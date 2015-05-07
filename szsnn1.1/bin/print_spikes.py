import sys
import pylab as lab

argn = (sys.argv)
argv = sys.argv

secondsC = int(argv[1])
neuronsC = int(argv[2])
files = argv[3:]

spikes_in_files = {}
for filename in files:
	with open(filename, 'r') as f:
		spikes_in_files[filename] = [[],[]]
		for line in f:
			i = 0
			if (line[0] != '#'):
				for x in line.split():
					spikes_in_files[filename][i].append(int(float(x)));
					i += 1


ax = lab.subplot(111)
colors = ['r', 'g', 'b']
markers= ['o', '^', 'd']
i = 0
for filename in files:
	ax.scatter(spikes_in_files[filename][0], spikes_in_files[filename][1], c=colors[i], marker=markers[i], faceted=False)
	i+=1
	
lab.xlim(0, secondsC)
lab.ylim(0, neuronsC)		
lab.show()
