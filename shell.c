void printDir();
int modulo(int, int);
void intToString(int, char*);
void createFile(char*);
main(){
	while(1){
		char input[80];
		char buffer[13312];
		int sectors = 0;
		int i=0;
		char filenames[40];
		char filename1[40];

		syscall(0,"\r\n");
		syscall(0, "\rSHELL>");
		syscall(1, input);
		if (input[0]=='t' && input[1]=='y' &&input[2]=='p' &&input[3]=='e' &&input[4]==' '){
			//print file with name parameter1
			syscall(3, input+5, buffer, &sectors);
			if (sectors < 1){
				syscall(0,"file not found\r\n");
			}
				else{
				syscall(0, buffer);
			}
		}
		else if (input[0] =='e' && input[1]=='x' &&input[2]=='e' &&input[3]=='c' &&input[4]==' '){
			//run a program stored on the drive with filename parameter1
			syscall(4, input+5);
		}
		else if (input[0]=='d' && input[1]=='i' && input[2]=='r'){
			printDir();
		}
		else if (input[0]=='d' && input[1]=='e' && input[2]=='l' && input[3]==' '){
			//delete file with name parameter1
			syscall(7, input+4);
		}
		else if (input[0]=='c' && input[1]=='o' && input[2]=='p' && input[3]=='y' && input[4]==' '){
			while(input[i+5]!=0xa){
				filenames[i] = input[i+5];
				i++;
			}
			i = 0;
			while(filenames[i]!= ' '){
				filename1[i] = filenames[i];
				i++;
			}
			syscall(3, filename1, buffer, &sectors);
			syscall(8, buffer, filenames + i + 1, sectors);
		}
		else if(input[0]=='c' && input[1]=='r' && input[2]=='e' && input[3]=='a' && input[4]=='t' && input[5]=='e' && input[6]==' '){
			createFile(input + 7);
		}
		else{
			syscall(0, "Command not found\r\n");
			syscall(5);
		}
	}
}

int modulo(int value, int divisor){
	int remainder=0;
	remainder  = value;
	if (value < divisor){
		return value;
	}
	while (remainder >= divisor){
		remainder = remainder-divisor;
	}
	return remainder;
}
void intToString(int inInt, char* outStr){
	char backwardsStr[32];
	int length = 0;
	int charIndex = 0;
	int remainder;
	//insert one's place into string, divide by 10, repeat
	while (inInt !=0){
		remainder = modulo(inInt, 10);

		if (remainder == 0){
			backwardsStr[length]='0';
		}
		else if (remainder == 1){
			backwardsStr[length]='1';
		}
		else if (remainder == 2){
			backwardsStr[length]='2';
		}
		else if (remainder == 3){
			backwardsStr[length]='3';
		}
		else if (remainder == 4){
			backwardsStr[length]='4';
		}
		else if (remainder == 5){
			backwardsStr[length]='5';
		}
		else if (remainder == 6){
			backwardsStr[length]='6';
		}
		else if (remainder == 7){
			backwardsStr[length]='7';
		}
		else if (remainder == 8){
			backwardsStr[length]='8';
		}
		else if (remainder == 9){
			backwardsStr[length]='9';
		}
		else {
			backwardsStr[length]='c';
		}
		length = length+1;
		syscall(0,"\r\n");
		inInt = (int) inInt/10;
	}
	//reverse the backwards string
	for (charIndex=0; charIndex < length; ++charIndex){
		outStr[charIndex] = backwardsStr[length-charIndex-1];
	}
}
void printDir(){
	int fileentry = 0;
	int filenameindex = 0;
	int sectorindex = 0;
	int size = 0;
	char numstr[32];
	char buffer[512];
	char filename[7];
	syscall(2, buffer, 2);
	syscall(0,"\r\n");
	//loop through all files
	for (fileentry = 0; fileentry < 512; fileentry=fileentry+32){
		if (buffer[fileentry] != 0x00){
			size = 0;
			//find out the name of the file
			for (filenameindex = 0; filenameindex < 6; filenameindex++){
				filename[filenameindex] = buffer[fileentry+filenameindex];
			}

			//calculate the size of the file
			for (sectorindex = 0; sectorindex < 26; sectorindex++){
				if (buffer[fileentry+6+sectorindex] != 0x00){
					size = size+1;
				}
			}
			intToString(size, numstr);
			//print out everything
			syscall(0,filename);

			syscall(0,"_");
			syscall(0, numstr);
			syscall(0,"_sectors\r\n\n");
		}
	}
}
void createFile(char* filename){
	char buffer[13312];
	char line[80];
	int i = 0;
	int filesize = 0;
	while(line[0] != '\r'){
		i = 0;
		syscall(0,"type a line:");
		syscall(1, line);
		while (line[i]!=0x00){
			buffer[filesize] = line[i];
			filesize++;
			i++;
		}
	}
	syscall(8, buffer, filename, ((int)(filesize+512)/512));
}
