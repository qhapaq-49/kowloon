/*connection*/ 
#ifndef _INC_CONNECTION_UEC_H_
#define _INC_CONNECTION_UEC_H_
#include "common/common.h"
#include "agent_uec.h"
#include "position.h"

#define PROTOCOL_VERSION	20070		//プロトコルヴァージョンを表す整数
#define DEFAULT_SERVER		"127.0.0.1"	//デフォルトのサーバのアドレス 文字列で指定
#define DEFAULT_PORT		42485		//デフォルトのポート番号 整数で指定
#define DEFAULT_NAME		"kowloon"	//デフォルトのクライアント名 文字列で指定

extern const int g_logging;
// UECサーバに接続する用のクラス
class ConnectionUEC{
  
  /*connection内大域変数*/
  //ソケット関連の変数を静的グローバル変数として宣言
  int g_sockfd;
  int g_buf_len;
  sockaddr_in g_client_addr;
  
  //接続するサーバ、ポートを格納する変数
  char     server_name[256];
  uint16_t port;
  //サーバに通知するクライアント名
  char     user_name[15];

  // 通信にまつわる変数
  int recv_table[8][15];
  int send_table[8][15];

  int one_gameend_flag;
  int accept_flag;

  int myid;
  int game_count;   //ゲームの回数を記憶する
  DagentUEC *agent; // 思考する部分
  
 public:
  //接続から試合まで勝手にやる関数
  void execute(char server_name_in[256], uint16_t port_in, char username_in[15]);
  void setAgent(DagentUEC *inagent);
  void checkArg(int argc,char* argv[]);
  int openSocket(const char addr[], uint16_t port_num);
  int sendProfile(const char user[15]);
  int sendTable(int table_val[8][15]);
  int receiveTable();
  int  entryToGame(void);
  void sendChangingCards(int cards[8][15]);
  int  receiveCards(int cards[8][15]);
  int  receiveInt();
  int  sendCards(int cards[8][15]);
  void lookField(int cards[8][15]);
  int  closeSocket();
};
#endif
