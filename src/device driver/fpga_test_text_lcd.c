/* FPGA Text LCD Test Application
File : fpga_test_text_lcd.c
Auth : largest@huins.com */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_BUFF 32
#define LINE_BUFF 16
#define FPGA_TEXT_LCD_DEVICE "/dev/fpga_text_lcd"

void moving(unsigned char *str);

/* [사용법] ./fpga_test_lcd text1 [text2]  
 *          ./fpga_test_lcd -m text1 [text2]   --> -m 옵션: 움직이는 텍스트
 */

int main(int argc, char **argv)
{
	int i;
	int dev;
	int str_size;
	int chk_size;
	int option = 0;

	unsigned char string[33];

	/* string 초기화 */
	memset(string, 0,sizeof(string));	

	/* 인자의 개수가 잘못된 경우 에러처리 */
	if(argc<2&&argc>4) {
		printf("Invalid Value Arguments!\n");
		return -1;
	}		
	
	/* 옵션 여부 판별 */
	if(strlen(argv[1]) == 2 && *argv[1] == '-')
		option = 1;
	
	/* 문자열이 한 줄의 최대 길이를 초과한 경우 에러처리 */
	if(strlen(argv[1+option])>LINE_BUFF || ((argc == 3+option) && strlen(argv[2+option])>LINE_BUFF) )
	{
		printf("16 alphanumeric characters on a line!\n");
		return -1;
	}
		
	/* 디바이스 open */
	dev = open(FPGA_TEXT_LCD_DEVICE, O_WRONLY);
	if (dev<0) {
		printf("Device open error : %s\n",FPGA_TEXT_LCD_DEVICE);
		return -1;
	}

	/* 첫번째 문자열의 길이를 얻어 string에 길이만큼 복사 */
	str_size=strlen(argv[1+option]);
	if(str_size>0) {
		strncat(string,argv[1+option],str_size);
		memset(string+str_size,' ',LINE_BUFF-str_size);
	}
	/* 두번째 문자열이 존재할 경우 string에 길이만큼 복사 */
	if(argc == 3+option){
		str_size=strlen(argv[2+option]);
		if(str_size>0) {
			strncat(string,argv[2+option],str_size);
			memset(string+LINE_BUFF+str_size,' ',LINE_BUFF-str_size);
		}
	}
	string[32] = '\0';
	write(dev,string,MAX_BUFF);		// text-LCD에 write

	/* 옵션이 있는 경우 moving() 함수를 사용 */
	if(option == 1){
		while(1){
			moving(string);
			write(dev, string, MAX_BUFF);
			usleep(200000);
		}
	}

	close(dev);		// 디바이스 close
	
	return(0);
}

/* 움직이는 텍스트 구현 */
void moving(unsigned char *str){
	unsigned char backup[33];
	int i;

	strcpy(backup, str+1);
	backup[31] = str[0];
	/* 각 줄 문자열을 한단계 앞으로 shift */
	for(i=0; i<16; i++){
		backup[i] = str[(i+1)%16];
		backup[i+16] = str[(i+1)%16+16];
	}
	backup[32] = '\0';
	strcpy(str, backup);
}
