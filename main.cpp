#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <ctype.h>
#include <vector>
#include <map>
#include <algorithm>
#include <stdlib.h>
using namespace std;
#define maxMem 66000

char mem[maxMem][2];
enum stype{ udef,code, byte, resb, word, resw};
enum optype{ujump=0,cjump=1,ldch=2,stch=3,lw=4,sw=5};

stype memType[maxMem];
bool memRefer[maxMem];
int maxProgAddrInt;
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
struct opn{
	string name;
	string type;
};
map <string, struct opn > optable;
vector < stmnt > stmnts;
map<string ,struct opn>::iterator it;
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
string dec2hex1(int x){
	int n,r[10],i=0,number,j=0;
	number=x;
	

	while(number>0)
	{
		r[i]=number%16;
		number=number/16;
		i++;
		j++;
	}
	string ans ="";
	for(i=j-1;i>=0;i--)
	{
		if(r[i]==10)
		ans+='A';
		else if(r[i]==11)
		ans+='B';
		else if(r[i]==12)
		ans+='C';
		else if(r[i]==13)
		ans+='D';
		else if(r[i]==14)
		ans+='E';
		else if(r[i]==15)
		ans+='F';
		else{
		
		ans+=(r[i]+'0');
		}
	}
	cout<<"ans "<<ans<<endl;
	return ans;
	
}
string dec2hex(int j){
	int bin[25];
	int i;
	for(i=0;j>0;i++)
    {
        bin[i]=j%16;
        j=j/16;
    }

    i--;
    string ans ="";
    for(;i>=0;i--)
    {
        if(bin[i]==10)
                ans+="A";
        else if(bin[i]==11)
                ans+="B";
        else if(bin[i]==12)
                ans+="C";
        else if(bin[i]==13)
                ans+="D";
        else if(bin[i]==14)
                ans+="E";
        else if(bin[i]==15)
                ans+="F";
        else
            ans+=bin[i];
    }
    return ans;
}
void initOptable(){
	//ofstream ofile("opcodesPrint.txt");
	ifstream infile("opcodes.txt");
	string s1 ,s2,s3;
	while(!infile.eof()){
		infile>>s1>>s2>>s3;
		struct opn op ;
		op.name=s1;
		op.type=s3;
		optable.insert(pair<string, struct opn > (s2,op));
		//ofile<<s1<<" "<<s2<<" "<<s3<<"\n";
	}
}
void traverse(string addr){ 
	// the addr is assumed to be in hexadecimal
	int idx = hex2dec(addr);
	cout<<"idx "<<idx<<endl;
	string shrtAddr= addr.substr(2,4);
	cout<<"shrtAdr "<<shrtAddr<<endl;
	if(mem[idx][0]!='G' && mem[idx][1]!='G'){
		
		if(memType[idx]==code){
			// exit if it is already seen code
			cout<<"already code"<<endl;
			return;
		}
		else if (memType[idx]!=udef && memType[idx]!=code){
			// ldch -> byte
			// stch -> resb
			// lda, -> word
			// sta, -> resw
			cout<<"in data"<<endl;
			string data="";
			string dataStr;
			string label = "L";
			label += shrtAddr;
			string opnm;
			// belongs to data section
			// loc -> idx
			// opname -> based on memType
			// value is calculate on the basis of  
			// if word then directly use the value of the 3 bytes - hex to binary to decimal
			if(memType[idx]==word){
				data+=mem[idx][0];
				data+=mem[idx][1];
				data+=mem[idx+1][0];
				data+=mem[idx+1][1];
				data+=mem[idx+2][2];
				data+=mem[idx+2][2];
				int dataInt = hex2dec(data);
				dataStr =""+dataInt;
				opnm="WORD";
			
				struct stmnt st( idx, label, opnm, dataStr);
				stmnts.push_back(st);
				traverse("00"+dec2hex(idx+3));
				return;
			}
			else if(memType[idx]==byte){
				int ct=1;
				int j=idx+1;
				while(j<maxProgAddrInt){
					if(memRefer[j]==false){
						ct++;
						j++;
					}
					else break;
				}
				
				for(int i=idx; i<(idx+ct); i++){
					data+=mem[i][0];
					data+=mem[i][1];
				}
				dataStr="X'";
				dataStr+=data;
				dataStr+="'";
				opnm="BYTE";
				
				struct stmnt st( idx, label, opnm, dataStr);
				stmnts.push_back(st);
				traverse("00"+dec2hex(idx+ct));
				return;
				
			}
			else if(memType[idx]==resw){
				int ct=1;
				int j=idx+1;
				while(j<maxProgAddrInt){
					if(memRefer[j]==false){
						ct++;
						j++;
					}
					else break;
				}
				if(ct%3!=0)cout<<"wrong number of res words"<<endl;
				ct/=3;
				
				dataStr=""+ct;
				opnm="RESW";
				
				struct stmnt st( idx, label, opnm, dataStr);
				stmnts.push_back(st);
				traverse("00"+dec2hex(idx+(3*ct)));
				return;
				
			}
			else if(memType[idx]==resb){
				int ct=1;
				int j=idx+1;
				while(j<maxProgAddrInt){
					if(memRefer[j]==false){
						ct++;
						j++;
					}
					else break;
				}
								
				dataStr=""+ct;
				opnm="RESB";
				
				struct stmnt st( idx, label, opnm, dataStr);
				stmnts.push_back(st);
				traverse("00"+dec2hex(idx+ct));
				return;
			}
			// make  a ctr of number of bytes in dec
			// increment until we in memory get a sentinel value 
			// basically until the other label is not referred 
			// upper bound this loop with maxAddressPossible
			// if resw then ctr/24 is value
			// if resb then ctr is value
			// if byte then ctr,- use c based only
			// make a stmnt object 
			// add it to vector 
			// increment by length of data
		}
		else{
			cout<<"in code"<<endl;
			string opcd= "";
			string otyp;
			opcd+= mem[idx][0];
			opcd+=mem[idx][1];	
			
			cout<<idx<<endl;
			cout<< mem[idx][0]<<" "<<mem[idx][1]<<endl;;
			cout<<opcd<<endl;
			it = optable.find(opcd);
			string opName;
			if(it!=optable.end()){
				opName = (it->second).name;
				otyp=(it->second).type;
				cout<<opName<<" "<<otyp<<endl;
			}
			else{
				
				cout<<"no such opcode "<<opcd<< "with idx "<<idx<<endl;
			}
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
			int diff = opndAddr[0]-'8';
			if(diff >=0){
				opnd+=",X";
			}
			
			// if rsub inst , clear opnd to a tab;
			if(opName =="RSUB")opnd="\t";
			
			struct stmnt st(idx, label, opName, opnd);
			stmnts.push_back(st);
			cout<<idx<<" "<<label<<" "<<opName<<" "<<opnd<<endl;
			int opndInt=hex2dec(opndAddr);
			cout<<opndInt<<endl;
			memType[idx]=code;
			memRefer[opndInt]=true;
			cout<<otyp<<endl;
			if(otyp=="2" ){
				memType[opndInt]=byte;
			}
			else if (otyp=="3"){
				memType[opndInt]=resb;
			}
			if(otyp=="4" ){
				memType[opndInt]=word;
			}
			else if (otyp=="5"){
				memType[opndInt]=resw;
				cout<<"resw"<<endl;
			}
			else {
				cout<<"hello jump"<<endl;
				traverse(opndAddr);
			}
			// depending on the type of current instruction 
			
			
			// if instruction is either a jump inst or jsub inst
			// traverse(opnd.substr(1,4));
			cout<<idx<<endl;
			int ab =idx+3;
			//string b = dec2hex1(ab);
			//cout<<b<<endl;
			string a = "00"+dec2hex1(ab);
			cout<<a<<endl;
			traverse(a);
			return;
		}
		
	}
	
	
}
struct less_than_key
{
    inline bool operator() (const stmnt& struct1, const stmnt& struct2)
    {
        return (struct1.loc < struct2.loc);
    }
};
void writeProgram(){
	// make a comparator based on locn 
	// sort the vector
	// output to file line by line
	ofstream myfile;
    myfile.open ("output.txt");
    sort(stmnts.begin(), stmnts.end(), less_than_key()); 
	int sz = stmnts.size();
	for(int i=0; i<sz; i++){
		struct stmnt st = stmnts[i];
		if(memRefer[st.loc]){
			myfile<<st.label<<"\t";
		}
		myfile<<st.opcode<<"\t";
		myfile<<st.opnd<<"\n";
	}
	
	myfile.close();
	
}
int main(){
	char * filename=NULL;
	string progName, progLen, startAdd, firstExecAdd;
	
	//cout<<"Enter the name of the object File "<<endl;
	//scanf("%s",filename);
	// use argv for it 
	// initialise mem 
	for(int i=0; i<maxMem; i++){
		mem[i][0]='G';
		mem[i][1]='G';
		memType[i]= udef;
		memRefer[i]=false;
	}
	initOptable();
	//ifstream infile(filename);
	ifstream infile("input.txt");
	string hr, tr;
	getline(infile, hr);
	progName = hr.substr(1, 6);
	cout<<progName<<endl;
	
	startAdd = hr.substr(7,6);
	cout<<startAdd<<endl;
	progLen = hr.substr(13,6);
	cout<<progLen<<endl;
	maxProgAddrInt= hex2dec(startAdd) + hex2dec(progLen);
	cout<<maxProgAddrInt<<endl;
	
	getline(infile, tr);
	
	string recStartAdd, recLen; 
	int memStIdx, loopLen;
	while(tr[0]!='E'){
		recStartAdd=tr.substr(1,6);
		cout<<recStartAdd<<endl;
		recLen = tr.substr(7,2);
		cout<<recLen <<endl;
		// initialise the mem array with the object code
		memStIdx = hex2dec(recStartAdd);
		loopLen=(2*hex2dec(recLen));
		cout<<memStIdx<<" "<<loopLen<<endl;
		int j= memStIdx;
		int i=0;
		while(i<loopLen){
			cout<<"i "<<i<<" j "<<j<<endl;
			mem[j][0]=tr[9+i];
			i++;
			cout<<"mem 0 "<<mem[j][0]<<endl;
			cout<<"i "<<i<<" j "<<j<<endl;
			
			mem[j][1]=tr[9+i];
			i++;
			cout<<"i "<<i<<" j "<<j<<endl;
			cout<<"mem 1 "<<mem[j][1]<<endl;
			j++;
			cout<<"i "<<i<<" j "<<j<<endl;
		}
		getline(infile, tr); 
	}
	firstExecAdd = tr.substr(1,6);
	cout<<firstExecAdd<<endl;
	traverse(firstExecAdd);
	writeProgram();
	
}
