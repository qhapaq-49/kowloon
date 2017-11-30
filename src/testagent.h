#ifndef _INC_TESTAGENT_H_
#define _INC_TESTAGENT_H_

#include "common/common.h"
#include "config.h"
#include "connection_uec.h"
#include "position.h"
#include "tournament_uec.h"
#include "agent_kowloon_uec.h"

using namespace std;

namespace Testagent{
  void ss2table(int table[8][15], stringstream &ss);
  void testencode(DagentUEC *agent, stringstream &ss);
  void testagent();
}

#endif
