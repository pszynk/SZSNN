
#ifndef POPULATION_HPP_
#define POPULATION_HPP_
#include <vector>
#include <string>

#include "utils.h"
#include "models/neuron_model_traits.hpp"

namespace szsnn {
	namespace construction {

template <typename NeuronModelT>
class Population {
private:
	typedef neuron_model_traits<NeuronModelT> traits_type;

public:
	typedef  typename traits_type::float_type floatT;

	Population(size_t size, size_t _firstNeuronID, std::string _label=""):
		firstNeuronID(_firstNeuronID), label(_label), neurons(size)
	{
		if(!size)
		{
			THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population() error: Population size="
					<< size << ", must be > 0");
		}
		neurons.resize(size);
	};
	Population():firstNeuronID(0), label("") {};

	void clear();
	void setNeurons(
			const floatT (& _params) [traits_type::param_count],
			const floatT (& _vars) [traits_type::var_count]);
	void setNeuronParameter(const unsigned long idx, const unsigned param_idx, const floatT val);
	void setNeuronParameters(const unsigned param_idx, const floatT val);
	void setNeuronVariables(const unsigned var_idx, const floatT val);

	void loadFromFile(const std::string filename);
	void loadFromTemplateFile(const std::string filename);

	void saveToFile(const std::string filename);
	void saveToTemplateFile(const std::string filename);

	typedef  typename std::vector<NeuronModelT>::iterator		iterator;
	typedef  typename std::vector<NeuronModelT>::const_iterator	const_iterator;


	iterator begin() {return neurons.begin(); };
	iterator end() {return neurons.end(); };

	const_iterator begin() const {return neurons.begin(); };
	const_iterator end() const {return neurons.end(); };

	size_t getFirstId() const {return firstNeuronID; };
	size_t getLastId() const {return firstNeuronID + neurons.size() -1; };
	std::string getLabel() const {return label; };

	size_t getSize() const {return neurons.size(); };

private:
	size_t firstNeuronID;
	std::string label;
	std::vector<NeuronModelT>	neurons;
};

template <typename NeuronModelT>
void Population<NeuronModelT>::clear()
{
	firstNeuronID = 0;
	label = "";
	neurons.clear();
}

template <typename NeuronModelT>
void  Population<NeuronModelT>::setNeurons(
		const floatT (& params) [traits_type::param_count],
		const floatT (& vars) [traits_type::var_count])
{
	for(iterator it = neurons.begin(); it != neurons.end(); ++it)
	{
		it->setAll(params, vars);
	}
}

template <typename NeuronModelT>
void Population<NeuronModelT>::setNeuronParameter(
		const unsigned long idx,
		const unsigned param_idx,
		const floatT val)
{
	neurons[idx].setParam(param_idx, val);
}
template <typename NeuronModelT>
void Population<NeuronModelT>::setNeuronParameters(
		const unsigned param_idx,
		const floatT val)
{
	for(iterator it = neurons.begin(); it != neurons.end(); ++it)
	{
		it->setParam(param_idx, val);
	}
}

template <typename NeuronModelT>
void Population<NeuronModelT>::setNeuronVariables(
		const unsigned var_idx,
		const floatT val)
{
	for(iterator it = neurons.begin(); it != neurons.end(); ++it)
	{
		it->setVar(var_idx, val);
	}
}

/*
 * POPULATION FILE FORMAT:
 * #
 * # < comments >
 * #
 * [neuron model ID]
 * [size]
 * [first ID]
 * [label]
 * [param1] [param2] ... [paramN]
 * [var1] [var2] ... [varN]
 * <next neuron>
 */
template <typename NeuronModelT>
void Population<NeuronModelT>::loadFromFile(
		const std::string filename)
{
	std::ifstream infile;
	clear();
	try
	{
		infile.open(filename.c_str(), std::ifstream::in);
		infile.exceptions ( std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
	} catch (const std::exception& e)
	{
		THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population::loadFromFile() error:"
				<< " couldn't open file `" << filename << "`: " <<e.what());
	}

	size_t populationSize;

	size_t lineIdx = 1;
	unsigned valueNr = 1;

	if (infile.is_open())
	{

		try {
			skipCommentsData(infile, SZSNN_COMMENT_CHAR);
			infile >> populationSize; ++lineIdx;
			infile >> firstNeuronID; ++lineIdx;
			infile >> label; ++lineIdx;


			if (!(populationSize > 0))
			{
				 THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population::loadFromFile() error:"
						<< " wrong data in file `" << filename << "` Population size must"
						<< " be > 0");
			}
			neurons.reserve(populationSize);

			valueNr = 1;
			unsigned neuronsLoaded = 0;
			while (infile.good() || neuronsLoaded == populationSize)
			{
				floatT params[traits_type::param_count];
				floatT vars[traits_type::var_count];
				for (valueNr = 1; valueNr <=  traits_type::param_count; ++valueNr)
				{
					infile >> params[valueNr - 1];
				}
				++lineIdx;
				for (valueNr = 1; valueNr <=  traits_type::var_count; ++valueNr)
				{
					infile >> vars[valueNr - 1];
				}
				neurons[neuronsLoaded].setAll(params, vars);
				++lineIdx;
				++neuronsLoaded;
			}
			if (neuronsLoaded != populationSize)
			{
				THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population::loadFromFile() error:"
								<< " not enough data in `" << filename << "` stopped at line #"
								<< lineIdx << " at value #"<< valueNr << " loaded "
								<< neuronsLoaded << " neurons");
			}
		}
		catch (std::ifstream::failure e)
		{
			THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population::loadFromFile() error:"
				<< " wrong format in file `" << filename << "` at line #"
				<< lineIdx << " at value #"<< valueNr << " what:"<< e.what());
		}
		infile.close();
		std::cout << "Population file `"<< filename << "` loaded" <<std::endl;
	}
	else
	{
	 THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population::loadFromFile() error:"
			<< " couldn't open file `" << filename << "`");
	}
}

/*
 * POPULATION TEMPLATE FILE FORMAT:
 * #
 * # < comments >
 * #
 * [size]
 * [first ID]
 * [label]
 * [params1] [params2] ... [paramsN]
 * [vars1] [vars2] ... [varsN]
 * <end>
 */
template <typename NeuronModelT>
void Population<NeuronModelT>::loadFromTemplateFile(const std::string filename)
{
	std::ifstream infile;
	clear();
	try
	{
		infile.open(filename.c_str(), std::ifstream::in);
		infile.exceptions ( std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
	} catch (const std::exception& e)
	{
		THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population::loadFromTemplateFile() error:"
				<< " couldn't open file `" << filename << "`: " <<e.what());
	}

	size_t populationSize;

	size_t lineIdx = 1;
	unsigned valueNr = 1;
	if (infile.is_open())
	{

		try {
			skipCommentsData(infile, SZSNN_COMMENT_CHAR);
			infile >> populationSize; ++lineIdx;
			infile >> firstNeuronID; ++lineIdx;
			infile >> label; ++lineIdx;


			if (!(populationSize > 0))
			{
				 THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population::loadFromTemplateFile() error:"
						<< " wrong data in file `" << filename << "` Population size must"
						<< " be > 0");
			}
			neurons.resize(populationSize);

			valueNr = 1;

			floatT params[traits_type::param_count];
			floatT vars[traits_type::var_count];
			for (valueNr = 1; valueNr <=  traits_type::param_count; ++valueNr)
			{
				infile >> params[valueNr - 1];
			}
			++lineIdx;
			for (valueNr = 1; valueNr <=  traits_type::var_count; ++valueNr)
			{
				infile >> vars[valueNr - 1];
			}
			++lineIdx;
			setNeurons(params, vars);
		}
		catch (std::ifstream::failure e)
		{
			THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population::loadFromFile() error:"
				<< " wrong format in file `" << filename << "` at line #"
				<< lineIdx << " at value #"<< valueNr << " what:"<< e.what());
		}
		infile.close();
		std::cout << "Population file `"<< filename << "` loaded" <<std::endl;
	}
	else
	{
	 THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population::loadFromFile() error:"
			<< " couldn't open file `" << filename << "`");
	}
}

template <typename NeuronModelT>
void Population<NeuronModelT>::saveToFile(const std::string filename)
{
	std::ofstream outfile;
	if (neurons.empty())
	{
		THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population::saveToFile() error:"
					<< " trying to save empty Population to file `" << filename);
	}
	try
	{
		outfile.open(filename.c_str(), std::ofstream::out);
		outfile.exceptions ( std::ofstream::failbit | std::ofstream::badbit);
	} catch (const std::exception& e)
	{
		THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population::saveToFile() error:"
				<< " couldn't open file `" << filename << "`: " <<e.what());
	}

	try {
		outfile << "# Population file for : " << label << std::endl;
		outfile << "# Model: " << traits_type::modle_name << std::endl;
		outfile << "# Size: " << neurons.size() << std::endl;
		outfile << "# First neuron ID: " << firstNeuronID << std::endl;
		outfile << neurons.size() << std::endl;
		outfile << firstNeuronID << std::endl;
		for(iterator it = neurons.begin(); it != neurons.end(); ++it)
		{
			for (unsigned pIdx = 0; pIdx < traits_type::param_count - 1; ++pIdx)
			{
				outfile << it->getParam(pIdx) << " ";
			}
			outfile << it->getParam(traits_type::param_count - 1) << std::endl;
			for (unsigned vIdx = 0; vIdx < traits_type::var_count - 1; ++vIdx)
			{
				outfile << it->getVar(vIdx) << " ";
			}
			outfile << it->getVar(traits_type::var_count - 1) << std::endl;
		}
	}
	catch (const std::ofstream::failure e)
	{
		THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population::saveToFile() error:"
			<< " output failure couldn't save to file `" << filename << "`: " <<e.what());
	}
	catch (const std::exception& e)
	{
		THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population::saveToFile() error:"
			<< "couldn't save to file `" << filename << "`: " <<e.what());
	}
	outfile.close();
}

template <typename NeuronModelT>
void Population<NeuronModelT>::saveToTemplateFile(const std::string filename)
{
	std::ofstream outfile;
	if (neurons.empty())
	{
		THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population::saveToTemplateFile() error:"
					<< " trying to save empty Population to template `" << filename);
	}
	try
	{
		outfile.open(filename.c_str(), std::ofstream::out);
		outfile.exceptions ( std::ofstream::failbit | std::ofstream::badbit);
	} catch (const std::exception& e)
	{
		THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population::saveToTemplateFile() error:"
				<< " couldn't open file `" << filename << "`: " <<e.what());
	}

	try {
		outfile << "# Population Template file for : " << label << std::endl;
		outfile << "# Model: " << traits_type::modle_name << std::endl;
		outfile << "# Size: " << neurons.size() << std::endl;
		outfile << "# First neuron ID: " << firstNeuronID << std::endl;
		outfile << neurons.size() << std::endl;
		outfile << firstNeuronID << std::endl;
		iterator it = neurons.front();
		for (unsigned pIdx = 0; pIdx < traits_type::param_count - 1; ++pIdx)
		{
			outfile << it->getParam(pIdx) << " ";
		}
		outfile << it->getParam(traits_type::param_count - 1) << std::endl;
		for (unsigned vIdx = 0; vIdx < traits_type::var_count - 1; ++vIdx)
		{
			outfile << it->getVar(vIdx) << " ";
		}
		outfile << it->getVar(traits_type::var_count - 1) << std::endl;

	}
	catch (const std::ofstream::failure e)
	{
		THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population::saveToFile() error:"
			<< " output failure couldn't save to file `" << filename << "`: " <<e.what());
	}
	catch (const std::exception& e)
	{
		THROW_SZSNNEX("SZSNN::CONSTRUCTION::Population::saveToFile() error:"
			<< "couldn't save to file `" << filename << "`: " <<e.what());
	}
	outfile.close();
}

	} /* construction */

} /* szsnn */

#endif /* POPULATION_H_ */
