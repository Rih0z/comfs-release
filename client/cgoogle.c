#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAX_NAME 256

int main(){
    char ctr[MAX_NAME];
    int connect;
    connect = open("m/TCP4/www.google.com/80/opt",O_RDWR);
    if(connect == -1){
        printf("connect faild\n");
        printf("Plese Setup Server \n");
        perror("fopen");
        return 0 ;
    }else{
        printf("connect SUCCESS!\n");
    }
    //存在しないファイルにはアクセスできない
    //NULLになるのは仕方ない？
    int endflag = 1;
    int count = 0;
    while(endflag){
        char senddata[256] ;
        printf("please input message \n");
        if(count == 0){
            snprintf(senddata,sizeof(senddata),"GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n");
            count = 1;}
        else{

            scanf("%s",senddata);

        }
        if(strlen(senddata) == 1){
            if(senddata[0] == 'E')
                endflag = 0 ;
        }
        //int ret = fwrite(senddata,sizeof(senddata),1,connect);
        int ret = write(connect,senddata,sizeof(senddata));
        if(ret == 0 ){
            printf("No write \n");
        }if(ret < 0 ){
            printf("error");
        }else{
            //厳密にいうとしっかり書き込まれたかこれではみれない
            //ブロックサイズまでは書けることが多いが普通のOSの場合512?当たるなでが「
            //書き込みのサイズを見る
            printf("sizeof %lu data is %s sended\n",sizeof(senddata) ,senddata);
        }

        printf("written\n");
        printf("if you input any words , reciev a reply\n");
        printf("read start\n");
        char message[256];
        //  ret = fread(message,256,1,connect);
        ret = read(connect ,message,sizeof(message));
        if(ret < 0 ){
            printf("read faild\n");
        }else{
            printf("read : %s \n",message);
        }
    }

    if(close(connect) == 0){
        printf("connect close success\n");
    }else{
        printf("connect close faild\n");
    }

    return 0 ;

}
