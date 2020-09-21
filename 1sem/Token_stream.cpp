#include "header.h"

Token Token_stream::get()
{
	if (full) { full=false; return buffer; }
	
	char ch;
	cin >> ch;
	
	if (ch == '\n')
		ch = print;
		
	switch (ch) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case fact:
	case power:
	case print:
	case let:
	case cons:
	case help:	
	case quit:
	case '=':
		return Token(ch);
	case '.': case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	{	cin.putback( ch );
		double val;
		cin >> val;
		return Token(number,val);
	}
	default:
		if (isalpha(ch)) {
			ch = tolower(ch);
			string s;
			s += ch;
			while(cin.get(ch) && (isalpha(ch) || (ch == '_') || isdigit(ch))) 
				{ch = tolower(ch);
					s += ch;}
			cin.putback(ch);
			if (s == declkey) return Token(let);
			if (s == conskey) return Token(cons);
			if (s == quitkey) return Token(quit);
			if (s == helpkey) return Token(help);
			return Token(name,s);
		}
	error("I can't read this.");
	}
}

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch = 0;
	while (cin)
	{
		ch = cin.get();
		if (ch == c) return;
	}
}
