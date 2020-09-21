#include "header.h"

Token_stream ts;
Symbol_table symb_t;

/*The grammar for input is:

Calculation:
	Statement
	Print
	Quit
	Help
	Calculation 
	Statement

Statement:
	Declaration
	Expression
	Name "=" Expression

Declaration:
	"let" Name "=" Expression
	"const" Name "=" Expression

Print:
	;

Quit:
	quit
	Quit

Help:
	help
	Help

Expression:
	Term
	Expression + Term
	Expression - Term

Term:
	Subterm
	Term * Subterm
	Term / Subterm
	Term % Subterm

Subterm:
	Interterm
	Subterm ^ Interterm
	
Interterm:
	Primary
	Interterm!

Primary:
	Number
	Name
	( Expression )
	- Primary
	+ Primary

Number:
	floating-point-literal
*/

int main()
	try {
		cout << "Welcome to the calculator.\nType the collocation to be solved here:\n";
		
		symb_t.define("pi",3.1415926535, 1);
		symb_t.define("e",2.7182818284, 1);
	
		calculate();
	
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
