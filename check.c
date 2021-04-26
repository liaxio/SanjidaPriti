/*csc 332 lab-tast1
Name : Sanjida Firdaws
part1*/

#include <stdio.h>
#include<unistd.h>
#include<errno.h>

int main(int argc, char*argv[])
	{
	char*filepath = argv[1];
	int returnvalue;
	returnvalue = access(filepath, F_OK);//file access
	if(returnvalue == 0){
	printf("\n%s exists\n", filepath);
	}
	else {
	if(errno==ENOENT)
	printf("%s does not exist\n", filepath);
	else if (errno == EACCES)
	printf("%s is not accessible\n", filepath);
	return 0;
	}
	returnvalue = access (filepath, R_OK);//file read access
	if(returnvalue == 0)
	printf("%s is readable\n", filepath);
	else
	printf("%s not readable, access denied \n", filepath);
	
	returnvalue = access(filepath, W_OK);//write access
	if(returnvalue == 0)
	printf("%s is writeable\n", filepath);
	else {
	if(errno == EACCES)
	printf("%s not writable\n",filepath);
	else if (errno == EROFS)
	printf("%s is not writable(readonly)\n", filepath);
	return 0;
	}
	}
