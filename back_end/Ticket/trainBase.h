#pragma once

#include "tool.h"

static const int maxDate = 31;
static const int maxClassN = 5;

struct station {
	static const int INITIAL_QUANTITY = 2000;
	String name;
	Time arrive, leave, stop;
	int num[maxDate][maxClassN]; // 2018-06-01 到2018-06-30每一天的余票数
	double price[maxClassN];

	station() = default;

	station(const String &str, const Time &_arrive, const Time &Leave, const Time &_stop,
		const vector<double> &V) :name(str), arrive(_arrive), leave(Leave), stop(_stop) {
		for (int i = 0; i < V.size(); i++) price[i] = V[i];
	}

	void init(int classN) {
		for (int i = 0; i < maxDate; i++)
			for (int j = 0; j < classN; j++)
				num[i][j] = INITIAL_QUANTITY;
	}

	bool operator == (const station &s) const { return name == s.name; }
	bool operator != (const station &s) const { return !this->operator==(s); }
};

struct train {
	static const int maxN = 60;
	int n; //经过站的数量
	int classN; //座位类别的数量
	String ID, name, catalog;
	String seatClass[maxClassN];
	station s[maxN];
	bool onsale;

	train() = default;
	train(const String &id, const String &_name, const String &cata,
		const vector<String> &C, const vector<station> &V)
		:ID(id), name(_name), catalog(cata), onsale(false) {
		classN = C.size();
		n = V.size();
		for (int i = 0; i < classN; i++) seatClass[i] = C[i];
		for (int i = 0; i < n; i++) {
			s[i] = V[i];
			s[i].init(classN);
		}
	}

	int getStationID(const String &target) const;
	int getClassID(const String &cls) const;
	bool ok(const String &from, const String &to)const;
};
