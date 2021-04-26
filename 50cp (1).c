/*csc 332 lab-tast1
Name : Sanjida Firdaws
part3*/

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	int fd; int rd; int acs; int dest;
	fd = open(argv[1], O_RDONLY);
	// Check if the file already exists
	acs = access(argv[2], F_OK);
	if (acs == 0)
	{
		unlink(argv[2]);
	}
	dest = open(argv[2], O_CREAT | O_RDWR, S_IRWXU);

	if (fd == -1)
	{
		printf("\n Error: [%s]\n", strerror(errno));
	}
	else {
		char buf[60];
		int rd;
		// While reading up to 50 chars, within every 50 chars and 			iterate through and if a '5' is scanned, replace with 'A'
		while (( rd = read(fd, buf, 51)) > 0) {	
			for (int i = 0; i < rd; i++)  	
			{
				if (buf[i] == '5')
					buf[i] = 'A';
			}
			//assign X,Y and Z to the array at index 50,51 and 52
			buf[rd] = 'X';
			buf[rd+1] = 'Y';
			buf[rd+2] = 'Z';
			write(dest, buf, rd+3);			
		}
	}

	close(fd);
	close(dest);

	return 0;

}

	
	
	

