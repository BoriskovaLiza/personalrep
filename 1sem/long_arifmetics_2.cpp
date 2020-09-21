#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include "../std_lib_facilities.h"

using namespace std;

int to_int (char c)
{
	if (!isdigit (c))
	  error ("is not a digit");
	  
	return c - '0';
}

char to_char (int i)
{
	if (i < 0 || i > 9)
	  error ("is not a digit");
	  
	return static_cast<char>('0' + i);
}


class Long
{	
private:
	vector<char> arr;
public:
	Long (string s) {
		reverse(s.begin(), s.end());
		
		for (int i = strlen(&s[0]) - 1; i >= 0; ++i)
			arr.push_back(s[i]);
	}
	
	Long (vector<char> s) {
		reverse(s.begin(), s.end());
		arr = s;
	}
	
	Long& operator += (const Long &b);
	Long& operator -= (const Long &b);
	
	char operator[](unsigned i) const;
	int size() const { return arr.size(); }
	
	Long& operator-() { arr.push_back('-'); return *this; }
};


Long& Long::operator += (const Long &b)
{
	int carry = 0;

	size_t n = std::max (arr.size(), b.arr.size());
	arr.resize (n, '0');
	for (size_t i = 0; i < n; ++i)
	{
		char bi = (i < b.arr.size()) ? b.arr[i] : '0';
		int tmp = to_int (arr[i]) + to_int (bi) + carry;
		arr[i] = to_char (tmp % 10);
		carry = tmp / 10;
	}
	
	if (carry)
	  arr.push_back ('1');
	
	return *this;
}

Long& Long::operator -= (const Long &b)
{
	int full = 0;

	for (size_t i = 0; i < arr.size(); ++i)
	{
		char bi = (i < b.arr.size()) ? b.arr[i] : '0';
		int tmp = to_int (arr[i]) - to_int (bi) - full;
		full = (tmp < 0);
		arr[i] = to_char ((tmp + 10) % 10);
	}
	
	while (arr.size() > 1 && arr.back() == '0')
	{
		arr.pop_back();
	}
	
	return *this;
}

bool operator >= (const Long &a, const Long &b)
{
	if (a.size() != b.size()) return a.size() > b.size();

	for (unsigned i = a.size(); i > 0 ; --i)
	{
		if (a[i - 1] != b[i - 1]) return (a[i - 1] > b[i - 1]);
	}
	return true;
}

Long operator - (const Long& a, const Long& b)
{	
	if (a >= b)
	{
		Long tmp = a;
		tmp -= b;
		return tmp;
	}
	else
	{
		Long tmp = b;
		tmp -= a;
		return -tmp;
	}
}

Long operator + (const Long& a, const Long& b)
{
	Long tmp = a;
	tmp += b;
	return tmp;
}

char Long::operator[](unsigned i) const
{
	if (i >= arr.size())
		error("invalid array index");
	return arr[i];
}

void clean_up_mess()
{
	while(cin.get()!='\n')
		continue;
};

ostream& operator<<(ostream& os, const Long &a){
	for (int i = a.size(); i > 0; --i)
		os << a[i-1];
	return os;
}

void calculate()
{
	try {
	vector<char> s1, s2;
	char ch, zn;
	
	while (cin.get() != 'q' || cin.eof())
	{	
		cin.unget();
		
		s1.clear();
		s2.clear();
		
		while (true)
		{
			ch = cin.get();
			if (isspace(ch)) break;
			if (!isdigit(ch)) error("Only digits are allowed");
			s1.push_back(ch);
		}
		
		cin >> zn;
		
		if (!zn)
			error("The sign is missing");
		if (zn != '+' && zn != '-')
			error("Wrong sign");
		 
		cin.get();
		while (true)
		{
			ch = cin.get();
			if (isspace(ch)) break;
			if (!isdigit(ch)) error("Only digits are allowed");
			s2.push_back(ch);
		}
	
		Long c1(s1);
		Long c2(s2);
	
		switch(zn) {
		case ('+'):{
			cout << (c1 + c2) << endl; 
			break;
		}
		case ('-'): {
			cout << (c1 - c2) << endl;
			break;
		}
		}
		cout.clear();
	}
	cin.unget();
}
catch(runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

int main()
try {
	while (cin.get() != 'q' || cin.eof())
	{
		cin.unget();
		calculate();
	}
	return 0;
}
catch(runtime_error& e) {
		cerr << e.what() << endl;
		return 1;
}
catch(...) {
		cerr << "exception\n";
		return 2;
	}
