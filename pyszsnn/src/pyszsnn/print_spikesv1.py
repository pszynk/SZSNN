import sys
import pylab as lab
import numpy as np

argn = (sys.argv)
argv = sys.argv
#title = "Integrage-and-fire (CUBA)"
title = "Izhikevich"
secondsC = int(argv[1])
neuronsC = int(argv[2])
simName1 = argv[3]
simName2 = argv[4]
simNames = [simName1, simName2]
files = argv[5:]

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


fig1 = lab.figure(facecolor='white')
ax1 = lab.axes(frameon=False)
ax1.set_frame_on(False)
axs = lab.subplot(111)
facecolors = ['none', 'r']
edgecolors = ['k', 'none', 'b']
sizes= [50, 10, 30]
markers= ['o', 'o', 'd']
i = 0
scatters = {}
for filename in files:
	scatters[simNames[i]] = axs.scatter(spikes_in_files[filename][0], 
			spikes_in_files[filename][1], s=sizes[i], 
			facecolors=facecolors[i], 
			edgecolors=edgecolors[i], marker=markers[i])#, faceted=False)
	i+=1
lab.legend(scatters.values(), scatters.keys())
lab.xlabel("czas [ms]")
lab.ylabel("indeks neuronu")
lab.title(title)
lab.xlim(0, secondsC)
lab.ylim(0, neuronsC)

#lab.show()


fig2 = lab.figure(facecolor='white')
ax2 = lab.axes(frameon=False)
ax2.set_frame_on(False)
axp = lab.subplot(111)
#facecolors = ['none', 'r']
#edgecolors = ['k', 'none', 'b']
#sizes= [50, 10, 30]
#markers= ['o', 'o', 'd']
idx = 0
bars = {}
ind = np.arange(len(range(0, secondsC, 10)))
width = 0.45
barcolors = ['b', 'r']
for filename in files:
	spikeCounts = [spikes_in_files[filename][0].count(i) for i in range(0, secondsC)]
	sumedSpikeCounts = [sum(spikeCounts[i:i+10]) for i in range(0, secondsC, 10)]
	#axp.plot(range(0, secondsC, 10), sumedSpikeCounts)
	bars[simNames[idx]] = axp.bar(ind + idx*width, sumedSpikeCounts, width,
                    		color=barcolors[idx])
	idx+=1
lab.legend((bars.values()[0]), (bars.keys()))
lab.xlabel("czas x10 [ms]")
lab.ylabel("indeks neuronu")
lab.title(title)
lab.show()