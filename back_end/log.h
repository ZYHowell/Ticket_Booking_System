#pragma once

#include "bplustree.hpp"
#include "tool.h"
#include "train.h"
#include "exceptions.h"

class purchaseLog;

struct keyInfo {
	int userID;
	String catalog, trainID, from, to;
	date Date;

	keyInfo(const int &u, const date &d, const String &c,
		const String &t, const String &_from, const String &_to)
		:userID(u), Date(d), catalog(c), trainID(t), from(_from), to(_to) {}
	
	keyInfo(const int &u, const date &d, const String &c)
		:userID(u), Date(d), catalog(c) {}

	bool operator < (const keyInfo &k) const {
		if (userID != k.userID) return userID < k.userID;
		if (Date!= k.Date) return Date < k.Date;
		if (catalog != k.catalog) return catalog < k.catalog;
		if (trainID != k.trainID) return trainID < k.trainID;
		if (from != k.from) return from < k.from;
		return to < k.to;
	}
};

struct Detail{
	static const int maxClassN = 5;
	Time leave, arrive;
	String seatClass[maxClassN];
	int classN,num[maxClassN];

	Detail() = default;
	Detail(const train &T, const String from,const String to, const String &seatCls, const int &quantity) {
		classN = T.classN;
		for (int i = 0; i < classN; i++) 
			seatClass[i] = T.seatClass[i],num[i] = 0;
		num[T.getClassID(seatCls)] = quantity;
		leave = T.s[T.getStationID(from)].leave;
		arrive = T.s[T.getStationID(to)].arrive;
	}
	void modify(const int &cls, const int &delta) {
		num[cls] += delta;
	}

	bool isZero() {
		for (int i = 0; i < classN; i++)
			if (num[i]) return false;
		return true;
	}
};

typedef std::pair<keyInfo, Detail> record;

class purchaseLog {
	bplustree<keyInfo, Detail, 4096> B;

public:
	purchaseLog() {
		B.initialize("logData","logBptFile","logAlloc","logBptAlloc");
	}

	void buy(const keyInfo &info,const train &T,const String &seatClass,const int &n);
	void refund(const keyInfo &info,const train &T,const String &seatClass,const int &n);
	vector<record> query(vector< std::pair<TYPE, String> > &V) const;

};

