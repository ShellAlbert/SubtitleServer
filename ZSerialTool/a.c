#include <stdio.h>
int main(void)
{
	char buffer[5];
	char bufferTemp[1024];
	int ret;
	ret=getchar();
	printf("ret=%d\n",ret);
	return 0;
	while(1)
	{
		printf("Serial>");
		scanf("%5s %x",buffer,&ret);
		fgets(bufferTemp,1024,stdin);
		printf("ret=%x,x=%s\n",ret,buffer);
	}
	return 0;
}
