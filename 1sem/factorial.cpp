#include "../std_lib_facilities.h"

struct Token {
	char kind;
	double value;
	string name;
	
	Token(char ch) :kind(ch), value(0) {}
	Token(char ch, double val) :kind(ch), value(val) {}
	Token(char ch, string n)   :kind(ch), name(n) {}
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

const char  let = '#',
			cons = '@',
			quit = '~',
			help = '"',
			print = ';',
			fact = '!',
			power = '^',
			number = '8',
			name = 'a';

const string conskey = "const",
			declkey = "let",
			quitkey = "quit",
			helpkey = "help",
			prompt = "> ",
			result = "= ";


Token Token_stream::get()
{
	if (full) { full=false; return buffer; }
	
	char ch;
	cin >> ch;
	
	if (isspace(ch) && ch == '/n')
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
		throw std::runtime_error{"I can't read this."};
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
	bool constant;
	Variable(string n, double v, bool is_const) :name(n), value(v), constant(is_const) { }
};

Token_stream ts;

double expression();
double primary();

class Symbol_table {
public:
	double get(string s)
	{
		for (unsigned int i = 0; i < var_table.size(); ++i)
			if (var_table[i].name == s) 
				return var_table[i].value;
		error("get: undefined name ",s);
	}
	
	double set(string var_name, double d)
	{
		for (unsigned int i = 0; i <= var_table.size(); ++i)
			if (var_table[i].name == var_name) 
			{
				if (var_table[i].constant == 0)
				{
					var_table[i].value = d;
					return d;
				}
				else error(var_name, " was declared as a constant.");
			}
		error("set: undefined name ",var_name);
	}
	
	double define(string var, double val, bool is_cons)
	{
		if (is_declared(var)) error(var," was declared twice.");
		
		if (is_cons)
				var_table.push_back(Variable{var,val,true});
			else
				var_table.push_back(Variable{var,val,false});
		return val;
	}

	bool is_declared(string s)
	{
		for (unsigned int i = 0; i < var_table.size(); ++i)
			if (var_table[i].name == s) return true;
		return false;
	}
	
private:
	vector<Variable> var_table;
};

Symbol_table symb_t;



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
		{
			Token t2 = ts.get();
			if (t2.kind == '=')
			{
				if (!symb_t.is_declared(t.name)) error ("Unknown variable.\n");
				
				double d = expression();
				symb_t.set(t.name, d);
				return d;
			}
			else
			{
				ts.putback(t2);
				return symb_t.get(t.name);
			}
		}
		default:
			error("primary expected");
	}
}

double subterm();

double term()
{
	double left = subterm();	//subterm
	Token t = ts.get();
	while(true) {
		switch(t.kind) {
		case '*':
			left *= primary();
			t = ts.get();
			break;
		case '/':
		{	double d = primary();
			if (d == 0) error("Divided by zero.");
			left /= d;
			t = ts.get();
			break;
		}
		case '%':
		{
			int i1 = narrow_cast<int>(left);
			int i2 = narrow_cast<int>(term());
			if (i2 == 0) error("Divided by zero with '%'.");
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

int factorial(int n)
{
	int res(1);
	
	for (int i = 1; i <= n; ++i)
		res *= i;
	
	return res;
};

double subterm()
{
	double left = primary();
	Token t = ts.get();
	switch(t.kind) {
		case fact:
		{
			int _left = narrow_cast<int>(left);
			return factorial(_left);
		}
		case power:
		{
			Token t2 = ts.get();
			return pow(left, t2.value);
		}
		default:
			ts.putback(t);
			return left;
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

double declaration(bool is_cons)
{
	Token t = ts.get();
	if (t.kind != name) error ("Name was expected in declaration.");
	string var_name = t.name;

	Token t2 = ts.get();
	if (t2.kind != '=') error("'=' missing in declaration of " ,var_name);

	double d = expression();
	return symb_t.define(var_name,d, is_cons);
}



double statement()
{
	Token t = ts.get();
	switch(t.kind) {
	case let:
		return declaration(false);
	case cons:
		return declaration(true);
	default:
		ts.putback(t);
		return expression();
	}
}

void clean_up_mess()
{
	ts.ignore(print);
}

void instructions()
{
	cout << "-------------\n"
		<< "   This is a calculating programm.\nIt can be used to solve math problems.\n"
		<< "Type the expression below. Arabian numbers and operators such as + - * / % () ^ ! are allowed\n"
		<< "   Creating constants and variables is avaliable by typing 'let' or 'const' variable name '=' its definition.\n"
		<< "The name of the variable must start with a letter, and include ONLY letters, numbers and '_' symbol.\n"
		<< "The register (case) has no effect on variables.\n"
		<< "   Type 'quit' to exit the calculator.\n"
		<< "-------------\n";
}

void calculate()
{
	while(cin)
	  try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t=ts.get();
		if (t.kind == quit) return;
		if (t.kind == help)
		{
			instructions();
			continue;
		}
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
		cout << "Welcome to the calculator.\nType the collocation to be solved here:\n";
		
		symb_t.define("pi",3.1415926535, 1);
		symb_t.define("e",2.7182818284, 1);

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

