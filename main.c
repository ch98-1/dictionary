//Copyright (C) 2014-2015  Corwin Hansen
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{//chunk of file
	long int start;//strart in bytes
	long int size;//length in bytes
}chunk;

chunk *list;//list of chunks
long int n;//number of chunks

FILE *fp;//file pointer

void new( long int start, long int end){
	n++;//increment size
	list = realloc(list, n * sizeof(chunk));//reallocate memory
	list[n - 1].start = start;//set data
	list[n - 1].size = end - start;
}

int bt(chunk a, chunk b){//compare 2 part of file. same as a > b
	char *mema, *memb;//memory to fill
	int result = 0;//result
	mema = malloc(a.size);//allocate memory
	memb = malloc(b.size);
	fseek(fp, a.start,  SEEK_SET);//go to that position
	if(fread(mema, a.size, 1, fp) != 1) exit(EXIT_FAILURE);//exit if unable to read from file
	fseek(fp, b.start, SEEK_SET);//go to that position
	if(fread(memb, b.size, 1, fp) != 1) exit(EXIT_FAILURE);//exit if unable to read from file
	long int i;
	for (i = 0; i < a.size; i++){//for each character in array
		if(mema[i] >= 'A' && mema[i] <= 'Z'){
			mema[i] -= 'A' - 'a';//convert to lower case
		}
	}
	for (i = 0; i < b.size; i++){//for each character in array
		if(memb[i] >= 'A' && memb[i] <= 'Z'){
			memb[i] -= 'A' - 'a';//convert to lower case
		}
	}
	for (i = 0; i < (a.size > b.size? b.size : a.size); i++){//for each character until end of 1
		if(mema[i] > memb[i]){//if bigger
			result = 1;
			goto end;
		}
		else if(mema[i] < memb[i]){//if smaller
			result = 0;
			goto end;
		}
	}
	if(a.size > b.size) result = 1;//if a is bigger, result is true
end:
	free(mema);//free memory
	free(memb);
	return result;
}

void free_mem (void){//free memory
	free(list);//free list
}

int main(int argc, char *argv[]){
	if(argc != 2) exit(EXIT_FAILURE);//exit if incorrect number of argument
	fp = fopen(argv[1], "rb");//open file for reading
	if(fp == NULL) exit(EXIT_FAILURE);//exit if unable to open file

	int data = 0;//data read
	int nl = 0;//number of newline
	n = 0;//reset n
	atexit(free_mem);//free memory at exit
	while(1){//for each character in file
		if((data = fgetc(fp)) == EOF){
			if(n == 0){
				new(0, ftell(fp));//create new chunk
			}
			else
			{
				new(list[n - 1].start + list[n - 1].size, ftell(fp));//create new chunk
			}
			break;
		}
		if(nl >= 2 && data != '\n' && data != '\r'){//if there is 2 newline
			if(n == 0){
				new(0, ftell(fp) - 1);//create new chunk
			}
			else
			{
				new(list[n - 1].start + list[n - 1].size, ftell(fp) - 1);//create new chunk
			}
		}
		if(data == '\n'){//if newline
			nl++;//increment newline
		}
		else if(data != '\r'){//if not carriage return
			nl = 0;//reset newline
		}
	}

	//insertation sort
	//https://en.wikipedia.org/wiki/Insertion_sort
	long int i;
	for(i = 1; i < n; i++){
		chunk cpy = list[i];
		long int j = i;
		while(j > 0 && bt(list[j - 1], cpy)){
			list[j] = list[j - 1];
			j--;
		}
		list[j] = cpy;
	}

	char *mem;//memory to fill

	for(i = 0; i < n; i++){
		fseek(fp, list[i].start, SEEK_SET);//go to that position
		mem = malloc(list[i].size);//set memory size
		if(fread(mem, list[i].size, 1, fp) != 1) exit(EXIT_FAILURE);//exit if unable to read from file
		if(fwrite(mem, list[i].size, 1, stdout) != 1) exit(EXIT_FAILURE);//exit if unable to write to file
		free(mem);
	}

	exit(EXIT_SUCCESS);//exit at end
}

