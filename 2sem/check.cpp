#include <iostream>
#include <iomanip>

extern "C" int check_property(const char *s);

void validate (const char *s, bool ans)
{
	std::cout << std::setw(20) << std::left << s << ' ' << std::flush;
	
	bool res = check_property(s);
	std::cout << "  [" << (res == ans ? "OK":"FAILED")
	<< ']' << std::endl;
}

int main()
{
	validate ("", false);
	validate ("a", false);
	validate ("A", true);
	
	validate ("\\\"", false);
	validate ("arj57uhfr3578jnb,,srfggkD", true);	
	validate ("AAAAAAAAAA", false);
	validate ("Zz", false);
	validate ("zZ", true);
	validate ("Z", true);
	validate ("\\=-865}{246'", false);
}
