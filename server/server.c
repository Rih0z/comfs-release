
/*****************************************************************
  ファイル名	: server_net.c
  機能		: サーバーのネットワーク処理
 *****************************************************************/

#include "server.h"
#include <stdbool.h>
#define EASY true

int main(int argc,char *argv[])
{

  int	num;
  pthread_t thread1;
  int ret ; 
  /* 引き数チェック */
  if(argc != 2){
    num = 1 ;
    fprintf(stdout,"client num is 1 \n");
  }else{
    if((num = atoi(argv[1])) < 0 ||  num > MAX_CLIENTS){
      fprintf(stderr,"clients limit = %d \n",MAX_CLIENTS);
      exit(-1);
    }
  }

  /* クライアントとの接続 */
  if(SetUpServer(num) == -1){
    fprintf(stderr,"Cannot setup server\n");
    exit(-1);
  }else{
    printf("conecct\n");
  }
  printf("set up comleate\n");

  ret =   pthread_create(&thread1,NULL,(void *)datacom,NULL);
  ret = pthread_join(thread1,NULL);
  if (ret != 0) {
    fprintf(stderr,"thread error\n");
  }

}
void datacom(void){
  int	endFlag = 1;
  while(endFlag){
    endFlag = SendRecvManager();

     //   rsize = recv( client_sockfd, buf, sizeof( buf ), 0 );
  };
}
/*****************************************************************
  関数名	: SendRecvMessageManager
  機能	: サーバーから送られてきたデータを処理する
  引数	: なし
  出力	: プログラム終了コマンドが送られてきた時0を返す．
  それ以外は1を返す
 *****************************************************************/
int SendRecvMessageManager(int pos )
{
  fd_set	readOK;
  int		endFlag = 1;
  char strdata[256] = "NULL"; 

//  readOK = gMask;
	memcpy(&readOK, &gMask, sizeof(fd_set));
  /* クライアントからデータが届いているか調べる */
  if(select(gWidth,&readOK,NULL,NULL,NULL) < 0){
    /* エラーが起こった */
    return endFlag;
  }

//  for(i=0;i<gClientNum;i++){
    if(FD_ISSET(gClients[pos].fd,&readOK)){
      RecvData(pos,strdata,sizeof(strdata));
      printf("got string %s\n",strdata);
      printf("please input the reply\n");
      char message[256];
      sprintf(message,"%s:reply",strdata);
      comfs_sendstr_command(pos,message);
      printf("message : %s  size %lu sended to client \n",message , sizeof(message));
      endFlag = 0 ; 
    }
 // }
  return endFlag;
}

/*****************************************************************
  関数名	: ExecuteCommand
  機能	: クライアントから送られてきたコマンドを元に，
  引き数を受信し，実行する
  引数	: char	command		: コマンド
  int	pos			: コマンドを送ったクライアント番号
  出力	: プログラム終了コマンドが送られてきた時には0を返す．
  それ以外は1を返す
 *****************************************************************/
int ExecuteCommand(char* command,int pos)
{
  unsigned char	data[MAX_DATA];
  int			dataSize;
  int			endFlag = 1;
  int numdata = 0  ;

  /* 引き数チェック */
  assert(0<=pos && pos<MAX_CLIENTS);

#ifndef NDEBUG
  printf("#####\n");
  printf("ExecuteCommand()\n");

  if(strlen(command) == 1){
  printf("Get command %c\n",command[0]);
  }else{
  printf("Get message %s\n",command);
  }
#endif
  if(strlen(command) == 1){
  switch(command[0]){
    case END_COMMAND:
      dataSize = 0;
      /* コマンドのセット */
      SetCharData2DataBlock(data,command[0],&dataSize);
      /* 全ユーザーに送る */
      SendData(ALL_CLIENTS,data,dataSize);
      endFlag = 0;
      break;
    case TOSRVINT_COMMAND:
      RecvIntData(pos,&numdata);
      printf("data is %d\n",numdata);
      break;
    case TOCLTINT_COMMAND:
      break;
    case TOSRVSTR_COMMAND:
      while(endFlag){
        endFlag = SendRecvMessageManager(pos);
        if(endFlag == 0)break;
      };
      break;
    default:
      /* 未知のコマンドが送られてきた */
      fprintf(stderr,"%s is not command!\n",command);
  }
  }else{
    printf("please input the reply\n");
    char message[256];
    sprintf(message,"%s:reply",command);
    comfs_sendstr_command(pos,message);
    printf("message : %s  size %lu sended to client \n",message , sizeof(message));
  }
  if(numdata  == 2){
    return numdata ;
  }
  return endFlag;
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
  char buf[1024];

  // 受信バッファ初期化
  memset( buf, 0, sizeof( buf ) );
  int rsize;
  int i ;  
  if(EASY){

      for(i=0;i<gClientNum;i++){
          rsize = recv(  gClients[i].fd, buf, sizeof( buf ), 0 );
          if ( rsize == 0 ) {
              break;
          } else if ( rsize == -1 ) {
              perror( "recv" );
          } else {
              if(strlen(buf) == 1){
                  if(buf[0] == END_COMMAND){
                      char * E = "E" ; 
                      ExecuteCommand(E,i);
                      return 0 ; 
                  }
              }
              printf( "got message :%s\n", buf );

              // 応答
              //
              sprintf(buf,"%s-reply",buf);
  //            comfs_sendstr_command(i,buf);
sleep(5);
              printf( "send : %s\n", buf );
        write(gClients[i].fd,buf,sizeof(buf));
          }
      }
      return endFlag ; 
  }
  printf("SendRecvManager\n");
  char	command[256];
  fd_set	readOK;

  readOK = gMask;
  /* クライアントからデータが届いているか調べる */
  if(select(gWidth,&readOK,NULL,NULL,NULL) < 0){
      /* エラーが起こった */
      return endFlag;
  }

  for(i=0;i<gClientNum;i++){
      if(FD_ISSET(gClients[i].fd,&readOK)){
          /* クライアントからデータが届いていた */
          /* コマンドを読み込む */

          printf("SendRecvManager got Data\n");
          RecvData(i,command,sizeof(command));
          /* コマンドに対する処理を行う */
          endFlag = ExecuteCommand(command,i);

          readOK = gMask;
          if(endFlag == 0)break;
      }
  }
  return endFlag;
}
/*****************************************************************
  関数名	: RecvIntData
  機能	: クライアントからint型のデータを受け取る
  引数	: int		pos	        : クライアント番号
  int		*intData	: 受信したデータ
  出力	: 受け取ったバイト数
 *****************************************************************/
int RecvIntData(int pos,int *intData)
{
    int n,tmp;

    /* 引き数チェック */
    assert(0<=pos && pos<gClientNum);
    assert(intData!=NULL);

    n = RecvData(pos,&tmp,sizeof(int));
    (*intData) = ntohl(tmp);

    return n;
}

/*****************************************************************
  関数名	: SendData
  機能	: クライアントにデータを送る
  引数	: int	   pos		: クライアント番号
  ALL_CLIENTSが指定された時には全員に送る
  void	   *data	: 送るデータ
  int	   dataSize	: 送るデータのサイズ
  出力	: なし
 *****************************************************************/
void SendData(int pos,void *data,int dataSize)
{
    int	i;

    /* 引き数チェック */
    assert(0<=pos && pos<gClientNum || pos==ALL_CLIENTS);
    assert(data!=NULL);
    assert(0<dataSize);

    if(pos == ALL_CLIENTS){
        /* 全クライアントにデータを送る */
        for(i=0;i<gClientNum;i++){
            write(gClients[i].fd,data,dataSize);
        }
    }
    else{
        write(gClients[pos].fd,data,dataSize);
    }
}


/*****************************************************************
  関数名	: SetUpServer
  機能	: クライアントとのコネクションを設立し，
  ユーザーの名前の送受信を行う
  引数	: int		num		  : クライアント数
  出力	: コネクションに失敗した時-1,成功した時0
 *****************************************************************/
int SetUpServer(int num)
{
    struct sockaddr_in	server;
    int			request_soc;
    int                 maxfd;
    int			val = 1;

    /* 引き数チェック */
    assert(0<num && num<=MAX_CLIENTS);

    gClientNum = num;
    bzero((char*)&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    /* ソケットを作成する */
    if((request_soc = socket(AF_INET,SOCK_STREAM,0)) < 0){
        fprintf(stdout,"Socket allocation failed\n");
        return -1;
    }
    setsockopt(request_soc,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));

    /* ソケットに名前をつける */
    if(bind(request_soc,(struct sockaddr*)&server,sizeof(server))==-1){
        fprintf(stderr,"Cannot bind\n");
        close(request_soc);
        return -1;
    }
    fprintf(stderr,"Successfully bind!\n");

    /* クライアントからの接続要求を待つ */
    if(listen(request_soc, gClientNum) == -1){
        fprintf(stderr,"Cannot listen\n");
        close(request_soc);
        return -1;
    }
    fprintf(stderr,"Listen OK\n");

    /* クライアントと接続する */
    maxfd = MultiAccept(request_soc, gClientNum);
    close(request_soc);
    if(maxfd == -1)return -1;

    /* 全クライアントの全ユーザー名を送る */
    //  SendAllName();

    /* select()のためのマスク値を設定する */
    SetMask(maxfd);

    return 0;
}



/*****************************************************************
  関数名	: MultiAccept
  機能	: 接続要求のあったクライアントとのコネクションを設立する
  引数	: int		request_soc	: ソケット
  int		num     	: クライアント数
  出力	: ソケットディスクリプタ
 *****************************************************************/
static int MultiAccept(int request_soc,int num)
{
    int	i,j;
    int	fd;

    for(i=0;i<num;i++){
        if((fd = accept(request_soc,NULL,NULL)) == -1){
            fprintf(stderr,"Accept error\n");
            for(j=i-1;j>=0;j--)close(gClients[j].fd);
            return -1;
        }
        Enter(i,fd);
    }
    return fd;
}

/*****************************************************************
  関数名	: Enter
  機能	: クライアントのユーザー名を受信する
  引数	: int		pos		: クライアント番号
  int		fd		: ソケットディスクリプタ
  出力	: なし
 *****************************************************************/
static void Enter(int pos, int fd)
{
    /* クライアントのユーザー名を受信する */
    //  read(fd,gClients[pos].name,MAX_NAME_SIZE);
#ifndef NDEBUG
    printf("%s is accepted\n",gClients[pos].name);
#endif
    gClients[pos].fd = fd;
}


/*****************************************************************
  関数名	: SetMask
  機能	: int		maxfd	: ソケットディスクリプタの最大値
  引数	: なし
  出力	: なし
 *****************************************************************/
static void SetMask(int maxfd)
{
    int	i;

    gWidth = maxfd+1;

    FD_ZERO(&gMask);    
    for(i=0;i<gClientNum;i++)FD_SET(gClients[i].fd,&gMask);
}
/*****************************************************************
  関数名	: RecvData
  機能	: クライアントからデータを受け取る
  引数	: int		pos	        : クライアント番号
  void		*data		: 受信したデータ
  int		dataSize	: 受信するデータのサイズ
  出力	: 受け取ったバイト数
 *****************************************************************/
static int RecvData(int pos,void *data,int dataSize)
{
    int n;

    /* 引き数チェック */
    assert(0<=pos && pos<gClientNum);
    assert(data!=NULL);
    assert(0<dataSize);

    n = read(gClients[pos].fd,data,dataSize);

    return n;
}

/*****************************************************************
  関数名	: SetStrData2DataBlock
  機能	: int 型のデータを送信用データの最後にセットする
  引数	: void		*data		: 送信用データ
  int		intData		: セットするデータ
  int		*dataSize	: 送信用データの現在のサイズ
  出力	: なし
 *****************************************************************/
static void SetStrData2DataBlock(void *data,const char *strData,int *dataSize)
{
    int i ; 

    /* 引き数チェック */
    assert(data!=NULL);
    assert(0<=(*dataSize));
    for(i = 0 ; i < strlen(strData); i++){
        SetCharData2DataBlock(data,strData[i],dataSize);
    }


}

//SENDSTR
static int comfs_sendstr_command(int pos ,char *wbuf){
    int ret = 0 ;


    unsigned char	data[MAX_DATA];
    int			dataSize;

#ifdef SHOWINFO
    writeLog("comfs_sendint_command","","#####");
    writeLog("comfs_sendint_command","","senddata");
#endif
    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data,TOCLTSTR_COMMAND,&dataSize);
    //文字列を送信
    SetStrData2DataBlock(data, wbuf, &dataSize);

    /* データの送信 */
    SendData(pos ,data,dataSize);

    return ret ;
}
