#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <SDL.h>
#include <SDL_ttf.h>
#define	PATH_LENGTH		24
#define LINE_LENGTH		128
#define CHART_HEIGHT	600
#define HEIGHT_ALPHA    180
#define BUFSIZE			32

// 프로세스의 내용을 담는 구조체 
struct task_struct_mini
{
		pid_t pid;
		pid_t ppid;
		pid_t real_ppid;
		int state;
		long flags;
};

int pstat(pid_t pid)
{
		FILE *fp;
		char path[PATH_LENGTH] = {0};
		char line[LINE_LENGTH] = {0};
		char buf[BUFSIZE];

		// 프로세스의 각 영역의 크기를 저장하는 변수들
		long vm_size  = 0;
		long vm_exe	 = 0;
		long vm_data	 = 0;
		long vm_lib	 = 0;
		long vm_pte	 = 0;
		long vm_swap	 = 0;
		long vm_etc	 = 0;
		long vm_stk	 = 0;

		// 프로세스의 각 영역의 비중을 저장하는 변수들
		float percent_exe 	= 0;
		float percent_data	= 0;
		float percent_lib 	= 0;
		float percent_pte 	= 0;
		float percent_swap 	= 0;
		float percent_etc 	= 0;
		float percent_stk 	= 0;

		// 실행 프로그램 화면 설정
		SDL_Event event;
		SDL_Init(SDL_INIT_EVERYTHING);
		TTF_Init();
		SDL_Surface* screen;
		screen = SDL_SetVideoMode(960, 720, 32, SDL_SWSURFACE);

		// pid.status 파일을 오픈한다
		sprintf(path, "/nfsroot/%d.status", pid);
		if ( (fp = fopen(path, "r")) == NULL)
		{
				printf("open() error\n");
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}

		// pid.status 파일에서 각 영역의 값을 읽는다.
		while (fgets(line, LINE_LENGTH, fp) != NULL)
		{
				if (strncmp(line, "VmSize:", strlen("VmSize:")) == 0)
						vm_size = atol(line + strlen("VmSize:"));
				if (strncmp(line, "VmExe:", strlen("VmExe:")) == 0)
						vm_exe = atol(line + strlen("VmExe:"));
				if (strncmp(line, "VmData:", strlen("VmData:")) == 0)
						vm_data = atol(line + strlen("VmData:"));
				if (strncmp(line, "VmLib:", strlen("VmLib:")) == 0)
						vm_lib = atol(line + strlen("VmLib:"));
				if (strncmp(line, "VmPTE:", strlen("VmPTE:")) == 0)
						vm_pte = atol(line + strlen("VmPTE:"));
				if (strncmp(line, "VmSwap:", strlen("VmSwap:")) == 0)
						vm_swap = atol(line + strlen("VmSwap:"));
				if (strncmp(line, "VmStk:", strlen("VmStk:")) == 0)
						vm_stk = atol(line + strlen("VmStk:"));

				memset(line, 0, strlen(line));
		}
		fclose(fp);

		// 실행중인 프로세스가 아닌경우
		if (vm_size == 0)
		{
				printf("the process is not running\n");
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}


		// 프로세스의 각 영역의 비중을 구한다
		percent_exe 	= (float) vm_exe  / vm_size;
		percent_data	= (float) vm_data / vm_size;
		percent_lib 	= (float) vm_lib  / vm_size;
		percent_pte 	= (float) vm_pte  / vm_size;
		percent_swap	= (float) vm_swap / vm_size;
		percent_stk 	= (float) vm_stk  / vm_size;

		vm_etc = vm_size - (vm_exe + vm_data + vm_lib + vm_pte + vm_swap + vm_stk);
		percent_etc 	= 1 - (percent_exe + percent_data + percent_lib +
						percent_pte + percent_swap + percent_stk);


		// 메모리를 차트로 그린다.
		SDL_Rect rect_white  = {0, 0, screen->w, screen->h};

		int chart_point = 10;
		int chart_plus  = CHART_HEIGHT * percent_exe;
		SDL_Rect rect_red     = {10, chart_point, 400, chart_plus};

		chart_point += chart_plus;
		chart_plus  = CHART_HEIGHT * percent_data;
		SDL_Rect rect_orange  = {10, chart_point, 400, chart_plus};

		chart_point += chart_plus;
		chart_plus  = CHART_HEIGHT * percent_lib;
		SDL_Rect rect_yellow  = {10, chart_point, 400, chart_plus};

		chart_point += chart_plus;
		chart_plus  = CHART_HEIGHT * percent_pte;
		SDL_Rect rect_green  = {10, chart_point, 400, chart_plus};

		chart_point += chart_plus;
		chart_plus  = CHART_HEIGHT * percent_swap;
		SDL_Rect rect_blue  = {10, chart_point, 400, chart_plus};

		chart_point += chart_plus;
		chart_plus  = CHART_HEIGHT * percent_etc;
		SDL_Rect rect_navy  = {10, chart_point, 400, chart_plus};

		chart_point += chart_plus;
		chart_plus  = CHART_HEIGHT * percent_stk;
		SDL_Rect rect_purple  = {10, chart_point, 400, chart_plus};


		SDL_FillRect(screen, &rect_white, SDL_MapRGB(screen->format,0xff,0xff,0xff));
		SDL_FillRect(screen, &rect_red, SDL_MapRGB(screen->format,0xff,0x00,0x00));
		SDL_FillRect(screen, &rect_orange, SDL_MapRGB(screen->format,0xff,0xa5,0x00));
		SDL_FillRect(screen, &rect_yellow, SDL_MapRGB(screen->format,0xff,0xff,0x00));
		SDL_FillRect(screen, &rect_green, SDL_MapRGB(screen->format,0x00,0xff,0x00));
		SDL_FillRect(screen, &rect_blue, SDL_MapRGB(screen->format,0x00,0x00,0xff));
		SDL_FillRect(screen, &rect_navy, SDL_MapRGB(screen->format,0x00,0x00,0x80));
		SDL_FillRect(screen, &rect_purple, SDL_MapRGB(screen->format,0x80,0x00,0x80));
		// 위의 줄까지가 메모리를 차트로 부분이다

		// 글자 폰트 환경을 설정한다
		TTF_Font *font;
		font = TTF_OpenFont("/usr/share/fonts/gnu-free/FreeSansBold.ttf", 18);
		SDL_Surface *text;
		SDL_Color text_color = {0x00, 0x00, 0x00};

		if (font == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}

		// 영역 설명 항목에 대한 작업을 실행한다.
		rect_red.x = 500;
		rect_red.y = 100 + HEIGHT_ALPHA;
		rect_red.w = 100;
		rect_red.h = 25;
		SDL_FillRect(screen, &rect_red, SDL_MapRGB(screen->format,0xff,0x00,0x00));

		memset(buf, 0, BUFSIZE);
		sprintf(buf, "VM_EXE           :");
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_Rect text_red  = {620, 100 + HEIGHT_ALPHA, 400, 50};
		SDL_BlitSurface(text, NULL, screen, &text_red);


		rect_orange.x = 500;
		rect_orange.y = 140 + HEIGHT_ALPHA;
		rect_orange.w = 100;
		rect_orange.h = 25;
		SDL_FillRect(screen, &rect_orange, SDL_MapRGB(screen->format,0xff,0xa5,0x00));
		memset(buf, 0, BUFSIZE);
		sprintf(buf, "VM_DATA        :");
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_Rect text_orange  = {620, 140 + HEIGHT_ALPHA, 400, 50};
		SDL_BlitSurface(text, NULL, screen, &text_orange);


		rect_yellow.x = 500;
		rect_yellow.y = 180 + HEIGHT_ALPHA;
		rect_yellow.w = 100;
		rect_yellow.h = 25;
		SDL_FillRect(screen, &rect_yellow, SDL_MapRGB(screen->format,0xff,0xff,0x00));
		memset(buf, 0, BUFSIZE);
		sprintf(buf, "VM_LIB             :");
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_Rect text_yellow  = {620, 180 + HEIGHT_ALPHA, 400, 50};
		SDL_BlitSurface(text, NULL, screen, &text_yellow);


		rect_green.x = 500;
		rect_green.y = 220 + HEIGHT_ALPHA;
		rect_green.w = 100;
		rect_green.h = 25;
		SDL_FillRect(screen, &rect_green, SDL_MapRGB(screen->format,0x00,0xff,0x00));
		memset(buf, 0, BUFSIZE);
		sprintf(buf, "VM_PTE           :");
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_Rect text_green  = {620, 220 + HEIGHT_ALPHA, 400, 50};
		SDL_BlitSurface(text, NULL, screen, &text_green);


		rect_blue.x = 500;
		rect_blue.y = 260 + HEIGHT_ALPHA;
		rect_blue.w = 100;
		rect_blue.h = 25;
		SDL_FillRect(screen, &rect_blue, SDL_MapRGB(screen->format,0x00,0x00,0xff));
		memset(buf, 0, BUFSIZE);
		sprintf(buf, "VM_SWAP        :");
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_Rect text_blue  = {620, 260 + HEIGHT_ALPHA, 400, 50};
		SDL_BlitSurface(text, NULL, screen, &text_blue);


		rect_navy.x = 500;
		rect_navy.y = 300 + HEIGHT_ALPHA;
		rect_navy.w = 100;
		rect_navy.h = 25;
		SDL_FillRect(screen, &rect_navy, SDL_MapRGB(screen->format,0x00,0x00,0x80));
		memset(buf, 0, BUFSIZE);
		sprintf(buf, "VM_ETC           :");
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_Rect text_navy  = {620, 300 + HEIGHT_ALPHA, 400, 50};
		SDL_BlitSurface(text, NULL, screen, &text_navy);


		rect_purple.x = 500;
		rect_purple.y = 340 + HEIGHT_ALPHA;
		rect_purple.w = 100;
		rect_purple.h = 25;
		SDL_FillRect(screen, &rect_purple, SDL_MapRGB(screen->format,0x80,0x00,0x80));
		memset(buf, 0, BUFSIZE);
		sprintf(buf, "VM_STK           :");
		text = TTF_RenderText_Solid(font, buf, text_color);  

		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_Rect text_purple  = {620, 340 + HEIGHT_ALPHA, 400, 50};
		SDL_BlitSurface(text, NULL, screen, &text_purple);


		memset(buf, 0, BUFSIZE);
		sprintf(buf, "VM_TOTAL      :        %4d KB", vm_size);
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_Rect text_total  = {620, 380 + HEIGHT_ALPHA, 400, 50};
		SDL_BlitSurface(text, NULL, screen, &text_total);


		SDL_Surface *image;
		image = SDL_LoadBMP("ourteam.bmp");
		if (image == NULL) {
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}

		SDL_Rect rect_image  = {500, 600, };
		SDL_BlitSurface(image, NULL, screen, &rect_image);

		memset(buf, 0, BUFSIZE);
		sprintf(buf,  "VM_EXE           :        %4ld KB", vm_exe);
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_BlitSurface(text, NULL, screen, &text_red);

		memset(buf, 0, BUFSIZE);
		sprintf(buf,  "VM_DATA        :        %4ld KB", vm_data);
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_BlitSurface(text, NULL, screen, &text_orange);

		memset(buf, 0, BUFSIZE);
		sprintf(buf,  "VM_LIB             :        %4ld KB", vm_lib);
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_BlitSurface(text, NULL, screen, &text_yellow);

		memset(buf, 0, BUFSIZE);
		sprintf(buf,  "VM_PTE           :        %4ld KB", vm_pte);
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_BlitSurface(text, NULL, screen, &text_green);

		memset(buf, 0, BUFSIZE);
		sprintf(buf,  "VM_SWAP        :        %4ld KB", vm_swap);
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_BlitSurface(text, NULL, screen, &text_blue);


		memset(buf, 0, BUFSIZE);
		sprintf(buf,  "VM_ETC           :        %4ld KB", vm_etc);
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_BlitSurface(text, NULL, screen, &text_navy);

		memset(buf, 0, BUFSIZE);
		sprintf(buf,  "VM_STK           :        %4ld KB", vm_stk);
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_BlitSurface(text, NULL, screen, &text_purple);

		// 위의 줄까지가 영역 설명 항목에 대한 작업이다

		// pid.task에 대한 파일을 읽는다
		int fd;
		int n;
		struct task_struct_mini task;

		sprintf(path, "/nfsroot/%d.task", pid);

		if ((fd = open(path, O_RDONLY)) == -1) {
				printf("fopen() error\n");
				return -2;
		}

		n = sizeof(task);
		if (read(fd, (char*)&task, n) != n) {
				printf("read() error\n");
				return -3;
		}
		// 위의 줄까지가 pid.task 파일을 읽는부분이다.

		// pid.task 로부터 읽은 내용들을 보여준다
		memset(buf, 0, BUFSIZE);
		sprintf(buf, "PID                  :        %d", task.pid);
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_Rect text_pid  = {540, 50, 400, 50};
		SDL_BlitSurface(text, NULL, screen, &text_pid);


		memset(buf, 0, BUFSIZE);
		sprintf(buf, "PPID                :        %d", task.ppid);
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_Rect text_ppid  = {540, 90, 400, 50};
		SDL_BlitSurface(text, NULL, screen, &text_ppid);

		memset(buf, 0, BUFSIZE);
		sprintf(buf, "REAL_PPID    :        %d", task.real_ppid);
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_Rect text_real  = {540, 130, 400, 50};
		SDL_BlitSurface(text, NULL, screen, &text_real);

		memset(buf, 0, BUFSIZE);
		sprintf(buf, "STATE            :        %d", task.state);
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_Rect text_state  = {540, 170, 400, 50};
		SDL_BlitSurface(text, NULL, screen, &text_state);


		memset(buf, 0, BUFSIZE);
		sprintf(buf, "FLAGS            :        %d", task.flags);
		text = TTF_RenderText_Solid(font, buf, text_color);  
		if (text == NULL)
		{
				TTF_Quit();
				SDL_Quit();
				exit(1);
		}
		SDL_Rect text_flags  = {540, 210, 400, 50};
		SDL_BlitSurface(text, NULL, screen, &text_flags);

		SDL_Rect line_up     = {500, 30, 330, 4};
		SDL_Rect line_bottom = {500, 240, 330, 4};
		SDL_Rect line_left   = {500, 30, 4, 210};
		SDL_Rect line_right  = {830, 30, 4, 214};
		SDL_FillRect(screen, &line_up, SDL_MapRGB(screen->format,0x00,0x00,0x00));
		SDL_FillRect(screen, &line_bottom,SDL_MapRGB(screen->format,0x00,0x00,0x00));
		SDL_FillRect(screen, &line_left,SDL_MapRGB(screen->format,0x00,0x00,0x00));
		SDL_FillRect(screen, &line_right,SDL_MapRGB(screen->format,0x00,0x00,0x00));
		// 위의 줄까지가 pid.task로 읽은 내용을 보여주는 작업이다.

		// 화면을 출력한다.
		SDL_Flip(screen);

		// 사용자로부터의 이벤트를 기다린다.
		printf("pstat start\n");
		short done = 0;
		while ( !done ) {
				while ( SDL_PollEvent(&event) ) {
						switch (event.type) {
								case SDL_KEYDOWN:
										done = 1;
										break;
								case SDL_QUIT:
										done = 1;
										break;
								default:
										break;
						}
				}
		}

		printf("pstat end\n");
		// 프로그램 종료
		TTF_Quit();	
		SDL_Quit();
		printf("real pstat end\n");
		return 0;
}
