# coding: UTF-8
import sys

# python filenames
args = sys.argv

"""
ohtoさん提供の棋譜をpos形式に変換する

データの形式：

dealt { d3 c4 s4 h6 h9 ct hj hk d2 h2 jo } { c3 s6 h7 c8 s8 ht cj dq ca ha c2 } { d5 d7 s7 dt st sj sq ck sk da } { d4 h4 c5 h5 s5 c7 c9 d9 dj dk sa } { h3 s3 c6 d6 d8 h8 s9 cq hq s2 } 
changed { h6 ct } { } { sk da } { c7 } { s2 } 
original { d3 c4 s4 h9 hj hk sk da d2 h2 jo } { c3 s6 h7 c8 s8 ht cj dq ca ha c2 } { d5 h6 d7 s7 ct dt st sj sq ck } { d4 h4 c5 h5 s5 c9 d9 dj dk sa s2 } { h3 s3 c6 d6 c7 d8 h8 s9 cq hq } 


originalがゲーム開始時の手札
play cs-4[44650] cd-9[31989] p[33022]...
と言った要領で手が列挙される[]の中の意味は謎

cs-4 ... クラブとスペードの4
d-ka2 ... ダイヤのka2の階段
cds-5(c) ... cdsの5を3枚出し。cはjokerを利用
s-678(8) ... スペードの678。8jokerを利用
"""


# 上記ルールに従って手のテキストを数値に変換する
def text2card(plytext) :
    if plytext[0:2] == "jk":
        return "o", "o", True # jk単体の扱い
    headidx = plytext.find('-')
    bodyidx = plytext.find('[')
    passidx = plytext.find('p[')
    jokeridx = plytext.find('(')
    isjk = False
    if jokeridx != -1:
        isjk = True
        bodyidx = jokeridx
    if passidx != -1:
        return "pass", "pass", isjk
    if headidx == -1:
        #print("error (assumed to be unreachable!!)")
        #print(plytext)
        return "pass","pass", isjk
    
    if bodyidx == -1 :
        bodyidx = len(plytext)
    return plytext[0:headidx], plytext[headidx+1:bodyidx], isjk

def char2rank(char):
    rank = 0
    if char.isdigit():
        rank = int(char) - 2
        if rank == 0:
            rank = 13
    else:
        if char == "t" :
            rank = 8
        if char == "j" :
            rank = 9
        if char == "q" :
            rank = 10
        if char == "k" :
            rank = 11
        if char == "a" :
            rank = 12
        if char == "o" :
            rank = 14
    return rank

def translate(head, body):
    suit = 0
    rank = 0
    handtype = 0 # 0はpass
    if head == "pass" :
        return suit, rank, handtype
    if len(body) == 1 :
        
        # この手はペアである
        rank = char2rank(body)
                    
        for i in range(len(head)):
            if head[i] == 'c' :
                suit += 1
                handtype += 1
            if head[i] == 'd' :
                suit += 2
                handtype += 1
            if head[i] == 'h' :
                suit += 4
                handtype += 1
            if head[i] == 's' :
                suit += 8
                handtype += 1
            if head[i] == 'o' :
                handtype = 1 # joker単体だしでしか出てこないはず
            if head[i] == 'x' :
                handtype += 1 # cdhsxによる5枚だし
    else:
        # この手は階段である
        
        strrank = body[0]
        rank = char2rank(strrank)
        
        handtype = 3 + len(body) 
        for i in range(len(head)):
            if head[i] == 'c' :
                suit += 1
            if head[i] == 'd' :
                suit += 2
            if head[i] == 'h' :
                suit += 4
            if head[i] == 's' :
                suit += 8
    #print([suit,rank,handtype])
    return suit, rank, handtype

def isyagiri(rank,handtype):
    if handtype < 6 :
        if rank == 6:
            return True
        else:
            return False
    else:
        if rank > 6:
            return False
        else :
            if rank + handtype - 4 < 6:
                return False
            else:
                return True

def hand2bit(char):
    bitsuit = 0
    if char == "jo" :
        return 52
    if char[0] == 'd' :
        bitsuit += 1
    if char[0] == 'h' :
        bitsuit += 2
    if char[0] == 's' :
        bitsuit += 3
    rank = char2rank(char[1])
    return (rank - 1) * 4 + bitsuit

def hands2bin(hands):
    # 手札の0/1を2進数に変換
    output = 0
    tmp = 16
    for i in range(52):
        if i in hands :
            output += tmp
        tmp = tmp * 2
    jokerbit = tmp * 16
    if 52 in hands:
        output += jokerbit
    return output


def geterases(erases, rank, suit, handtype, jk):
    if handtype == 0:
        return
    if jk :
        erases.append(52)
            
    if handtype < 6 :
        # pairの場合の削る手札
        if suit & 1 != 0:
            erases.append((rank - 1) * 4)
        if suit & 2 != 0:
            erases.append((rank - 1) * 4 + 1)
        if suit & 4 != 0:
            erases.append((rank - 1) * 4 + 2)
        if suit & 8 != 0:
            erases.append((rank - 1) * 4 + 3)
    else :
        # kaidanの場合の削る手札
        if suit & 1 != 0:
            bsuit = 0
        if suit & 2 != 0:
            bsuit = 1
        if suit & 4 != 0:
            bsuit = 2
        if suit & 8 != 0:
            bsuit = 3

        for i in range(handtype-3):
            erases.append((rank-1+i) * 4 + bsuit)


class board:
    def __init__(self):
        self.hand = [[],[],[],[],[]]
        self.playedhand = [[],[],[],[],[]]
        self.suitlock = 0
        self.kakumei = 0
        self.chair = [0,1,2,3,4]
        self.turn = 0
        self.prevsuit = 0
        self.prevrank = 0
        self.prevhandtype = 0
        self.lastplay = 0
        self.passed = [False,False,False,False,False]
        
    def clearboard(self):
        # 手札と縛りのクリア
        for i in range(5):
            self.hand[i].clear()
            self.playedhand[i].clear()
            self.passed[i] = False
        self.suitlock = 0
        self.prevsuit = 0
        self.prevrank = 0
        self.prevhandtype = 0
        self.lastplay = 0
        self.kakumei = 0
        self.prevjk = False
            
    def setturn(self):
        # d3を持ってる人が初手
        for i in range(5):
            if 1 in self.hand[i]:
                for j in range(5):
                    if self.chair[j] == i:
                        self.turn = j
                return
        print("error no d3 player")
        
    def getNext(self):
        self.turn += 1
        self.turn  = self.turn % 5
        # 次のプレイヤーが上がってる場合
        if len(self.hand[self.chair[self.turn]]) == 0:
            self.getNext()
            return
        # 次のプレイヤーが既にパスをしている場合
        if self.passed[self.turn] == True:
            self.getNext()
            return
        
    def plycard(self,suit, rank, handtype, jk):
        #print("plycard input")
        #print([suit,rank,handtype,jk,self.chair[self.turn],self.prevrank,self.suitlock])
        # 手を指させる。ぶっちゃけ一番面倒な要素だ
        erases = []
        if handtype == 0:
            self.passed[self.turn] = True
            isFlush = True
            # 全員がパスした = 場が流れた
            for i in range(5):
                if self.passed[i] == False and len(self.hand[self.chair[i]]) != 0:
                    isFlush = False
                    break
            if isFlush:
                self.flush()
            return True
        isjks3 = False
        if self.prevrank == 14 and rank == 1 :
            # jk s3のコンボ
            isjks3 = True

        if handtype == 4 or handtype == 5 or handtype > 7 :
            self.kakumei = not self.kakumei
            
        self.lastplay = self.turn
        if suit == self.prevsuit:
            self.suitlock = suit
        else:
            self.suitlock = 0
        self.prevsuit = suit
        self.prevrank = rank
        self.prevhandtype = handtype
        self.prevjk = jk
        geterases(erases, rank, suit, handtype, jk)
        # erasesの手を削る
        selfcheck = 0
        #print("eraselist")
        #print(erases)
        for erase in erases:
            if erase in self.hand[self.chair[self.turn]]:
                self.hand[self.chair[self.turn]].remove(erase)
                self.playedhand[self.chair[self.turn]].append(erase)
            else:
                selfcheck += 1

        # 合法手判定。もっと良い書き方がありそう
        # 注意：jokerがなくても作れる役をjoker込で作るのは非合法扱い
        if (handtype == 5 and selfcheck == 0) or (selfcheck == 0 and (not jk or (jk and handtype == 1))) or (selfcheck == 1 and jk) :
            pass
        else:
            print("something wrong in plycard")           
            print([suit,rank,handtype,jk,self.turn])
            print(self.hand[self.chair[self.turn]])
            return False

        # 上がり判定
        if len(self.hand[self.chair[self.turn]]) == 0:
            #print("agari pl :" + str(self.chair[self.turn]))
            # agariはpassを内包する。flush判定を入れなければならない
            isFlush = True
            # 全員がパスした = 場が流れた
            for i in range(5):
                if self.passed[i] == False and len(self.hand[self.chair[i]]) != 0:
                    isFlush = False
                    break
            if isFlush:
                self.flush()
                return True
            # self.flush()
            
        # 手に8が含まれる場合、即座にflushする
        # jkからのs3の場合、即座にflushする
        if isyagiri(rank, handtype) or isjks3:
            self.flush()
            return True
        
    def printposbin(self,suit, rank, handtype, jk):
        # 各盤面を吐き出す。此方は手札の情報を圧縮したもの
        outstr = ""
        #print(hands2bin(self.hand[self.chair[self.turn]]))
        #print(hands2bin(self.playedhand[self.chair[self.turn]]))
        outstr += "hand " + str(hands2bin(self.hand[self.chair[self.turn]])) + "\n"
        
        outstr += "myply " + str(hands2bin(self.playedhand[self.chair[self.turn]])) + "\n"

        idx = 0
        for i in range(5):
            if i == self.turn :
                continue
            #print(hands2bin(self.playedhand[self.chair[i]]))
            outstr += "eply " + str(idx) + " " + str(hands2bin(self.playedhand[self.chair[i]])) + "\n"
            idx += 1
                                    
        #print([self.kakumei,self.prevsuit, self.prevrank, self.prevhandtype, self.suitlock])
        if self.kakumei :
            outstr += "kakumei 1\n"
        else:
            outstr += "kakumei 0\n"
        cboard = []
        geterases(cboard, self.prevrank, self.prevsuit, self.prevhandtype, self.prevjk)
        outstr += "cboard " + str(hands2bin(cboard)) + "\n"
        outstr += "suitlock " + str(self.suitlock) + "\n"
        outstr += "type " + str(self.prevhandtype) + "\n"
        outstr += "rank " + str(self.prevrank) + "\n"
        cboard = []
        geterases(cboard, rank, suit, handtype, jk)
        outstr += "movtype " + str(handtype) + "\n"
        outstr += "movrank " + str(rank) + "\n"
        if jk :
            outstr += "movjk 1\n"
        else:
            outstr += "movjk 0\n"
        outstr += "movc " + str(hands2bin(cboard)) + "\n"
        outstr += "eop"

        #print([suit, rank, handtype, jk])
        
        return outstr

            
    def printpos(self,suit, rank, handtype, jk):
        # 各盤面を吐き出す。本スクリプトの存在意義とも言える
        # debug
        for i in range(5):
            print(self.hand[i])
        for i in range(5):
            print(self.playedhand[i])
        print(self.kakumei)
        print([self.prevsuit, self.prevrank, self.prevhandtype, self.suitlock])
        print([suit, rank, handtype, jk])
            
    
    def flush(self):
        # 場が流れることに相当。suitlockを解除し、次の手番のプレイヤーを決める
        #print("flushed lastplayed : " + str(self.lastplay))
        self.suitlock = 0
        self.prevsuit = 0
        self.prevrank = 0
        self.prevhandtype = 0
        self.prevjk = False
        self.turn = (self.lastplay + 4) % 5
        for i in range(5):
            self.passed[i] = False

    def original2hand(self,originaltext) :
        # originalで与えられる手札を代入する
        idx = 0
        datas = originaltext.split(" ")
        for data in datas :
            if data == "{" or data == "original" :
                pass
            elif data == "}" :
                idx += 1
                if idx == 5:
                    break
            else:
                self.hand[idx].append(hand2bit(data))
        # debug
        #print(self.hand)
                
    def setseat(self,seattext):
        # seat 1 4 0 2 3 と言った形式で与えられる席順を代入
        datas = seattext.split(" ")
        for i in range(5):
            self.chair[int(datas[i+1])] = i
        # debug
        #print(self.chair)    
    def domove(self,suit, rank, handtype, jk): 
        result = self.plycard(suit,rank,handtype,jk)

        # ゲームが終了していたら即座に打ち切る
        if self.endgamecheck():
            return result
        
        self.getNext()
        return result
            
    def endgamecheck(self):
        endcnt = 0
        for i in range(5):
            if len(self.hand[i]) == 0:
                endcnt += 1
        if endcnt >=4 :
            return True
        return False

class glicine2pos:
    def __init__(self):
        self.bd = board()
        self.gamecnt = 0
        
    def load(self,fname,outfname):
        f = open(fname)
        lines = f.readlines()
        f.close()
        fw = open(outfname , 'w')
        for line in lines:
            if line[0:4] == "game":
                #print("gamestart")
                self.gamecnt += 1
                self.bd.clearboard()
            if line[0:4] == "seat":
                self.bd.setseat(line[4:])
            if line[0:4] == "orig":
                self.bd.original2hand(line)
                # 手札を確定されたら初手のプレイヤーが決まる
                self.bd.setturn()
            if line[0:4] == "play" and not (line[0:6] == "player" ) :
                datas = (line[5:]).split(" ")
                fw.write("gamestr " + line.replace(" ", ",") + "\n")
                for data in datas:
                    head, body, jk = text2card(data)
                    suit, rank, handtype = translate(head,body)
                    outpos = self.bd.printposbin(suit,rank,handtype,jk)
                    if (handtype != 0 and rank == 0) or handtype > 9 :
                        print("illeagal hand in the game")
                        print(line)
                        print(data)
                        break
                    #print(data)
                    result = self.bd.domove(suit,rank,handtype,jk)
                    if result == False:
                        print(line)
                        print(data)
                        break
                    fw.write(outpos + "\n")
                    if self.bd.endgamecheck():
                        break
                if self.gamecnt > 114514:
                    fw.close()
                    return
                
                    
head, body, jk = text2card("d-345(5)[39960]")
suit, rank, handtype = translate(head,body)
#print([suit,rank,handtype])

gl = glicine2pos()
#gl.load("../../record_for_learn/Glicine_50000.dat", "../reclearn/Glicine_50000.txt")
#gl.load("../../record_for_learn/Glicine_50000_1.dat", "../reclearn/Glicine_50000_1.txt")
#gl.load("../../record_for_learn/Glicine_50000_2.dat", "../reclearn/Glicine_50000_2.txt")
gl.load("../../record_for_learn/Glicine_50000_13.dat", "../reclearn/Glicine_50000_13.txt")
#gl.load("../../record_for_learn/Glicine_50000_4.dat", "../reclearn/Glicine_50000_4.txt")
#gl.load("../../record_for_learn/Glicine_50000_7.dat", "../reclearn/Glicine_50000_7.txt")
#gl.load("../../record_for_learn/Glicine_50000_9.dat", "../reclearn/Glicine_50000_9.txt")
#gl.load("../../record_for_learn/Glicine_50000_11.dat", "../reclearn/Glicine_50000_11.txt")


#gl.load("/media/shiku/04E4E0C7E4E0BC54/record_for_learn/Glicine_50000.dat", "../reclearn/Glicine_50000.txt")
# playで与えられる手を代入する
#head, body, jk = text2card(plytext)
#suit, rank, handtype = translate(head,body)
