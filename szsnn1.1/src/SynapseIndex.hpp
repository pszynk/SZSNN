/**
 * \file SynapseIndex.hpp
 *
 * \date 16-12-2012
 * \author pawel
 * \description
 */

#ifndef SYNAPSEINDEX_HPP_
#define SYNAPSEINDEX_HPP_

#include <vector>
#include <ostream>
#include <sstream>

#include <boost/tuple/tuple.hpp>
#include <boost/unordered_map.hpp>
#include <boost/functional/hash.hpp>


#include "types.h"

namespace szsnn {

/*
 *
 */
class SynapseIndex {
public:


	typedef boost::tuple<
			partitionIdx_t,
			neuronIdx_t,
			partitionIdx_t,
			delay_t>
	group_key;

	typedef boost::tuple<
			partitionIdx_t,
			neuronIdx_t,
			delay_t>
	source_key; //TODO inna nazwa

	/**
	 * mapuje docelową partycje, na wszyskie indeksy bundlów które
	 * zawierają synapsy których neuron poststynaptyczny znajduje się
	 * w danje partycji
	 */
	typedef boost::unordered_map<partitionIdx_t, std::vector<size_t> > groups;

private:

	/**
	 *
	 * map{(s_part, s_neur, delay) -> map{(t_part) -> vector{bundle1,..., bundleN}}}
	 */
	typedef boost::unordered_map<source_key, groups> bundle_map;

public:
	typedef bundle_map::const_iterator iterator;

	typedef groups::const_iterator group_iterator;
	SynapseIndex():bundle_size(BUNDLE_SIZE){};

	iterator begin() const { return bundles.begin(); };
	iterator end() const { return bundles.end(); };


	//void createSynapseIndex(const NetworkGraph& graph);

	synapse_addr_t addSynapse(
			partitionIdx_t 	sourcePartition,
			neuronIdx_t		sourceNeuron,
			partitionIdx_t	targetPartition,
			delay_t			deley,
			size_t nextFreeBundleIdx);

	unsigned getSourceCount(const source_key& key) const;

	void printBundleSizeHistogram(std::ostream& out) const;
private:

	size_t		bundle_size;

	bundle_map  bundles;

	boost::unordered_map<group_key, unsigned> groupCount;

	boost::unordered_map<source_key, unsigned> sourceCount;

};

} /* namespace szsnn */
#endif /* SYNAPSEINDEX_HPP_ */
