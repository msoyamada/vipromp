#include <stdio.h>

void __it_handle()
{
	printf("to dentro do __it_handle\n");
}


int main()
{
	unsigned char *m;
	int i;

	printf("comecei a executar\n");
	fflush(stdout);	
	

	m = 0x80000000;
	for (i = 0; i<0x10; i++)
	{
		m[i] = 'a';
	}

	return 0;
}


