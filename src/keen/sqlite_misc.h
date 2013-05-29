#ifndef SQLITE_MISC_H__
#define SQLITE_MISC_H__

/* Copyright 2012 Tom Kerr
   See LICENSE file for details */

#include "sqlite_obj.h"

namespace keen {
namespace sqlite {

	inline void bind_one(Statement &, int) {}

	template<typename A1, typename...Args>
		void bind_one(Statement &s, int i, A1 a1, Args... args) {
			s.bind(i, a1);
			bind_one(s, i+1, args...);
		}

	template<typename... Args>
		void bind_all(Statement &statement, Args... args) {
			bind_one(statement, 1, args...);
		}

	template<typename... Args>
		bool execute_once(sqlite3 *db_ptr, const std::string &sql, Args... args) {
			Statement statement(db_ptr, sql);
			bind_all(statement, args...);
			return statement.execute();
		}

	template<typename... Args>
		bool execute_once(Database &db, const std::string &sql, Args... args) {
			return execute_once(db.get_ptr(), sql, args...);
		}

}} // keen sqlite

#endif

