import sys
import pylab as lab
import numpy as np

argn = (sys.argv)
argv = sys.argv
title = "Integrage-and-fire (CUBA)"
#title = "Izhikevich"
simulatorsC = int(argv[1])
simNames = argv[2:2+simulatorsC]
fileName = argv[2+simulatorsC]

results = {}
data = simNames
data.insert(0, 'sizes')
print data
results = dict(zip(data, [[] for i in range(0, len(data))]))
print results
with open(fileName, 'r') as f:
	for line in f:
		i = 0
		if (line[0] != '#'):
			for x in line.split():
				results[data[i]].append(float(x))
				i += 1
print results

fig1 = lab.figure(facecolor='white')
ax1 = lab.axes(frameon=False)
ax1.set_frame_on(False)
axs = lab.subplot(111)
linestyles = ['-', '-']
colors = ['b', 'r']
sizes= [50, 10, 30]
markers= ['o', 's']
for idx in range(1, simulatorsC+1):
	axs.plot(results['sizes'], sorted(results[data[idx]]), marker=markers[idx-1],
			 linestyle=linestyles[idx-1] , color=colors[idx-1], linewidth=2.0, label=data[idx])

lab.legend(loc=2)
lab.xlabel("rozmiar sieci")
lab.ylabel("czas symulacji 1s")
lab.title(title)

fig2 = lab.figure(facecolor='white')
ax2 = lab.axes(frameon=False)
ax2.set_frame_on(False)
axs2 = lab.subplot(111)
axs2.plot(results['sizes'], sorted(results[simNames[2]]), marker="s",
		 linestyle=linestyles[1] , color=colors[1], linewidth=2.0, label=simNames[2])

lab.legend(loc=2)
lab.xlabel("rozmiar sieci")
lab.ylabel("czas symulacji 1s")
lab.title(title)

fig3 = lab.figure(facecolor='white')
ax3 = lab.axes(frameon=False)
ax3.set_frame_on(False)
axs3 = lab.subplot(111)
acc = [sorted(results[simNames[1]])[i]/sorted(results[simNames[2]])[i] for i in range(0, len(results[simNames[1]]))]
axs3.plot(results['sizes'], acc, marker="o",
		 linestyle=linestyles[1] , color='k', linewidth=2.0, label="Brian / SNN")

lab.legend(loc=2)
lab.xlabel("rozmiar sieci")
lab.ylabel("przyspieszenie")
lab.title(title)


lab.show()