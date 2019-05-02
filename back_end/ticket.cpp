#include "ticket.h"

std::ostream &operator << (std::ostream &os, const Seat &s) {
	os << s.type << ' ' << s.num << ' ' << s.price;
	return os;
}

std::ostream &operator << (std::ostream &os, const ticket &t) {
	os << t.tID << ' ' << t.from << ' ' << t.Date << ' ' << t.leave
		<< t.to << ' ' << t.arrive << ' ';
	for (int i = 0; i < t.seat.size(); i++)
		os << t.seat[i] << ' ';
	return os;
}

bool cmpByFirstDim(const std::pair<String,String> &lhs, const std::pair<String,String>& rhs) {
	return lhs.first < rhs.first;
}

std::pair<bool,String> checkTransfer //返回是否构成中转方案和中转站
(const train &T1, const train &T2, const String &from, const String &to) {
	int x = T1.getStationID(from), y = T2.getStationID(to);
	for (int i = x + 1; i < T1.n; i++) {
		// 以T1.s[i]为中转站
		int j = 0;
		while (j < y && T2.s[j] != T1.s[i]) j++;
		if (j < y) return std::make_pair(true,T1.s[i].name);
	}
	return std::make_pair(false,String(""));
}

ticketPair myMin(const ticketPair &p1,const ticketPair &p2) {
	return p1.second.arrive < p2.second.arrive ? p1:p2;
}

void ticketSystem::add(const String &st, const String &id) {
	B.insert(std::make_pair(st,id),id);
}


vector<ticket> ticketSystem::query(const String &from, const String &to,
	const date &d,const String &catalog) {
	auto  V = B.listof(std::make_pair(from,String()), cmpByFirstDim);
	auto  U = B.listof(std::make_pair(to, String()), cmpByFirstDim);
	vector<String> C;
	int i = 0, j = 0;
	while (i <= V.size()) {
		while (U[j].second < V[i].second && j < U.size()) j++;
		if (j == U.size()) break;
		if (V[i].second == U[j].second) 
			C.push_back(V[i].second);
		i++;
	}
	vector<ticket> ret;
	for (i = 0; i < C.size(); i++) {
		train t = TS->query(C[i]).second;
		if (t.ok(from, to) && t.catalog == catalog) ret.push_back(ticket(t,from,to,d));
	}
	return ret;
}

ticketPair ticketSystem::transfer(const String &from, const String &to,
	const date &d,const String &catalog) {
	auto  V = B.listof(std::make_pair(from, String()), cmpByFirstDim);
	auto  U = B.listof(std::make_pair(to, String()), cmpByFirstDim);
	ticketPair ret;
	for (int i = 0; i < V.size(); i++) {
		train T1 = TS->query(V[i].second).second;
		if (T1.catalog != catalog) continue;
		for (int j = 0; j < U.size(); j++) {
			train T2 = TS->query(U[j].second).second;
			if (T2.catalog != catalog) continue;
				auto check = checkTransfer(T1, T2, from, to);
			if (check.first)
				ret = myMin(ret,
					ticketPair(ticket(T1, from, check.second, d), ticket(T2, check.second, to, d))
				);
		}
	}
	return ret;
}