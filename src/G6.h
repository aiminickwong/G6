/*
 * G6 - TCP Bridge && LB Dispenser
 * Author      : calvin
 * Email       : calvinwillliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#ifndef _H_G6_
#define _H_G6_

#if ( defined __linux ) || ( defined __unix )
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>
#include <limits.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
#define _VSNPRINTF			vsnprintf
#define _SNPRINTF			snprintf
#define _CLOSESOCKET			close
#define _CLOSESOCKET2(_fd1_,_fd2_)	close(_fd1_),close(_fd2_);
#define _ERRNO				errno
#define _EWOULDBLOCK			EWOULDBLOCK
#define _ECONNABORTED			ECONNABORTED
#define _EINPROGRESS			EINPROGRESS
#define _ECONNRESET			ECONNRESET
#define _ENOTCONN			ENOTCONN
#define _EISCONN			EISCONN
#define _SOCKLEN_T			socklen_t
#define _GETTIMEOFDAY(_tv_)		gettimeofday(&(_tv_),NULL)
#define _LOCALTIME(_tt_,_stime_) \
	localtime_r(&(_tt_),&(_stime_));
#elif ( defined _WIN32 )
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <io.h>
#include <windows.h>
#define _VSNPRINTF			_vsnprintf
#define _SNPRINTF			_snprintf
#define _CLOSESOCKET			closesocket
#define _CLOSESOCKET2(_fd1_,_fd2_)	closesocket(_fd1_),closesocket(_fd2_);
#define _ERRNO				GetLastError()
#define _EWOULDBLOCK			WSAEWOULDBLOCK
#define _ECONNABORTED			WSAECONNABORTED
#define _EINPROGRESS			WSAEINPROGRESS
#define _ECONNRESET			WSAECONNRESET
#define _ENOTCONN			WSAENOTCONN
#define _EISCONN			WSAEISCONN
#define _SOCKLEN_T			int
#define _GETTIMEOFDAY(_tv_) \
	{ \
		SYSTEMTIME stNow ; \
		GetLocalTime( & stNow ); \
		(_tv_).tv_usec = stNow.wMilliseconds * 1000 ; \
		time( & ((_tv_).tv_sec) ); \
	}
#define _SYSTEMTIME2TIMEVAL_USEC(_syst_,_tv_) \
		(_tv_).tv_usec = (_syst_).wMilliseconds * 1000 ;
#define _SYSTEMTIME2TM(_syst_,_stime_) \
		(_stime_).tm_year = (_syst_).wYear - 1900 ; \
		(_stime_).tm_mon = (_syst_).wMonth - 1 ; \
		(_stime_).tm_mday = (_syst_).wDay ; \
		(_stime_).tm_hour = (_syst_).wHour ; \
		(_stime_).tm_min = (_syst_).wMinute ; \
		(_stime_).tm_sec = (_syst_).wSecond ;
#define _LOCALTIME(_tt_,_stime_) \
	{ \
		SYSTEMTIME	stNow ; \
		GetLocalTime( & stNow ); \
		_SYSTEMTIME2TM( stNow , (_stime_) ); \
	}
#endif

#include "LOGC.h"
#include "rbtree.h"

#ifndef ULONG_MAX
#define ULONG_MAX 0xffffffffUL
#endif

#define FOUND				9	/* �ҵ� */
#define NOT_FOUND			4	/* �Ҳ��� */

#define MATCH				1	/* ƥ�� */
#define NOT_MATCH			-1	/* ��ƥ�� */

#define LOAD_BALANCE_ALGORITHM_G	"G"	/* �����˿� */
#define LOAD_BALANCE_ALGORITHM_MS	"MS"	/* ����ģʽ */
#define LOAD_BALANCE_ALGORITHM_RR	"RR"	/* ��ѯģʽ */
#define LOAD_BALANCE_ALGORITHM_LC	"LC"	/* ��������ģʽ */
#define LOAD_BALANCE_ALGORITHM_RT	"RT"	/* ��С��Ӧʱ��ģʽ */
#define LOAD_BALANCE_ALGORITHM_RD	"RD"	/* ���ģʽ */
#define LOAD_BALANCE_ALGORITHM_HS	"HS"	/* HASHģʽ */

#define DEFAULT_RULES_INITCOUNT			2
#define DEFAULT_RULES_INCREASE			5

#define DEFAULT_CLIENTS_INITCOUNT_IN_ONE_RULE	2
#define DEFAULT_CLIENTS_INCREASE_IN_ONE_RULE	5
#define DEFAULT_FORWARDS_INITCOUNT_IN_ONE_RULE	2
#define DEFAULT_FORWARDS_INCREASE_IN_ONE_RULE	5
#define DEFAULT_SERVERS_INITCOUNT_IN_ONE_RULE	2
#define DEFAULT_SERVERS_INCREASE_IN_ONE_RULE	5

#define DEFAULT_FORWARD_SESSIONS_MAXCOUNT	10000	/* ȱʡ����������� */
#define DEFAULT_FORWARD_TRANSFER_BUFSIZE	4096	/* ȱʡͨѶת����������С */

#define RULE_ID_MAXLEN				64
#define LOAD_BALANCE_ALGORITHM_MAXLEN		2
#define IP_MAXLEN				30
#define PORT_MAXLEN				10

#define WAIT_EVENTS_COUNT			1024 /* һ�λ�ȡepoll�¼����� */

#define FORWARD_SESSION_TYPE_UNUSED		0 /* δʹ�� */
#define FORWARD_SESSION_TYPE_LISTEN		1 /* �����Ự */
#define FORWARD_SESSION_TYPE_CLIENT		2 /* ��Ϊ�ͻ�������Զ�˷���˻Ự */
#define FORWARD_SESSION_TYPE_SERVER		3 /* ��Ϊ����˽���Զ�����ӻỰ */
#define FORWARD_SESSION_TYPE_MANAGER		4 /* ��Ϊ��������˽���Զ�����ӻỰ */

#define FORWARD_SESSION_STATUS_UNUSED		0 /* δʹ�� */
#define FORWARD_SESSION_STATUS_READY		1 /* ׼��ʹ�� */
#define FORWARD_SESSION_STATUS_LISTEN		2 /* ������ */
#define FORWARD_SESSION_STATUS_CONNECTING	3 /* �Ƕ��������� */
#define FORWARD_SESSION_STATUS_CONNECTED	4 /* ������� */
#define FORWARD_SESSION_STATUS_COMMAND		5 /* ���� */

#define IO_BUFFER_SIZE				4096 /* ���������������С */

#define DEFAULT_MORATORIUM_SECONDS		60 /* ȱʡ�ݽ�ʱ�䣬��λ���� */
#define DEFAULT_TIMEOUT_SECONDS			60 /* ȱʡ��ʱʱ�䣬��λ���� */

#define G6_LISTEN_SOCKFDS			"G6_LISTEN_SOCKFDS"

/* IP-CONNECTIONͳ����Ϣ�ṹ */
struct IpConnection
{
	char		used_flag ; /* ʹ�ñ�־ */
	uint32_t	ip_int ; /* IP��ַ */
	unsigned int	connection_count ; /* �������� */
} ;

#define DEFAULT_IP_CONNECTION_ARRAY_SIZE	2

struct IpConnectionStat
{
	unsigned int			max_ip ; /* ���IP���� */
	unsigned int			max_connections ; /* ���CONNECTION���� */
	unsigned int			max_connections_per_ip ; /* ÿ��IP����������� */
	struct IpConnection		*ip_connection_array ; /* IP-CONNECTIONͳ������ */
	unsigned int			ip_count ; /* ��ǰIP���� */
	unsigned int			connection_count ; /* ��ǰCONNECTION���� */
	unsigned int			ip_connection_stat_size ; /* ���IP-CONNECTION���� */
} ;

/* �����ַ��Ϣ�ṹ */
struct NetAddress
{
	char			ip[ IP_MAXLEN + 1 ] ; /* ip��ַ */
	union
	{
		char		port_str[ PORT_MAXLEN + 1 ] ;
		unsigned int	port_int ;
	} port ; /* �˿� */
	struct sockaddr_in	sockaddr ; /* sock��ַ�ṹ */
} ;

/* �ͻ�����Ϣ�ṹ */
struct ClientNetAddress
{
	struct NetAddress	netaddr ; /* �����ַ�ṹ */
	
	struct IpConnectionStat	ip_connection_stat ; /* IP-CONNECTIONͳ����Ϣ��Ҳ������������ */
} ;

/* ת������Ϣ�ṹ */
struct ForwardNetAddress
{
	struct NetAddress	netaddr ; /* �����ַ�ṹ */
	int			sock ; /* sock������ */
	
	unsigned int		timeout ; /* ��ʱ���� */
} ;

/* �������Ϣ�ṹ */
#define SERVER_CONNECTION_COUNT_INCREASE(_penv_,_server_addr_,_d_) \
	pthread_mutex_lock( & ((_penv_)->server_connection_count_mutex) ); \
	(_server_addr_).ip_connection_stat.connection_count+=(_d_); \
	pthread_mutex_unlock( & ((_penv_)->server_connection_count_mutex) );

#define SERVER_CONNECTION_COUNT_DECREASE(_penv_,_server_addr_,_d_) \
	pthread_mutex_lock( & ((_penv_)->server_connection_count_mutex) ); \
	(_server_addr_).ip_connection_stat.connection_count-=(_d_); \
	pthread_mutex_unlock( & ((_penv_)->server_connection_count_mutex) );

struct ServerNetAddress
{
	struct NetAddress	netaddr ; /* �����ַ�ṹ */
	
	unsigned int		heartbeat ; /* �������� */
	unsigned char		server_unable ; /* ����˿��ñ�־ */
	time_t			timestamp_to ; /* �ָ�������� */
	
	time_t			rtt ; /* �����ʱ��� */
	time_t			wtt ; /* ���дʱ��� */
	
	struct IpConnectionStat	ip_connection_stat ; /* IP-CONNECTIONͳ����Ϣ��Ҳ������������ */
} ;

/* ת������ṹ */
struct ForwardRule
{
	char				rule_id[ RULE_ID_MAXLEN + 1 ] ; /* ����ID */
	char				load_balance_algorithm[ LOAD_BALANCE_ALGORITHM_MAXLEN + 1 ] ; /* ���ؾ����㷨 */
	
	struct IpConnectionStat		client_ip_connection_stat ; /* IP-CONNECTIONͳ����Ϣ��Ҳ������������ */
	
	struct ClientNetAddress		*client_addr_array ; /* �ͻ��˵�ַ�ṹ */
	unsigned int			client_addr_size ; /* �ͻ��˹�������������� */
	unsigned int			client_addr_count ; /* �ͻ��˹����������� */
	
	unsigned int			forward_timeout ; /* ��ʱ���� */
	
	struct ForwardNetAddress	*forward_addr_array ; /* ת���˵�ַ�ṹ */
	unsigned int			forward_addr_size ; /* ת���˹�������������� */
	unsigned int			forward_addr_count ; /* ת���˹����������� */
	
	unsigned int			server_heartbeat ; /* �������� */
	struct IpConnectionStat		server_ip_connection_stat ; /* IP-CONNECTIONͳ����Ϣ��Ҳ������������ */
	
	struct ServerNetAddress		*server_addr_array ; /* ����˵�ַ�ṹ */
	unsigned int			server_addr_size ; /* ����˹�������������� */
	unsigned int			server_addr_count ; /* ����˹����������� */
	unsigned int			selected_addr_index ; /* ��ǰ��������� */
} ;

/* ת���Ự�ṹ */
struct ForwardSession
{
	unsigned char			status ; /* �Ự״̬ */
	unsigned char			type ; /* �����ˡ��ͻ��ˡ�����ˡ���������� */
	
	int				sock ; /* sock������ */
	struct NetAddress		netaddr ; /* �����ַ�ṹ */
	struct ForwardRule		*p_forward_rule ; /* ת������ */
	unsigned int			client_index ; /* �ͻ������� */
	unsigned int			forward_index ; /* ת�������� */
	unsigned int			server_index ; /* ��������� */
	
	char				io_buffer[ IO_BUFFER_SIZE + 1 ] ; /* ������������� */
	int				io_buffer_len ; /* ���������������Ч���ݳ��� */
	int				io_buffer_offsetpos ; /* ���������������Ч���ݿ�ʼƫ���� */
	
	struct ForwardSession		*p_reverse_forward_session ; /* ����Ự */
	
	struct rb_node			timeout_rbnode ; /* ��ʱ ������ڵ� */
	int				timeout_timestamp ; /* ��ʱʱ��� */
} ;

/* �����в��� */
struct CommandParameter
{
	char				*config_pathfilename ; /* -f ... */
	unsigned int			forward_thread_size ; /* -t ... */
	unsigned int			forward_session_size ; /* -s ... */
	int				log_level ; /* --log-level (DEBUG|INFO|WARN|ERROR|FATAL)*/
	int				no_daemon_flag ; /* --no-daemon */
	int				close_log_flag ; /* --close-log */
} ;

/* �����������ṹ */
extern struct ServerEnv			*g_penv ;
extern int				g_exit_flag ;

struct PipeFds
{
	int				fds[ 2 ] ;
} ;

#define INIT_TIME \
	UpdateTimeNow( & g_time_tv , g_date_and_time ); \

#define UPDATE_TIME \
	pthread_mutex_lock( & (penv->time_cache_mutex) ); \
	g_time_tv.tv_sec = penv->time_tv.tv_sec ; \
	memcpy( g_date_and_time , penv->date_and_time , sizeof(penv->date_and_time) ); \
	pthread_mutex_unlock( & (penv->time_cache_mutex) );

struct ServerEnv
{
	struct timeval			time_tv ;
	char				date_and_time[ sizeof(g_date_and_time) ] ;
	
	struct CommandParameter		cmd_para ; /* �����в����ṹ */
	char				**argv ;
	
	struct ForwardNetAddress	*old_forward_addr_array ; /* ����ƽ���������Ͻ��������˿���Ϣ */
	unsigned int			old_forward_addr_count ;
	
	unsigned int			moratorium ; /* �ݽ�ʱ�� */
	unsigned int			timeout ; /* ��ʱ���� */
	struct rb_root			timeout_rbtree ; /* ��ʱ ����� */
	unsigned int			heartbeat ; /* �������� */
	struct IpConnectionStat		ip_connection_stat ; /* IP-CONNECTIONͳ����Ϣ��Ҳ������������ */
	
	struct ForwardRule		*forward_rule_array ; /* ת���������� */
	unsigned int			forward_rule_size ; /* ת�����������С */
	unsigned int			forward_rule_count ; /* ת��������װ������ */
	
	struct ForwardSession		*forward_session_array ; /* ͨѶ�Ự���� */
	unsigned int			forward_session_count ; /* ͨѶ�Ự�����С */
	unsigned int			forward_session_use_offsetpos ; /* ת���Ự���ʹ�õ�Ԫƫ���� */
	
	pid_t				pid ; /* ��������PID */
	struct PipeFds			accept_request_pipe ; /* ����������������ܵ� */
	struct PipeFds			accept_response_pipe ; /* ����������Ӧ����ܵ� */
	int				accept_epoll_fd ; /* �����˿�epoll�� */
	
	pthread_t			time_thread_tid ; /* ʱ������߳� */
	pthread_t			*forward_thread_tid_array ; /* �ַ��߳�TID */
	struct PipeFds			*forward_request_pipe ; /* �ַ��߳���������ܵ� */
	struct PipeFds			*forward_response_pipe ; /* �ַ��߳���Ӧ����ܵ� */
	int				*forward_epoll_fd_array ; /* �����շ�epoll�� */
	
	pthread_mutex_t			ip_connection_stat_mutex ; /* IP-CONNECTIONͳ�� �ٽ������� */
	pthread_mutex_t			forward_session_count_mutex ; /* ͨѶ�Ự���� �ٽ������� */
	pthread_mutex_t			server_connection_count_mutex ; /* ������������� �ٽ������� */
	pthread_mutex_t			timeout_rbtree_mutex ; /* ��ʱ����� �ٽ������� */
	pthread_mutex_t			time_cache_mutex ; /* ��ʱ����� �ٽ������� */
} ;

/********* util *********/

int Rand( int min, int max );
unsigned long CalcHash( char *str );
void SetReuseAddr( int sock );
void SetNonBlocking( int sock );
void SetNagleClosed( int sock );
void SetCloseExec( int sock );
void SetCloseExec2( int sock , int sock2 );
void SetCloseExec3( int sock , int sock2 , int sock3 );
void SetCloseExec4( int sock , int sock2 , int sock3 , int sock4 );
void SetNetAddress( struct NetAddress *p_netaddr );
void GetNetAddress( struct NetAddress *p_netaddr );
int BindDaemonServer( char *pcServerName , int (* ServerMain)( void *pv ) , void *pv , int (* ControlMain)(long lControlStatus) );
int IsMatchString(char *pcMatchString, char *pcObjectString, char cMatchMuchCharacters, char cMatchOneCharacters);

/********* Envirment *********/

int InitEnvirment( struct ServerEnv *penv );
int InitEnvirment2( struct ServerEnv *penv );
void CleanEnvirment( struct ServerEnv *penv );

int SaveListenSockets( struct ServerEnv *penv );
int LoadOldListenSockets( struct ServerEnv *penv );
int CleanOldListenSockets( struct ServerEnv *penv );

int AddListeners( struct ServerEnv *penv );

struct ForwardSession *GetForwardSessionUnused( struct ServerEnv *penv );
#define IsForwardSessionUsed(_p_forward_session_)	((_p_forward_session_)->status!=FORWARD_SESSION_STATUS_UNUSED?1:0)
void SetForwardSessionUnused( struct ServerEnv *penv , struct ForwardSession *p_forward_session );
void SetForwardSessionUnused2( struct ServerEnv *penv , struct ForwardSession *p_forward_session , struct ForwardSession *p_forward_session2 );

void RemoveTimeoutTreeNode( struct ServerEnv *penv , struct ForwardSession *p_forward_session );
void RemoveTimeoutTreeNode2( struct ServerEnv *penv , struct ForwardSession *p_forward_session , struct ForwardSession *p_forward_session2 );
int AddTimeoutTreeNode( struct ServerEnv *penv , struct ForwardSession *p_forward_session , unsigned int timeout_timestamp );
int AddTimeoutTreeNode2( struct ServerEnv *penv , struct ForwardSession *p_forward_session , struct ForwardSession *p_forward_session2 , unsigned int timeout_timestamp );
int UpdateTimeoutNode( struct ServerEnv *penv , struct ForwardSession *p_forward_session , unsigned int timeout_timestamp );
int UpdateTimeoutNode2( struct ServerEnv *penv , struct ForwardSession *p_forward_session , struct ForwardSession *p_forward_session2 , unsigned int timeout_timestamp );
int GetLastestTimeout( struct ServerEnv *penv );
struct ForwardSession *GetExpireTimeoutNode( struct ServerEnv *penv );

int InitIpConnectionStat( struct IpConnectionStat *p_ip_connection_stat );
void CleanIpConnectionStat( struct IpConnectionStat *p_ip_connection_stat );
int AddIpConnectionStat( struct ServerEnv *penv , struct IpConnectionStat *p_ip_connection_stat , uint32_t ip_int );
int RemoveIpConnectionStat( struct ServerEnv *penv , struct IpConnectionStat *p_ip_connection_stat , uint32_t ip_int );

/********* Config *********/

int LoadConfig( struct ServerEnv *penv );
void UnloadConfig( struct ServerEnv *penv );

/********* MonitorProcess *********/

int MonitorProcess( struct ServerEnv *penv );
int _MonitorProcess( void *pv );

/********* WorkerProcess *********/

int WorkerProcess( struct ServerEnv *penv );

/********* AcceptThread *********/

void *AcceptThread( struct ServerEnv *penv );
void *_AcceptThread( void *pv );

/********* ForwardThread *********/

void *ForwardThread( unsigned long forward_thread_index );
void *_ForwardThread( void *pv );

/********* TimeThread *********/

void UpdateTimeNow( struct timeval *p_time_tv , char *p_date_and_time );
void *TimeThread();
void *_TimeThread( void *pv );

#endif
