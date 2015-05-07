#ifndef TESTNEURONMODEL_HPP_
#define TESTNEURONMODEL_HPP_

#include <iostream>
#include "NeuronModel.hpp"
namespace szsnn {


template <typename floatT>
class TestNeuronModel : public NeuronModel<TestNeuronModel<floatT> > {
public:
	typedef NeuronModel<TestNeuronModel<floatT> > base_type;
	typedef typename base_type::NeuronType base_neuron_type;
	TestNeuronModel() {};
	TestNeuronModel(floatT pa, floatT pb, floatT va, floatT vb)
		//:
		//NeuronModel<TestNeuronModel<floatT> >({pa, pb}, {va, vb}) //TODO tylko w x0
	{
		floatT tparams[2] = {pa, pb};
		floatT tvars[2] = {va, vb};
		this->setAll(tparams,tvars);
	};

	virtual void randomizeMe(bool isInhib)
	{
		floatT tparams[2] = {5, 4};
		floatT tvars[2] = {2, 9};
		this->setAll(tparams, tvars);
	}
};

} /* namespace szsnn */
#endif /* TESTNEURONMODEL_HPP_ */
