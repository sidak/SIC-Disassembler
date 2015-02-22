#include <iostream>
#include <fstream>
#include <stdio.h>
using namespace std;

int main(){
	char * filename=NULL;
	string progName, progLen, startAdd, firstExecAdd;
	cout<<"Enter the name of the object File "<<endl;
	scanf("%s",filename);
	// use argv for it 
	ifstream infile(filename);
	string hr, tr;
	getline(infile, hr);
	progName = hr.substr(1, 6);
	startAdd = hr.substr(7,6);
	progLen = hr.substr(13,6);
	
	getline(infile, tr);
	string recStart, recLen; 
	while(tr[0]!='E'){
		recStart=tr.substr(1,6);
		recLen = tr.substr(7,2);
		// initialise the mem array with the object code
		getline(infile, tr); 
	}
	firstExecAdd = tr.substr(1,6);
	
}
