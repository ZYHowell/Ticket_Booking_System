#include "user.h"

int userSystem::add(const vector<parameter> &V) {
	if (V.size() != 4) return false;
	for (int i = 0; i < 4; i++) if (V[i].first != STRING) return false;
	if (names.count(V[1].second)) return -1;
	B.insert(currentID, user(currentID, V[0].second, V[1].second, V[2].second,V[3].second));
	return currentID++;
}

bool userSystem::login(const int &id, const String pswd) const {
	if (!B.count(id)) return false;
	return B.find(id).match(pswd);
}

std::pair<bool, user> userSystem::query(const int &id) const {
	if (!B.count(id)) return std::make_pair(false,user());
	return std::make_pair(true, B.find(id));
}

bool userSystem::modify(const vector<parameter> &V) {
	if (V.size() != 5) return false;
	if (V[0].first != INT) return false;
	for (int i = 1; i < 5; i++) if (V[i].first != STRING) return false;
	int id = V[0].second.asint();
	if (!B.count(id)) return false;
	user t = B.find(id);
	if (V[1].second != t.name) {
		
	}

}



