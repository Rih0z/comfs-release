
#include "net_common.h"

static int	gSocket;	/* ソケット */
static fd_set	gMask;	/* select()用のマスク */
static int	gWidth;		/* gMask中ののチェックすべきビット数 */
int RecvData(void *data,int dataSize);
int RecvIntData(int *intData);

int ExecuteCommand(char command);

void SendEndCommand(void);
int SendRecvManager(void);
int SetUpClient(char *hostName,int *clientID,int *num,char clientNames[][MAX_NAME_SIZE]);
void SendData(void *data,int dataSize);

void datacom(void);
static void SetCharData2DataBlock(void *data,char charData,int *dataSize);
static void SetMask(void);
