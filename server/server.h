#include "net_common.h"

static void Enter(int pos, int fd);
static int MultiAccept(int request_soc,int num);

static void SetMask(int maxfd);


int SetUpServer(int num);
int SendRecvManager(void);
/* クライアントを表す構造体 */
int RecvIntData(int pos,int *intData);

static void SetCharData2DataBlock(void *data,char charData,int *dataSize);
void SendData(int pos,void *data,int dataSize);
static int RecvData(int pos,void *data,int dataSize);

static void SetStrData2DataBlock(void *data,const char *strData,int *dataSize);

static int comfs_sendstr_command(int pos ,char *wbuf);
void datacom(void);

int ExecuteCommand(char* command,int pos);
typedef struct{
  int		fd;
  char	name[MAX_NAME_SIZE];
}CLIENT;

static CLIENT	gClients[MAX_CLIENTS];	/* クライアント */
static int	gClientNum;					/* クライアント数 */

static fd_set	gMask;					/* select()用のマスク */
static int	gWidth;						/* gMask中のチェックすべきビット数 */

