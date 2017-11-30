#include "testagent.h"

void Testagent::ss2table(int table[8][15], stringstream &ss){
  for(int i=0;i<8;++i){
    for(int j=0;j<15;++j){
      ss >> table[i][j];
    }
  }
}

void Testagent::testencode(DagentUEC *agent, stringstream &ss){
    string token;
    ss >> token;
    int table[8][15];
    int tableout[8][15];
    if(token=="receive_board"){
      ss2table(table, ss);
      agent->receive_board(table);
    }else if(token=="send_change"){
      ss2table(table, ss);
      agent->send_change(table,tableout);
    }else if(token=="option"){
      agent->option(ss);
    }else if(token=="load"){
      ss>>token;
      ifstream ifs(token.c_str());
      string temp;
      while(getline(ifs,temp)){
	stringstream ss2(temp);
	testencode(agent, ss2);
      }
      ifs.close();
    }
}

#define AGENT_KOWLOON
// agentのデバッグ用コード。正しいioができてるかを確かめる
void Testagent::testagent(){
  
  string cmd;

  while(getline(cin,cmd)){
    if(cmd=="quit"){break;}
    stringstream ss(cmd);
#ifdef AGENT_KOWLOON
    testencode(&kowloon,ss);
#endif
  }
}
