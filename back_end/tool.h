#pragma once

#include <string>
#include <iostream>
#include <windows.h>
#include "vector.hpp"
struct Time;
struct date;


using sjtu::vector;

enum TYPE{STRING,DATE,TIME,INT,DOUBLE};


class String;
typedef std::pair<TYPE,String> parameter;


class String {
	static const int SIZE = 40;
	char s[SIZE+1];
	int l;

	int cmp(const String &a) const {
		if (l != a.l) return l < a.l ? -1 : 1;
		for (int i = 0; i < l; i++)
			if (s[i] != a.s[i])
				return  s[i] < a.s[i] ? -1 : 1;
		return 0;
	}
public:
	String() :l(0) {}
	String(std::string str) {
		l = str.length();
		if (l > SIZE) throw std::string("Name is too long.");
		for (int i = 0; i < l; i++) s[i] = l;
		s[l] = '\0';
	}
	String(const String &other) {
		l = other.l;
		for (int i = 0; i <= l; i++) s[i] = other.s[i];
	}
	bool operator < (const String &a) const { return cmp(a) == -1; }
	bool operator == (const String &a) const { return cmp(a) == 0; }
	bool operator > (const String &a) const { return cmp(a) == 1; }
	int asint() const ;
	double asdouble() const;
	date asdate() const;
	Time asTime() const;
};

struct Time {
	int hour, minuite;
	Time(int h = 0, int m = 0) :hour(h), minuite(m) {}
	bool operator < (const Time &t) const {
		if (hour != t.hour) return hour < t.hour;
		return minuite < t.minuite;
	}
	bool operator == (const Time &t) const {
		return hour == t.hour && minuite == t.minuite;
	}
};

struct date {
	int year, month, day;
	date(int y = 0,int m = 1,int d = 1):year(y),month(m),day(d){}
	bool operator < (const date &d) const {
		if (year != d.year) return year < d.year;
		return month < d.month || (month == d.month && day < d.day);
	}
	bool operator == (const date &d) const {
		return year == d.year && month == d.month && day == d.day;
	}
};