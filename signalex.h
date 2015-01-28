#ifndef _SIGNALEX_H__
#define _SIGNALEX_H__
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<arpa/inet.h>

struct   signal_value 
{
	int  signo;   //信号值;
    char *signame;//信号名称;
    void (*signal_handler)(int  sig);
};

void   signal_happen_handler(int  sig);
int   signal_set_init(struct  sigaction  *act);
int   add_signal_to_master_process_set( sigset_t  *set);
#endif

