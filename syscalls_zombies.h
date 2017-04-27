#include <errno.h>

int set_max_zombies (int max, pid_t pid) {
	unsigned int res;
	__asm__(
		"int $0x80;"
		: "=a" (res)
		: "0" (243) ,"b" (max) ,"c" (pid)
		: "memory"
	);

	if (res >= (unsigned long)(-125)) {
		errno = -res;
		res = -1;
	}

	return (int) res;
}

int get_zombies_count (pid_t pid) {
	unsigned int res;
	__asm__(
		"int $0x80;"
		: "=a" (res)
		: "0" (244) ,"b" (pid)
		: "memory"
	);

	if (res >= (unsigned long)(-125)) {
		errno = -res;
		res = -1;
	}

	return (int) res;
}

int get_max_zombies () {
	unsigned int res;
	__asm__(
		"int $0x80;"
		: "=a" (res)
		: "0" (245)
		: "memory"
	);

	if (res >= (unsigned long)(-125)) {
		errno = -res;
		res = -1;
	}

	return (int) res;
}

int get_zombie_pid (int n) {
	unsigned int res;
	__asm__(
		"int $0x80;"
		: "=a" (res)
		: "0" (246) ,"b" (n)
		: "memory"
	);

	if (res >= (unsigned long)(-125)) {
		errno = -res;
		res = -1;
	}

	return (int) res;
}

int give_up_zombie (int n, pid_t adopter_pid) {
	unsigned int res;
	__asm__(
		"int $0x80;"
		: "=a" (res)
		: "0" (247) ,"b" (n), "c" (adopter_pid)
		: "memory"
	);

	if (res >= (unsigned long)(-125)) {
		errno = -res;
		res = -1;
	}

	return (int) res;
}
