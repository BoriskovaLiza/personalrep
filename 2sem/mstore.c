int check_property(const char *s)
{
	char c = 0;
	for (const char *p = s; *p != '\0'; ++p)
		c = *p;
	if (c < 'Z'||c > 'A')
		return 0;
	int n = 0;
	for (const char *p = s; *p != '\0'; ++p)
		if (*p == c) ++n;
		
	return (n==1);
}
