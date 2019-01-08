
/*****************************************************************
ファイル名	: client_main.c
機能		: クライアントのメインルーチン
*****************************************************************/

#include "client.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
char buf[256] ;
bool gotmes ;
bool end ;
clock_t start_e,end_e;
double diff ;
int main(int argc,char *argv[])
{

    start_e = clock();
    int		num;
    char	name[MAX_CLIENTS][MAX_NAME_SIZE];
    char	localHostName[]="localhost";
    char	*serverName;
    int		clientID;
    int ret ;
    pthread_t thread1 ;

    /* 引き数チェック */
    if(argc == 1){
        serverName = localHostName;
    }
    else if(argc == 2){
        serverName = argv[1];
    }
    else{
        fprintf(stderr, "Usage: %s, Cannot find a Server Name.\n", argv[0]);
        return -1;
    }

    /* サーバーとの接続 */
    if(SetUpClient(serverName,&clientID,&num,name)==-1){
        fprintf(stderr,"setup failed : SetUpClient\n");
        return -1;
    }else{
        end = true ;
        printf("client set up successed\n");
    }
    end_e = clock();
    printf("setup time %f\n",(double)(end_e - start_e));
        ret = pthread_create(&thread1,NULL,(void *)datacom,NULL);
    char senddata[256] ;

    while(end){
        printf("please input wrhite message\n");
        scanf("%s",senddata) ;
        write(gSocket,senddata,(int)strlen(senddata));
    }

    printf("end\n");

    return ret ;
}
void datacom(void){
    pthread_detach( pthread_self( ));
    int endFlag = 1 ;
    while(endFlag){
        //    SendEndCommand();
        endFlag = SendRecvManager();
    };
}
/*****************************************************************
  関数名	: SendRecvManager
  機能	: サーバーから送られてきたデータを処理する
  引数	: なし
  出力	: プログラム終了コマンドが送られてきた時0を返す．
  それ以外は1を返す
 *****************************************************************/
int SendRecvManager(void)
{

    int		endFlag = 1;

    int rsize;
    char buf2[1024];
    // 受信バッファ初期化
    memset( buf2, 0, sizeof( buf2 ) );

    rsize = recv(  gSocket , buf2, sizeof( buf2 ), 0 );
    if ( rsize == 0 ) {
        return endFlag ;
    } else if ( rsize == -1 ) {
        perror( "recv" );
    } else {
        if(strlen(buf2) == 1){
            if(buf2[0] == END_COMMAND){
                endFlag = ExecuteCommand(END_COMMAND);
                return 0 ; 
            }
        }
    }
    printf("recv : %s\n",buf2);
    memset( buf, 0, sizeof( buf ) );
    strcpy(buf,buf2);
    gotmes = true ;
    return endFlag ; 
}
/*****************************************************************
  関数名	: ExecuteCommand
  機能	: サーバーから送られてきたコマンドを元に，
  引き数を受信し，実行する
  引数	: char	command		: コマンド
  出力	: プログラム終了コマンドがおくられてきた時には0を返す．
  それ以外は1を返す
 *****************************************************************/
int ExecuteCommand(char command)
{
    int	endFlag = 1;
#ifndef NDEBUG
    printf("#####\n");
    printf("ExecuteCommand()\n");
    printf("command = %c\n",command);
#endif
    switch(command){
        case END_COMMAND:
            end = false ;
            endFlag = 0;
            exit(1);
            break;
        case TOSRVINT_COMMAND:
            break;
        case TOCLTINT_COMMAND:
            break;
    }
    return endFlag;
}

/*****************************************************************
  関数名	: SetUpClient
  機能	: サーバーとのコネクションを設立し，
  ユーザーの名前の送受信を行う
  引数	: char	*hostName		: ホスト
  int	*num			: 全クライアント数
  char	clientNames[][]		: 全クライアントのユーザー名
  出力	: コネクションに失敗した時-1,成功した時0
 *****************************************************************/
int SetUpClient(char *hostName,int *clientID,int *num,char clientNames[][MAX_NAME_SIZE])
{
    struct hostent	*servHost;
    struct sockaddr_in	server;

    /* ホスト名からホスト情報を得る */
    if((servHost = gethostbyname(hostName))==NULL){
        fprintf(stderr,"Unknown host\n");
        return -1;
    }

    bzero((char*)&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    bcopy(servHost->h_addr,(char*)&server.sin_addr,servHost->h_length);

    /* ソケットを作成する */
    if((gSocket = socket(AF_INET,SOCK_STREAM,0)) < 0){
        fprintf(stderr,"socket allocation failed\n");
        return -1;
    }

    /* サーバーと接続する */
    if(connect(gSocket,(struct sockaddr*)&server,sizeof(server)) == -1){
        fprintf(stderr,"cannot connect\n");
        close(gSocket);
        return -1;
    }
    fprintf(stderr,"connected\n");

    /* 名前を読み込みサーバーに送る */
    /*
       do{
       printf("Enter Your Name\n");
       fgets(str,BUF_SIZE,stdin);
       len = strlen(str)-1;
       str[len]='\0';
       }while(len>MAX_NAME_SIZE-1 || len==0);
       SendData(str,MAX_NAME_SIZE);

       printf("Please Wait\n");
       */
    /* 全クライアントのユーザー名を得る */
    //  GetAllName(clientID,num,clientNames);

    /* select()のためのマスク値を設定する */
    SetMask();

    return 0;
}

/*****************************************************************
  関数名	: SendData
  機能	: サーバーにデータを送る
  引数	: void		*data		: 送るデータ
  int		dataSize	: 送るデータのサイズ
  出力	: なし
 *****************************************************************/
void SendData(void *data,int dataSize)
{
    /* 引き数チェック */
    assert(data != NULL);
    assert(0 < dataSize);

    write(gSocket,data,dataSize);
}

/*****************************************************************
  関数名	: RecvIntData
  機能	: サーバーからint型のデータを受け取る
  引数	: int		*intData	: 受信したデータ
  出力	: 受け取ったバイト数
 *****************************************************************/
int RecvIntData(int *intData)
{
    int n,tmp;

    /* 引き数チェック */
    assert(intData!=NULL);

    n = RecvData(&tmp,sizeof(int));
    (*intData) = ntohl(tmp);

    return n;
}

/*****************************************************************
  関数名	: SetMask
  機能	: select()のためのマスク値を設定する
  引数	: なし
  出力	: なし
 *****************************************************************/
static void SetMask(void)
{

    FD_ZERO(&gMask);
    FD_SET(gSocket,&gMask);

    gWidth = gSocket+1;
}

/*****************************************************************
  関数名	: RecvData
  機能	: サーバーからデータを受け取る
  引数	: void		*data		: 受信したデータ
  int		dataSize	: 受信するデータのサイズ
  出力	: 受け取ったバイト数
 *****************************************************************/
int RecvData(void *data,int dataSize)
{
    /* 引き数チェック */
    assert(data != NULL);
    assert(0 < dataSize);

    return read(gSocket,data,dataSize);
}
/*****************************************************************
  関数名	: SendEndCommand
  機能	: プログラムの終了を知らせるために，
  サーバーにデータを送る
  引数	: なし
  出力	: なし
 *****************************************************************/
void SendEndCommand(void)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

#ifndef NDEBUG
    printf("#####\n");
    printf("SendEndCommand()\n");
#endif
    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data,END_COMMAND,&dataSize);

    /* データの送信 */
    SendData(data,dataSize);
}
/*****************************************************************
  関数名	: SetCharData2DataBlock
  機能	: char 型のデータを送信用データの最後にセットする
  引数	: void		*data		: 送信用データ
  int		intData		: セットするデータ
  int		*dataSize	: 送信用データの現在のサイズ
  出力	: なし
 *****************************************************************/
static void SetCharData2DataBlock(void *data,char charData,int *dataSize)
{
    /* 引き数チェック */
    assert(data!=NULL);
    assert(0<=(*dataSize));

    /* int 型のデータを送信用データの最後にコピーする */
    *(char *)(data + (*dataSize)) = charData;
    /* データサイズを増やす */
    (*dataSize) += sizeof(char);
}
