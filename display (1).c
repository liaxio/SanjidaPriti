/*csc 332 lab-tast1
Name : Sanjida Firdaws
part1*/

#include <stdio.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>

int main(int argc, char *argv[]) {
	int fd; int rd; int ct;
	errno = 0;
	fd = open(argv[1], O_RDWR);
	// opening argument file for read-writing

	if (fd == -1)
	{
	printf("\n Error: [%s]\n", strerror(errno));
	}
	else {
		char buf[BUFSIZ]; 
		// Declare an array of char called buf that has a size of 			BUFSIZ (a macro constant)

	while ( (rd = read(fd, buf, BUFSIZ)) > 0) {		
		// rd is assigned the return value of read, which is the 			number of characters that are read from fd
		write(1,buf, rd); 							
		// write to the terminal whatever the 'read' syscall reads 			from the file
		}
	}
	close(fd);
	return 0;

}

