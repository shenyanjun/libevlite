
#ifndef SRC_IOLAYER_H
#define SRC_IOLAYER_H

#include <stdint.h>

#include "event.h"
#include "threads.h"
#include "network.h"

#include "session.h"

struct iolayer
{
	// 基础配置
	uint8_t		nthreads;
	uint32_t	nclients;
	
	// 网络线程组
	iothreads_t group;

	// 会话管理器
	void **		managers;
};

// 接收器
struct acceptor
{
	int32_t 	fd;

	// 接收事件
	event_t		event;
	
	// 绑定的地址以及监听的端口号
	char		host[INET_ADDRSTRLEN];
	uint16_t	port;

	// 逻辑
	void * 		context;
	int32_t 	(*cb)(void *, sid_t, const char *, uint16_t);

	// 通信层句柄
	struct iolayer * parent;
};

// 连接器
struct connector
{
	int32_t		fd;

	// 连接事件
	event_t		event;

	// 连接服务器的地址和端口号
	char		host[INET_ADDRSTRLEN];
	uint16_t	port;

	// 逻辑
	int32_t		seconds;
	void *		context;
	int32_t		(*cb)( void *, int32_t, const char *, uint16_t, sid_t);

	// 通信层句柄
	struct iolayer * parent;
};

//
// NOTICE: 网络任务的最大长度不超过56
//

// NOTICE: task_assign长度已经达到48bytes
struct task_assign
{
	int32_t		fd;							// 4bytes

	uint16_t	port;						// 2bytes
	char 		host[INET_ADDRSTRLEN];		// 16bytes + 2bytes

	void *		context;					// 8bytes
	int32_t		(*cb)(void *, sid_t, const char *, uint16_t);	// 8bytes
};

struct task_send
{
	sid_t						id;			// 8bytes
	char *						buf;		// 8bytes
	uint32_t					nbytes;		// 4bytes+4bytes
};

// 看样子内存对齐不需要使用了
#pragma pack(1)
#pragma pack()

// 分配一个会话
struct session * iolayer_alloc_session( struct iolayer * self, int32_t key );

// 重新连接远程服务器
int32_t iolayer_reconnect( struct iolayer * self, struct connector * connector );

// 销毁连接器 
int32_t iolayer_free_connector( struct iolayer * self, struct connector * connector );

// 给当前线程分发一个会话
int32_t iolayer_assign_session( struct iolayer * self, uint8_t index, struct task_assign * task );

#endif

