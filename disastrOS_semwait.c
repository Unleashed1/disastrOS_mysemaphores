#include <assert.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "disastrOS_globals.h"

void internal_semWait(){
	//as usual we take the fd form the pcb  
	int fd = running->syscall_args[0];
	SemDescriptor* sem_desc = SemDescriptorList_byFd(&running->sem_descriptors,fd);
	if(!sem_desc){
running->syscall_retvalue = DSOS_ESEMWAIT_SEMDESC_IS_NOT_IN_PROCESS;
		return ;
	}
	//when we invoke this func the sem got to wait untill the counter become <0  then insert running in waiting list and 
	//change the running process 
	Semaphore* sem = sem_desc->semaphore;
	assert(sem);
	sem->count--; 
	// counter decreased now insert in waiting list 
	PCB* tmp = running;
	if(sem->count<0){
		SemDescriptorPtr* sem_desc_ptr = SemDescriptorPtr_alloc(sem_desc);
		assert(sem_desc_ptr);
		List_insert(&sem->waiting_descriptors, sem->waiting_descriptors.last, (ListItem*)sem_desc_ptr);
		//change the running status 
		running->status = Waiting; 
		List_insert(&waiting_list, waiting_list.last, (ListItem*)running);
		running = (PCB*)List_detach(&ready_list,ready_list.first);
		running->status = Running ; 
	}
    tmp->syscall_retvalue = 0;
}
