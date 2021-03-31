#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <network.h>
#include "threadpool.h"

#define BUF_SIZE	(4096)
#define MAX_CLIENT 	(50)

NetWork* clients[MAX_CLIENT];

int add_clients(NetWork* nw)
{
	for(int i=0; i<MAX_CLIENT; i++)
	{
		if(NULL == clients[i])
		{
			clients[i] = nw;
			return i;
		}
	}
}

void send_cliens(int index,char* str)
{
	for(int i=0; i<MAX_CLIENT; i++)
	{
		if(NULL != clients[i] && i != index) 
		{
			send_nw(clients[i],str,strlen(str)+1);
		}
	}
}

void enter(void* arg)
{
	// 添加到客户端数组
	int index = add_clients(arg);
	char* buf = malloc(BUF_SIZE);

	// 接收昵称
	int ret = recv_nw(clients[index],buf,BUF_SIZE);
	if(0 >= ret || 0 == strcmp("quit",buf))
	{
		close_nw(clients[index]);
		clients[index] = NULL;
		free(buf);
		return;
	}

	strcat(buf," 进入了聊天室，大家小心!");
	send_cliens(index,buf);

	buf[ret-1] = ':';
	char* msg = buf+ret;

	for(;;)
	{
		int ret = recv_nw(clients[index],msg,BUF_SIZE);
		if(0 >= ret || 0 == strcmp("quit",msg))
		{
			sprintf(msg," 退出聊天室，欢送!");
			send_cliens(index,buf);
			close_nw(clients[index]);
			clients[index] = NULL;
			free(buf);
			return;
		}
		send_cliens(index,buf);
	}
}

int main(int argc,const char* argv[])
{
	// 检查命令行参数
	if(3 != argc)
	{
		puts("Use：chat_server <ip> <port>");
		return EXIT_FAILURE;
	}

	// 创建线程池
	ThreadPool* thread = create_threadpool(MAX_CLIENT,20,enter);
	start_threadpool(thread);

	// 启动网络通信
	NetWork* svr_nw = init_nw(SOCK_STREAM,atoi(argv[2]),argv[1],true);
	if(NULL == svr_nw)
	{
		puts("网络错误，请检查");
		return EXIT_FAILURE;
	}

	for(;;)
	{
		// 等待客户端连接
		NetWork* cli_nw = accept_nw(svr_nw);
		if(NULL == cli_nw)
		{
			puts("网络出现异常!");
			return EXIT_FAILURE;
		}

		// 添加到仓库
		push_threadpool(thread,cli_nw);
	}
}
