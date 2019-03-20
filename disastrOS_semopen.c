#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "disastrOS_globals.h"
#include "disastrOS_constants.h"

void internal_semOpen(){
	
	//Looking for some possible errors 
	//1. Check if the num. of the semaphores reached the MAX_NUM_SEMAPHORES (1024), probably no xp
	if(Semaphore->count > MAX_NUM_SEMAPHORES){
		running->syscall_retvalue = DSOS_ESYSCALL_OUT_OF_RANGE;
		return;
	}
	
	//2. Check if the semaphore is already open  if not we add it in the list (we alloc first obv.)
	Semaphore* sem = SemaphoreList_byId((SemaphoreList*)&semaphores_list, id);
	if(!sem) {
		sem = Semaphore_alloc(id, 1);
		assert(sem);
		List_insert(&semaphores_list, semaphores_list.last, (ListItem*)sem);
	}
	//Now alloc the SemDescriptor and add it on fd 
	SemDescriptor* sem_desc = SemDescriptor_alloc(running->last_sem_fd,sem,running);
	assert(sem_desc);
	running->last_sem_fd+=1;
	
	//Then alloc SemDescriptorPtr for sem_desc
	SemDescriptorPtr* sem_d_ptr = SemDescriptorPtr_alloc(sem_desc);
	assert(sem_d_ptr);
	
	sem_desc->ptr = sem_d_ptr;

	//Add sem_desc to the sem_descriptors list && link the ptr && insert sem
	List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*)sem_desc);
	sem_desc->ptr = sem_d_ptr;
	List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*)sem_d_ptr);
  
	//Finally return the value of the syscall
	running->syscall_retvalue = sem_desc->fd;

}
