#include    <stdio.h>
#include    <termios.h>
#include    <unistd.h>
#include    <errno.h>
#include    <string.h>


#define     MAX_INPUT_LEN           40


int getattr(struct termios *oldt)
{
    if ( -1 == tcgetattr(STDIN_FILENO, oldt) )
    {
        perror("Cannot get standard input description");
        return 1;
    }
    return 0;
}

int setattr(struct termios newt)
{
    newt.c_lflag &= ~(ICANON | ECHO | ISIG);
    newt.c_cc[VTIME] = 0;
    newt.c_cc[VMIN] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    return 0;
}

int resetattr(struct termios *oldt)
{
    tcsetattr(STDIN_FILENO, TCSANOW, oldt);
    return 0;
}

int getkey(void)
{
    char buf[3];
    ssize_t bytes_read;
    switch ( bytes_read = read(STDIN_FILENO, &buf, 3) )
    {
        case -1:
            perror("Error");
            return -1;
        case 1:
            //printf("Entered character: %d, %d\n", buf[0], buf[0]);
            return buf[0];
        case 3:
            switch(buf[2])
            {
                case 'A':
                    //printf("Press up\n");
                    return 0xe048;
                case 'B':
                    //printf("Press down\n");
                    return 0xe050;
                case 'C':
                    //printf("Press right\n");
                    return 0xe04d;
                case 'D':
                    //printf("Press left\n");
                    return 0xe04b;
            }
    }
    return  -1;
}





static   int  DelCharWithPos(char  *pbuf, int  len, int  pos)
{
    char  *ps = NULL;
    char  *pn = NULL;
    int   j = 0;

    if(NULL==pbuf || pos<1)
        return -1;
    if(len<0)
        return -1;

    for(j=pos-1;j<len;j++){
        pbuf[j] = pbuf[j+1];
    }
    return 0;
}


static  int  InsertCharWithPos(char  *pbuf, int cnt, int pos, char key)
{
    int  j;

    if(NULL==pbuf || pos>cnt)
        return -1;
    if(cnt<0 || pos<0)
        return -1;
    for(j=cnt+1;j>pos-1;j--){
        pbuf[j] = pbuf[j-1];
    }
    pbuf[pos] = key;
    pbuf[cnt+2] = 0;
    return 0;
}


int main(int  argc, const  char  *argv[])
{
    int  key;
    int  pos, cnt;
    struct termios oldt;
    char  buf[MAX_INPUT_LEN+1]="";



    getattr(&oldt);
    setattr(oldt);
    pos = 0;
    cnt = 0;
    printf("\033[1;31m Welcome to this test! \033[0m \n");


    while (1)
    {
        key = getkey();

        if(cnt>=MAX_INPUT_LEN)
            break;

        if (key == '\n') // input is end?
            break;
        else if(127==key){  // is backspace?
            if(pos>0 && pos<=cnt){
                DelCharWithPos(buf, cnt, pos);
                if(cnt>0){
                    buf[cnt] = 0;
                    cnt--;
                }
            }
            printf("\r\033[K\033[1;31m%s\033[0m", buf);
            pos = cnt;
            fflush(stdout);
        }else if(0xe04b==key){  //is left?
            if(pos>0){
                pos--;
                printf("\033[1D");
                fflush(stdout);
            }
        }else if(0xe04d==key){  //is right?
            if(pos<=cnt){
                printf("\033[1C");
                fflush(stdout);
                pos++;
            }
        }else{
            if(pos==cnt){
                buf[cnt] = key;
                printf("\033[1;31m%c\033[0m", key);
                pos++;
                cnt++;
            }else{
                InsertCharWithPos(buf, cnt, pos, key);
                printf("\r\033[K\033[1;31m%s\033[0m", buf);
                cnt++;
                pos = cnt;
                fflush(stdout);
            }
            fflush(stdout);
        }
    }

    printf("\n%s\n", buf);
    printf("cnt=%d  pos=%d\n", cnt, pos);
    resetattr(&oldt);
    return 0;
}