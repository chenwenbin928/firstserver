#include"signalex.h"
#include"msgprocess.h"  
/*信号组的静态初始化*/
struct    signal_value   signals[]=
{
	{SIGINT,"SIGINT",signal_happen_handler},
	{SIGQUIT,"SIGQUIT",signal_happen_handler},
	{SIGCHLD,"SIGCHID",signal_happen_handler},
	{COMM_RESTART,"COMM_RESTART",signal_happen_handler},
	{COMM_DISPLAY_SERVER,"COMM_DISPLAY_SERVER",signal_happen_handler},
};

/*初始化绑定master进程的信号*/
int   signal_set_init(struct  sigaction  *act)
{
	struct     signal_value    *signum=NULL;
	for(signum=signals;signum->signo!=0;signum++)
	{
		act->sa_handler=signal_happen_handler;
		act->sa_flags|=SA_RESTART;
		sigemptyset(&act->sa_mask);
		if(sigaction(signum->signo,act,NULL)<0)
		{
			return   -1;
		}
	}
	return  1;
}


/*把绑定的信号和命令添加至当前信号集中*/
int   add_signal_to_master_process_set( sigset_t  *set)
{   
	sigemptyset(set);
	//添加信号初始化的信号到当前进程;
	if(sigaddset(set,SIGINT)<0)
	{
	   return  -1;
	}
	if(sigaddset(set,SIGQUIT)<0)
	{
	   return  -1;
	}
	if(sigaddset(set,SIGCHLD)<0)
	{
	   return  -1;
	}
	if(sigaddset(set,COMM_RESTART)<0)
	{
	   return  -1;
	}
	if(sigaddset(set,COMM_DISPLAY_SERVER)<0)
	{
		return -1;
	}
	if(sigprocmask(SIG_BLOCK,&server->set,NULL)<0)
	{
		return  -1;
	}
	sigemptyset(&server->set);//初始化下;
	return  1;
}



