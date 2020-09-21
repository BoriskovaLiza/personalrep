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
			
struct Variable {
	string name;
	double value;
	bool constant;
	Variable(string n, double v, bool is_const) :name(n), value(v), constant(is_const) { }
};

class Symbol_table {
public:
	double get(string s);
	double set(string var_name, double d);
	double define(string var, double val, bool is_cons);
	bool is_declared(string s);
	
private:
	vector<Variable> var_table;
};


extern Token_stream ts;
extern Symbol_table symb_t;

double expression();
double primary();
double term();
double subterm();
double interterm();
double declaration();
double statement();
void calculate();
void clean_up_mess();
void instructions();
