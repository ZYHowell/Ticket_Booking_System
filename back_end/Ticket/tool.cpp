#include "tool.h"

std::ostream &operator << (std::ostream &os,const Time &t){
	if (t.hour == -1) os << "xx:xx";
	else os << t.hour << ':' << t.minute;
	return os;
}

std::ostream &operator << (std::ostream &os, const date &d) {
	os << d.year << '-' << d.month << '-' << d.day;
	return os;
}

std::ostream &operator << (std::ostream &os, const String &str) {
	os<<str.s;
	return os;
}

int String::asint()const {
	std::stringstream ss(s);
	int ret;
	ss >> ret;
	return ret;
}

double String::asdouble()const {
	std::stringstream ss(s);
	double ret;
	ss >> ret;
	return ret;
}

date String::asdate() const {
	if (l != 10) throw wrong_parameter();
	return date(2018, (s[5] - '0') * 10 + s[6] - '0', (s[8] - '0') * 10 + s[9] - '0');
}

Time String::asTime() const {
	if (l != 5) throw wrong_parameter();
	if (s[0] == 'x') return Time(-1, -1);
	return Time((s[0] - '0') * 10 + s[1] - '0', (s[3] - '0') * 10 + s[4] - '0');
}
