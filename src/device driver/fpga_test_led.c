/* FPGA LED Test Application
File : fpga_test_led.c
Auth : largest@huins.com */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LED_DEVICE "/dev/fpga_led"

int main(int argc, char *argv[])
{
	int i, j;
	int dev;
	unsigned int data[] = {1, 3, 6, 12, 24, 48, 96, 192, 128};
	unsigned int zero = 0;
	int retval;
	int flag;

	/* 디바이스 open */
	dev = open(LED_DEVICE, O_RDWR);
	if (dev<0) {
		printf("Device open error : %s\n",LED_DEVICE);
		exit(1);
	}
	
	/* 두번째 인자가 0인 경우 LED를 끔 */
	if(argc == 2){
		flag = atoi(argv[1]);
		if(flag == 0){
			retval = write(dev, &zero, sizeof(data));
			if(retval < 0){
				printf("Write Error!\n");
				return -1;
			}
			return(0);
		}
	}

	/* 딜레이를 두고 LED를 data 배열값으로 설정 */
	for(i=0; 1; i++){
		write(dev,&data[i],sizeof(int));
		usleep(60000);
		if(i == 8){
			for(j=8; j>=0; j--){
				write(dev, &data[j], sizeof(int));
				usleep(60000);
			}
			i=-1;
		}
	}
	close(dev);		// 디바이스 close
	return(0);
}
