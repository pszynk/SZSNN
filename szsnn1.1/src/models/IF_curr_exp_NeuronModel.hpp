#ifndef IFCURREXPNEURONMODEL_HPP_
#define IFCURREXPNEURONMODEL_HPP_

#include <iostream>
#include "NeuronModel.hpp"
namespace szsnn {


	template <typename floatT>
	class IF_curr_exp_NeuronModel : public NeuronModel<IF_curr_exp_NeuronModel<floatT> > {
	public:
		enum Params //9
		{
			p_V_REST = 0,	//0
			p_CM,			//1
			p_TAU_M,		//2
			p_TAU_REFRAC,	//3
			p_TAU_SYN_E,	//4
			p_TAU_SYN_I,	//5
			p_I_OFFSET,		//6
			p_V_RESET,		//7
			p_V_THRESH		//8
		};
		enum Vars //4
		{
			v_V = 0,
			v_IE,
			v_II,
			v_FIRED,
		};

		typedef NeuronModel<IF_curr_exp_NeuronModel<floatT> > base_type;
		typedef typename base_type::NeuronType base_neuron_type;
		IF_curr_exp_NeuronModel() 
		{
			floatT _params[9];
			_params[0] = -65.0;
			_params[1] =  1.0;
			_params[2] =  20.0;
			_params[3] =  0;
			_params[4] =  5.0;
			_params[5] =  5.0;
			_params[6] =  0.0;
			_params[7] = -65.0;
			_params[8] = -65.0;

			floatT _vars[4];
			_vars[0] = 0.0;
			_vars[1] = 0.0;
			_vars[2] = 0.0;
			_vars[3] = 10000.0;
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
