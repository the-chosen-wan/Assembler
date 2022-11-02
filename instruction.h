#include <iostream>
#include <bits/stdc++.h>
using namespace std;
#define LEN 67
/*
Ins format
--8 opcode--16 dst--16 src--1 dst indirect--1 src indirect--4 dst reg--4 src reg--1 imm--16 imm val
*/

class Instruction{
    public:
        bitset<LEN> ins;

        Instruction(){

        }

        void setValue(int value,int start,int end){
            int c=start;

            while(value>0){
                if(value&1)
                    ins.set(c);
                c++;
                value>>=1;
            }

            return;
        }

        int getValue(int start,int end){
            int ans=0;
            int cnt=0;

            for(int i=start;i<=end;i++){
                if(ins.test(i))
                    ans+=(1<<cnt);
                cnt++;
            }
            return ans;
        }

        int getOpcode() {return getValue(0,7); }
        void setOpcode(int value) { setValue(value,0,7); }

        int getDst() { return getValue(8,23); }
        void setDst(int value) { setValue(value,8,23); }

        int getSrc() { return getValue(24,39); }
        void setSrc(int value) { setValue(value,24,39); }

        int getDstInd() { return getValue(40,40); }
        void setDstInd(int value) { setValue(value,40,40); }

        int getSrcInd() { return getValue(41,41); }
        void setSrcInd(int value) { return setValue(value,41,41); }

        int getDstReg() { return getValue(42,45); }
        void setDstReg(int value) { setValue(value,42,45); }

        int getSrcReg() { return getValue(46,49); }
        void setSrcReg(int value) { setValue(value,46,49); }

        int getImmFlag() { return getValue(50,50); }
        void setImmFlag(int value) { return setValue(value,50,50); }

        int getImm() { return getValue(51,66); }
        void setImm(int value) { return setValue(value,51,65); }
};