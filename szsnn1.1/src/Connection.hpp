
#ifndef CONNECTION_HPP_
#define CONNECTION_HPP_

#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>

#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include "utils.h"
#include "Population.hpp"


namespace szsnn {
	namespace construction {

template <typename floatT>
class Connection {
private:
	typedef std::pair<size_t, size_t> conn;
	typedef typename std::pair<floatT, unsigned> data;
	typedef std::pair<conn, data> conn_pair;

public:


	Connection(size_t from, size_t to):
		fromStartID(from), toStartID(to) {};

	void loadFromFile(const std::string filename);
	void saveToFile(const std::string filename);
	void addConnection(size_t fromIdx, size_t toIdx, floatT weight, unsigned delay);

	typedef  typename std::map<conn, data>::iterator		iterator;
	typedef  typename std::map<conn, data>::const_iterator	const_iterator;


	iterator begin() {return connections.begin(); };
	iterator end() {return connections.end(); };

	const_iterator begin() const {return connections.begin(); };
	const_iterator end() const {return connections.end(); };

	size_t getFromStartID() const {return fromStartID; };
	size_t getToStartID()const {return toStartID; };

private:

	size_t fromStartID;
	size_t toStartID;

	std::map<conn, data> connections;

	struct connCmp {
	    bool operator()(const conn_pair &lhs, const conn_pair &rhs) {
	    	if (lhs.first.first == rhs.first.first) return lhs.first.second < rhs.first.second;
	        return lhs.first.first < rhs.first.first;
	    }
	};
};
template <typename floatT>
void Connection<floatT>::addConnection(size_t fromIdx, size_t toIdx, floatT weight, unsigned delay)
{
	conn key(fromIdx, toIdx);
	std::pair<floatT, unsigned> value(weight, delay);
	connections[key] = value;

}
template <typename floatT>
void Connection<floatT>::loadFromFile(const std::string filename)
{
	std::ifstream infile;
	size_t lineIdx = 1;
	try
	{
		infile.open(filename.c_str(), std::ifstream::in);
	} catch (const std::exception& e)
	{
		THROW_SZSNNEX("SZSNN::CONSTRUCTION::Connection::loadFromFile() error:"
				<< " couldn't open file `" << filename << "`: " <<e.what());
	}

	 if (infile.is_open())
	 {
		 std::string thrash;
		 getline( infile, thrash);
		 getline( infile, thrash );
		 unsigned valueNr = 1;
		 while (infile.good())
		 {
			 std::string line;
			 size_t fromIdx;
			 size_t toIdx;
			 unsigned delay;
			 floatT	weight;
			 valueNr = 1;
			 if (!getline( infile, line )) break;

			 std::vector<std::string> values;
			 std::stringstream buffer(line);
			 std::copy(std::istream_iterator<std::string>(buffer),
					 std::istream_iterator<std::string>(),
					 std::back_inserter(values));

			 valueNr = values.size();
			 if (values.size() != 4) break;
			 try {
				 valueNr = 1;
				 fromIdx = boost::numeric_cast<unsigned long>((boost::lexical_cast<float>(values[0])));
				 valueNr = 2;
				 toIdx = boost::lexical_cast<float>(values[1]);
				 valueNr = 3;
				 weight = boost::lexical_cast<floatT>(values[2]);
				 valueNr = 4;
				 delay = boost::numeric_cast<unsigned>(boost::lexical_cast<float>(values[3]));
			 } catch (boost::bad_lexical_cast &) {
				 std::cout << "BOOST ERROR" << std::endl;
				 break;
			 }
			 addConnection(fromIdx, toIdx, weight, delay);
			 lineIdx += 1;
		 }
		 if(!infile.eof())
		 {
			 infile.close();
			 THROW_SZSNNEX("SZSNN::CONSTRUCTION::Connection::loadFromFile() error:"
					<< " wrong format in file `" << filename << "` at line #"
					<< lineIdx << " at value #"<< valueNr);
		 } else {
			 infile.close();
			 std::cout << "Connection file `"<< filename << "` loaded" <<std::endl;
		 }
	 }
	 else
	 {
		 THROW_SZSNNEX("SZSNN::CONSTRUCTION::Connection::loadFromFile() error:"
				<< " couldn't open file `" << filename << "`");
	 }
}

template <typename floatT>
void Connection<floatT>::saveToFile(const std::string filename)
{
	std::ofstream outfile;
	try
	{
		outfile.open(filename.c_str(), std::ifstream::out);
	} catch (const std::exception& e)
	{
		THROW_SZSNNEX("SZSNN::CONSTRUCTION::Connection::saveToFile() error:"
				<< " couldn't open file `" << filename << "`: " <<e.what());
	}

	std::vector<conn_pair> vec(connections.begin(), connections.end());
	std::sort(vec.begin(), vec.end(), connCmp());

	outfile << "#label p1" <<std::endl;
	outfile << "#label p2" <<std::endl;
	for(typename std::vector<conn_pair>::iterator it = vec.begin(); it != vec.end();  ++it)
	{
		outfile << it->first.first; outfile << "\t";
		outfile << it->first.second; outfile << "\t";
		outfile << it->second.first; outfile << "\t";
		outfile << ((float) it->second.second); outfile << std::endl;;
	}

	outfile.close();
}

	} /* construction */
} /* szsnn */

#endif /* CONNECTION_H_ */
