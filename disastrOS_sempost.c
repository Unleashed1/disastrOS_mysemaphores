#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "disastrOS_globals.h"


void internal_semPost(){int fd = running->syscall_args[0];
	SemDescriptor* sem_desc = SemDescriptorList_byFd(&running->sem_descriptors, fd);
	if(!sem_desc){
		running->syscall_retvalue = DSOS_ESEMPOST_SEMDESC_IS_NOT_IN_PROCESS;
		return;
	}
	//while in the sem_wait decrease in the post increase the counter and when its vallue is
	//<=0 then change the state of the first in the sem waiting list can go to the ready state 
	Semaphore* sem = sem_desc->semaphore;
	assert(sem);
	sem->count++;
	
	if(sem->count <= 0 ) {
		SemDescriptorPtr* sem_desc_ptr = (SemDescriptorPtr*)List_detach(&sem->waiting_descriptors, sem->waiting_descriptors.first);
		PCB* pcb = sem_desc_ptr->descriptor->pcb;
		SemDescriptorPtr_free(sem_desc_ptr);
		pcb->status = Ready;
		List_detach(&waiting_list, (ListItem*)pcb);
		List_insert(&ready_list, ready_list.last, (ListItem*)pcb);
	}
	running->syscall_retvalue = 0 ;
}
