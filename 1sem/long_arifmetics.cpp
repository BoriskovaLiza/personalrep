#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include "../std_lib_facilities.h"

using namespace std;

class Long
{	
private:
	vector<char> arr ; //array of numbers
	//int l; //length
public:
	Long (string s) {
		reverse(s.begin(), s.end());
		
		for (int i = strlen(&s[0]) - 1; i >= 0; ++i)
			arr.push_back(s[i]);
	} //constructor
	
	Long (vector<char> s) {
		reverse(s.begin(), s.end());
		arr = s;
	}
	
	~Long () {
		arr.clear();
	}
	
	vector<char> num() {
		return arr;
	}

	
};

vector<char> operator+(Long &a, Long &b)
{
	int carry = 0;
	
	vector<char> copy_a = a.num();
	vector<char> copy_b = b.num();
	
	for (unsigned int i = 0; (i < copy_a.size()) && (i < copy_b.size()); ++i)
	{
		int buf = static_cast <int>(copy_a[i]) - '0'; 
		buf += static_cast <int>(copy_b[i]) - '0';
		buf += carry;
		carry = 0;
		if (buf / 10) 
		{
			copy_a[i] = static_cast <char> ( (buf % 10) + '0');
			carry = 1;
		}
		else {
			copy_a[i] = static_cast <char> (buf + '0');
		}
	}
	
	if (copy_a.size() < copy_b.size())
	{
		for (unsigned int i = copy_a.size(); i < copy_b.size(); ++i)
		{
			int buf = static_cast <int>(copy_b[i]) - '0';
			buf += carry;
			carry = 0;
			if (buf / 10) 
			{
				copy_a.push_back(static_cast <char> (buf % 10 + '0'));
				carry = 1;
			}
			else 
				copy_a.push_back(static_cast <char> (buf + '0'));
		}
	}
	else if (copy_a.size() > copy_b.size())
	{
		for (unsigned int i = copy_b.size(); i < copy_a.size(); ++i)
		{
			int buf = static_cast <int>(copy_a[i]) - '0';
			buf += carry;
			carry = 0;
			if (buf / 10) 
			{
				copy_a[i] = static_cast <char> (buf % 10 + '0');
				carry = 1;
			}
			else 
				copy_a[i] = static_cast <char> (buf + '0');
		}
	}
	
	if (carry)
		copy_a.push_back('1');
		
	reverse(copy_a.begin(), copy_a.end());
	
	return copy_a;
}

void print( vector<char> r ){
	for(unsigned int i = 0; i < r.size(); i++ ){
		cout << r[i];
	}
}

void clean_up_mess()
{
	while(cin.get()!='\n')
		continue;
};

int main()
try {
	
	vector<char> s1, s2;
	char ch, zn;
	
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
	if (zn != '+')
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
	
	vector <char> k;
	
	switch(zn) {
		case ('+'):
			k = c1 + c2;
			print (k);
			return 0;
		//case ('-'):
			//k = c1 - c2;
			//print (k);
			//return 0;
		default:
			return 1;
	}
}
catch(runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
