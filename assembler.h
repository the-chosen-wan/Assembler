#include "memory.h"
#include "instruction.h"
#define pb push_back
#define mp make_pair
#define FRWD 12345

int stringToHex(string s){
    int x;
    std::stringstream ss;
    ss << std::hex << s;
    ss >> x;
    return x;
}

void erase_apos(string &s)
{
    int l = s.size();
    for (int i = 0; i < l; i++)
        if (s[i] == ',')
            s[i] = ' ';
    return;
}

bool is_number(string& s)
{
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

class Assembler{
    public:

        MemoryManager manager;
        map<string,int> symtab;
        map<string,int> optab;
        int pc;
        short recentVal;
        vector<Instruction> instructions;
        bool forwardFlag;
        stack<int> sp;

        Assembler(){
            pc=0;
            recentVal=0;
            forwardFlag = false;
        }

        Assembler(MemoryManager manager){
            this->manager = manager;
            pc=0;
            recentVal=0;
            forwardFlag = false;

            symtab["al"] = 0;symtab["ah"] = 1; symtab["bl"] = 2; symtab["bh"] = 3;symtab["cl"] = 4;symtab["ch"] = 5;
            symtab["dl"] = 6;symtab["dh"] = 7;symtab["si"] = 8;symtab["ax"] = 9;symtab["bx"] = 10;symtab["cx"] = 11;symtab["dx"]=12;

            optab["add"] = 0;optab["sub"] = 1;optab["inc"]=2;optab["dec"]=3;optab["print"]=4;optab["jnz"]=5;optab["jz"]=6;
            optab["end"]=7;optab["jmp"]=8;optab["mov"]=9;optab["cmp"] = 10;optab["jnc"]=11;optab["jc"]=12;optab["input"]=13;
            optab["and"]=14;optab["or"]=15;optab["nop"]=16;optab["push"] = 24;optab["pop"]=17;optab["call"]=18;optab["ret"]=19;
            optab["lea"]=20;optab["int"]=21;optab["ral"]=22;optab["rar"]=23;
        }

        Instruction getIns(string s){
            Instruction ins;
            erase_apos(s);

            stringstream ss(s);
            string word;
            vector<string> words;

            while (ss >> word)
                words.pb(word);

            if(words.size()>1){
                if(words[1]=="proc"){
                    symtab[words[0]]=pc+1;
                    ins.setOpcode(optab["nop"]);
                    return ins;
                }

                if(words[1]=="endp"){
                    ins.setOpcode(optab["end"]);
                    return ins;
                }
            }

            if(optab.find(words[0])==optab.end()){
                if(symtab.find(words[0])==symtab.end())
                    symtab[words[0]]=pc;
                words.erase(words.begin());
            }

            if(words.empty()){
                ins.setOpcode(optab["nop"]);
                return ins;
            }

            int opcode = optab[words[0]];
            ins.setOpcode(opcode);

            if(words.size()==1)
                return ins;

            if (words[1].find('[') == string::npos){
                if(symtab.find(words[1])==symtab.end())
                    ins.setDst(FRWD);
                else
                    ins.setDst(symtab[words[1]]);
            }
            else
            {
                string base = "";string reg="";
                for (int i = 0; i < words[1].size(); i++)
                {
                    if (words[1][i] != '[')
                        base += words[1][i];
                    else
                    {
                        reg += words[1][i + 1];
                        if (words[1][i + 2] != ']')
                            reg += words[1][i + 2];
                        break;
                    }
                }
                ins.setDst(symtab[base]);
                ins.setDstInd(1);
                ins.setDstReg(symtab[reg]);
            }

            if(words.size()==2)
                return ins;

            if (symtab.find(words[2]) == symtab.end() && words[2].find('[') == string::npos)
            {
                ins.setImmFlag(1);
                ins.setImm(stringToHex(words[2]));
            }
            else
            {
                if (words[2].find('[') == string::npos)
                    ins.setSrc(symtab[words[2]]);
                else
                {
                    string base="";string reg="";
                    for (int i = 0; i < words[2].size(); i++)
                    {
                        if (words[2][i] != '[')
                            base += words[2][i];
                        else
                        {
                            reg += words[2][i + 1];
                            if (words[2][i + 2] != ']')
                                reg += words[2][i + 2];
                            break;
                        }
                    }
                    ins.setSrc(symtab[base]);
                    ins.setSrcInd(1);
                    ins.setSrcReg(symtab[reg]);
                }
            }
            return ins;
        }

        bool execute(Instruction ins){
            int opcode = ins.getOpcode();
            int src = ins.getSrc();
            int dst = ins.getDst();

            int dstInd = ins.getDstInd();
            int dstReg = ins.getDstReg();

            int srcInd = ins.getSrcInd();
            int srcReg = ins.getSrcReg();

            int dstAddr = dst+(dstInd*manager.getValue(dstReg));
            int srcAddr = src+(srcInd*manager.getValue(srcReg));

            int immFlag = ins.getImmFlag();
            int immval = ins.getImm();

            int srcVal,dstVal;

            if(immFlag)
                srcVal = immval;
            else
                srcVal = manager.getValue(srcAddr);
            
            dstVal = manager.getValue(dstAddr);

            if (opcode == optab["end"])
            {
                return false;
            }

            else if(opcode==optab["nop"]){
                pc++;
            }

            else if(opcode==optab["mov"]){
                manager.setValue(dstAddr,srcVal);
                pc++;
            }

            else if(opcode==optab["add"]){
                recentVal =  srcVal+dstVal;
                manager.setValue(dstAddr,recentVal);
                pc++;
            }

            else if(opcode==optab["sub"]){
                recentVal = dstVal-srcVal;
                manager.setValue(dstAddr,(recentVal+(1<<16))%(1<<16));
                pc++;
            }

            else if(opcode==optab["cmp"]){
                recentVal = dstVal - srcVal;
                pc++;
            }

            else if(opcode==optab["input"]){
                string temp;cin>>temp;
                manager.setValue(dstAddr,stringToHex(temp));
                pc++;
            }

            else if(opcode==optab["print"]){
                int temp = dstVal;
                cout<<std::hex<<temp<<endl;
                pc++;
            }

            else if(opcode==optab["jmp"]){
                pc = dst;
            }

            else if(opcode==optab["jnz"]){
                if(recentVal!=0)
                    pc=dst;
                else
                    pc++;
            }

            else if(opcode==optab["jz"]){
                if(recentVal==0)
                    pc=dst;
                else
                    pc++;
            }

            else if(opcode==optab["jnc"]){
                if(recentVal>=(1<<16)||recentVal<0)
                    pc++;
                else
                    pc=dst;
            }

            else if(opcode==optab["jc"]){
                if(recentVal>=(1<<16)||recentVal<0)
                    pc=dst;
                else
                    pc++;
            }

            else if(opcode==optab["push"]){
                sp.push(dstVal);
                pc++;
            }

            else if(opcode==optab["pop"]){
                int temp = sp.top();
                sp.pop();
                manager.setValue(dstAddr,temp);
                pc++;
            }

            else if(opcode==optab["lea"]){
                manager.setValue(dstAddr,srcAddr);
                pc++;
            }
            else if(opcode==optab["dec"]){
                recentVal = dstVal-1;
                manager.setValue(dstAddr, (recentVal + (1 << 16)) % (1 << 16));
                pc++;
            }

            else if(opcode==optab["inc"]){
                recentVal = dstVal+1;
                manager.setValue(dstAddr,recentVal);
                pc++;
            }

            else if(opcode==optab["ral"]){
                dstVal<<=srcVal;
                manager.setValue(dstAddr,dstVal);
                pc++;
            }

            else if(opcode==optab["rar"]){
                dstVal>>=srcVal;
                manager.setValue(dstAddr,dstVal);
                pc++;
            }

            else if(opcode==optab["call"]){
                sp.push(pc+1);
                pc = dst;
            }

            else if(opcode==optab["ret"]){
                int temp = sp.top();
                pc = temp;
                sp.pop();
            }

            return true;
        }

        void processData(string s){
            erase_apos(s);
            Instruction ret;
            stringstream ss(s);
            string word;
            vector<string> words;

            while (ss >> word)
                words.pb(word);

            if(words.size()==3){
                if(is_number(words[2])){
                    symtab[words[0]]=manager.Allocate(1);
                    manager.setValue(symtab[words[0]],stringToHex(words[2]));
                }
                else{
                    symtab[words[0]]=manager.Allocate(words[2].size());
                    int base = symtab[words[0]];
                    for(int i=base;i<base+words[2].size();i++)
                        manager.setValue(i,(unsigned short)(words[2][i-base]));
                }
            }

            else{
                symtab[words[0]] = manager.Allocate(stringToHex(words[3]));
            }
            return;
        }

        void run(string filename){
            ifstream file(filename);

            vector<string> lines;
            string line;

            while (true)
            {
                getline(file, line);

                if (line != "")
                    lines.pb(line);
                if (line == "end")
                    break;
            }

            int i = 1;
            if (lines[0] == ".data")
            { // initialize data variables
                string temp;
                for (i = 1; i < lines.size(); i++)
                {
                    if (lines[i] != ".code")
                        processData(lines[i]);
                    else
                        break;
                }
            }

            pc = 0;
            i++;
            vector<pair<int, int>> frwrd;

            for (; i < lines.size(); i++)
            {
                instructions.pb(getIns(lines[i])); // convert each line into Instruction object
                if (instructions[pc].getDst() == FRWD)
                    frwrd.pb(mp(i, pc)); // dst==-2 indicates forward reference, in this case push into array
                pc++;
            }

            for (pair<int, int> p : frwrd)
                instructions[p.second] = getIns(lines[p.first]); // Resolve forward references

            int cont = true;
            pc = 0;
            while (cont)
            {
                cont = execute(instructions[pc]); // Run until end
            }
            file.close();
            return;
        }
};