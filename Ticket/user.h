#pragma once

#include "dataFile.h"
#include "tool.h"
#include "index.h"

class user;

class userSystem {
	friend class user;
	dataFile<user> B;

	bplustree<String, int, 4096> EMAIL;
	bplustree<String, int, 4096> PHONE;

	static const int INITIAL_ID = 2019;

public:
	userSystem() :B("user") {
		EMAIL.init("email","email_alloc");
		PHONE.init("phone", "phone_alloc");
	}

	int add(const vector<token> &V);

	int login(const int &id, const String &pswd) const;

	std::pair<bool, user> query(const int &id) const;

	int modify(const vector<token> &V);

	int modify_nopswd(const vector<token> &V);

	bool modifyPrivilege(const int &master, const int &id, int p);

	void clear() {
		B.clear();
	}

	int loginWithInfo(const String &info, const String &pswd) const;
};


struct user {
	enum userType {COMMON=1,ADMIN};

	int id;
	userType type;
	shortString passwd, email, phone,name;

public:
	user() = default;
	user(const int &i, const String &n, const String &p,
		const String &e, const String &ph, const userType &t = COMMON)
		:id(i), name(n), passwd(p), email(e), phone(ph), type(t){}

	bool match(String _pswd) { return _pswd == passwd; }
	void reset(userSystem *US,const String &n, const String &p, const String &e, const String &ph) {
		name = n; 
		passwd = p;

		US->EMAIL.remove(email);
		email = e;
		US->EMAIL.insert(email, id);

		US->PHONE.remove(phone);
		phone = ph;
		US->EMAIL.insert(email, id);
	}
	void reset_nopswd(userSystem *US,const String &n, const String &e, const String &ph) {
		name = n;

		US->EMAIL.remove(email);
		email = e;
		US->EMAIL.insert(email, id);

		US->PHONE.remove(phone);
		phone = ph;
		US->EMAIL.insert(email, id);
	}
};


std::ostream &operator << (std::ostream &os, const user &u);