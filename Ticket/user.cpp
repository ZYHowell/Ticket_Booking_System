#include "user.h"

std::ostream &operator << (std::ostream &os, const user &u) {
	os << u.name << ' ' << u.email << ' ' << u.phone << ' ' << static_cast<int>(u.type);
	return os;
}

int userSystem::add(const vector<token> &V) {
	if (V.size() != 4) return -1;
	for (int i = 0; i < 4; i++) if (V[i].first != STRING) return 0;
	if (EMAIL.count(V[2].second)) return -1;
	if (PHONE.count(V[3].second)) return -1;
	int id = B.size() + INITIAL_ID;
	if (B.size() == 0) 
		B.push(user(id, V[0].second, V[1].second, V[2].second, V[3].second, user::ADMIN));
	else 
		B.push(user(id, V[0].second, V[1].second, V[2].second, V[3].second));

	EMAIL.insert(V[2].second, id);
	PHONE.insert(V[3].second, id);
	return id;
}

int userSystem::login(const int &id, const String &pswd) const {
	//std::cout << "login: " << id << " " << pswd << endl;
	if (id < INITIAL_ID || id - INITIAL_ID>= B.size()) return false;
	if(B.get(id - INITIAL_ID + 1).match(pswd)) return id;
	else return 0;
}

std::pair<bool, user> userSystem::query(const int &id) const {
	if (id - INITIAL_ID >= B.size()) return std::make_pair(false,user());
	return std::make_pair(true,B.get(id-INITIAL_ID+1));
}

int userSystem::modify(const vector<token> &V) {
	if (V.size() != 5 && V.size()!= 6) return false;
	int id = V[0].second.asint();

	auto result1 = EMAIL.find(V[3].second);
	auto result2 = PHONE.find(V[4].second);
	if (result1.first && result1.second != id) return -1;
	if (result2.first && result2.second != id) return -2;

	if (id - INITIAL_ID >= B.size()) return false;
	auto cur = B.get(id - INITIAL_ID + 1);
	cur.reset(this,V[1].second,V[2].second,V[3].second,V[4].second);
	//std::cout << cur << endl;
	if (V.size() == 6) cur.type = user::userType(V[5].second.asint());
	B.replace(cur,id-INITIAL_ID+1);
	return true;
}

int userSystem::modify_nopswd(const vector<token> &V) {
	if (V.size() != 4 && V.size() != 5) return false;
	
	int id = V[0].second.asint();
	if (id - INITIAL_ID >= B.size()) return false;
	auto cur = B.get(id - INITIAL_ID + 1);

	auto result1 = EMAIL.find(V[2].second);
	auto result2 = PHONE.find(V[3].second);
	if (result1.first && result1.second != id) return -1;
	if (result2.first && result2.second != id) return -2;

	cur.reset_nopswd(this,V[1].second, V[2].second, V[3].second);
	//std::cout << cur << endl;
	if (V.size() == 5) cur.type = user::userType(V[4].second.asint());
	B.replace(cur, id - INITIAL_ID + 1);
	return true;
}

bool userSystem::modifyPrivilege(const int &admin, const int &id, int p) {
//	if (!B.count(admin) || !B.count(id)) return false;
	if (admin - INITIAL_ID >= B.size() || id - INITIAL_ID >= B.size()) return false;
	user _admin =B.get(admin-INITIAL_ID+1);
	if (_admin.type != user::ADMIN) return false;
	user u = B.get(id - INITIAL_ID + 1);
	if (u.type == user::ADMIN) return p == 2;
	u.type = user::userType(p);
	B.replace(u,id-INITIAL_ID+1);
	return true;
}

int userSystem::loginWithInfo(const String &info, const String &pswd) const {
	if (info.isEmail()) {
		auto result = EMAIL.find(info);
		if (!result.first) return 0;
		return login(result.second, pswd);
	}
	else {
		auto result = PHONE.find(info);
		if (!result.first) return 0;
		return login(result.second, pswd);
	}
}