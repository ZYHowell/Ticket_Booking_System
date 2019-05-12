#pragma once
//#define DEBUGMODE

#include "tool.h"
#include "bplustree.hpp"
#include "train.h"

struct Seat{
	String type;
	double price;
	int num;

	Seat() = default;
	Seat(const String &s, const double &p, const int &n) :type(s), price(p), num(n) {}
};

struct ticket {
	String tID, from, to;
	Time leave, arrive;
	date Date;
	vector<Seat> seat;

	ticket():tID("DEFAULT"){}
	
	bool valid() { return tID != "DEFAULT"; }
	ticket(const train &T, const String &_from, const String &_to,const date &_d)
		:from(_from), to(_to), tID(T.ID),Date(_d){
		int x = T.getStationID(from), y = T.getStationID(to);
		leave = T.s[x].leave;
		arrive = T.s[y].arrive;
		if (leave < T.s[0].leave) Date.day++;
		int d = Date.asint();
		for (int i = 0; i < T.classN; i++) {
			double price = 0;
			for (int j = x + 1; j <= y; j++) price += T.s[j].price[i];
			short _min = T.s[x+1].num[d][i];
			for (int j = x + 2; j <= y; j++)
				_min = min(_min, T.s[j].num[d][i]);
			seat.push_back(Seat(T.seatClass[i],price,_min));
		}
	}
};

typedef std::pair<ticket, ticket> ticketPair;

class ticketSystem {
	trainSystem *TS;
	bplustree< std::pair<String, String>, String, 4096 > B;


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