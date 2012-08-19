#ifndef SQLITE_OBJ_H__
#define SQLITE_OBJ_H__

/* Copyright 2012 Tom Kerr
   See LICENSE file for details */

#include "sqlite_func.h"
#include "str.h"

namespace keen {
namespace sqlite {

	// not safe regarding bad_alloc
	class sqlite_error : public std::exception {
	public:
		sqlite_error(const char *msg, sqlite3 *db) {
			int error_code = sqlite3_errcode(db);
			if (error_code != SQLITE_OK) {
				message = boost::str( boost::format("%1%: %2%(%3%)") 
						% msg 
						% error_code
						% sqlite3_errmsg(db)
					);
			} else {
				message = msg;
			}
		}
		virtual ~sqlite_error() throw() {}
		const char *what() const throw() { return message.c_str(); }
		std::string message;
	};

	struct DatabaseRef {
		DatabaseRef() : db(0) {}
		~DatabaseRef() { close_safe(); }
		bool close_safe() {
			if (close(db) == SQLITE_OK) {
				db = 0;
				return true;
			} else {
				return false;
			}
		}
		sqlite3 *get_ptr() {return db;}
		sqlite3 **get_raw() {return &db;}
		sqlite3 *db;
	};

	struct StatementRef {
		StatementRef() : stmt(0) {}
		~StatementRef() { close_safe(); }
		bool close_safe() {
			if (finalize(stmt)) {
				stmt = 0;
				return true;
			} else {
				return false;
			}
		}
		sqlite3_stmt *get_ptr() {return stmt;}
		sqlite3_stmt **get_raw() {return &stmt;}
		sqlite3_stmt *stmt;
	};

	class Database {
	public:
		Database(const std::string &loc, int flags)
		{
			db = boost::shared_ptr<DatabaseRef>(new DatabaseRef);
			if (!open(loc, db->get_raw(), flags)) {
				// copy error state before closing
				db->close_safe(); // log if failed
				// throw with error state
				throw std::runtime_error("Database failed");
			}
		}
		boost::shared_ptr<DatabaseRef> get_ref() const {return db;}
		sqlite3 *get_ptr() const {return db->get_ptr();}

	private:
		boost::shared_ptr<DatabaseRef> db;
	};

	class Statement {
	public:
		Statement(const Database &db_, const std::string &sql) 
				: dref(db_.get_ref()), db_ptr(db_.get_ptr()) {
			init(sql);
		}
		Statement(sqlite3 *db_ptr_, const std::string &sql) 
				: db_ptr(db_ptr_) {
			init(sql);
		}
		bool execute() {
			int res;
			switch (res = step(get_ptr())) {
				case SQLITE_ROW:
					return true;
				case SQLITE_DONE:
					return false;
				case SQLITE_BUSY:
				case SQLITE_ERROR:
				default:
					throw sqlite_error("execute failed (Statement)", db_ptr);
			}
		}
		template<typename T>
			void bind(size_t i, T val) {
				if (!sqlite::bind(get_ptr(), i, val))
					throw sqlite_error("bind failed", db_ptr);
			}
		void bind(size_t i, const std::string &val) {
			if (!sqlite::bind(get_ptr(), i, val))
				throw sqlite_error("bind failed", db_ptr);
		}
		template<typename T>
			bool get(size_t i, T &val) {
				return sqlite::column_check(get_ptr(), i, val);
			}
		template<typename T>
			T get_or(size_t i, T def) {
				T val;
				return get(i, val) ? val : def;
			}
		std::string get_or(size_t i, const std::string &def) {
			std::string val;
			return get(i, val) ? val : def;
		}
		size_t size() {
			return sqlite3_column_count(get_ptr());
		}
		void reset() {
			sqlite::reset(get_ptr());
		}
		void clear_bindings() {
			sqlite::clear_bindings(get_ptr());
		}
		sqlite3_stmt *get_ptr() const {return sref->get_ptr();}
		sqlite3_stmt **get_raw() const {return sref->get_raw();}
	private:
		void init(const std::string &sql) {
			sref = boost::shared_ptr<StatementRef>(new StatementRef);
			if (!prepare(db_ptr, sql.c_str(), get_raw()))
				throw sqlite_error("prepare failed (Statement)", db_ptr);
		}
		boost::shared_ptr<StatementRef> sref;
		boost::shared_ptr<DatabaseRef> dref;
		sqlite3 *db_ptr;
	};

	class Transaction {
	public:
		Transaction(Database db)
				: dref(db.get_ref())
				, db_ptr(db.get_ptr())
				, name(keen::next_ident("trans"))
				, active(true) {
			init();
		}
		Transaction(sqlite3 *db_ptr_)
				: db_ptr(db_ptr_)
				, name(next_ident("trans"))
				, active(true) {
			init();
		}
		~Transaction() {
			if (active)
				rollback();
		}
		void rollback() {
			Statement statement(db_ptr, str("ROLLBACK TRANSACTION TO SAVEPOINT %1%;", name));
			statement.execute();
			active = false;
		}
		void commit() {
			Statement statement(db_ptr, str("RELEASE SAVEPOINT %1%;", name));
			statement.execute();
			active = false;
		}

	private:
		void init() {
			Statement statement(db_ptr, str("SAVEPOINT %1%;", name));
			statement.execute();
		}
		boost::shared_ptr<DatabaseRef> dref;
		sqlite3 *db_ptr;
		std::string name;
		bool active;
	};

}} // keen sqlite

#endif

