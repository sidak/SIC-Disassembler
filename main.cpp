#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctype.h>
#include <vector>
using namespace std;
#define maxMem 66000

char mem[maxMem][2];
enum typ{ udef, code, resb, resw, byte, word};
typ memType[maxMem];
bool memRefer[maxMem];
class stmnt{
	public:
		int loc;
		string label, opcode, opnd ;
	stmnt(int l , string lbl, string opcd, string opd){
		loc=l;
		label=lbl;
		opcode=opcd;
		opnd=opd;
	}
	
};
vector < stmnt > stmnts;
int hex2dec(string str){
    int l=str.length();
    int n=0,d;
    for(int i=0;i<l;i++){
        if(isalpha(str[i]))d=str[i]-'A'+10;
        else d=str[i]-'0';
        n=n*16+d;
    }
    return n;
}

int hex2dec(char *str,int l){
    int n=0,d;
    for(int i=0;i<l;i++){
        if(isalpha(str[i]))d=str[i]-'A'+10;
        else d=str[i]-'0';
        n=n*16+d;
    }
    return n;
}
void traverse(string addr){ 
	// the addr is assumed to be in hexadecimal
	int idx = hex2dec(addr);
	while(mem[idx][0]!='G' && mem[idx][1]!='G'){
		if(memType[idx]==code){
			// exit if it is already seen code
			break;
		}
		else if (memType[idx]!=udef && memType[idx]!=code){
			// belongs to data section
			// loc -> idx
			// opname -> based on memType
			// value is calculate on the basis of  
			// if word then directly use the value of the 3 bytes - hex to binary to decimal
			
			// make  a ctr of number of bytes in dec
			// increment until we in memory get a sentinel value 
			// basically until the other label is not referred 
			// upper bound this loop with maxAddressPossible
			// if resw then ctr/24 is value
			// if resb then ctr is value
			// if byte then ctr, then think if x based or c based
		}
		else{
			
			string opcd= "";
			opcd+= mem[idx][0];
			opcd+=mem[idx][1];
			string opName = Optab.getInst();
			string shrtAddr= addr.substr(2,4);
			// add the address bits from the next instruction;
			string opnd= "L";
			opnd+=mem[idx+1][0];
			opnd+=mem[idx+1][1];
			opnd+=mem[idx+2][0];
			opnd+=mem[idx+2][1];
			
			string opndAddr= opnd.substr(1,4);
			string label = "L";
			label+=shrtAddr;
			// check if index based addressing 
			//
			// opnd+=",X";
			// if rsub inst , clear opnd to a tab;
			if(opName =="RSUB")opnd="\t";
			
			struct stmnt st(idx, label, opName, opnd);
			stmnts.push_back(st);
			
			int opndInt=hex2dec(opndAddr);
			memType[idx]=code;
			memRefer[opndInt]=true;
			// depending on the type of current instruction 
			// set the mem type of opnd 
			//memType[opndInt]=
			
			// if instruction is either a jump inst or jsub inst
			// traverse(opnd.substr(1,4));
			
			traverse(dec2hex(idx+3));
			
		}
		
	}
	
}
void writeProgram(){
	// make a comparator based on locn 
	// sort the vector
	// output to file line by line 
}
int main(){
	char * filename=NULL;
	string progName, progLen, startAdd, firstExecAdd;
	cout<<"Enter the name of the object File "<<endl;
	scanf("%s",filename);
	// use argv for it 
	// initialise mem 
	for(int i=0; i<maxMem; i++){
		mem[i][0]='G';
		mem[i][1]='G';
		memType[i]= udef;
		memRefer[i]=false;
	}
	ifstream infile(filename);
	string hr, tr;
	getline(infile, hr);
	progName = hr.substr(1, 6);
	startAdd = hr.substr(7,6);
	progLen = hr.substr(13,6);
	
	getline(infile, tr);
	string recStartAdd, recLen; 
	int memStIdx, loopLen;
	while(tr[0]!='E'){
		recStartAdd=tr.substr(1,6);
		recLen = tr.substr(7,2);
		// initialise the mem array with the object code
		memStIdx = hex2dec(recStartAdd);
		loopLen=(2*hex2dec(recLen));
		int j= memStIdx;
		int i=0;
		while(i<loopLen){
			mem[j][0]=tr[9+i];
			i++;
			mem[j][1]=tr[9+i];
			i++;
			j++;
		}
		getline(infile, tr); 
	}
	firstExecAdd = tr.substr(1,6);
	traverse(firstExecAdd);
	writeProgram();
	
}
