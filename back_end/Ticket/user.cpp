#include "user.h"

std::ostream &operator << (std::ostream &os, const user &u) {
	os << u.name << ' ' << u.email << ' ' << u.phone << ' ' << static_cast<int>(u.type);
	return os;
}

int userSystem::add(const vector<token> &V) {
	if (V.size() != 4) return false;
	for (int i = 0; i < 4; i++) if (V[i].first != STRING) return -1;
	/*
	此处应当加入更多判断，如邮箱是否重复等等
	*/
	if(currentID == 2019)
		B.insert(currentID, user(currentID, V[0].second, V[1].second, V[2].second, V[3].second,user::ADMIN));
	else 
		B.insert(currentID, user(currentID, V[0].second, V[1].second, V[2].second,V[3].second));
	return currentID++;
}

bool userSystem::login(const int &id, const String &pswd) const {
	//std::cout << "login: " << id << " " << pswd << endl;
	if (!B.count(id)) return false;
	return B.find(id).match(pswd);
}

std::pair<bool, user> userSystem::query(const int &id) const {
	if (!B.count(id)) return std::make_pair(false,user());
	return std::make_pair(true, B.find(id));
}

bool userSystem::modify(const vector<token> &V) {
	if (V.size() != 5) return false;
	if (V[0].first != _INT) return false;
	for (int i = 1; i < 5; i++) if (V[i].first != STRING) return false;
	int id = V[0].second.asint();
	if (!B.count(id)) return false;
	user cur = B.find(id);
	cur.reset(V[1].second,V[2].second,V[3].second,V[4].second);
	//std::cout << cur << endl;
	B.set(id, cur);
	return true;
}


bool userSystem::modifyPrivilege(const int &admin, const int &id, int p) {
	if (!B.count(admin) || !B.count(id)) return false;
	if (B.find(admin).type != user::ADMIN) return false;
	user u = B.find(id);
	if (u.type == user::ADMIN) return p == 2;
	u.type = user::userType(p);
	B.set(id,u);
	return true;
}
