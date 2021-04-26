/*
Name : Sanjida Firdaws 
Task 3
part3 step 2: Copy the contents of source.txt into destination1.txt and destination2.txt  as per the following procedure until you reach the end of the file.
read the 50 char from source.txt and write to destination1.txt
then the next 100 char are read from source.txt and written into destination2.txt

*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
	int source, rd, acs, dest1, dest2;
	
	source = open("source.txt", O_RDONLY); // Open the source file.

	dest1 = open("destination1.txt", O_CREAT | O_RDWR, S_IRWXU);  // 				Open destination1 and destination2.
	
	dest2 = open("destination2.txt", O_CREAT | O_RDWR, S_IRWXU);

	if (source == -1 && dest1 == -1 && dest2 == -1)
	{
		printf("Error: [%s]\n", strerror(errno));
	}
	
	else{
		char buf[51];		// Create an array of chars size 51 						to read 50 chars.
		int n = 51;

		while ( (rd = read(source, buf, n)) > 0)
		{

			if (n==51){
				write(dest1, buf, rd);
				n = 101; // Change n to 101 so 					that the next 100 characters will be scanned 
			}
			
			else{
				write(dest2, buf, rd);
				n = 101;
			}
		}

	}	
	close(source);
	close(dest1);
	close(dest2);

	return 0;

}
