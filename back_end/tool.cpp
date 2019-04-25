#include "tool.h"

std::ostream &operator << (std::ostream &os,const Time &t){
	os << t.hour << ':' << t.minuite;
	return os;
}

std::ostream &operator << (std::ostream &os, const date &d) {
	os << d.year << '-' << d.month << '-' << d.day;
	return os;
}
