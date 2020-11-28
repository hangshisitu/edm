#include <iostream>
#include <unistd.h>
#include <pthread.h>

using namespace std;

void *fun(void *arg)
{
	int i,j=5;
	unsigned char buffer[200];
	while (1)
	{
		for ( i=1;i<100;i++ )
		{
			buffer[i] = j * (j-1) * (j-2) * (j-3) * (j-4);
		}
		sleep(1);
	}
}

int main(void)
{
	pthread_t t[1024];
	int i;
	int ret;


	for ( i=0;i<2048;i++ )
	{
		cout << "Create thread " << i;
		ret = pthread_create(&t[i], NULL, fun, NULL);

		// ÐÞ¸Ä²¿·Ö ******************
		cout << "result: " << ret << endl;
		if ( ret ) break;
		//**************************
	}

	for ( i=0;i<2048;i++ )
	{
		pthread_cancel(t[i]);
	}
	return 0;
}
