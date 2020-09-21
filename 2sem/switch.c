


int str_leng(const char* s)
{
	int n = 0;
	for (; *s != '\0'; ++s)
		++n;
	return n;
}
