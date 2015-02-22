#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctype.h>
using namespace std;
char mem[66000][2];

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

int main(){
	char * filename=NULL;
	string progName, progLen, startAdd, firstExecAdd;
	cout<<"Enter the name of the object File "<<endl;
	scanf("%s",filename);
	// use argv for it 
	// initialise mem 
	for(int i=0; i<66000; i++){
		mem[i][0]='G';
		mem[i][1]='G';
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
	
}
