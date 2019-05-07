#pragma once
/*
车次相关的功能

*/
#include "bplustree.hpp"
#include "trainBase.h"
#include "tool.h"
#include "log.h"

class trainSystem {
	bplustree<String,train,4096> B;
public:
	trainSystem() {
		B.init("trainData", "trainAlloc");
	}

	bool add(const String &id, const String &name, const String catalog,
		const vector<String> &classes, const vector<station> &V);

	bool sale(const String &id);

	std::pair<bool,train> query(const String &id);

	bool remove(const String &id);

	bool modify(const String &id, const String &name, const String catalog,
		const vector<String> &classes, const vector<station> &V);

	bool modifyTicket(purchaseLog *log, const vector<token> &V,int f = 1);

	void clear() {
		B.clear();
	}
};

std::ostream &operator << (std::ostream &os, const train &t);