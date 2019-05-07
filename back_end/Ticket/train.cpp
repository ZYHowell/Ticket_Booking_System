#include "train.h"


std::ostream &operator << (std::ostream &os, const train &t) {
	os << t.ID << ' ' << t.name << ' ' << t.catalog << ' '
		<< t.n << ' ' << t.classN << ' ';
	for (int i = 0; i < t.classN; i++) os << t.seatClass[i] << ' ';
	for (int i = 0; i < t.n; i++) {
		os << t.s[i].name << ' ' << t.s[i].arrive << ' ' << t.s[i].leave << ' ' << t.s[i].stop << ' ';
		for (int j = 0; j < t.classN; j++)
			os << '¥' << t.s[i].price[j] << ' ';
		os << '\n';
	}
	return os;
}

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


bool trainSystem::add(const String &id, const String &name, const String catalog,
	const vector<String> &classes, const vector<station> &V) {
	//std::cout << "add: " << id << endl;
	if (B.count(id)) return false;
	B.insert(id,train(id,name,catalog,classes,V));
	return true;
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
	return true;
}

bool trainSystem::modifyTicket(purchaseLog *log, const vector<token> &V,int f) {

	train t = B.find(V[2].second);
	date Date = V[5].second.asdate();
	int d = Date.asint();
	int c = t.getClassID(V[6].second);
	int st = t.getStationID(V[3].second), ed = t.getStationID(V[4].second);
	int delta = V[1].second.asint() * f;
	if (st == -1 || ed == -1 || st >= ed) throw wrong_token();
	for (int i = st; i <= ed; i++)
		if (t.s[i].num[d][c] + delta < 0) return false;
	for (int i = st; i <= ed; i++)
		t.s[i].num[d][c];
	if (f == 1)
		log->buy(keyInfo(V[0].second.asint(), Date, t.catalog, t.ID, V[3].second, V[4].second), t, V[6].second, delta);
	else
		log->refund(keyInfo(V[0].second.asint(), Date, t.catalog, t.ID, V[3].second, V[4].second), t, V[6].second, -delta);
}
