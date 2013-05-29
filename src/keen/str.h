#ifndef STR_H__
#define STR_H__

/* Copyright 2012 Tom Kerr
   See LICENSE file for details */

#include <atomic>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/format.hpp>
#include <sstream>
#include <string>

namespace keen {

	inline std::string str_one(boost::format &fmt) {
		return boost::str(fmt);
	}

	template<typename A1, typename... Args>
		std::string str_one(boost::format &fmt, const A1 &a1, const Args... args) {
			fmt % a1;
			return str_one(fmt, args...);
		}

	template<typename... Args>
		std::string str(const std::string &fmt_str, const Args... args) {
			boost::format fmt(fmt_str);
			return str_one(fmt, args...);
		}

	template<typename IT>
		std::string str_trans(const std::string &fmt_str, IT beg, IT end) {
			if (!(beg < end))
				return "";
			bool once = false;
			std::stringstream ss;
			for (IT i = beg; i != end; i++) {
				if (once)
					ss << ',';
				ss << str(fmt_str, *i);
				once = true;
			}
			return ss.str();
		}

	template<>
		inline std::string str_trans<size_t>(const std::string &fmt_str, size_t beg, size_t end) {
			return str_trans(fmt_str, boost::counting_iterator<size_t>(beg), boost::counting_iterator<size_t>(end));
		}

	inline std::string next_ident(const std::string &base) {
		static std::atomic<size_t> count(1);
		return str("%1%%2%", base, count++);
	}

} // keen

#endif

