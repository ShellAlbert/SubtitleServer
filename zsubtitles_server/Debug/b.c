#include <stdio.h>
int main(void)
{
char *color="0xFF123456";
int temp;
unsigned char buffer[32];
sscanf(color,"%x",&temp);
printf("temp=%x\n",temp);

printf("%d\n",sizeof(float));
printf("%d\n",sizeof(double));
printf("bufsize:%d\n",sizeof(buffer));
return 0;
}
