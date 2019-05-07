#include "bookingSystem.h"


const int numOfCommands = 17;


const std::string CMD[numOfCommands] = {
	"register","query_profile","modify_profile","modify_privilege",
	"query_ticket,", "query_transfer",
	"buy_ticket", "query_order", "refund_ticket",
	"add_train", "sale_train", "query_train","delete_train","modify_train",
	"clean","exit",
	"login"
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
	case 15: Exit();
	case 16: login(V); break;
	default: break;

	}
} 

void ticketBookingSystem::Exit() {
	os << "BYE" << endl;
}

void ticketBookingSystem::Register(const vector<token> &V) {
	os << User.add(V) << endl;
}

void ticketBookingSystem::profile(const vector<token> &V) {
	if (V.size() != 1 || V[0].first != _INT) throw wrong_token();
	auto result = User.query(V[0].second.asint());
	if (result.first) os << result.second << endl;
	else os << 0 << endl;
}

void ticketBookingSystem::modifyProfile(const vector<token> &V) {
	os << (User.modify(V)) << endl;
}

void ticketBookingSystem::modifyType(const vector<token> &V) {
	if (V.size() != 3) throw wrong_token();
	for (int i = 0; i < 3; i++)
		if (V[i].first != _INT) throw wrong_token();
	os << User.modifyPrivilege(V[0].second.asint(), V[1].second.asint(), V[2].second.asint())<<endl;
}

void  ticketBookingSystem::queryTicket(const vector<token> &V) {
	if (V.size() != 4 || V[2].first != _DATE ||
		V[0].first != STRING || V[1].first != STRING || V[3].first != STRING)
		throw wrong_token();
	auto U = Ticket.query(V[0].second, V[1].second, V[2].second.asdate(),V[3].second);
	for (int i = 0; i < U.size(); i++)
		os << U[i] << endl;
}

void ticketBookingSystem::transfer(const vector<token> &V) {
	if (V.size() != 4 || V[2].first != _DATE ||
		V[0].first != STRING || V[1].first != STRING || V[3].first != STRING)
		throw wrong_token();
	auto t = Ticket.transfer(V[0].second, V[1].second, V[2].second.asdate(), V[3].second);
	if (t.first.valid()) os << t.first << '\n' << t.second << endl;
	else os << -1 << endl;
}

void ticketBookingSystem::buy(const vector<token> &V) {
	if (V.size() != 7 ||V[0].first != _INT || V[1].first != _INT || V[5].first != _DATE)
		throw wrong_token();
	if (!Train.modifyTicket(&Log, V))
		throw wrong_token();
}

void ticketBookingSystem::queryOrder(const vector<token> &V) {
	auto U = Log.query(V);
	os << U.size() << endl;
	for (int i = 0; i < U.size(); i++) os << U[i] << endl;
}

void ticketBookingSystem::refund(const vector<token> &V) {
	if (V.size() != 7 ||
		V[0].first != _INT || V[1].first != _INT || V[5].first != _DATE)
		throw wrong_token();
	Train.modifyTicket(&Log, V, -1);
}

void ticketBookingSystem::sale(const vector<token> &V) {
	if (V.size() != 1 || V[0].first != STRING) throw wrong_token();
	bool sale = Train.sale(V[0].second);
	os << sale << endl;
	if (sale) {
		train t = Train.query(V[0].second).second;
		vector<String> V;
		for (int i = 0; i < t.n; i++) Ticket.add(t.s[i].name, t.ID);
	}
}

void ticketBookingSystem::remove(const vector<token> &V) {
	if (V.size() != 1 || V[0].first != STRING) throw wrong_token();
	os << Train.remove(V[0].second)<< endl;
}

void ticketBookingSystem::clear() {
	Log.clear();
	Train.clear();
	Ticket.clear();
	User.clear();
}

void ticketBookingSystem::queryTrain(const vector<token> &V) {
	if (V.size() != 1 || V[0].first != STRING) throw wrong_token();
	auto result = Train.query(V[0].second);
	if (!result.first) os << 0 << endl;
	else os << result.second << endl;
}

void ticketBookingSystem::addTrain(const vector<token> &V) {
	vector<String> classes;
	vector<station> S;
	if (V[3].first != _INT || V[4].first != _INT) throw wrong_token();
	int n = V[3].second.asint(),m = V[4].second.asint();
	if (V.size() != 5 + m + n * (m + 4)) throw wrong_token();
	for (int i = 0; i < m; i++) classes.push_back(V[i+5].second);
	for (int i = 0; i < n; i++) {
		int st = 4 + m + i * (4 + m) + 1;
		vector<double> price;
		for (int j = 0; j < m; j++) price.push_back(V[st + 4 + j].second.asdouble());
		S.push_back(station(V[st].second,
			V[st + 1].second.asTime(), V[st + 2].second.asTime(), V[st + 3].second.asTime(), price));
	}
	os << Train.add(V[0].second, V[1].second, V[2].second, classes, S) << endl;
}

void ticketBookingSystem::modifyTrain(const vector<token> &V) {
	vector<String> classes;
	vector<station> S;
	if (V[3].first != _INT || V[4].first != _INT) throw wrong_token();
	int n = V[3].second.asint(), m = V[4].second.asint();
	if (V.size() != 5 + m + n * (m + 4)) throw wrong_token();

	for (int i = 0; i < m; i++) classes.push_back(V[i + 5].second);
	for (int i = 0; i < n; i++) {
		int st = 4 + m + i * (4 + m) + 1;
		vector<double> price;
		for (int j = 0; j < m; j++) price.push_back(V[st + 4 + j].second.asdouble());
		S.push_back(station(V[st].second,
			V[st + 1].second.asTime(), V[st + 2].second.asTime(), V[st + 3].second.asTime(), price));
	}
	os << Train.modify(V[0].second, V[1].second, V[2].second, classes, S) << endl;
}

void ticketBookingSystem::login(const vector<token> &V) {
	if (V.size() != 2 || V[0].first != _INT || V[1].first != STRING) throw wrong_parameter();
	//for (int i = 0; i < V.size(); i++) std::cout << V[i].second << " "; std::cout << endl;
	std::cout<<User.login(V[0].second.asint(), V[1].second)<<endl;
}