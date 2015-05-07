#ifndef CONST_RATE_INPUTMODEL_HPP_
#define CONST_RATE_INPUTMODEL_HPP_


#include "NeuronModel.hpp"
namespace szsnn {

	template <typename floatT>
	class Const_rate_InputModel : public NeuronModel<Const_rate_InputModel<floatT> > {
	public:
		enum Params //1
		{
			p_RATE = 0,	//0
		};
		enum Vars //1
		{
			v_LAST = 0,
		};

		typedef NeuronModel<Const_rate_InputModel<floatT> > base_type;
		typedef typename base_type::NeuronType base_neuron_type;
		Const_rate_InputModel()
		{
			floatT _params[1];
			_params[0] = 10.0;

			floatT _vars[1];
			_vars[0] = 0.0;

			this->setAll(_params, _vars);

		};

		virtual void randomizeMe(bool isInhib)
		{
		}
	};

} /* namespace szsnn */
#endif /* CONST_RATE_INPUTMODEL_HPP_ */
