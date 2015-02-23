#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <ctype.h>
#include <vector>
#include <map>
#include <algorithm>
#include <stdlib.h>
#include <string>

using namespace std;

#define maxMem 66000 	// since max memory size of sic machine is 2^15
#define outFile "out.txt"
#define opFile "opcodes.txt"
// ------------------ CLASSES & STRUCTS --------------- //

class stmnt{
	/*
	 * This class corresponds to a statement or 
	 * line of code in the sic program
	 *
	 */   
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
	/*
	 * This struct corresponds to an operation
	 * supported by SIC Machine
	 * 
	 */
	string name;
	char type;
};

struct less_than_key
{
	/*
	 * This struct is used as a comparator 
	 * the struct stmmt
	 * 
	 */
    inline bool operator() (const stmnt& struct1, const stmnt& struct2)
    {
        return (struct1.loc < struct2.loc);
    }
};

// ------------------ GLOBAL VARIABLES ---------------- //

char mem[maxMem][2]; // the memory where the code and data will be loaded in

enum stype{ udef,code, byte, resb, word, resw}; // the type of statement

stype memType[maxMem]; // an array for storing memType

bool memRefer[maxMem]; // an array to check if code or data has been referred

int maxProgAddrInt; 

string progName, progLen, startAdd, firstExecAdd;

map <string, struct opn > optable; // the optab with key as opcode and value as corresponding opn data

vector < stmnt > stmnts; // the vector of statements

map<string ,struct opn>::iterator it; // iterator to the optab

// ------------------- HELPER METHODS ---------------//

int hex2dec(string str){
    /*
     * Converts a hex string to corresponding int
     */
    int l=str.length();
    int n=0,d;
    for(int i=0;i<l;i++){
        if(isalpha(str[i]))d=str[i]-'A'+10;
        else d=str[i]-'0';
        n=n*16+d;
    }
    return n;
}

string NumberToString ( int Number )
{
	/*
	 * Converts an int to a string object
	 */
	stringstream ss;
	ss << Number;
	return ss.str();
}
string dec2hex1(int x)
{
	/*
	 * Converts an int to a hex string
	 */
	
	int r[10],i=0,number,j=0;
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
	return ans;
	
}

void initOptable(){
	/*
	 * This function initialise the optab
	 * from the opn data given in file - opFile
	 */
	ifstream infile(opFile);
	string s1 ,s2;
	char s3;
	while(!infile.eof()){
		infile>>s1>>s2>>s3;
		struct opn op ;
		op.name=s1;
		op.type=s3;
		optable.insert(pair<string, struct opn > (s2,op));
	}
}
void traverseData(string addr){ // the addr is assumed to be in hexadecimal
	
	/*
	 * This function is used to iterate over
	 * the possible data statements
	 */
	
	int idx = hex2dec(addr);
	string shrtAddr= addr.substr(2,4);
	if(idx<maxProgAddrInt){
		
		
		if (memType[idx]!=udef && memType[idx]!=code){
			
			string data="";
			string dataStr;
			string label = "L";
			label += shrtAddr;
			string opnm;
			
			
			if(memType[idx]==word){
				// if the data type is considered to be word
				data+=mem[idx][0];
				data+=mem[idx][1];
				data+=mem[idx+1][0];
				data+=mem[idx+1][1];
				data+=mem[idx+2][0];
				data+=mem[idx+2][1];
				// convert the data into decimal form
				int dataInt = hex2dec(data);
				dataStr =NumberToString(dataInt);
				opnm="WORD";
			
				struct stmnt st( idx, label, opnm, dataStr);
				stmnts.push_back(st);
				traverseData("00"+dec2hex1(idx+3));
				return;
			}
			else if(memType[idx]==byte){
				// if the data type is considered to be byte
				int ct=1;
				int j=idx+1;
				// find the next unreferenced data memory
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
				traverseData("00"+dec2hex1(idx+ct));
				return;
				
			}
			else if(memType[idx]==resw){
				// if the data type is considered to be resw
				int ct=1;
				int j=idx+1;
				// find the next unreferenced data memory
				while(j<maxProgAddrInt){
					if(memRefer[j]==false){
						ct++;
						j++;
					}
					else break;
				}
				// 1 word consists of 3 bytes
				if(ct%3!=0)cout<<"wrong number of res words"<<endl;
				int temo =ct;
				ct/=3;
				
				dataStr=NumberToString(ct);
				opnm="RESW";
				
				struct stmnt st( idx, label, opnm, dataStr);
				stmnts.push_back(st);
				traverseData("00"+dec2hex1(idx+(temo)));
				return;
				
			}
			else if(memType[idx]==resb){
				// if the data type is considered to be resb
				int ct=1;
				int j=idx+1;
				// find the next unreferenced data memory
				while(j<maxProgAddrInt){
					if(memRefer[j]==false){
						ct++;
						j++;
					}
					else break;
				}
								
				dataStr=NumberToString(ct);
				opnm="RESB";
				
				struct stmnt st( idx, label, opnm, dataStr);
				stmnts.push_back(st);
				traverseData("00"+dec2hex1(idx+ct));
				return;
			}
			
		}
		else{
			
			int ab =idx+3;
			string a = "00"+dec2hex1(ab);
			traverseData(a);
		}
	}
	
}
void traverse(string addr){ // the addr is assumed to be in hexadecimal
	/*
	 * This function is used to iterate over
	 * the possible data statements
	 */
	
	int idx = hex2dec(addr);
	string shrtAddr= addr.substr(2,4);
	if(mem[idx][0]!='G' && mem[idx][1]!='G'){
		
		if(memType[idx]==code){
			// exit if it is already seen code
			return;
		}
		
		else if (memType[idx]==udef){
			// encountering the instruction for the first time;
			string opcd= "";
			char otyp;
			opcd+= mem[idx][0];
			opcd+=mem[idx][1];	
			
			it = optable.find(opcd);
			string opName;
			if(it!=optable.end()){
				opName = (it->second).name;
				otyp=(it->second).type;
			}
			else{
				cout<<"no such opcode "<<opcd<< "with idx "<<idx<<endl;
			}
			string shrtAddr= addr.substr(2,4);
			string opnd= "L";
			opnd+=mem[idx+1][0];
			opnd+=mem[idx+1][1];
			opnd+=mem[idx+2][0];
			opnd+=mem[idx+2][1];
			
			string opndAddr= opnd.substr(1,4);
			string label = "L";
			label+=shrtAddr;
			
			// check if index based addressing 
			int opndInt=hex2dec(opndAddr);
			if(opndInt>=(1<<15)){
				opndInt-=(1<<15);
				if(opnd[1]=='8')opnd[1]='0';
				else if(opnd[1]=='9')opnd[1]='1';
				else if(opnd[1]=='A')opnd[1]='2';
				else if(opnd[1]=='B')opnd[1]='3';
				else if(opnd[1]=='C')opnd[1]='4';
				else if(opnd[1]=='D')opnd[1]='5';
				else if(opnd[1]=='E')opnd[1]='6';
				else if(opnd[1]=='F')opnd[1]='7';
				opnd+=",X";
			}
			
			// if rsub inst , clear the operand;
			if(opName =="RSUB")opnd="\t";
			
			struct stmnt st(idx, label, opName, opnd);
			stmnts.push_back(st);
			
			memType[idx]=code;
			
			// exit the traversal if rsub is encountered
			if(opName=="RSUB")return;
			
			// mark the referred operand 
			memRefer[opndInt]=true;
			
			// mark the memType of the operand as per 
			// the given instruction
			if(otyp=='2' ){
				if(memType[opndInt]==udef)
				memType[opndInt]=byte;
			}
			else if (otyp=='3'){
				if(memType[opndInt]==udef){
					memType[opndInt]=resb;
				}
			}
			if(otyp=='4' ){
				if(memType[opndInt]==udef)
				memType[opndInt]=word;
			}
			else if (otyp=='5'){
				if(memType[opndInt]==udef){
					memType[opndInt]=resw;
				}
			}
			else if(otyp=='1' || otyp=='0') {
				// if jump type of instruction
				traverse("00"+opndAddr);
			}
			
			int ab =idx+3;
			
			string a = "00"+dec2hex1(ab);
			traverse(a);
			return;
		}
		
	}
	
	
}

void writeProgram(){
	/*
	 * This function is used to write the
	 * disassembled statements into the output file
	 */
	 
	ofstream myfile;
	myfile.open(outFile);
    sort(stmnts.begin(), stmnts.end(), less_than_key());
    
    string firstLabel="L";
    firstLabel+=firstExecAdd.substr(2,4);
    // print start directive 
    myfile<<progName<<"\t"<<"START"<<"\t"<<firstExecAdd.substr(2,4)<<"\n";
    
    if(!firstExecAdd.empty())stmnts[0].label=firstLabel;
    memRefer[stmnts[0].loc]=true; 
	int sz = stmnts.size();
	
	// print the remaining statements
	for(int i=0; i<sz; i++){
		struct stmnt st = stmnts[i];
		if(memRefer[st.loc]){
			myfile<<st.label<<"\t";
		}
		else{
			myfile<<"\t";
		}
		myfile<<st.opcode<<"\t";
		myfile<<st.opnd<<"\n";
	}
	
	// print the end directive
	myfile<<"\tEND\t"<<firstLabel<<"\n";
	myfile.close();
	
}

// ---------------------- MAIN METHOD ----------------//
int main(){
	
	cout<<"SIC Disassembler\n"<<endl;
	cout<<"\t Made by : "<<endl;
	cout<<"\t Sidak Pal Singh "<<endl;
	cout<<"\t 13114064 \n"<<endl;
	
	// initialise the memory for code and data
	for(int i=0; i<maxMem; i++){
		mem[i][0]='G';
		mem[i][1]='G';
		memType[i]= udef;
		memRefer[i]=false;
	}
	// intialise the optab
	initOptable();
	
	// get file for input
	string fname;
	cout<<"Enter the name of the input file "<<endl;
	cin>>fname;
	const char *cstr = fname.c_str();

	ifstream infile(cstr);
	string hr, tr;
	
	// read header record
	getline(infile, hr);
	progName = hr.substr(1, 6);
	startAdd = hr.substr(7,6);
	progLen = hr.substr(13,6);
	maxProgAddrInt= hex2dec(startAdd) + hex2dec(progLen);
	
	// read text records until you get the end record
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
	
	// parse data from end record 
	firstExecAdd = tr.substr(1,6);
	
	// start the visiting of instruction from the first executable instruction 
	traverse(firstExecAdd);
	
	// start iterating over thedata statements
	traverseData(firstExecAdd);
	
	// write the program to output file
	writeProgram();
	
	cout<<"Successfully disassembled input object code"<<endl;
	cout<<"Check \"out.txt\" file for the output"<<endl;
}
