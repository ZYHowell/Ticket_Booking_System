#include "log.h"


bool partailCompare(const keyInfo &lhs,const keyInfo &rhs) {
	if (lhs.userID != rhs.userID) return lhs.userID < rhs.userID;
	if (lhs.Date != rhs.Date) return lhs.Date < rhs.Date;
	return lhs.catalog < rhs.catalog;
}

std::ostream &operator << (std::ostream &os, const record &t) {
	os << t.first.trainID << ' ' 
		<< t.first.from << ' ' << t.first.Date << ' ' << t.second.leave << ' '
		<< t.first.to << ' ' << t.first.Date << ' ' << t.second.arrive << ' ';
	for (int i = 0; i < t.second.classN; i++)
		os << t.second.seatClass[i] << ' ' << t.second.num[i] << ' ';
	return os;
}

void purchaseLog::buy(const keyInfo &info, const train &T, const String &seatClass, const int &n) {
	if (B.count(info)) {
		Detail rec = B.find(info);
		rec.modify(T.getClassID(seatClass),n);
		B.set(info, rec);
	}
	else 
		B.insert(info,Detail(T,info.from,info.to,seatClass,n));
}

void purchaseLog::refund(const keyInfo &info, const train &T, const String &seatClass, const int &n) {
	Detail rec = B.find(info);
	rec.modify(T.getClassID(seatClass), -n);
	if (rec.isZero()) B.remove(info);
	else B.set(info, rec);
}

vector<record> purchaseLog::query(vector< std::pair<TYPE,String> > &V)  const {
	if (V.size() != 3 || V[0].first != INT || V[1].first != DATE || V[2].first != STRING)
		throw wrong_parameter();
	keyInfo key(V[0].second.asint(),V[1].second.asdate(),V[2].second);
	return B.listof(key,partailCompare);
}

