#ifndef SPIKERECORDER_HPP_
#define SPIKERECORDER_HPP_

#include <map>
#include <set>
#include <vector>
#include <fstream>

#include "utils.h"

namespace szsnn {
	namespace record {
class SpikeRecorder {
private:
	typedef unsigned cycleNr_t;
	typedef unsigned neuronID_t;

	typedef std::map<neuronID_t, std::set<cycleNr_t> >	neuron_key_map;
	typedef std::map<cycleNr_t,  std::set<neuronID_t> >	cycle_key_map;
public:
	SpikeRecorder(cycleNr_t _recordStart, cycleNr_t _recordStop = 0):
		recordStart(_recordStart), recordStop(_recordStop), currentCycle(_recordStart)
	{};

	void saveToFileByNeurons(const std::string filename) const;
	void saveToFileByCycles(const std::string filename) const;

	void recordCycleSpikes(const std::vector<neuronID_t>& spikes);
	void recordCycleSpikes(cycleNr_t cycle, const std::vector<neuronID_t>& spikes);
private:
	cycleNr_t	recordStart;
	cycleNr_t	recordStop;
	cycleNr_t	currentCycle;

	neuron_key_map	neurons_to_cycles;
	cycle_key_map cycles_to_neurons;


};

	}
}

#endif /* SPIKERECORDER_HPP_ */
