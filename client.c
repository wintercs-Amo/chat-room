#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <network.h>
#include <pthread.h>

char buf[4096] = {};
size_t buf_size = sizeof(buf);

void* run(void* arg)
{
	NetWork* nw = arg;
	for(;;)
	{
		int ret = recv_nw(nw,buf,buf_size);
		if(0 >= ret)
		{
			puts("服务器正在升级请稍候！\n");
			exit(EXIT_FAILURE);
		}

		printf("\r--%s\n",buf);
		printf(">>>");
		fflush(stdout);
	}
}

int main(int argc,const char* argv[])
{
	if(3 != argc)
    {
		puts("Use：chat_server <ip> <port>");
	    return EXIT_FAILURE;
	}

	NetWork* nw = init_nw(SOCK_STREAM,atoi(argv[2]),argv[1],false);
	printf("请输入你的昵称：");
	gets(buf);
	send_nw(nw,buf,strlen(buf)+1);

	pthread_t tid;
	pthread_create(&tid,NULL,run,nw);

	for(;;)
	{
		printf(">>>");
		gets(buf);
		int ret = send_nw(nw,buf,strlen(buf)+1);
		if(0 == strcmp("quit",buf))
		{
			puts("退出聊天室");
			return EXIT_SUCCESS;
		}
		if(0 >= ret)
		{
			puts("服务器正在升级请稍候！\n");
			return EXIT_FAILURE;
		}
	}
}
