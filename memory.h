#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <fstream>
#include <typeinfo>
#define MAXN 100000
using namespace std;
/*
al - 0, ah - 1
bl - 2, bh - 3 
cl - 4, ch - 5
dl - 6, dh - 7
si - 8
ax - 9
bx - 10
cx - 11
dx - 12
*/

class Registers{
    public:
        vector<unsigned short> reg;

        Registers(){
            reg.resize(5,0);
        }

        int getRegValue(int addr){
            if(addr>=9)
                return reg[addr-9];

            int num = addr/2;

            if(addr==8)
                return reg[4];
            
            int val = reg[num];

            if(addr&1){
                val=val&0xff00;
                return val>>8;
            }

            else{
                val=val&0xff;
                return val;
            }
        }

        void setRegValue(int addr,int val){
            if(addr>=9){
                reg[addr-9] = val;
                return;
            }

            int num = addr/2;

            if(num>=8){
                reg[4]=val;
                return;
            }

            if(addr&1){
                reg[num] = reg[num]&0x00ff;
                reg[num]+=(val<<8);
            }

            else{
                reg[num] = reg[num]&0xff00;
                reg[num]+=val;
            }

            return;
        }
};

class Memory{
    public:
        int offset;
        vector<unsigned short> mem;

    Memory(){

    }
    
    Memory(int offset){
        this->offset = offset;
        mem.resize(MAXN,0);
    }

    int getMemValue(int addr){
        addr-=offset;
        return mem[addr];
    }

    void setMemValue(int addr,int val){
        addr-=offset;
        mem[addr] = val;
        return;
    }
};

class MemoryManager{
    Registers* reg;
    Memory* mem;
    int memcnt;

    public:
        MemoryManager(){

        }

        MemoryManager(int offset){
            memcnt = offset;
            mem = new Memory(offset);
            reg = new Registers();
        }

        int getValue(int addr){
            if(addr>=mem->offset)
                return mem->getMemValue(addr);
            else
                return reg->getRegValue(addr);
        }

        void setValue(int addr,int val){
            if(addr>=mem->offset)
                mem->setMemValue(addr,val);
            else
                reg->setRegValue(addr,val);
        }

        int Allocate(int size){
            int temp = memcnt;
            memcnt+=size;
            return temp;
        }

        int deAllocate(int size){
            if(memcnt-size<mem->offset)
                return -1;
            memcnt-=size;
            return memcnt;
        }
};
