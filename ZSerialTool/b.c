#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>

int getch( );

int main( )
{
    int ch;
    while(1){
        ch = getch( );
        printf("You Pressed %x,%c\n", ch,ch);
        if(ch == 'E')
             break;
    }
}

int getch()
{
    char ch;
    struct termios save, ne;
    ioctl(0, TCGETS, &save);
    ioctl(0, TCGETS, &ne);
    ne.c_lflag &= ~(ECHO | ICANON);
    ioctl(0, TCSETS, &ne);
    read(0, &ch, 1);
    ioctl(0, TCSETS, &save);
    return ch;
}
