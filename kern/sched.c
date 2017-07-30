#include <inc/assert.h>

#include <kern/sched.h>
#include <kern/user_environment.h>
#include <kern/memory_manager.h>
#include <kern/command_prompt.h>
#include <kern/trap.h>

//void on_clock_update_WS_time_stamps();
extern uint32 isBufferingEnabled();
extern void cleanup_buffers(struct Env* e);
extern inline uint32 pd_is_table_used(struct Env *e, uint32 virtual_address);
extern inline void pd_set_table_unused(struct Env *e, uint32 virtual_address);
extern inline void pd_clear_page_dir_entry(struct Env *e, uint32 virtual_address);

///Local Vars
//============
struct Env_list env_ready_queue;	// queue of all running envs
struct Env_list env_new_queue;	// queue of all new envs

//2015:
struct Env_list env_exit_queue;	// queue of all exited envs
//===============

struct Env* sched_next = NULL;

struct Env* sched_next_circular(struct Env* env);

void
fos_scheduler(void)
{
	// Implement simple round-robin scheduling.
	// Pick next environment in ready queue,
	// in circular fashion starting after the previously running env,
	// and switch to the first such environment found.
	// It's OK to choose the previously running env if no other env
	// is runnable.

	scheduler_status = SCH_STARTED;
	struct Env* current_env = NULL;

	{
		//we have a runnable env, so update the sched pointer and run it
		current_env = sched_next;
		sched_next = sched_next_circular(sched_next);
	}

	//cprintf("Scheduler\n");
	if(current_env != NULL)
	{
	//	if (strcmp(current_env->prog_name, "midterm") != 0)
		{
			//cprintf("sched: %s, next = %s\n",current_env->prog_name, sched_next->prog_name);
//			cprintf("ready queue: ");
//			struct Env * ptr_env;
//			LIST_FOREACH(ptr_env, &env_ready_queue)
//			{
//				cprintf("%s	", ptr_env->prog_name);
//			}
//			cprintf("\n");
		}
		env_run(current_env);
	} // Run the special idle environment when nothing else is runnable.
	else //if (envs[0].env_status == ENV_RUNNABLE)
		//	env_run(&envs[0]);
		//else
	{
		/*2015*///No more envs... curenv doesn't exist any more! return back to command prompt
		curenv = NULL;
		//lcr3(K_PHYSICAL_ADDRESS(ptr_page_directory));
		lcr3(phys_page_directory);

		//cprintf("SP = %x\n", read_esp());

		scheduler_status = SCH_STOPPED;
		//cprintf("[sched] no envs - nothing more to do!\n");
		while (1)
			run_command_prompt(NULL);

	}
}

/*
void
fos_scheduler(void)
{
	// Implement simple round-robin scheduling.
	// Pick next environment in ready queue,
	// in circular fashion starting after the previously running env,
	// and switch to the first such environment found.
	// It's OK to choose the previously running env if no other env
	// is runnable.

	uint32 env_found = 0;
	struct Env* current_env = NULL;

	if (isRunSingleTask())
	{
		if(curenv)
		{
			env_found  =1 ;
			current_env = curenv ;
		}
	}
	if (isRunMultiTask())
	{
		if (current_env == NULL)
			cprintf("current_env = NULL\n") ;
		else
		{
			cprintf("current_env = %s\n", current_env->prog_name) ;
		}

		if(current_env == NULL)
		{
			current_env = LIST_FIRST(&env_ready_queue);
		}
		else
		{
			current_env = LIST_NEXT(current_env);
		}



		while(!env_found)
		{
			if (current_env == NULL)
				cprintf("current_env = NULL\n") ;
			else
				cprintf("current_env = %s\n", current_env->prog_name) ;

			if(current_env == sched_last_run_env)
			{
				if(current_env != NULL && current_env->env_status==ENV_READY)
				{
					env_found=1;
				}
				break;
			}

			if(current_env != NULL)
			{
				if(current_env->env_status == ENV_READY)
				{
					env_found = 1;
					break;
				}
				current_env = LIST_NEXT(current_env);
			}
			else
			{
				current_env = LIST_FIRST(&env_ready_queue);
			}

		}

		//we have a runnable env, so update the sched pointer and run it



	}
	//cprintf("Scheduler\n");
	if(env_found)
	{
		sched_last_run_env = current_env;
		cprintf("sched: %s\n",current_env->prog_name);
		env_run(current_env);
	} // Run the special idle environment when nothing else is runnable.
	else //if (envs[0].env_status == ENV_RUNNABLE)
		//	env_run(&envs[0]);
		//else
	{
		//cprintf("Destroyed all environments - nothing more to do!\n");
		while (1)
			run_command_prompt(NULL);

	}
}
 */

void sched_init()
{
	old_pf_counter = 0;
	LIST_INIT(&env_ready_queue);
	LIST_INIT(&env_new_queue);
	LIST_INIT(&env_exit_queue);
	sched_next = NULL;
	scheduler_status = SCH_STOPPED;
}


struct Env* sched_next_circular(struct Env* env)
{
	if(env == NULL) return NULL;

	struct Env* next = LIST_NEXT(env);
	if(next == NULL) next = LIST_FIRST(&env_ready_queue);

	return next;
}

void sched_insert_ready(struct Env* env)
{
	if(env != NULL)
	{
		env->env_status = ENV_READY ;
		if(sched_next != NULL)
		{
			LIST_INSERT_BEFORE(&env_ready_queue, sched_next, env);
		}
		else
		{
			LIST_INSERT_TAIL(&env_ready_queue, env);
			sched_next = env;
		}
	}
}

void sched_remove_ready(struct Env* env)
{
	if(env != NULL)
	{
		if(env == sched_next)
		{
			sched_next = sched_next_circular(env);
			if(sched_next == env)
			{
				sched_next = NULL;
			}
		}
		LIST_REMOVE(&env_ready_queue, env);
		env->env_status = ENV_UNKNOWN;
	}
}

void sched_insert_new(struct Env* env)
{
	if(env != NULL)
	{
		env->env_status = ENV_NEW ;
		LIST_INSERT_TAIL(&env_new_queue, env);
	}
}
void sched_remove_new(struct Env* env)
{
	if(env != NULL)
	{
		LIST_REMOVE(&env_new_queue, env) ;
		env->env_status = ENV_UNKNOWN;
	}
}

void sched_insert_exit(struct Env* env)
{
	if(env != NULL)
	{
		env->env_status = ENV_EXIT ;
		LIST_INSERT_TAIL(&env_exit_queue, env);
	}
}
void sched_remove_exit(struct Env* env)
{
	if(env != NULL)
	{
		LIST_REMOVE(&env_exit_queue, env) ;
		env->env_status = ENV_UNKNOWN;
	}
}

void sched_print_all()
{
	struct Env* ptr_env ;
	if (!LIST_EMPTY(&env_new_queue))
	{
		cprintf("\nThe processes in NEW queue are:\n");
		LIST_FOREACH(ptr_env, &env_new_queue)
		{
			cprintf("	[%d] %s\n", ptr_env->env_id, ptr_env->prog_name);
		}
	}
	else
	{
		cprintf("\nNo processes in NEW queue\n");
	}
	cprintf("================================================\n");
	if (!LIST_EMPTY(&env_ready_queue))
	{
		cprintf("The processes in READY queue are:\n");
		LIST_FOREACH(ptr_env, &env_ready_queue)
		{
			cprintf("	[%d] %s\n", ptr_env->env_id, ptr_env->prog_name);
		}
	}
	else
	{
		cprintf("No processes in READY queue\n");
	}
	cprintf("================================================\n");
	if (!LIST_EMPTY(&env_exit_queue))
	{
		cprintf("The processes in EXIT queue are:\n");
		LIST_FOREACH(ptr_env, &env_exit_queue)
		{
			cprintf("	[%d] %s\n", ptr_env->env_id, ptr_env->prog_name);
		}
	}
	else
	{
		cprintf("No processes in EXIT queue\n");
	}
}

void sched_run_all()
{
	struct Env* ptr_env=NULL;
	LIST_FOREACH(ptr_env, &env_new_queue)
	{
		sched_remove_new(ptr_env);
		sched_insert_ready(ptr_env);
	}
	/*2015*///if scheduler not run yet, then invoke it!
	if (scheduler_status == SCH_STOPPED)
		fos_scheduler();
}

void sched_kill_all()
{
	struct Env* ptr_env ;
	if (!LIST_EMPTY(&env_new_queue))
	{
		cprintf("\nKILLING the processes in the NEW queue...\n");
		LIST_FOREACH(ptr_env, &env_new_queue)
		{
			cprintf("	killing[%d] %s...", ptr_env->env_id, ptr_env->prog_name);
			sched_remove_new(ptr_env);
			start_env_free(ptr_env);
			cprintf("DONE\n");
		}
	}
	else
	{
		cprintf("No processes in NEW queue\n");
	}
	cprintf("================================================\n");
	if (!LIST_EMPTY(&env_ready_queue))
	{
		cprintf("KILLING the processes in the READY queue...\n");
		LIST_FOREACH(ptr_env, &env_ready_queue)
		{
			cprintf("	killing[%d] %s...", ptr_env->env_id, ptr_env->prog_name);
			sched_remove_ready(ptr_env);
			start_env_free(ptr_env);
			cprintf("DONE\n");
		}
	}
	else
	{
		cprintf("No processes in READY queue\n");
	}
	cprintf("================================================\n");
	if (!LIST_EMPTY(&env_exit_queue))
	{
		cprintf("KILLING the processes in the EXIT queue...\n");
		LIST_FOREACH(ptr_env, &env_exit_queue)
		{
			cprintf("	killing[%d] %s...", ptr_env->env_id, ptr_env->prog_name);
			sched_remove_exit(ptr_env);
			start_env_free(ptr_env);
			cprintf("DONE\n");
		}
	}
	else
	{
		cprintf("No processes in EXIT queue\n");
	}

	//reinvoke the scheduler since there're no env to return back to it
	fos_scheduler();
}


void sched_new_env(struct Env* e)
{
	//add the given env to the scheduler NEW queue
	if (e!=NULL)
	{
		sched_insert_new(e);
	}
}
/*
void sched_new_env(uint32 envId)
{
	struct Env* e= NULL;
	envid2env(envId, &e, 0);

	if(e == 0) return;

	sched_insert_new(e);
}
 */
void sched_run_env(uint32 envId)
{
	struct Env* ptr_env=NULL;
	LIST_FOREACH(ptr_env, &env_new_queue)
	{
		if(ptr_env->env_id == envId)
		{
			sched_remove_new(ptr_env);
			sched_insert_ready(ptr_env);

			/*2015*///if scheduler not run yet, then invoke it!
			if (scheduler_status == SCH_STOPPED)
			{
				fos_scheduler();
			}
			break;
		}
	}
//	cprintf("ready queue:\n");
//	LIST_FOREACH(ptr_env, &env_ready_queue)
//	{
//		cprintf("%s - %d\n", ptr_env->prog_name, ptr_env->env_id);
//	}

}

void sched_exit_env(uint32 envId)
{
	struct Env* ptr_env=NULL;
	LIST_FOREACH(ptr_env, &env_new_queue)
	{
		if(ptr_env->env_id == envId)
		{
			sched_remove_new(ptr_env);
			sched_insert_exit(ptr_env);
			if(isBufferingEnabled()) {cleanup_buffers(ptr_env);}
			return;
		}
	}
	ptr_env=NULL;
	LIST_FOREACH(ptr_env, &env_ready_queue)
	{
		if(ptr_env->env_id == envId)
		{
			sched_remove_ready(ptr_env);
			sched_insert_exit(ptr_env);
			if(isBufferingEnabled()) {cleanup_buffers(ptr_env);}

			//If it's the curenv, then reinvoke the scheduler as there's no meaning to return back to an exited env
			if (curenv->env_id == envId)
			{
				fos_scheduler();
			}
			return;
		}
	}
}

/*2015*/
void sched_kill_env(uint32 envId)
{
	struct Env* ptr_env=NULL;
	LIST_FOREACH(ptr_env, &env_new_queue)
	{
		if(ptr_env->env_id == envId)
		{
			cprintf("killing[%d] %s from the NEW queue...", ptr_env->env_id, ptr_env->prog_name);
			sched_remove_new(ptr_env);
			start_env_free(ptr_env);
			cprintf("DONE\n");
			return;
		}
	}
	ptr_env=NULL;
	LIST_FOREACH(ptr_env, &env_ready_queue)
	{
		if(ptr_env->env_id == envId)
		{
			cprintf("killing[%d] %s from the READY queue...", ptr_env->env_id, ptr_env->prog_name);
			sched_remove_ready(ptr_env);
			start_env_free(ptr_env);
			cprintf("DONE\n");

			//If it's the curenv, then reset it and reinvoke the scheduler
			//as there's no meaning to return back to a killed env
			if (curenv->env_id == envId)
			{
				//lcr3(K_PHYSICAL_ADDRESS(ptr_page_directory));
				lcr3(phys_page_directory);
				curenv = NULL;
				fos_scheduler();
			}
			return;
		}
	}
	ptr_env=NULL;
	LIST_FOREACH(ptr_env, &env_exit_queue)
	{
		if(ptr_env->env_id == envId)
		{
			cprintf("killing[%d] %s from the EXIT queue...", ptr_env->env_id, ptr_env->prog_name);
			sched_remove_exit(ptr_env);
			start_env_free(ptr_env);
			cprintf("DONE\n");
			return;
		}
	}
}


void clock_interrupt_handler()
{
	//cputchar('i');

	if(isPageReplacmentAlgorithmLRU())
	{
		update_WS_time_stamps();
	}
	//cprintf("Clock Handler\n") ;
	fos_scheduler();
}
/*
void on_clock_update_WS_time_stamps()
{
	//cprintf("Updating time stamps\n");

	struct Env *curr_env_ptr = NULL;

	LIST_FOREACH(curr_env_ptr, &env_ready_queue)
	{
		int i ;
		for (i = 0 ; i < PAGE_WS_MAX_SIZE; i++)
		{
			if( curr_env_ptr->ptr_pageWorkingSet[i].empty != 1)
			{
				//update the time if the page was referenced
				uint32 page_va = curr_env_ptr->ptr_pageWorkingSet[i].virtual_address ;
				uint32 perm = pt_get_page_permissions(curr_env_ptr, page_va) ;
				if (perm & PERM_USED)
				{
					curr_env_ptr->ptr_pageWorkingSet[i].time_stamp = time ;
					pt_set_page_permissions(curr_env_ptr, page_va, 0 , PERM_USED) ;
				}
			}
		}

		{
			//uint32 eflags_val = read_eflags();
			//write_eflags(eflags_val & (~FL_IF));

			//if((curr_env_ptr->pageFaultsCounter-old_pf_counter) > 0)
			{
				//cprintf("[%s]: last clock # of PFs  = %d, timenow = %d\n", curr_env_ptr->prog_name ,curr_env_ptr->pageFaultsCounter-old_pf_counter, time);
			}
			//mydblchk += curr_env_ptr->pageFaultsCounter-old_pf_counter;
			old_pf_counter = curr_env_ptr->pageFaultsCounter;

			//eflags_val = read_eflags();
			//write_eflags(eflags_val | (FL_IF));
		}
	}

}
 */
void update_WS_time_stamps()
{
	struct Env *curr_env_ptr = curenv;

	if(curr_env_ptr != NULL)
	{
		{
			int i ;
			for (i = 0 ; i < (curr_env_ptr->page_WS_max_size); i++)
			{
				if( curr_env_ptr->ptr_pageWorkingSet[i].empty != 1)
				{
					//update the time if the page was referenced
					uint32 page_va = curr_env_ptr->ptr_pageWorkingSet[i].virtual_address ;
					uint32 perm = pt_get_page_permissions(curr_env_ptr, page_va) ;
					uint32 oldTimeStamp = curr_env_ptr->ptr_pageWorkingSet[i].time_stamp;

					if (perm & PERM_USED)
					{
						curr_env_ptr->ptr_pageWorkingSet[i].time_stamp = (oldTimeStamp>>2) | 0x80000000;
						pt_set_page_permissions(curr_env_ptr, page_va, 0 , PERM_USED) ;
					}
					else
					{
						curr_env_ptr->ptr_pageWorkingSet[i].time_stamp = (oldTimeStamp>>2);
					}
				}
			}
		}

		{
			int t ;
			for (t = 0 ; t < __TWS_MAX_SIZE; t++)
			{
				if( curr_env_ptr->__ptr_tws[t].empty != 1)
				{
					//update the time if the page was referenced
					uint32 table_va = curr_env_ptr->__ptr_tws[t].virtual_address;
					uint32 oldTimeStamp = curr_env_ptr->__ptr_tws[t].time_stamp;

					if (pd_is_table_used(curr_env_ptr, table_va))
					{
						curr_env_ptr->__ptr_tws[t].time_stamp = (oldTimeStamp>>2) | 0x80000000;
						pd_set_table_unused(curr_env_ptr, table_va);
					}
					else
					{
						curr_env_ptr->__ptr_tws[t].time_stamp = (oldTimeStamp>>2);
					}
				}
			}
		}
	}
}


