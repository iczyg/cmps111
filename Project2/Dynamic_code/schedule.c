#include "sched.h"
#include "schedproc.h"
#include <assert.h>
#include <minix/com.h>
#include <machine/archtypes.h>
#include "kernel/proc.h" 

PRIVATE timer_t sched_timer;
PRIVATE unsigned balance_timeout;


PRIVATE struct schedproc *temp;
PRIVATE int tic_num = 0;
  

#define BALANCE_TIMEOUT	5 /* how often to balance queues in seconds */
#define WIN_Q  14
#define LOS_Q 15
#define PAST_WINNER   13/*OLD winner that didnt execute*/

FORWARD _PROTOTYPE(int schedule_process, (struct schedproc * rmp));
FORWARD _PROTOTYPE(void balance_queues, (struct timer *tp));

#define DEFAULT_USER_TIME_SLICE 200

/*===========================================================================*
 *				play_lottery				     *
 *===========================================================================*/
PRIVATE void play_lottery(void)
{
	struct schedproc *rmp;
	int winning_ticket; 
	int proc_nr;
	int rv;
	int sum=0;
	/*Run a for loop to find the total number of tickets.*/
	for (proc_nr=0, rmp=schedproc; proc_nr < NR_PROCS; proc_nr++, rmp++) {
		if (rmp->flags & IN_USE && PROCESS_IN_USER_Q(rmp)) {
			sum += rmp->tickets;
		}
	}
    	winning_ticket = rand() % tic_num;
	printf("Playing lottery. winning_ticket: %d\n", winning_ticket);
	sum=0;
/*------------------------------------------------------------------------------*/
	/*Determine the winner process. Change the winner priority to Winner
	Queue, Also save the winner queue to temp -type schedproc.*/
	for(proc_nr = 0, rmp = schedproc; proc_nr < NR_PROCS; proc_nr++, rmp++){
		if(rmp->flags & IN_USE && PROCESS_IN_USER_Q(rmp)) {
			if((winning_ticket-=rmp->tickets) < 0){
				rmp->priority=WIN_Q;
				temp = rmp; 
				schedule_process(rmp);
				break;
			}
		}
	}
}
 
/*===========================================================================*
 *				do_noquantum				     *
 *===========================================================================*/

PUBLIC int do_noquantum(message *m_ptr)
{
	register struct schedproc *rmp;
	int rv, winner,proc_nr_n;
  int loop;
    
	if (sched_isokendpt(m_ptr->m_source, &proc_nr_n) != OK) {
		printf("SCHED: WARNING: got an invalid endpoint in OOQ msg %u.\n",
		m_ptr->m_source);
		return EBADEPT;
	}
    
	rmp = &schedproc[proc_nr_n];

  /* modified */
  if (rmp->priority == WIN_Q ){
      rmp->priority = LOS_Q; /* lower priority */
  	  schedule_process(rmp);
      if(rmp->tickets > 1){
      	rmp->tickets -=1; /*decrease number of tickets*/
      	printf("Winning ticket used up quantum. Ticket count decreased from %d to %d", rmp->tickets -1, rmp->tickets);
      }
  }else if (rmp->priority == LOS_Q) {/*IO process since it blocked)*/
    temp->priority = LOS_Q;
    schedule_process(temp);
    if(temp->tickets < 100){/*max tickets*/
      temp->tickets += 1;
      printf("Winning ticket blocks. Ticket count increased from %d to %d on ticket: %d", temp->tickets -1, temp->tickets,temp->endpoint);
    }
  }
   play_lottery();
	if ((rv = schedule_process(rmp)) != OK) {
		return rv;
	}
	return OK;
}

/*===========================================================================*
 *				do_stop_scheduling			     *
 *===========================================================================*/
PUBLIC int do_stop_scheduling(message *m_ptr)
{
	register struct schedproc *rmp;
	int rv, proc_nr_n;

	/* check who can send you requests */
	if (!accept_message(m_ptr))
		return EPERM;

	if (sched_isokendpt(m_ptr->SCHEDULING_ENDPOINT, &proc_nr_n) != OK) {
		printf("SCHED: WARNING: got an invalid endpoint in OOQ msg "
		"%ld\n", m_ptr->SCHEDULING_ENDPOINT);
		return EBADEPT;
	}
	rmp = &schedproc[proc_nr_n];
	rmp->flags = 0; /*&= ~IN_USE;*/
	tic_num -= rmp->tickets;
	return OK;
}

/*===========================================================================*
 *				do_start_scheduling			     *
 *===========================================================================*/
PUBLIC int do_start_scheduling(message *m_ptr)
{
	register struct schedproc *rmp;
	int rv, proc_nr_n, parent_nr_n, nice;
	
	/* we can handle two kinds of messages here */
	assert(m_ptr->m_type == SCHEDULING_START || 
		m_ptr->m_type == SCHEDULING_INHERIT);

	/* check who can send you requests */
	if (!accept_message(m_ptr))
		return EPERM;

	/* Resolve endpoint to proc slot. */
	if ((rv = sched_isemtyendpt(m_ptr->SCHEDULING_ENDPOINT, &proc_nr_n))
			!= OK) {
		return rv;
	}
	rmp = &schedproc[proc_nr_n];

	/* Populate process slot */
	rmp->endpoint     = m_ptr->SCHEDULING_ENDPOINT;
	rmp->parent       = m_ptr->SCHEDULING_PARENT;
	rmp->max_priority = (unsigned) m_ptr->SCHEDULING_MAXPRIO;
	if (rmp->max_priority >= NR_SCHED_QUEUES) {
		return EINVAL;
	}
	
	switch (m_ptr->m_type) {

	case SCHEDULING_START:
		/* We have a special case here for system processes, for which
		 * quantum and priority are set explicitly rather than inherited 
		 * from the parent */
            
     
		rmp->priority = LOS_Q;
		rmp->tickets = 20;
        tic_num += rmp->tickets; 
  
		rmp->time_slice = (unsigned) m_ptr->SCHEDULING_QUANTUM;
     
		break;
		
	case SCHEDULING_INHERIT:
		/* Inherit current priority and time slice from parent. Since there
		 * is currently only one scheduler scheduling the whole system, this
		 * value is local and we assert that the parent endpoint is valid */
		if ((rv = sched_isokendpt(m_ptr->SCHEDULING_PARENT,
				&parent_nr_n)) != OK)
			return rv;
		rmp->priority = LOS_Q;
		rmp->time_slice = schedproc[parent_nr_n].time_slice;
    	rmp->tickets = 20; /*basic lottery scheduling*/
        tic_num += rmp->tickets; 
    	break;
		
	default: 
		/* not reachable */
		assert(0);
	}

	/* Take over scheduling the process. The kernel reply message populates
	 * the processes current priority and its time slice */
	if ((rv = sys_schedctl(0, rmp->endpoint, 0, 0)) != OK) {
		printf("Sched: Error taking over scheduling for %d, kernel said %d\n",
			rmp->endpoint, rv);
		return rv;
	}
	rmp->flags = IN_USE;

	/* Schedule the process, giving it some quantum */
    /*printf("Scheduling new process in do_start\n");*/
	if ((rv = schedule_process(rmp)) != OK) {
		printf("Sched: Error while scheduling process, kernel replied %d\n",
			rv);
		return rv;
	}

	/* Mark ourselves as the new scheduler.
	 * By default, processes are scheduled by the parents scheduler. In case
	 * this scheduler would want to delegate scheduling to another
	 * scheduler, it could do so and then write the endpoint of that
	 * scheduler into SCHEDULING_SCHEDULER
	 */

	m_ptr->SCHEDULING_SCHEDULER = SCHED_PROC_NR;

	return OK;
}

/*===========================================================================*
 *				do_nice					     *
 *===========================================================================*/
PUBLIC int do_nice(message *m_ptr)
{
	struct schedproc *rmp;
	int rv;
	int proc_nr_n;
	unsigned old_q, old_max_q, old_tickets;

	/* check who can send you requests */
	if (!accept_message(m_ptr))
		return EPERM;

	if (sched_isokendpt(m_ptr->SCHEDULING_ENDPOINT, &proc_nr_n) != OK) {
		printf("SCHED: WARNING: got an invalid endpoint in OOQ msg "
		"%ld\n", m_ptr->SCHEDULING_ENDPOINT);
		return EBADEPT;
	}

	rmp = &schedproc[proc_nr_n];

	if (((unsigned) m_ptr->SCHEDULING_MAXPRIO + 20) > 100){
        rmp->tickets = 100;
    }else {
        rmp->tickets += (unsigned) m_ptr->SCHEDULING_MAXPRIO;
  }

	/* Store old values, in case we need to roll back the changes */
	old_q       = rmp->priority;
	old_max_q   = rmp->max_priority;
	/* Update the proc entry and reschedule the process */
	rmp->max_priority = rmp->priority;
    
    
    
	if ((rv = schedule_process(rmp)) != OK) {
		/* Something went wrong when rescheduling the process, roll
		 * back the changes to proc struct */
		rmp->priority     = old_q;
		rmp->max_priority = old_max_q;
	}

	return rv;
}

/*===========================================================================*
 *				schedule_process			     *
 *===========================================================================*/
PRIVATE int schedule_process(struct schedproc * rmp)
{
	int rv;

	if ((rv = sys_schedule(rmp->endpoint, rmp->priority,
			rmp->time_slice)) != OK) {
		if(rmp->endpoint != 0){
		  printf("SCHED: An error occurred when trying to schedule %d: %d\n",
		  rmp->endpoint, rv);
	  }
	}

	return rv;
}


/*===========================================================================*
 *				start_scheduling			     *
 *===========================================================================*/

PUBLIC void init_scheduling(void)
{
	balance_timeout = BALANCE_TIMEOUT * sys_hz();
	init_timer(&sched_timer);
	set_timer(&sched_timer, balance_timeout, balance_queues, 0);
}

/*===========================================================================*
 *				balance_queues				     *
 *===========================================================================*/

/* This function in called every 100 ticks to rebalance the queues. The current
 * scheduler bumps processes down one priority when ever they run out of
 * quantum. This function will find all proccesses that have been bumped down,
 * and pulls them back up. This default policy will soon be changed.
 */
PRIVATE void balance_queues(struct timer *tp)
{
	struct schedproc *rmp;
	int proc_nr;
	int rv;
	set_timer(&sched_timer, balance_timeout, balance_queues, 0);
}
