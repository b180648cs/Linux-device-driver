//test_chr_drv.c
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>


int8_t write_buf[1024];
int8_t read_buf[1024];


int main()
{
	int fd;    //file discriptor
	char option;
	
	printf("Welcome to the character device driver......\n");
	
	fd = open("/dev/my_device",O_RDWR);   //Read Write permissions
	if(fd<0){
		printf("We cannot open the device file....\n");
		return 0;
	}
	
	while(1){
		printf("****** Please option your options******\n");
		printf("          1. Write            \n");
		printf("          2. Read            \n");
		printf("          3. Exit            \n");
		scanf(" %c",&option);
		printf(" your options = %c\n",option);
		
		switch(option){
			case '1':
				printf("Enter the string to write to the driver:\n");
				scanf("%[^\t\n]s",write_buf);
				printf("Data written......");
				write(fd,write_buf,strlen(write_buf)+1);
				printf("DONE..\n");
			case '2':  
				printf("Data is reading....");
				read(fd,read_buf,1024);
				printf("Done...\n\n");
				printf("Data=%s\n\n",read_buf);
				break;
			case '3':
				close(fd);
				exit(1);
				break;
			default:
				printf("Enter valid option =%c\n",option);
				break;
		}
	}
	close(fd);
}
	

