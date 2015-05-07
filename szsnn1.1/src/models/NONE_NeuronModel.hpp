#ifndef NONENEURONMODEL_HPP_
#define NONENEURONMODEL_HPP_

#include <iostream>
#include "NeuronModel.hpp"
namespace szsnn {


	template <typename floatT>
	class NONE_NeuronModel : public NeuronModel<NONE_NeuronModel<floatT> > {
	public:

		//typedef NeuronModel<NONE_NeuronModel<floatT> > base_type;
		//typedef typename base_type::NeuronType base_neuron_type;
		//NONE_NeuronModel();

		virtual void randomizeMe(bool isInhib)
		{}
	};

} /* namespace szsnn */
#endif /* TESTNEURONMODEL_HPP_ */
