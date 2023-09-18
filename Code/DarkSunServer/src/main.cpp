#include "dsspch.h"
#include "gateway/server.h"
#include "core/db.h"

dss::gateway::Server* s;

std::string to_send =
"\x1b[1z<!ELEMENT RName '<FONT COLOR=Red><B>' FLAG = \"RoomName\">"
"<!ELEMENT RDesc FLAG='RoomDesc'>"
"<!ELEMENT Rexits '<FONT COLOR=aqua>' FLAG='RoomExit'>"
"<!ELEMENT Ex '<SEND>'>"
"<!ELEMENT Prompt FLAG=\"Prompt\">"
"<!ELEMENT Hp FLAG=\"Set hp\">"
"<!ELEMENT MaxHp FLAG=\"Set maxhp\">"
"<RName>The Main Temple</RName>\n"
"\x1b[1z<RDesc>This is the main hall of the MUD where everyone starts.\n"
"\x1b[1zMarble arches lead south into the town, and there is a <i>lovely</i>\n"
"\x1b[1z<send \"drink &text;\">fountain</send> in the center of the temple,</RDesc>\n"
"\x1b[1z<Rexits>Exits: <Ex>N</Ex>, <Ex>S</Ex>, <Ex>E</Ex>, <Ex>W</Ex></Rexits>\n\n"
"\x1b[1z<Prompt>[<Hp>100</Hp>/<MaxHp>120</MaxHp>hp]</Prompt>\x1b[3z";

void on_message(dss::gateway::Session& session, std::string message)
{
	DSS_LOG_INFO("Message from {}: {}", session.id(), message);

	session.send_message(to_send);
}

int main()
{
	dss::Log::init();
	
	dss::db::init(std::getenv("DB_USERNAME"), std::getenv("DB_PASSWORD"), "darksun_server");

	asio::io_service io_service;

	s = new dss::gateway::Server(io_service, 5252, [](dss::gateway::Session& session) {
		std::thread t([&session]() {
			DSS_LOG_TRACE("New Session: {}", session.id());

			session.send_message("\xff\xfb\x5b");

			std::string data = session.wait_for_message();
			if (data != "\xff\xfd\x5b")
			{
				DSS_LOG_ERROR("MXP Not supported", data);
				session.disconnect();
				return;
			}
			
			session.send_message(to_send);

			session.register_message_callback(on_message);
		});
		t.detach();
	});

	io_service.run();

	delete s;

	return 0;
}
