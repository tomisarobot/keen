
#include <boost/regex.hpp>
#include <stdexcept>
#include <string>

/* Copyright 2013 Tom Kerr
   See LICENSE file for details */

namespace keen {

	bool check_flag(const std::vector<std::string>& args, const std::string& flag) {
		static const boost::regex re("^--([^=]+)$");
		for (size_t i = 0; i < args.size(); i++) {
			boost::smatch what;
			if (boost::regex_match(args.at(i), what, re, boost::match_extra)) {
				if (what.size() == 2 && what[1] == flag) {
					return true;
				}
			}
		}
		return false;
	}

	bool check_flag_value(const std::vector<std::string>& args, const std::string& flag, std::string& value) {
		static const boost::regex re("^--([^=]+)=(.+)$");
		for (size_t i = 0; i < args.size(); i++) {
			boost::smatch what;
			if (boost::regex_match(args.at(i), what, re, boost::match_extra)) {
				if (what.size() == 3 && what[1] == flag) {
					value = what[2];
					return true;
				}
			}
		}
		return false;
	}

	std::string get_flag_value(const std::vector<std::string>& args, const std::string& flag) {
		std::string value;
		if (check_flag_value(args, flag, value))
			return value;
		throw std::runtime_error("couldn't find flag in arguments");
	}

	std::string get_flag_value(const std::vector<std::string>& args, const std::string& flag, const std::string& def_value) {
		std::string value;
		if (check_flag_value(args, flag, value))
			return value;
		return def_value;
	}

} // keen

