#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MOTOR_ATTRIBUTE_ERROR_RANGE 4
#define FPGA_STEP_MOTOR_DEVICE "/dev/fpga_step_motor"

/*
 * [사용법]
 * 실행명    : 오른쪽으로 회전
 * 실행명 1  : 오른쪽으로 회전(위와 동일)
 * 실행명 0  : 정지
 */

void usage(char* dev_info); 

int main(int argc, char **argv)
{
	int i;
	int dev;
	int str_size;

	int motor_action;		// 모터의 상태( stop, start )
	int motor_direction;	// 모터가 도는 방향 0-1 ( left, right )
	int motor_speed;		// 모터가 도는 속도 0-255 [fast - slow]


	unsigned char motor_state[3];
	
	/* motor_state 초기화 */
	memset(motor_state,0,sizeof(motor_state));

	/* 인자의 개수가 잘못된 경우 에러처리 */
	if(argc > 2) {
		printf("Please input the parameter! \n");
		usage(argv[0]);
		return -1;
	}
	
	motor_direction = 1;	// motor의 방향을 오른쪽으로 설정
	motor_speed = 15; 		// motor의 속도를 15로 설정
	
	/* 인자가 없는 경우 motor를 활성화 */
	if(argc == 1)
		motor_action = 1;
	else{
		/* 인자가 있는경우 인자의 값이 1이면 활성화, 0이면 비활성화 */
		motor_action = atoi(argv[1]);
		if(motor_action<0||motor_action>1) {
			printf("Invalid motor action!\n");
			usage(argv[0]);
			return -1;
		}
	}
	
	/* motor의 상태를 설정 */
	motor_state[0]=(unsigned char)motor_action;
	motor_state[1]=(unsigned char)motor_direction;;
	motor_state[2]=(unsigned char)motor_speed;

	/* 디바이스 open */
	dev = open(FPGA_STEP_MOTOR_DEVICE, O_WRONLY);
	if (dev<0) {
		printf("Device open error : %s\n",FPGA_STEP_MOTOR_DEVICE);
		exit(1);
	}
	
	/* motor의 상태를 반영 */
	write(dev,motor_state,3);	
	close(dev);
	
	return 0;
}

/* 사용법 출력하는 함수 */
void usage(char* dev_info) 
{
	printf("<Usage> %s [Motor Action] [Motor Diretion] [Speed]\n",dev_info);
	printf("Motor Action : 0 - Stop / 1 - Start\n");
}

