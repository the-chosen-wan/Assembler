#include "assembler.h"

int main(int argc,char** argv){
    Registers r;
    string filename = argv[1];
    // r.setRegValue(0,1);
    // r.setRegValue(1,1);
    // cout<<r.getRegValue(1)<<endl;
    MemoryManager m(13);
    Assembler a(m);
    a.run(filename);
}