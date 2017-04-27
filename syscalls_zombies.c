#include <linux/errno.h>
#include <linux/sched.h>

int sys_set_max_zombies(int max, pid_t pid) {
	if (max < 0)
		return -EINVAL;
	if (pid < 0)
		return -ESRCH;

	find_task_by_pid(pid)->zombies_limit = max;
	return 0;
}

int sys_get_zombies_count(pid_t pid) {
	if (pid < 0)
		return -ESRCH;

	return find_task_by_pid(pid)->zombies_count;
}

int sys_get_max_zombies() {
	if (current->zombies_limit == -1)
		return -EINVAL;
	else
		return current->zombies_limit;
}

int sys_get_zombie_pid(int n) {
	if(current->zombies_count <=  n || n < 0)
		return -EINVAL;

	if (current->zombies_limit == -1)
		return -EINVAL;

	list_t *it;
	int i = 0;
	
	list_for_each(it, &current->zombies_list) {
		if (i == n) break;
		i++;
	}

	return list_entry(it, task_t, zombies_list)->pid;
}

int sys_give_up_zombie(int n, pid_t adopter_pid) {
	if(n < 0 || n > current->zombies_count)
		return -EINVAL;

	if(adopter_pid < 0) 
		return -ESRCH;

	task_t *adopter = find_task_by_pid(adopter_pid);

	if (adopter->zombies_limit == -1)
		return -EINVAL;

	if (adopter_pid == current->pid &&
		current->zombies_count > current->zombies_limit)
			return -EINVAL;

	if (adopter_pid != current->pid && 
		n + adopter->zombies_count > adopter->zombies_limit)
			return -EINVAL;

	list_t *it, *temp;
	int i = 0;
	
	list_for_each_safe(it, temp, &current->zombies_list) {
		if (i == n) break;
		task_t *child_to_reparent = list_entry(it, task_t, zombies_list);

		list_del(it);
		list_add_tail(it, &adopter->zombies_list);

		REMOVE_LINKS(child_to_reparent);
		child_to_reparent->p_opptr = adopter;
		child_to_reparent->p_pptr = adopter;
		SET_LINKS(child_to_reparent);
		
		i++;
	}

	adopter->zombies_count += n;
	current->zombies_count -= n;

	return 0;
}