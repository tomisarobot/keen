#ifndef SQLITE_FUNC_H__
#define SQLITE_FUNC_H__

/* Copyright 2012 Tom Kerr
   See LICENSE file for details */

#include <sqlite3.h>
#include <cstring>

namespace keen {
namespace sqlite {

	inline bool open(const std::string &loc, sqlite3 **db, int flags) {
		int return_code = sqlite3_open_v2(loc.c_str(), db, flags, NULL);
		return return_code == SQLITE_OK;
	}

	inline int close(sqlite3 *db) {
		int return_code = sqlite3_close(db);
		return return_code;
	}

	inline bool prepare(sqlite3 *db, const std::string &sql, sqlite3_stmt **stmt) {
		int return_code = sqlite3_prepare_v2(db, sql.c_str(), sql.size(), stmt, 0);
		return return_code == SQLITE_OK;
	}

	inline bool finalize(sqlite3_stmt *stmt) {
		int return_code = sqlite3_finalize(stmt);
		return return_code == SQLITE_OK;
	}

	inline int step(sqlite3_stmt *stmt) {
		int return_code = sqlite3_step(stmt);
		return return_code;
	}

	inline bool reset(sqlite3_stmt *stmt) {
		int return_code = sqlite3_reset(stmt);
		return return_code == SQLITE_OK;
	}

	inline bool bind(sqlite3_stmt *stmt, int i, int val) {
		int return_code = sqlite3_bind_int(stmt, i, val);
		return return_code == SQLITE_OK;
	}

	inline bool bind(sqlite3_stmt *stmt, int i, double val) {
		int return_code = sqlite3_bind_double(stmt, i, val);
		return return_code == SQLITE_OK;
	}

	inline bool bind(sqlite3_stmt *stmt, int i, const std::string &val) {
		const char *str = val.c_str();
		int return_code = sqlite3_bind_text(stmt, i, str, strlen(str), SQLITE_TRANSIENT);
		return return_code == SQLITE_OK;
	}

	template<typename T>
		struct column_traits;

#define COLUMN_TRAIT(TYPE,CODE) \
	template<> \
		struct column_traits< TYPE > { \
			enum { datatype_code = CODE }; \
		};

	COLUMN_TRAIT( int         , SQLITE_INTEGER );
	COLUMN_TRAIT( double      , SQLITE_FLOAT   );
	COLUMN_TRAIT( std::string , SQLITE_TEXT    );

#undef COLUMN_TRAIT

	template<typename T>
		struct is_same {
			bool operator()(sqlite3_value *sval) const {
				return sqlite3_value_type(sval) == column_traits<T>::datatype_code;
			}
			bool operator()(sqlite3_stmt *stmt, int i) const {
				return sqlite3_column_type(stmt, i) == column_traits<T>::datatype_code;
			}
		};

	struct is_not_null {
		bool operator()(sqlite3_value *sval) const {
			return sqlite3_value_type(sval) != SQLITE_NULL;
		}
		bool operator()(sqlite3_stmt *stmt, int i) const {
			return sqlite3_column_type(stmt, i) != SQLITE_NULL;
		}
	};

	struct is_numeric {
		bool operator()(sqlite3_value *sval) const {
			return sqlite3_value_numeric_type(sval) != SQLITE_NULL;
		}
	};

	template<typename T>
		T column_get(sqlite3_stmt *stmt, int i);

	template<>
		inline int column_get<int>(sqlite3_stmt *stmt, int i) {
			return sqlite3_column_int(stmt, i);
		}

	template<>
		inline double column_get<double>(sqlite3_stmt *stmt, int i) {
			return sqlite3_column_double(stmt, i);
		}

	template<>
		inline std::string column_get<std::string>(sqlite3_stmt *stmt, int i) {
			const char *chr = (const char *)sqlite3_column_text(stmt, i);
			std::string str(chr, chr+strlen(chr));
			return str;
		}

	template<typename T, typename V = is_same<T> >
		inline bool column_check(sqlite3_stmt *stmt, int i, T &val) {
			static const V validate;
			if (validate(stmt, i)) {
				val = column_get<T>(stmt, i);
				return true;
			} else {
				return false;
			}
		}

	template<typename T>
		T value_get(sqlite3_value *sval);

	template<>
		inline int value_get<int>(sqlite3_value *sval) {
			return sqlite3_value_int(sval);
		}

	template<>
		inline double value_get<double>(sqlite3_value *sval) {
			return sqlite3_value_double(sval);
		}

	template<>
		inline std::string value_get<std::string>(sqlite3_value *sval) {
			const char *chr = (const char *)sqlite3_value_text(sval);
			std::string str(chr, chr+strlen(chr));
			return str;
		}

	template<typename T, typename V = is_same<T> >
		bool value_check(sqlite3_value *sval, T &val) {
			static const V validate;
			if (validate(sval)) {
				val = value_get<T>(sval);
				return true;
			} else {
				return false;
			}
		}

	template<typename T, typename V = is_same<T> >
		T value(sqlite3_value *sval) {
			T retval;
			if (value_check<T, V>(sval, retval)) {
				return retval;
			} else {
				throw std::runtime_error("value access failed");
			}
		}

	template<typename T, typename V = is_same<T> >
		T value_or(sqlite3_value *sval, const T &defval) {
			T retval;
			if (value_check<T, V>(sval, retval)) {
				return retval;
			} else {
				return defval;
			}
		}

	inline bool column_is_null(sqlite3_stmt *stmt, int i) {
		return sqlite3_column_type(stmt, i) == SQLITE_NULL;
	}

	inline bool clear_bindings(sqlite3_stmt *stmt) {
		int return_code = sqlite3_clear_bindings(stmt);
	   	return return_code == SQLITE_OK;
	}

}} // keen sqlite

#endif

