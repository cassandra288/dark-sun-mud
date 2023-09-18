#pragma once

namespace dss::db
{
	extern pqxx::connection* c;
	
	void init(std::string username, std::string password, std::string database);
}
