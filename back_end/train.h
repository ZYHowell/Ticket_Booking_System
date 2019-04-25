#pragma once
/*
车次相关的功能

*/
#include "bplustree.hpp"
#include "tool.h"

static const int maxDate = 31;
static const int maxClassN = 5;

struct station {
	static const int INITIAL_QUANTITY = 2000;
	String name;
	Time arrive, leave;
	int num[maxDate][maxClassN]; // 2018-06-01 到2018-06-30每一天的余票数
	double price;

	void init(int classN) {
		for (int i = 0; i < maxDate; i++)
			for (int j = 0; j < classN; j++)
				num[i][j] = INITIAL_QUANTITY;
	}

	bool operator == (const station &s) const { return name == s.name; }
	bool operator != (const station &s) const { return !this->operator==(s); }
};

class trainSystem;

struct train {
	static const int maxN = 60;
	int n; //经过站的数量
	int classN; //座位类别的数量
	String ID, name, catalog, seatClass[maxClassN];
	station s[maxN];
	bool onsale;

	train() = default;
	train(const String &id, const String &_name, const String &cata, 
		const vector<String> &C, const vector<station> &V)
		:ID(id), name(_name), catalog(cata), onsale(false){
		classN = C.size();
		n = V.size();
		for (int i = 0; i < classN; i++) seatClass[i] = C[i];
		for (int i = 0; i < n; i++) {
			s[i] = V[i];
			s[i].init(classN);
		}
	}

	int getStationID(const String &target) const ;
	int getClassID(const String &cls) const;
	bool ok(const String &from, const String &to)const;
};


class trainSystem {
	bplustree<String,train,4096> B;
public:
	trainSystem() {
		B.initialize("trainData", "trainBptFile", "trainAlloc", "trainBptAlloc");
	}

	void add(const String &id, const String &name, const String catalog,
		const vector<String> &classes, const vector<station> &V);

	bool sale(const String &id);

	std::pair<bool,train> query(const String &id);

	bool remove(const String &id);

	bool modify(const String &id, const String &name, const String catalog,
		const vector<String> &classes, const vector<station> &V);

	void modifyTicket(const String &id, const String &Station, const String &cls, int d, int delta);

	
};
