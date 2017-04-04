// #include "globals.h"

bool sanity_check() {
	assert(evictClock.size() < MAX_PHYSICAL_MEMORY_PAGES);
}

void create_zero_page() {
	for (unsigned int i = 0; i < VM_PAGESIZE; i++) {
		*((char *) vm_physmem + i) = 0;
	}
}

void copy_page(unsigned int page, unsigned int target) {
	assert(page < MAX_PHYSICAL_MEMORY_PAGES && target < MAX_PHYSICAL_MEMORY_PAGES);
	for (unsigned int i = 0; i < VM_PAGESIZE; i++) {
		*((char *) vm_physmem + (target * VM_PAGESIZE + i)) = *((char *) vm_physmem + (page * VM_PAGESIZE + i));
	}
}
