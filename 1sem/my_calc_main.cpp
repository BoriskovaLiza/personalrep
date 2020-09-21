#include "../std_lib_facilities.h"

struct Token {
	char kind;
	double value;
	string name;
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
	Token(char ch, string n)   : kind(ch), name(n)    { }
};

class Token_stream {
	bool full;
	Token buffer;
public:
	Token_stream() :full(0), buffer(0) { }

	Token get();
	void putback(Token t) { buffer=t; full=true; }

	void ignore(char c);
};

const char let = 'L';
const char quit = 'q';
const char print = ';';
const char number = '8';
const char name = 'a';
const string declkey = "let";
const string quitkey = "quit";
const string prompt = "> ";
const string result = "= ";

Token Token_stream::get()
{
	if (full) { full=false; return buffer; }
	char ch;
	cin >> ch;
	switch (ch) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case print:
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
			string s;
			s += ch;
			while(cin.get(ch) && (isalpha(ch) || (ch == '_') || isdigit(ch))) s += ch;
			cin.putback(ch);
			if (s == declkey) return Token(let);
			if (s == quitkey) return Token(quit);
			return Token(name,s);
		}
		error("Bad token.");
	}
}

void Token_stream::ignore(char c)
{
	if (full && c==buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch = 0;
	while (cin >> ch)
		if (ch == c) return;
}

struct Variable {
	string name;
	double value;
	Variable(string n, double v) :name(n), value(v) { }
};

vector<Variable> names;

double get_value(string s)
{
	for (unsigned int i = 0; i < names.size(); ++i)
		if (names[i].name == s) 
			return names[i].value;
	error("get: undefined name ",s);
}

void set_value(string s, double d)
{
	for (unsigned int i = 0; i <= names.size(); ++i)
		if (names[i].name == s) 
		{
			names[i].value = d;
			return;
		}
	error("set: undefined name ",s);
}

bool is_declared(string s)
{
	for (unsigned int i = 0; i<names.size(); ++i)
		if (names[i].name == s) return true;
	return false;
}

double define_name(string var, double val)
	// add (var,val) to names
{
	if (is_declared(var)) error(var," was declared twice.");
	names.push_back(Variable(var,val));
	return val;
}

Token_stream ts;

double expression();

double primary()
{
	Token t = ts.get();
	switch (t.kind) 
	{
		case '(':
		{	double d = expression();
			t = ts.get();
			if (t.kind != ')') error("A bracket is missing.");
			return d;
		}
		case '-':
			return - primary();
		case number:
			return t.value;
		case name:
			return get_value(t.name);
		default:
			error("Primary expected");
	}
}

double term()
{
	double left = primary();
	Token t = ts.get();
	while(true) {
		switch(t.kind) {
		case '*':
			left *= primary();
			t = ts.get();
			break;
		case '/':
		{	double d = primary();
			if (d == 0) error("Dividing by zero is prohibited.");
			left /= d;
			t = ts.get();
			break;
		}
		case '%':
		{
			int i1 = narrow_cast<int>(left);
			int i2 = narrow_cast<int>(term());
			if (i2 == 0) error("Dividing by zero with '%' is also prohibited.");
			left = i1%i2;
			t = ts.get();
			break;
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}

double expression()
{
	double left = term();
	Token t = ts.get();
	while(true) {
		switch(t.kind) {
		case '+':
			left += term();
			t = ts.get();
			break;
		case '-':
			left -= term();
			t = ts.get();
			break;
		default:
			ts.putback(t);
			return left;
		}
	}
}

double declaration()
{
	Token t = ts.get();
	if (t.kind != name) error ("Name was expected in declaration.");
	string var_name = t.name;

	Token t2 = ts.get();
	if (t2.kind != '=') error("'=' missing in declaration of " ,var_name);

	double d = expression();
	define_name(var_name,d);
	return d;
}

double statement()
{
	Token t = ts.get();
	switch(t.kind) {
	case let:
		return declaration();
	default:
		ts.putback(t);
		return expression();
	}
}

void clean_up_mess()
{
	ts.ignore(print);
}



void calculate()
{
	while(cin)
	  try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t=ts.get();
		if (t.kind == quit) return;
		ts.putback(t);
		cout << result << statement() << endl;
	}
	catch(exception& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

int main()

	try {
		define_name("pi",3.1415926535);
		define_name("e",2.7182818284);

		calculate();

		keep_window_open();
		return 0;
	}
	catch (exception& e) {
		cerr << e.what() << endl;
		keep_window_open("~~");
		return 1;
	}
	catch (...) {
		cerr << "exception\n";
		keep_window_open("~~");
		return 2;
	}

