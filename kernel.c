//Isaiah's Operating System Kernel
//Modified by Tom and Tim
//kernel.c

void handleInterrupt21(int,int,int,int);
void printChar(char);
void printstring(char*);
void readstring(char*);
void readsector(char*, int);
void makeInterrupt21(int,int,int,int);
void readFile(char*, char*, int*);
void executeProgram(char*);
void writeSector(char*, int);
void terminate();
void deletefile(char*);
void writeFile(char*, char*, int);

void main(){
	char buffer[13312];
	makeInterrupt21();
	interrupt(0x21,8,"this is a test messag", "testmg",3);
	interrupt(0x21,4,"shell",0,0);
        while(1);
}

void printChar(char c){
	interrupt(0x10,0xe*256+c,0,0,0);
}

void printstring(char* letters){

	while(*letters != 0x0) {
		interrupt(0x10,0xe*256+*letters,0,0,0);
		//advance letters pointer
		letters++;
	}
}

void readsector(char* buffer, int sector){
	interrupt(0x13, 2*256+1, buffer, 0*256+(sector+1),0*256+0x80);
}

void readstring(char output[]){
	int i=0;
	output[i] = interrupt(0x16,0,0,0,0);
	interrupt(0x10,0xe*256+output[i],0,0,0);
	while(output[i] != 0xd){
		i++;
		output[i] = interrupt(0x16,0,0,0,0);
		if(output[i] == 0x8 && i>=1){
			i--;
			output[i] = 0x20;
			printChar(0x8);
			printChar(output[i]);
			printChar(0x8);
			i--;
		}
		else if (output[i] != 0x8){
			interrupt(0x10,0xe*256+output[i],0,0,0);
		}
	}
	i++;
	output[i] = 0xa;
	interrupt(0x10,0xe*256+output[i],0,0,0);
	i++;
	output[i] = 0x0;
	interrupt(0x10,0xe*256+output[i],0,0,0);
}

void handleInterrupt21(int AX, int BX, int CX, int DX){
	if (AX==0){
	printstring(BX);
	}
	else if (AX==1){
	readstring(BX);
	}
	else if (AX==2){
	readsector(BX,CX);
	}
	else if (AX==3){
	readFile(BX,CX,DX);
	}
	else if (AX==4){
	executeProgram(BX);
	}
	else if (AX==5){
	terminate();
	}
	else if (AX==6){
	writeSector(BX,CX);
	}
	else if (AX==7){
	deletefile(BX);
	}
	else if(AX==8){
	writeFile(BX,CX,DX);
	}
	else if (AX==70){
	printChar(BX);
	}
	else{
	printstring("Error with interrupt 21");
	}
}
void readFile(char* filename, char* buffer, int* sectorsread){
	char directory[512];
	int filenamematch = 1;
	int i = 0;
	int fileentry = 0;
	int sectorcount = 0;
	int pad = 0;
	readsector(directory, 2);
	for (fileentry = 0; fileentry < 512; fileentry=fileentry+32){
		filenamematch = 1;
		pad = 0;
		for (i = 0; i < 6; ++i){
			if(filename[i] == '\n' || filename[i] == '\r')
				pad = 1;
			if(pad == 1)
				filename[i] = '\0';
			if(directory[fileentry+i] == filename[i])
				filenamematch;
			else if(directory[fileentry+i] != filename[i]){
				filenamematch = 0;
				break;
			}
		}
		if (filenamematch){
			for (i = 0; i < 26; ++i)
				if (directory[fileentry+6+i] != 0x00){{
					readsector(buffer+512*i, directory[fileentry+6+i]);
					sectorcount = sectorcount+1;
				}
			}
			*sectorsread = sectorcount;
			fileentry = 512;
		}
	}
}
void executeProgram(char* name){
	char buffer[13312];
	int sectorsread = 0;
	int i = 0;
	int j = 0;
	interrupt(0x21, 3, name, buffer, &sectorsread);
	for (i = 0; i < 13312; i++){
		putInMemory(0x2000,i,buffer[i]);
	}
	launchProgram(0x2000);
}

void terminate(){
	char shellname[6];
	shellname[0]='s';
	shellname[1]='h';
	shellname[2]='e';
	shellname[3]='l';
	shellname[4]='l';
	shellname[5]='\0';
	executeProgram(shellname);
}

void writeSector(char* buffer, int sector){
	interrupt(0x13, 3*256+1, buffer, 0*256+(sector+1),0*256+0x80);
}


void deletefile(char* filename){
	char dir[512];
	char map[512];
	int fileEntry = 0;
	int i = 0;
	int filenamematch = 1;
	int pad = 0;
	readsector(dir, 2);
	readsector(map, 1);
	for (fileEntry = 0; fileEntry <512; fileEntry=fileEntry+32){
		filenamematch=1;
		pad = 0;
		for (i = 0; i < 6;++i){
			if(filename[i] == '\n' || filename[i] == '\r')
                                pad = 1;
                        if(pad == 1)
                                filename[i] = '\0';
                        if(dir[fileEntry+i] == filename[i])
                                filenamematch;
                        else if(dir[fileEntry+i] != filename[i]){
                                filenamematch = 0;
                                break;
                        }
		}
		if (filenamematch){
			dir[fileEntry]=0x00;
			for (i = 0; i < 26;++i){
				if (dir[fileEntry+i+6] != 0x00){
					map[dir[fileEntry+i+6]]=0;
				}
			}
			for (i = 0; i < 32;++i){
			dir[fileEntry+i] = 0x00;
			}
		}
	}
	writeSector(map,1);
	writeSector(dir,2);
}

void writeFile(char* buffer, char* filename, int numberOfSectors){
	char dir[512];
        char map[512];
        int fileEntry = 0;
        int i = 0;
        int fileInCurrentLocation = 0;
	int totalSectors = numberOfSectors;
        readsector(dir, 2);
        readsector(map, 1);
	for(i=0; i<6; i++){
		if(filename[i] == '\r' || filename[i] == '\n')
			filename[i] = 0x00;
	}
        for (fileEntry = 0; fileEntry <512; fileEntry=fileEntry+32){
                fileInCurrentLocation = 0;
                for(i=0; i<6; i++){
                        if(dir[fileEntry+i] != 0x00)
				fileInCurrentLocation = 1;
                }
                if (1-fileInCurrentLocation){
                        for(i=0; i<6; i++){
				dir[fileEntry+i] = filename[i];
			}
			i = 3;
                        while(numberOfSectors>0){
                                if (dir[fileEntry+totalSectors-numberOfSectors+6] == 0x00 && map[i] != 0xFF){
                                        map[i] = 0xFF;
					dir[fileEntry + 6 + totalSectors - numberOfSectors] = i;
                                	writeSector(buffer, i);
					buffer = buffer + 512;
					numberOfSectors--;
				}
				i++;
                        }
			fileEntry = 512;
                }
        }
        writeSector(map,1);
        writeSector(dir,2);
}
