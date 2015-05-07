/*
 * SpikeRecorder.cpp
 *
 *  Created on: 13-01-2013
 *      Author: pawel
 */

#include "SpikeRecorder.hpp"

namespace szsnn {
	namespace record {

void SpikeRecorder::saveToFileByCycles(const std::string filename) const
{
	std::ofstream outfile;
	try
	{
		outfile.open(filename.c_str(), std::ifstream::out);
	} catch (const std::exception& e)
	{
		THROW_SZSNNEX("SZSNN::RECORD::SpikeRecorder::saveToFileByCycles() error:"
				<< " couldn't open file `" << filename << "`: " <<e.what());
	}
	outfile << "#spikes by cycles" <<std::endl;
	outfile << "#record start at: " << recordStart;
	if(recordStop)
	{
		outfile << "   stop at: " << recordStop <<std::endl;
	}

	for(cycle_key_map::const_iterator it_cycles = cycles_to_neurons.begin();
			it_cycles != cycles_to_neurons.end(); ++it_cycles)
	{
		for(std::set<neuronID_t>::const_iterator it_neurons = it_cycles->second.begin();
				it_neurons != it_cycles->second.end(); ++it_neurons)
		{
			outfile << it_cycles->first << "\t" << (*it_neurons) << std::endl;
		}
	}
	outfile.close();
}

void SpikeRecorder::saveToFileByNeurons(const std::string filename) const
{
	std::ofstream outfile;
	try
	{
		outfile.open(filename.c_str(), std::ifstream::out);
	} catch (const std::exception& e)
	{
		THROW_SZSNNEX("SZSNN::RECORD::SpikeRecorder::saveToFileByNeurons() error:"
				<< " couldn't open file `" << filename << "`: " <<e.what());
	}
	outfile << "#spikes by neurons" <<std::endl;
	outfile << "#record start at: " << recordStart;
	if(recordStop)
	{
		outfile << "   stop at: " << recordStop <<std::endl;
	}

	for(neuron_key_map::const_iterator it_neurons = neurons_to_cycles.begin();
			it_neurons != neurons_to_cycles.end(); ++it_neurons)
	{
		for(std::set<cycleNr_t>::const_iterator it_cycles = it_neurons->second.begin();
				it_cycles != it_neurons->second.end(); ++it_cycles)
		{
			outfile << (*it_cycles)  << "\t" << it_neurons->first<< std::endl;
		}
	}

	outfile.close();
}


void SpikeRecorder::recordCycleSpikes(cycleNr_t cycle, const std::vector<neuronID_t> & spikes)
{
	if (cycle < recordStart) return;
	if (recordStop)
	{
		if(cycle>recordStop) return;
	}
	if(cycles_to_neurons.count(cycle))
	{
		THROW_SZSNNEX("SZSNN::RECORD::SpikeRecorder::recordCycleSpikes error: "
				<< "cycle `" << cycle <<"` already recorder");
	}
	cycles_to_neurons.insert(
			cycle_key_map::value_type(
					cycle, std::set<neuronID_t>(spikes.begin(), spikes.end())));

	if (cycles_to_neurons[cycle].size() != spikes.size())
	{
		THROW_SZSNNEX("SZSNN::RECORD::SpikeRecorder::recordCycleSpikes error: "
				<< "in cycle `" << cycle <<"` some spikes were duplicated");
	}
	for(std::vector<neuronID_t>::const_iterator it = spikes.begin();
			it != spikes.end(); ++it)
	{
		std::set<cycleNr_t>& cycles = neurons_to_cycles[*it];
		cycles.insert(cycle);
	}

	++currentCycle;
}

void SpikeRecorder::recordCycleSpikes(const std::vector<neuronID_t> & spikes)
{
	recordCycleSpikes(currentCycle, spikes);
}


	}
}

