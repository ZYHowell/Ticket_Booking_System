#pragma once

#include "bplustree.hpp"
#include "tool.h"


struct user {
	int id;
	String name, passwd, email, phone;
public:
	user() = default;
	user(const int &i, const String &n, const String &p,
		const String &e, const String &ph)
		:id(i), name(n), passwd(p), email(e), phone(ph) {}

	bool match(String _pswd) { return _pswd == passwd; }
};

class userSystem {
	bplustree<int, user, 4096> B;
	bplustree<String, int, 4096> names;
	int currentID;
public:
	userSystem() {
		B.initialize("userData", "userBptFile", "userAlloc", "userBptAlloc");
		names.initialize("userName", "userNameBpt", "userNameAlloc", "userNameBptAlloc");
		currentID = B.size() + 2019;
	}

	int add(const vector<parameter> &V);

	bool login(const int &id, const String pswd) const;

	std::pair<bool, user> query(const int &id) const;

	bool modify(const vector<parameter> &V);

	bool modifyPrivilege(const int &master, const int &id, int p);
};