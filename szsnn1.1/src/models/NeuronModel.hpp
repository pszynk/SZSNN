/**
 * \file NeuronModel.hpp
 *
 * \date 22-12-2012
 * \author pawel
 * \description
 */

#ifndef NEURONMODEL_HPP_
#define NEURONMODEL_HPP_

//#include <array>
#include <string>
#include <iostream>
#include <cstring>

#include <boost/utility/enable_if.hpp>

#include "../SzsnnException.hpp"
#include "../utils.h"
#include "neuron_model_traits.hpp"


namespace szsnn {

//template <typename T>
//struct neuron_model_traits;


template <typename NeuronModelT>
class NeuronModel{

public:
	enum NeuronType {
		EXCITATORY = 0,
		INHIBITORY,
	};
private:


	//static NeuronModelT _generator;
protected:
	typedef neuron_model_traits<NeuronModelT> traits_type;
	typedef  typename traits_type::float_type floatT;

	floatT	params[traits_type::param_count];
	floatT	vars[traits_type::var_count];


	inline bool checkParamIdx(unsigned idx) const
		{ return idx < traits_type::param_count; };

	inline bool checkVarIdx(unsigned idx) const
		{ return idx < traits_type::var_count; };



	NeuronModel()
	{
		memset(params, 0, sizeof(floatT) * traits_type::param_count);
		memset(vars, 0, sizeof(floatT) * traits_type::var_count);
	};

	NeuronModel(
		const floatT (& _params) [traits_type::param_count],
		const floatT (& _vars) [traits_type::var_count])
	{
		memcpy(params, _params, sizeof(params));
		memcpy(vars, _vars, sizeof(vars));
	};

public:
	// GETTERS ------------------------
	template <unsigned paramIdx>
	inline typename boost::enable_if_c<paramIdx < traits_type::param_count, floatT>::type
	getParam() const {return params[paramIdx];} ;

	inline floatT getParam(unsigned paramIdx) const {
		if (checkParamIdx(paramIdx))
			return params[paramIdx];
		THROW_SZSNNEX("SZSNN::NeuronModel::getParam(paramIdx): paramIdx=["<<paramIdx
				<< "] >= param_count=["<< traits_type::param_count<<"]")
		};


	template <unsigned varIdx>
	inline typename boost::enable_if_c<varIdx < traits_type::var_count, floatT>::type
	getVar() const {return vars[varIdx];} ;

	inline floatT getVar(unsigned varIdx) const {
		if (checkVarIdx(varIdx))
			return vars[varIdx];
		THROW_SZSNNEX("SZSNN::NeuronModel::getVar(varIdx): varIdx=["<<varIdx
				<< "] >= var_count=["<< traits_type::var_count<<"]")
	} ;

	// SETTERS ------------------------
	template <unsigned paramIdx>
	inline typename boost::enable_if_c<paramIdx < traits_type::param_count, void>::type
	setParam(floatT _new) {params[paramIdx] = _new;} ;

	inline void setParam(unsigned paramIdx, floatT _new) {
		if (checkParamIdx(paramIdx))
			params[paramIdx] = _new;
		else {
		THROW_SZSNNEX("SZSNN::NeuronModel::getParam(paramIdx): paramIdx=["<<paramIdx
				<< "] >= param_count=["<< traits_type::param_count<<"]")
		}
	};


	template <unsigned varIdx>
	inline typename boost::enable_if_c<varIdx < traits_type::var_count, void>::type
	setVar(floatT _new) {vars[varIdx] = _new;} ;

	inline void setVar(unsigned varIdx, floatT _new) {
		if (checkVarIdx(varIdx))
			vars[varIdx] = _new;
		else {
		THROW_SZSNNEX("SZSNN::NeuronModel::getVar(varIdx): varIdx=["<<varIdx
				<< "] >= var_count=["<< traits_type::var_count<<"]")
		}
	};

	void setAll(
		const floatT (& _params) [traits_type::param_count],
		const floatT (& _vars) [traits_type::var_count])
	{
		memcpy(params, _params, sizeof(params));
		memcpy(vars, _vars, sizeof(vars));
	}


	//virtual void debug(NeuronType type) = 0;

	//virtual NeuronModelT _genRandom(NeuronType type) = 0;

	// virtual void randomizeMe(NeuronType type) = 0;
	virtual void randomizeMe(bool type) = 0;
//	static NeuronModelT getRandomNeuron(NeuronType type)
//	{
//		return _generator->_genRandom(type);
//	}
//	NeuronModel();
//	virtual ~NeuronModel();
};




} /* namespace szsnn */
#endif /* NEURONMODEL_HPP_ */
