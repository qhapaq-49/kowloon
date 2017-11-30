#include "common/common.h"
#include "config.h"
#include "connection_uec.h"
#include "position.h"
#include "tournament_uec.h"
#include "agent_kowloon_uec.h"

using namespace std;

DagentUEC_kowloon agent[5];

void worker(int tid){
  ConnectionUEC con; // 鯖との接続

  // 初期化コマンド
  const string leval = (tid == 0 ? "init" : "eval evaltest");
  stringstream ss(leval);
  agent[tid].option(ss);
  
  con.setAgent(&agent[tid]);
  con.execute(DEFAULT_SERVER, DEFAULT_PORT, "kowloon");
}

int Tournament::main_tournament(int argc,char* argv[]){
  
  ConnectionUEC con; // 鯖との接続
  const string leval = "init";
  stringstream ss(leval);
  agent[0].option(ss);
  
  con.setAgent(&agent[0]);
  con.execute(DEFAULT_SERVER, DEFAULT_PORT, "kowloon");

  /*
  // 自分を5つ起動して鯖につなぐ。対戦テスト用だな。
  vector<thread> ths(5);

  for(int i=0;i<5;++i){
    ths[i] = thread(worker,i);
  }

  for (auto& th : ths) {
    th.join();
  }
  //引数のチェック 引数に従ってサーバアドレス、接続ポート、クライアント名を変更
  //con.checkArg(argc,argv);
  */
  exit(0);
  
}
