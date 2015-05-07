#ifndef IZHIKEVICHNEURONMODEL_HPP_
#define IZHIKEVICHNEURONMODEL_HPP_

#include <iostream>
#include "NeuronModel.hpp"
namespace szsnn {


	template <typename floatT>
	class Izhikevich_NeuronModel : public NeuronModel<Izhikevich_NeuronModel<floatT> > {
	public:
		enum Params //9
		{
			p_A = 0,	//0
			p_B,		//1
			p_C,		//2
			p_D,		//3
		};
		enum Vars //4
		{
			v_V = 0,
			v_U,
			//v_FIRED,
		};

		typedef NeuronModel<Izhikevich_NeuronModel<floatT> > base_type;
		typedef typename base_type::NeuronType base_neuron_type;
		Izhikevich_NeuronModel()
		{
			floatT _params[4];
			_params[0] =  0.02;
			_params[1] =  0.2;
			_params[2] =  -65.0;
			_params[3] =  2;


			floatT _vars[2];
			_vars[0] = 0.0;
			_vars[1] = 0.0;
			//_vars[2] = 10000.0;
			this->setAll(_params, _vars);

		};
		//IF_curr_exp_NeuronModel(floatT pa, floatT pb, floatT va, floatT vb)
		//	//:
		//	//NeuronModel<TestNeuronModel<floatT> >({pa, pb}, {va, vb}) //TODO tylko w x0
		//{
		//	floatT tparams[2] = {pa, pb};
		//	floatT tvars[2] = {va, vb};
		//	this->setAll(tparams,tvars);
		//};

		virtual void randomizeMe(bool isInhib)
		{
		}
	};

} /* namespace szsnn */
#endif /* TESTNEURONMODEL_HPP_ */
