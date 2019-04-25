#include "bookingSystem.h"


const int numOfCommands = 16;

const std::string CMD[numOfCommands] = {
	"register","query_profile","modify_profile","modify_privilege",
	"query_ticket,", "query_transfer",
	"buy_ticket", "query_order", "refund_ticket",
	"add_train", "sale_train", "query_train","delete_train","modify_train",
	"clean","exit"
};

void ticketBookingSystem::process
(const std::string &cmd, const vector<token> &V) {
	int idx = 0;
	while (idx < numOfCommands && cmd != CMD[idx]) idx++;
	if (idx == numOfCommands) throw wrong_command();
	switch (idx){

	case 0:	Register(V); break;
	case 1:	profile(V); break;
	case 2: modifyProfile(V); break;
	case 3: modifyType(V); break;
	case 4: queryTicket(V); break;
	case 5: transfer(V); break;
	case 6: buy(V); break;
	case 7: queryOrder(V); break;
	case 8: refund(V); break;
	case 9: addTrain(V); break;
	case 10: sale(V); break;
	case 11: queryTrain(V); break;
	case 12: remove(V); break;
	case 13: modifyTrain(V); break;
	case 14: clear(); break;
	case 15: exit(0);
	default: break;

	}
} 

void ticketBookingSystem::Register(const vector<token> &V) {
	if (User.add(V) == -1) {

	}
}

void ticketBookingSystem::profile(const vector<token> &V) {
	if (V.size() != 1 || V[0].first != INT) throw wrong_parameter();
	auto result = User.query(V[0].second.asint());
	if (result.first) os << result.second << endl;
	else os << 0 << endl;
}