#pragma once
//#define DEBUGMODE

#include "tool.h"
#include "bplustree.hpp"
#include "train.h"

struct Seat{
	int id;
	float price;
	int num;

	Seat() = default;
	Seat(const int &type, const float &p, const int &n) :id(type), price(p), num(n) {}
};

struct ticket {
	shortString tID, from, to;
	Time leave, arrive;
	date Date,Date2;
	Seat seat[seatN];
	ticket():tID("DEFAULT"){}
	
	bool valid() { return tID != "DEFAULT"; }
	ticket(const train &T, const String &_from, const String &_to,const date &_d)
		:from(_from), to(_to), tID(T.ID),Date(_d),Date2(_d){
		int x = T.getStationID(from), y = T.getStationID(to);
		leave = T.s[x].leave;
		arrive = T.s[y].arrive;
		int d = Date.asint();
		Date.day += T.getDay(x);
		Date2.day += T.getDay2(y);
		for (int i = 0; i < seatN; i++) {
			int clsid = T.getClassID(seatType[i]);
			if (clsid != -1) {
				float price = 0;
				for (int j = x + 1; j <= y; j++) price += T.s[j].price[clsid];
				short _min = T.s[x + 1].num[d][clsid];
				for (int j = x + 2; j <= y; j++)
					_min = min(_min, T.s[j].num[d][clsid]);
				seat[i] = Seat(i, price, _min);
			}
			else seat[i] = Seat(i, -1, -1);
		}
	}
};

typedef std::pair<ticket, ticket> ticketPair;

class ticketSystem {
	trainSystem *TS;
	bplustree< std::pair<shortString, shortString>, shortString, 4096> B;


public:
	ticketSystem() {
		B.init("ticketData", "ticketAlloc");
	}
	void init(trainSystem *_TS) { TS = _TS; }

	void add(const String &st, const String &id);
	vector<ticket> query(const String &from, const String &to,
		const date &d, const String &catalog);
	
	ticketPair transfer(const String &from, const String &to,
		const date &d, const String &catalog);
	void clear() {
		B.clear();
	}
}; 

std::ostream &operator << (std::ostream &os, const Seat &s);


std::ostream &operator << (std::ostream &os, const ticket &t);
