#ifndef NEURON_MODEL_TRAITS_HPP_
#define NEURON_MODEL_TRAITS_HPP_

/**
 * \file neuron_model_traits.hpp
 *
 * \date 23-12-2012
 * \author pawel
 * \description
 */


#include <string>

#include "../locales.h"

namespace szsnn {

template <typename floatPointT>
struct szsnn_supported_floating_point
{
	enum {value = 0};
};

template <>
struct szsnn_supported_floating_point<float>
{
	static const std::string kernel_flags;
	enum {value = 1};
};

template <>
struct szsnn_supported_floating_point<double>
{
	static const std::string kernel_flags;
	enum {value = 1};
};
//template <>
const std::string szsnn_supported_floating_point<double>::kernel_flags ="-D FLOAT_TYPE=double";

//template <>
const std::string szsnn_supported_floating_point<float>::kernel_flags =
		"-D FLOAT_TYPE=float -cl-single-precision-constant";



/*
 * name
 * param count
 * state var count
 * membran pot variale index
 */
template <typename NeuronModelT>
struct neuron_model_traits {};

template <typename floatT>
class TestNeuronModel;

template <typename floatT>
struct neuron_model_traits<TestNeuronModel<floatT> >
{
	typedef floatT float_type;
	static const unsigned param_count = 2;
	static const unsigned var_count = 2;
	static const std::string model_name;
	static const std::string update_kernel_name;
	static const std::string update_kernel_filename;
};
template <typename floatT>
const std::string neuron_model_traits<TestNeuronModel<floatT> >
	::model_name = "test_neuron_model";

template <typename floatT>
const std::string neuron_model_traits<TestNeuronModel<floatT> >
	::update_kernel_name = "testneuron_update";

template <typename floatT>
const std::string neuron_model_traits<TestNeuronModel<floatT> >
	::update_kernel_filename = "testneuron_update_kernel.cl";

/* IF_cur_exp */

template <typename floatT>
class IF_curr_exp_NeuronModel;

template <typename floatT>
struct neuron_model_traits<IF_curr_exp_NeuronModel<floatT> >
{
	typedef floatT float_type;
	static const unsigned param_count = 9;
	static const unsigned var_count = 4;
	static const std::string model_name;
	static const std::string update_kernel_name;
	static const std::string update_kernel_filename;
};
template <typename floatT>
const std::string neuron_model_traits<IF_curr_exp_NeuronModel<floatT> >
	::model_name = "IF_curr_exp";

template <typename floatT>
const std::string neuron_model_traits<IF_curr_exp_NeuronModel<floatT> >
	::update_kernel_name = "IF_curr_exp_update";

template <typename floatT>
const std::string neuron_model_traits<IF_curr_exp_NeuronModel<floatT> >
	::update_kernel_filename = "IF_curr_exp_update_kernel.cl";



/* Izhikevich */

template <typename floatT>
class Izhikevich_NeuronModel;

template <typename floatT>
struct neuron_model_traits<Izhikevich_NeuronModel<floatT> >
{
	typedef floatT float_type;
	static const unsigned param_count = 4;
	static const unsigned var_count = 2;
	static const std::string model_name;
	static const std::string update_kernel_name;
	static const std::string update_kernel_filename;
};
template <typename floatT>
const std::string neuron_model_traits<Izhikevich_NeuronModel<floatT> >
	::model_name = "Izhikevich";

template <typename floatT>
const std::string neuron_model_traits<Izhikevich_NeuronModel<floatT> >
	::update_kernel_name = "Izhikevich_update";

template <typename floatT>
const std::string neuron_model_traits<Izhikevich_NeuronModel<floatT> >
	::update_kernel_filename = "Izhikevich_update_kernel.cl";


/*************************************************************************************
 * CONST_RATE_INPUTMODEL
 */

template <typename floatT>
class Const_rate_InputModel;

template <typename floatT>
struct neuron_model_traits<Const_rate_InputModel<floatT> >
{
	typedef floatT float_type;
	static const unsigned param_count = 1;
	static const unsigned var_count = 1;
	static const std::string model_name;
	static const std::string update_kernel_name;
	static const std::string update_kernel_filename;
};
template <typename floatT>
const std::string neuron_model_traits<Const_rate_InputModel<floatT> >
	::model_name = "Const_rate";

template <typename floatT>
const std::string neuron_model_traits<Const_rate_InputModel<floatT> >
	::update_kernel_name = "Const_rate_update";

template <typename floatT>
const std::string neuron_model_traits<Const_rate_InputModel<floatT> >
	::update_kernel_filename = "Const_rate_update_kernel.cl";


/*************************************************************************************
 * NONE
 */

template <typename floatT>
class NONE_NeuronModel;

template <typename floatT>
struct neuron_model_traits<NONE_NeuronModel<floatT> >
{
	typedef floatT float_type;
	static const unsigned param_count = 0;
	static const unsigned var_count = 0;
	static const std::string model_name;
	static const std::string update_kernel_name;
	static const std::string update_kernel_filename;
};
template <typename floatT>
const std::string neuron_model_traits<NONE_NeuronModel<floatT> >
	::model_name = _SZSNN_NONE_KERNEL;

template <typename floatT>
const std::string neuron_model_traits<NONE_NeuronModel<floatT> >
	::update_kernel_name = _SZSNN_NONE_KERNEL;

template <typename floatT>
const std::string neuron_model_traits<NONE_NeuronModel<floatT> >
	::update_kernel_filename = _SZSNN_NONE_KERNEL;

} /* namespace szsnn */


#endif /* NEURON_MODEL_TRAITS_HPP_ */
