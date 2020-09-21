#include "header.h"

double Symbol_table::get(string s)
	{
		for (unsigned int i = 0; i < var_table.size(); ++i)
			if (var_table[i].name == s) 
				return var_table[i].value;
		error("get: undefined name ",s);
	}
	
double Symbol_table::set(string var_name, double d)
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
	
double Symbol_table::define(string var, double val, bool is_cons)
	{
		if (is_declared(var)) error(var, " was declared twice.");
		
		if (is_cons)
				var_table.push_back(Variable{var,val,true});
			else
				var_table.push_back(Variable{var,val,false});
		return val;
	}

bool Symbol_table::is_declared(string s)
	{
		for (unsigned int i = 0; i < var_table.size(); ++i)
			if (var_table[i].name == s) return true;
		return false;
	}
