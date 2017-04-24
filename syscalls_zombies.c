#include <linux/errno.h>
#include <linux/sched.h>

int sys_set_max_zombies(int max, pid_t pid) {
	if(max < 0)
		return -EINVAL;
	if(pid < 0)
		return -ESRCH;
	find_task_by_pid(pid)->zombies_limit = max;
	return 0;
}

int sys_get_zombies_count(pid_t pid) {
	if(pid < 0)
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

	struct list_head *it;
	int count = 0;
	list_for_each(it,current->first_own_zombie->zombies_list.prev) {
		if(count == n)
			break;
		count++;
	}

	return list_entry(it,task_t,zombies_list)->pid;
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

	if (n == 0) return 0;

	struct list_head *it;
	int count = 0;
	list_for_each(it,current->first_own_zombie->zombies_list.prev) {
		if(count == n-1)
			break;
		count++;
	}

	current->zombies_count -= n;
	adopter->zombies_count += n;

	struct list_head* nth_member = it->next;
	it->next = &current->first_own_zombie->zombies_list;
	current->first_own_zombie->zombies_list.prev = it;

	nth_member->prev = &current->first_own_zombie->zombies_list.prev;
	current->first_own_zombie->zombies_list.prev->zombies_list.next = nth_member;

	current->first_own_zombie = list_entry(nth_member,task_t,zombies_list);
	list_splice(it->next, &adopter->first_own_zombie->zombies_list.prev);
	adopter->first_own_zombie->zombies_list.prev = list_entry(&adopter->first_own_zombie->zombies_list.prev, task_t, zombies_list);

	
	// if (current->zombies_count == 0) {
	// 	list_splice(&current->first_own_zombie->zombies_list, &adopter->last_own_zombie->zombies_list);
	// 	current->first_own_zombie = NULL;
	// 	current->last_own_zombie = NULL;
	// } else {
	// 	struct list_head* nth_member = it->next;
	// 	it->next = &current->first_own_zombie->zombies_list;
	// 	current->first_own_zombie->zombies_list.prev = it;

	// 	nth_member->prev = &current->last_own_zombie->zombies_list;
	// 	current->last_own_zombie->zombies_list.next = nth_member;

	// 	current->first_own_zombie = list_entry(nth_member,task_t,zombies_list);
	// 	list_splice(it->next, &adopter->last_own_zombie->zombies_list);
	// }
	// adopter->last_own_zombie = list_entry(&adopter->first_own_zombie->zombies_list.prev, task_t, zombies_list);		

	return 0;
}
