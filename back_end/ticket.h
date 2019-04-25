#pragma once

#include "tool.h"
#include "bplustree.hpp"
#include "train.h"

struct ticket {
	String tID, from, to;
	Time leave, arrive;
	date Date;
	vector< std::pair<String, int> > seat;

	ticket():tID("DEFAULT"){}
	ticket(const train &T, const String &_from, const String &_to,const date &_d)
		:from(_from), to(_to), tID(T.ID),Date(_d){
		int x = T.getStationID(from), y = T.getStationID(to);
		leave = T.s[x].leave;
		arrive = T.s[x].arrive;
		for (int i = 0; i < T.classN; i++) {
			int _min = T.s[x].num[Date.day][i];
			for (int j = x + 1; j <= y; j++)
				_min = min(_min, T.s[j].num[Date.day][i]);
			seat.push_back(std::make_pair(T.seatClass[i],_min));
		}
	}
};

typedef std::pair<ticket, ticket> ticketPair;

class ticketSystem {
	trainSystem *TS;
	bplustree< std::pair<String, String>, String, 4096 > B;
	void _add(const String &st, const String &id);

public:
	ticketSystem() {
		B.initialize("ticketData", "ticketBptFile", "ticketAlloc", "ticketBptAlloc");
	}
	void init(trainSystem *_TS) { TS = _TS; }

	void add(const vector<String> &stations, const String &id);

	vector<ticket> query(const String &from, const String &to,const date &d);
	
	ticketPair transfer(const String &from, const String &to,const date &d);

}; 
