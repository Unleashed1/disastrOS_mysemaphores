#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
  //First of all take the fd of the semDescriptor that we wants to close(check the list)
  
  
  int myfd = running->syscall_args[0];
  
  SemDescriptor* sem_desc = SemDescriptorList_byFd(&running->sem_descriptors,myfd);
  //Need to be sure 
  if(!sem_desc){
	  running->syscall_retvalue = DSOS_ESEMCLOSE_SEMDESC_IS_NOT_IN_PROCESS;
	  return;
  }
  
  //Now remove it from the list, remove the ptr fom the list of descriptors && free resources

  List_detach(&running->sem_descriptors, (ListItem*)sem_desc);
  Semaphore* sem = sem_desc->semaphore;
  assert(sem);
  SemDescriptorPtr* sem_desc_ptr = (SemDescriptorPtr*)List_detach(&sem->descriptors, (ListItem*)(sem_desc->ptr));
  assert(sem_desc_ptr);
  SemDescriptorPtr_free(sem_desc_ptr);
  SemDescriptor_free(sem_desc);
  
  //Before return the syscall success(0),check if there are another process that could be on sem
  if(sem->descriptors.size==0) {
	  //cast
	  sem = (Semaphore*)List_detach(&semaphores_list,(ListItem*)sem);
	  assert(sem);
	  Semaphore_free(sem);
  }
  running->syscall_retvalue = 0;
}
