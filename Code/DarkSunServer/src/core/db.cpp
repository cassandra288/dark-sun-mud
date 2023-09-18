#include "dsspch.h"
#include "db.h"

namespace dss::db
{
	pqxx::connection* c = nullptr;
	
	void init(std::string username, std::string password, std::string database)
	{
		try
		{
			c = new pqxx::connection("postgresql://" + username + ":" + password + "@localhost:5432/" + database);
			DSS_DATABASE_LOG_INFO("Database connection established");
		}
		catch (const std::exception& e)
		{
			DSS_DATABASE_LOG_ERROR("Init failed: {}", e.what());
		}
	}
}

