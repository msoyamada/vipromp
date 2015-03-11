#include <stdio.h>


int main()
{
 	unsigned char *i;
	long long int *g;

	g=0x80000008;
	*g= 100000000000;

	i = 0x80000000;
	i[0] = 'a';
	i[1] = 'b';
	i[2] = 'c';

	printf("\n>>> %lld\n", *g);

	return 0;
}
