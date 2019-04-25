#include "train.h"

int train::getStationID(const String &target)const {
	int i = 0;
	while (i < n && s[i].name != target) i++;
	return i == n ? -1 : i;
}

int train::getClassID(const String &cls) const {
	int i = 0;
	while (i < classN && seatClass[i] != cls) i++;
	return i == n ? -1 : i;
}

bool train::ok(const String &from, const String &to)const {
	return getStationID(from) < getStationID(to);
}


void trainSystem::add(const String &id, const String &name, const String catalog,
	const vector<String> &classes, const vector<station> &V) {
	B.insert(id,train(id,name,catalog,classes,V));
}

bool trainSystem::sale(const String &id) {
	if (!B.count(id)) return false;
	train t = B.find(id);
	if (t.onsale) return false;
	t.onsale = true;
	return true;
}

std::pair<bool, train> trainSystem::query(const String &id) {
	if (!B.count(id)) return std::make_pair(false,train());
	return std::make_pair(true, B.find(id));
}

bool trainSystem::remove(const String &id) {
	if (!B.count(id)) return false;
	B.remove(id);
	return true;
}

bool trainSystem::modify(const String &id, const String &name, const String catalog,
	const vector<String> &classes, const vector<station> &V) {
	if (!B.count(id) || B.find(id).onsale) return false;
	B.set(id,train(id,name,catalog,classes,V));
}

void trainSystem::modifyTicket(const String &id, const String &Station,
	const String &cls, int d,int delta) {
	if (d < 0 || d > 30) throw std::string("Date out of bound");
	if (!B.count(id)) throw std::string("Train ID does not exist.");
	train t = B.find(id);
	int i = 0,c = t.getClassID(cls);
	if (c == t.classN) throw std::string("Seat class does not exist.");
	while (t.s[i].name != Station && i < t.n) i++;
	if (i == t.n) throw std::string("This train does not pass by the given station.");
	t.s[i].num[d][c] += delta;
}

