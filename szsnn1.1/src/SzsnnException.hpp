/**
 * \file SzsnnException.hpp
 *
 * \date 21-11-2012
 * \author pawel
 * \description
 */

#ifndef SZSNNEXCEPTION_HPP_
#define SZSNNEXCEPTION_HPP_

#include <exception>
#include <string>
#include <sstream>

namespace szsnn {

/*
 *
 */
class SzsnnException : public std::exception
{
private:
	std::string msg;

public:
	SzsnnException(std::string _msg):msg(_msg){};
	SzsnnException(int err, std::string _msg){
		std::stringstream ss;
		ss << _msg << ": " << err;
		msg = ss.str();
	}

	~SzsnnException() throw() {}; //TODO poco?

	const char* what() const throw() //TODO poco?
	{
		return msg.c_str();
	}

};

} /* namespace szsnn */
#endif /* SZSNNEXCEPTION_HPP_ */
