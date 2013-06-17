/* FPGA DotMatirx Test Application
File : fpga_test_dot.c
Auth : largest@huins.com */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FPGA_DOT_DEVICE "/dev/fpga_dot"

int main(int argc, char *argv[])
{
	int i;
	int dev;
	int str_size;
	int set_num;

	char dot[10] = {0x00,};

	/* 디바이스 open */
	dev = open(FPGA_DOT_DEVICE, O_WRONLY);
	if (dev<0) {
		printf("Device open error : %s\n",FPGA_DOT_DEVICE);
		exit(1);
	}

	str_size=sizeof(dot);	// Dot Matrix 데이터 값의 크기
	
	while(1){
		/* Dot Matrix 에서 한줄씩 불을 켬 */
		for(i=9; i>=0; i--){
			dot[i] = 0x7f;
			write(dev,dot,str_size);	
			usleep(20000);				// 딜레이
			if(i == 0){
				/* 모두 켜진 경우 반대로 한줄씩 불을 끔 */
				for(j=0; j<10; j++){
					dot[j] = 0x00;
					write(dev, dot, str_size);
					usleep(20000);		// 딜레이
				}
			}
		}
		
		/* 위에서 아래로 한줄씩 불을 켬 */
		for(i=9; i>=0; i--){
			dot[i] = 0x7f;
			write(dev,dot,str_size);	
			usleep(30000);
			if(i == 0){
				/* 모두 켜진 경우 위에서 아래로 한줄씩 불을 끔 */
				for(i=9; i>=0; i--){
					dot[i] = 0x00;
					write(dev, dot, str_size);
					usleep(30000);
				}
			}
		}

		/* 아래서 위로 한줄씩 불을 켬 */
		for(i=0; i<10; i++){
			dot[i] = 0x7f;
			write(dev,dot,str_size);	
			usleep(30000);
			if(i == 9){
				/* 모두 켜진 경우 아래서 위로 한줄씩 불을 끔 */
				for(i=0; i<10; i++){
					dot[i] = 0x00;
					write(dev, dot, str_size);
					usleep(30000);
				}
			}
		}

	}
	close(dev);

	return 0;
}
