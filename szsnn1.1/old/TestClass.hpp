/**
 * \file TestClass.hpp
 *
 * \date 22-11-2012
 * \author pawel
 * \description
 */

#ifndef TESTCLASS_HPP_
#define TESTCLASS_HPP_

#include "Neuron_Interface.hpp"
//#include "Test_Neuron_Interface.hpp"

namespace szsnn {

/*
 *
 */
class TestClass {
public:
	Neuron_Interface<CPU_neuronType> NI;
	TestClass();
	virtual ~TestClass();
	void byleco() {
		Neuron_Interface<CPU_neuronType> ni1;
		ni1.print_me();
		NI.print_me();
	};
};

} /* namespace szsnn */
#endif /* TESTCLASS_HPP_ */
