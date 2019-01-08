#include <stdio.h>
#include <string.h>
#define MAX_NAME 256
int main(){
    char ctr[MAX_NAME];
    FILE *connect;
    printf("please set up server\n");
    scanf("%s",ctr);
    connect = fopen("TCP4/localhost/8888/opt=REUSEADDR","w+");

    char senddata[256] ;
    while(1){
        printf("please input message \n");
        scanf("%s",senddata);
        int ret = fwrite(senddata,sizeof(senddata),1,connect);
        if(ret == 0 ){
            printf("No write \n");
        }if(ret < 0 ){
            printf("error");
        }else{
            printf("sizeof %lu data is %s sended\n",sizeof(senddata) ,senddata);
        }
        if( fflush(connect) == 0){
            printf("flush success");
        }

        printf("written\n");
        printf("read start\n");
        char message[1100];

        ret = fread(message,256,1,connect);

        if(ret < 0 ){
            printf("read faild\n");
        }else{
            printf("read : %s \n",message);
        }
        if(strlen(message) == 1){
            if(message[0] == 'E')
                break;
        } 


    }


    if(fclose(connect) == 0){
        printf("connect close success\n");
    }else{
        printf("connect close faild\n");
    }

    return 0 ;

}
