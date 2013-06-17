/* FPGA FND Test Application
File : fpga_test_fnd.c
Auth : largest@huins.com */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define MAX_DIGIT 4
#define FND_DEVICE "/dev/fpga_fnd"

int main(int argc, char **argv)
{
	int dev;
	unsigned char data[4];
	int retval;
	int i, j;
	int str_size;

	/* data 초기화 */
	memset(data,0,sizeof(data));

	/* 인자의 개수가 잘못된 경우 에러처리 */
	if(argc!=2) {
		printf("please input the parameter! \n");
		printf("ex)./test_fnd a1\n");
		return -1;
	}

	/* 입력된 문자열의 길이를 구함 */
	str_size=(strlen(argv[1]));

	/* 4자리를 초과한 경우 경고 메세지 출력 */
	if(str_size>MAX_DIGIT)
	{
		printf("Warning! 4 Digit number only!\n");
		str_size=MAX_DIGIT;
	}

	/* 입력된 문자를 숫자로 변환하여 data에 저장 */ 
	for(i=str_size-1, j=3; i>=0; i--, j--){
		if((argv[1][i]<0x30)||(argv[1][i])>0x39) {
			printf("Error! Invalid Value!\n");
			return -1;
		}
		data[j]=argv[1][i]-0x30;
	}

	/* 디바이스 open */
	dev = open(FND_DEVICE, O_RDWR);
	if (dev<0) {
		printf("Device open error : %s\n",FND_DEVICE);
		exit(1);
	}

	/* 디바이스에 숫자를 씀 */
	retval = write(dev,&data,4);	
	if(retval<0) {
		printf("Write Error!\n");
		return -1;
	}

	/* data 재 초기화 */
	memset(data,0,sizeof(data));

	close(dev);		// 디바이스 close

	return(0);
}
