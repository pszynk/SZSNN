/**
 * \file SynapseIndex.cpp
 *
 * \date 16-12-2012
 * \author pawel
 * \description
 */
#include <boost/tuple/tuple_comparison.hpp>

#include "SynapseIndex.hpp"


namespace boost {
	namespace tuples {



std::size_t hash_value(szsnn::SynapseIndex::group_key const& key)
{
    std::size_t seed = 0;
    boost::hash_combine( seed, key.get<0>() );
    boost::hash_combine( seed, key.get<1>() );
    boost::hash_combine( seed, key.get<2>() );
    boost::hash_combine( seed, key.get<3>() );
    return seed;
}

std::size_t hash_value(szsnn::SynapseIndex::source_key const& key)
{
    std::size_t seed = 0;
    boost::hash_combine( seed, key.get<0>() );
    boost::hash_combine( seed, key.get<1>() );
    boost::hash_combine( seed, key.get<2>() );
    return seed;
}

	}
}

namespace szsnn {

synapse_addr_t SynapseIndex::addSynapse(
		partitionIdx_t 	sourcePartition,
		neuronIdx_t		sourceNeuron,
		partitionIdx_t	targetPartition,
		delay_t			deley,
		size_t nextFreeBundleIdx)
{
	group_key gKey(sourcePartition, sourceNeuron, targetPartition, deley);
	unsigned idxInBundle = groupCount[gKey] % bundle_size;
	groupCount[gKey] += 1;

	source_key sKey(sourcePartition, sourceNeuron, deley);
	std::vector<size_t>& _bundles = bundles[sKey][targetPartition];

	if(idxInBundle == 0) {
		/* nowa paczka */
		_bundles.push_back(nextFreeBundleIdx);
		sourceCount[sKey] += 1;
		return synapse_addr_t(nextFreeBundleIdx, idxInBundle);
	}
	return synapse_addr_t(*_bundles.rbegin(), idxInBundle);
}

unsigned SynapseIndex::getSourceCount(const source_key& key) const
{
	boost::unordered_map<source_key, unsigned>::const_iterator it =
			sourceCount.find(key);
	if(it == sourceCount.end())
	{
		return 0;
	}

	return it->second;

}

void specUploadToDevice()
{

}
void specDownloadFromDevice()
{

}
void specFreeHostData()
{

}
} /* namespace szsnn */
