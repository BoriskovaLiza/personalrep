#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>

using namespace std;

class Long
{	
private:
	vector<char> arr ; //array of numbers
	int l; //length
public:
	Long (string s) {
		reverse(s.begin(), s.end());
		l = strlen(&s[0]);
		
		for (int i = l - 1; i >= 0; ++i)
			arr.push_back(s[i]);
	} //constructor
	
	Long (vector<char> s) {
		reverse(s.begin(), s.end());	
		l = s.size();
		arr = s;
	}
	
	Long () : arr{0}, l{0} {}
	
	~Long () {
		arr.clear();
	}
	
	vector<char> num() {
		return arr;
	}
	
	Long* operator+( Long *&a )
	{
		int carry = 0;
		
		vector<char> copy_a = a->num();
		vector<char> copy_b = this->num(); 
		
		
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
		
		cout << copy_a[0] << endl;
		
		Long *c = new Long(copy_a);
		
		return c;
	}
	
};


	

/*
vector<char> operator-(Long &a, Long &b)
{
	vector<char> add = 0;
	int carry = 0;
	
	vector<char> copy_a = a.num();
	vector<char> copy_b = b.num();
	
	if (copy_a.size() < copy_b.size())
	{
		copy_a = (b - a);
		copy_a.push_back('-');
		return copy_a;
	}
	if (copy_a.size() > copy_b.size())
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
	
	if (carry)
		copy_a.push_back('1');
		
	reverse(copy_a.begin(), copy_a.end());
	
	return copy_a;
}
* */

void print (vector<char> r) {
	reverse( r.begin(), r.end() );
	for (unsigned int i = 0; i < r.size(); i++) {
		cout << r[i];
	}
}



int main()
{	
	vector<char> s1, s2;
	char ch, zn;
	
	while (true)
	{
		ch = cin.get();
		if (isspace(ch)) break;
		s1.push_back(ch);
	}
	
	cin >> zn;
	cin.get();
	
	while (true)
	{
		ch = cin.get();
		if (isspace(ch)) break;
		s2.push_back(ch);
	}

	Long *c1 = new Long(s1);
	Long *c2 = new Long(s2);
	
	Long *c3 = new Long();
	
	switch(zn) {
		case ('+'):{
			c3 = c1+c2;
			print (c3->num()); 
			return 0;
		}
		//case ('-'):
			//k = c1 - c2;
			//print (k);
			//return 0;
		default:
			return 1;
	}
}
