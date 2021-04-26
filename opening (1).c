/*csc 332 lab-tast1
Name : Sanjida Firdaws
partb*/

#include <stdio.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>

int main()
{ int fd = open("destination.txt", O_RDONLY|O_WRONLY|O_CREAT);
if (fd== -1)
{printf("Unable to create a new file, error number %d\n", errno);
}
else{ printf("destination.txt file is created\n");
}
return 0;

}


