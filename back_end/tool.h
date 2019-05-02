#pragma once

#include <string>
#include <iostream>
#include <windows.h>
#include "vector.hpp"
#include <sstream>
struct Time;
struct date;
using sjtu::vector;

class String;

enum TYPE{STRING,DATE,TIME,INT,DOUBLE};
typedef std::pair<TYPE, String> token;


typedef std::pair<TYPE,String> parameter;


class String {
	friend std::ostream &operator << (std::ostream &os, const String &str);

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
	String(const char *c) {
		l = strlen(c) + 1;
		for (int i = 0; i < l; i++) s[i] = c[i];
		s[l] = '\0';
	}
	String(const String &other) {
		l = other.l;
		for (int i = 0; i <= l; i++) s[i] = other.s[i];
	}
	bool operator < (const String &a) const { return cmp(a) == -1; }
	bool operator == (const String &a) const { return cmp(a) == 0; }
	bool operator != (const String &a) const { return cmp(a) != 0; }
	bool operator > (const String &a) const { return cmp(a) == 1; }
	int asint() const ;
	double asdouble() const;
	date asdate() const;
	Time asTime() const;
};

struct Time {
	friend std::ostream &operator << (std::ostream &os, const String &Time);
	int hour, minute;
	Time(int h = 0, int m = 0) :hour(h), minute(m) {}
	bool operator < (const Time &t) const {
		if (hour != t.hour) return hour < t.hour;
		return minute < t.minute;
	}
	bool operator == (const Time &t) const {
		return hour == t.hour && minute == t.minute;
	}

	Time operator - (const Time &rhs) const {
		if (minute > rhs.minute) return Time(hour - rhs.hour, minute - rhs.minute);
		return Time(hour - rhs.hour - 1, minute - rhs.minute + 60);
	}
};

struct date {
	friend std::ostream &operator << (std::ostream &os, const String &date);
	int year, month, day;
	date(int y = 0,int m = 1,int d = 1):year(y),month(m),day(d){}

	bool operator < (const date &d) const {
		if (year != d.year) return year < d.year;
		return month < d.month || (month == d.month && day < d.day);
	}
	bool operator == (const date &d) const {
		return year == d.year && month == d.month && day == d.day;
	}
	bool operator != (const date &d) const {
		return !this->operator==(d);
	}

	int asint() { 
		if (month != 6)  throw wrong_parameter();
		return day-1; 
	}
};

std::ostream &operator << (std::ostream &os, const Time &t);

std::ostream &operator << (std::ostream &os, const date &d);

std::ostream &operator << (std::ostream &os, const String &str);