#ifdef DEBUG
#define _(x) x
#else
#define _(x)
#endif

#include <algorithm>
#include "globals.h"
#include <iostream>

using namespace std;

// TODO : DONT USE SPACES!!!!!


/*
 * vm_init
 *
 * Called when the pager starts.  It should set up any internal data structures
 * needed by the pager.
 *
 * vm_init is passed the number of physical memory pages and the number
 * of blocks in the swap file.
 */
void vm_init(size_t memory_pages, size_t swap_blocks) {
	MAX_NUM_SWAP_BLOCKS = (unsigned int) swap_blocks;
	MAX_PHYSICAL_MEMORY_PAGES = (unsigned int) memory_pages;

	// TODO:ALBERT MAKE THIS LOOK BETTER
	// Initialize all of the data dependency files
	vector<dataDependency> init;

	for (unsigned int i = 0; i < memory_pages; i++) {
		physicalMemoryDependencies.push_back(init);
	}

	for (unsigned int i = 0; i < swap_blocks; i++) {
		freeSwapBlocks.push_back(i);
		swapBlockDependencies.push_back(init);
	}

	// creates a page of all 0s
	memset(vm_physmem, 0, VM_PAGESIZE);

	_(
		char * mem = (char *) vm_physmem;
		int counter = 0;
		for(unsigned int i = 0; i < VM_PAGESIZE ; i++) {
			// cout << "read bit: " << base._pageTablePtr->ptes[i].read_enable << " write bit: " << base._pageTablePtr->ptes[i].write_enable << " ppage: " << base._pageTablePtr->ptes[i].ppage << endl;
			if (mem[i] == 0) {
				counter++;
			}
		}
		cout << "arena size: " << VM_PAGESIZE << endl;
		cout << "counter size: " << counter << endl;
		assert(VM_ARENA_SIZE/VM_PAGESIZE  == counter)
	);


}

/*
 * vm_create
 * Called when a parent process (parent_pid) creates a new process (child_pid).
 * vm_create should cause the child's arena to have the same mappings and data
 * as the parent's arena.  If the parent process is not being managed by the
 * pager, vm_create should consider the arena to be empty.
 * Note that the new process is not run until it is switched to via vm_switch.
 * Returns 0 on success, -1 on failure.
 */
int vm_create(pid_t parent_pid, pid_t child_pid) {
	_(
		cout << "\nstart vm_create\n";
		print_contents();
		sanity();
	);
	if(mapOfProcesses.find(parent_pid) != mapOfProcesses.end()){
		if (mapOfProcesses[parent_pid].numSwapBlocksUsed > freeSwapBlocks.size()) { //not enough swap blocks?
			return -1;
		}
	}
	// check for switch blocks
	mapOfProcesses[child_pid] = processData();
	mapOfProcesses[child_pid]._pageTablePtr = new page_table_t;
	mapOfProcesses[child_pid]._extraInfoMapPtr = new extraInfoMap;
	if(mapOfProcesses.find(parent_pid) != mapOfProcesses.end()) {
		// copy parents page_table_t to your page_table_t
		copy(mapOfProcesses[parent_pid]._pageTablePtr->ptes, mapOfProcesses[parent_pid]._pageTablePtr->ptes + VM_ARENA_SIZE/VM_PAGESIZE, mapOfProcesses[child_pid]._pageTablePtr->ptes);
		mapOfProcesses[child_pid]._extraInfoMapPtr->eim = mapOfProcesses[parent_pid]._extraInfoMapPtr->eim;
		mapOfProcesses[child_pid]._vmArenaBound = mapOfProcesses[parent_pid]._vmArenaBound;
		mapOfProcesses[child_pid].numSwapBlocksUsed = mapOfProcesses[parent_pid].numSwapBlocksUsed;
		// eager reservation of swap blocks
		for (unsigned int i = 0; i < mapOfProcesses[child_pid].numSwapBlocksUsed; i++) {
			// pull in blocks
			unsigned int newGuy = freeSwapBlocks.front();
			reservedSwapBlocks.push_back(newGuy);
			freeSwapBlocks.pop_front();
		}
		//copying physicalMemoryDependencies
		for (unsigned int i = 0; i < MAX_PHYSICAL_MEMORY_PAGES; i++) {
			unsigned int size = physicalMemoryDependencies[i].size();
			for (unsigned int j = 0; j < size; j++) {
				if (physicalMemoryDependencies[i][j]._processNum == parent_pid) {
					physicalMemoryDependencies[i].push_back(dataDependency(child_pid, physicalMemoryDependencies[i][j]._virtualPage));
				}
			}
			if(size != physicalMemoryDependencies[i].size() && mapOfProcesses[parent_pid]._extraInfoMapPtr->eim[physicalMemoryDependencies[i][0]._virtualPage]._isSwapBacked) {
				page_table_entry_t * copy_pte = &(mapOfProcesses[parent_pid]._pageTablePtr->ptes[physicalMemoryDependencies[i][0]._virtualPage]);
				extraInfo * copy_extraInfo = &(mapOfProcesses[parent_pid]._extraInfoMapPtr->eim[physicalMemoryDependencies[i][0]._virtualPage]);
				assert(i == copy_pte->ppage);
				unsigned int ppage_num = copy_pte->ppage;
				unsigned int read_enable = copy_pte->read_enable;
				unsigned int write_enable = 0;
				unsigned int dirty_bit = copy_extraInfo->_dirty;
				unsigned int reference_bit = copy_extraInfo->_reference;
				changeDependencies(physicalMemoryDependencies[i], ppage_num, read_enable, write_enable, dirty_bit, reference_bit);
			}
		}
		// copying fileDependencies
		for (auto it0 = fileDependencies.begin(); it0 != fileDependencies.end(); ++it0) {
			for (auto it1 = it0->second.begin(); it1 != it0->second.end() ; ++it1) {
				unsigned int size = it1->second.dataDependencies.size();
				for(unsigned int k = 0; k < size; ++k) {
					if (it1->second.dataDependencies[k]._processNum == parent_pid) {
						it1->second.dataDependencies.push_back(dataDependency(child_pid, it1->second.dataDependencies[k]._virtualPage));
					}
				}
			}
		}
		// copying swapBlockDependencies
		for (unsigned int i = 0; i < swapBlockDependencies.size(); ++i) {
			unsigned int size = swapBlockDependencies[i].size();
			for (unsigned int j = 0; j < size; ++j) {
				if (swapBlockDependencies[i][j]._processNum == parent_pid) {
					swapBlockDependencies[i].push_back(dataDependency(child_pid, swapBlockDependencies[i][j]._virtualPage));
				}
			}
		}
	} else {
		page_table_entry_t temp;              
		temp.ppage = MAX_PHYSICAL_MEMORY_PAGES;
		temp.read_enable = 0;
		temp.write_enable = 0;
		fill(mapOfProcesses[child_pid]._pageTablePtr->ptes, mapOfProcesses[child_pid]._pageTablePtr->ptes + VM_ARENA_SIZE/VM_PAGESIZE, temp);
	}

	_(
		cout << "\nend vm_create" << endl;;
		print_contents();
		sanity();
	);
	return 0;
}

/*
 * vm_switch
 *
 * Called when the kernel is switching to a new process, with process
 * identifier "pid".
 */   
void vm_switch(pid_t pid) {
	// Find processData associated with pid
	auto temp = mapOfProcesses.find(pid);

	if (temp != mapOfProcesses.end()) {
		// ProcessData found. Update current process  
		_(cout << "In vm_switch(). Found PID " << pid << " in mapOfProcesses!" << endl);
		// current process = mapOfProcesses[pid];
		currentProcessID = pid;	
		page_table_base_register = temp->second._pageTablePtr;
		_(assert(temp->second._pageTablePtr == mapOfProcesses[pid]._pageTablePtr));
	} else {
		_(cerr << "ERROR: In vm_switch(), PID not found within map of processes." << endl);
		assert(0);
	}	
}

/*
 * vm_fault
 *
 * Called when current process has a fault at virtual address addr.  write_flag
 * is true if the access that caused the fault is a write.
 * Returns 0 on success, -1 on failure.
 */

// change how eager reservations work (processData struct)
int vm_fault(const void *addr, bool write_flag) {
	if (!validAddress((unsigned long) addr, currentProcessID)) {
		return -1;
	}
	_( 
		cout << "start vm_fault\n";
	);

	unsigned int virtualPage = getVirtualPage((unsigned long) addr);	// page holds the virtual page number
	processData * target = &mapOfProcesses[currentProcessID];

	bool isSwapBacked = target->_extraInfoMapPtr->eim[virtualPage]._isSwapBacked;
	string fileName = target->_extraInfoMapPtr->eim[virtualPage]._filename;
	unsigned int blockNum = target->_extraInfoMapPtr->eim[virtualPage]._blockNum;

	bool read_enable = target->_pageTablePtr->ptes[virtualPage].read_enable;
	bool write_enable =	target->_pageTablePtr->ptes[virtualPage].write_enable;
	bool dirty_bit = target->_extraInfoMapPtr->eim[virtualPage]._dirty;
	unsigned int curr_ppage = target->_pageTablePtr->ptes[virtualPage].ppage;

	if (curr_ppage == MAX_PHYSICAL_MEMORY_PAGES) {
		// Since ppage is invalid and pointing to MAX_PHYSICAL_MEMORY_PAGES, we know that this memory has been evicted.
		// Bring this page into memory!
		unsigned int openSpot = clockEvict(); // open block in vm_physmem
		char * physical_addr = ((char *) vm_physmem + openSpot * VM_PAGESIZE);

		if (isSwapBacked) { // swapbacked
			if(file_read(nullptr, blockNum, (void *) physical_addr) == -1) {
				freeClockBlocks.push_back(openSpot);
				_(
					cout << "failed to read in vm_fault: " << endl;
				);
				return -1;
			}
			_( cout << "failed to transfer swap-backed" << endl);
			transferDependencies(swapBlockDependencies[blockNum],  physicalMemoryDependencies[openSpot]);
		} else { // filebacked
			if(file_read(fileName.c_str(), blockNum, (void *) physical_addr) == -1) {
				freeClockBlocks.push_back(openSpot);
				_(
					cout << "failed to read in vm_fault: " << endl;
				);
				return -1;
			}
			_( cout << "failed to transfer file_backed" << endl;);
			transferDependencies(fileDependencies[fileName][blockNum].dataDependencies, physicalMemoryDependencies[openSpot]);
		}
		evictClock.push_back(openSpot);

		read_enable = 1;
		write_enable = 0;
		dirty_bit = 0;
		curr_ppage = openSpot;

		changeDependencies(physicalMemoryDependencies[openSpot], openSpot, read_enable, write_enable, dirty_bit, 1);
	} 
	if (isSwapBacked) { 
		// swap-backed 
		// read the state
		if (write_flag) {
			// pull from an unused
			// if swapblock dependency vector > 1
			// writing to a swapblock with dependencies. need to reserve a block
			if (curr_ppage == 0 || physicalMemoryDependencies[target->_pageTablePtr->ptes[virtualPage].ppage].size() > 1) { 
				// swap block with dependencies or pointing to pinned memory

				// grab a new swap block
				assert(!reservedSwapBlocks.empty());
				unsigned int newKidOnTheBlock = reservedSwapBlocks.front();
				reservedSwapBlocks.pop_front();
				mapOfProcesses[currentProcessID]._extraInfoMapPtr->eim[virtualPage]._blockNum = newKidOnTheBlock;

				assert(swapBlockDependencies[newKidOnTheBlock].empty());

				// remove ourselves from the other data dependency vector
				for (auto it = physicalMemoryDependencies[curr_ppage].begin(); it != physicalMemoryDependencies[curr_ppage].end(); it++) {
					if (it->_processNum == currentProcessID && it->_virtualPage == virtualPage) {
						physicalMemoryDependencies[curr_ppage].erase(it);
						break;
					}
				} // end for
				// change state of swap block if size() == 1 after a dependency is removed.
				_(cout << "__1" << endl;);
				if(physicalMemoryDependencies[curr_ppage].size() == 1) {
					read_enable = 1;
					write_enable = dirty_bit ? 1 : 0;
					changeDependencies(physicalMemoryDependencies[curr_ppage], curr_ppage, read_enable, write_enable, dirty_bit, 1);
				} else if(curr_ppage != 0) {
					_(cout << "__2" << endl;);
					read_enable = 1;
					write_enable = 0;
					changeDependencies(physicalMemoryDependencies[curr_ppage], curr_ppage, read_enable, write_enable, dirty_bit, 1);
				}
				_(cout << "__3" << endl;);
				// save the current page into a buffer
				char temp_buff[VM_PAGESIZE];
				copy_page(((char *) vm_physmem + curr_ppage * VM_PAGESIZE), temp_buff);


				// clock evict
				unsigned int openSpot = clockEvict();
				evictClock.push_back(openSpot);

				// use the copy page function
				copy_page(temp_buff, ((char *) vm_physmem + openSpot * VM_PAGESIZE));

				assert(physicalMemoryDependencies[openSpot].empty());
				// add yourself to the dependencies for that block
				physicalMemoryDependencies[openSpot].push_back(dataDependency(currentProcessID, virtualPage));

				read_enable = 1;
				write_enable = 1;	// TODO: HERE?
				dirty_bit = 1;
				curr_ppage = openSpot;

				_(cout << "__4" << endl;);
				changeDependencies(physicalMemoryDependencies[curr_ppage],curr_ppage,read_enable,write_enable,dirty_bit,1);
				_(
					cout << "end vm_fault: " << endl;
				);
				return 0;
			} else {
				read_enable = 1;
				write_enable = 1;
				dirty_bit = 1;
				// just state change in here to be referenced and dirty and write enabled and such
				changeDependencies(physicalMemoryDependencies[curr_ppage],curr_ppage,read_enable,write_enable,dirty_bit,1);
				_(
					cout << "end vm_fault: " << endl;
				);
				return 0;
			} 
		} else {
			// trying to read
			read_enable = 1;
			write_enable = dirty_bit && (physicalMemoryDependencies[curr_ppage].size() == 1) ? 1 : 0 ;
			changeDependencies(physicalMemoryDependencies[target->_pageTablePtr->ptes[virtualPage].ppage],curr_ppage,read_enable,write_enable, dirty_bit, 1);
		} // end swap
		_(
			cout << "end vm_fault: " << endl;
		);
		return 0;
	} else { 
		// file-backed 
		if (write_flag) {
			// write
			read_enable = 1;
			write_enable = 1;
			dirty_bit = 1;
			changeDependencies(physicalMemoryDependencies[target->_pageTablePtr->ptes[virtualPage].ppage],curr_ppage,read_enable,write_enable, dirty_bit, 1);
		} else {
			read_enable = 1;
			write_enable = dirty_bit ? 1 : 0 ;
			// read
			changeDependencies(physicalMemoryDependencies[target->_pageTablePtr->ptes[virtualPage].ppage],curr_ppage,read_enable,write_enable, dirty_bit, 1);
		}
		_( 	
			cout << "end vm_fault: " << endl;
		);
		return 0;
	}
	// if this ever runs, something is very very wrong
	assert(0);
	return -1;
}

/*
 * vm_destroy
 *
 * Called when current process exits.  This gives the pager a chance to
 * clean up any resources used by the process.
 */
void vm_destroy() {
	// make this work using searchProcessMap
	auto target = mapOfProcesses.find(currentProcessID);

	if (target != mapOfProcesses.end()) {

		_(
			cout << "begin vm_destroy" << endl;
		);

		//remove yourself from physicalMemoryDependencies
		for (unsigned int i = 0; i < MAX_PHYSICAL_MEMORY_PAGES; i++) {
			vector<dataDependency> keep;
			vector<dataDependency> discard;
			for (unsigned int j = 0; j < physicalMemoryDependencies[i].size(); j++) {
				if (physicalMemoryDependencies[i][j]._processNum != currentProcessID) {
					keep.push_back(dataDependency(physicalMemoryDependencies[i][j]._processNum, physicalMemoryDependencies[i][j]._virtualPage));
				} else {
					discard.push_back(dataDependency(physicalMemoryDependencies[i][j]._processNum, physicalMemoryDependencies[i][j]._virtualPage));
				}
			}
			if(!discard.empty()) {
				if(mapOfProcesses[discard[0]._processNum]._extraInfoMapPtr->eim[discard[0]._virtualPage]._isSwapBacked) { // dependencies are being removed from a swapBacked page
					if(i) { // dependencies are being removed from a page in vm_physmem
						assert(discard.size() == 1);
						--mapOfProcesses[discard[0]._processNum].numSwapBlocksUsed;
						if(keep.size() == 1) { // only one dependency left. returns ownership of the block to the process
							assert(!reservedSwapBlocks.empty());
							freeSwapBlocks.push_back(reservedSwapBlocks.front());
							reservedSwapBlocks.pop_front();
							page_table_entry_t * pte_ptr = &(mapOfProcesses[keep[0]._processNum]._pageTablePtr->ptes[keep[0]._virtualPage]);
							extraInfo * extra_ptr = &(mapOfProcesses[keep[0]._processNum])._extraInfoMapPtr->eim[keep[0]._virtualPage];
							unsigned int read_enable = pte_ptr->read_enable;
							unsigned int write_enable = extra_ptr->_reference && extra_ptr->_dirty ? 1 : 0;
							unsigned int dirty_bit = extra_ptr->_dirty;
							unsigned int reference_bit = extra_ptr->_reference;
							assert(i == pte_ptr->ppage);
							changeDependencies(keep, i, read_enable, write_enable, dirty_bit, reference_bit);
						} else if (keep.empty()) { // no dependencies left after removing ourselves. block is "free"
							freeSwapBlocks.push_back(mapOfProcesses[discard[0]._processNum]._extraInfoMapPtr->eim[discard[0]._virtualPage]._blockNum);
							freeClockBlocks.push_back(i);
							auto it = evictClock.begin();
							while(it != evictClock.end()) {
								if(*it == i) {
									evictClock.erase(it);
									break;
								}
								++it;
							} // end while
						}
					} else { // dependencies are being removed from the pinned (zero) page
						for(unsigned int i = 0; i < discard.size(); ++i) {
							assert(currentProcessID == discard[i]._processNum);
							assert(!reservedSwapBlocks.empty());
							freeSwapBlocks.push_back(reservedSwapBlocks.front());
							reservedSwapBlocks.pop_front();
							--mapOfProcesses[discard[i]._processNum].numSwapBlocksUsed;
						}// end for
					} // end if-else
				} else { // dependencies are being removed from a fileBacked page
					if(keep.empty()) {
						page_table_entry_t * pte_ptr = &(mapOfProcesses[discard[0]._processNum]._pageTablePtr->ptes[discard[0]._virtualPage]);
						extraInfo * extra_ptr = &(mapOfProcesses[discard[0]._processNum])._extraInfoMapPtr->eim[discard[0]._virtualPage];
						danglingFiles[i] = forgottenFiles(extra_ptr->_filename, extra_ptr->_blockNum, pte_ptr->ppage, extra_ptr->_dirty, extra_ptr->_reference);
					}
				} // end if-else
				physicalMemoryDependencies[i] = keep;
			}
		}
		// remove yourself from fileDependencies
		for (auto it0 = fileDependencies.begin(); it0 != fileDependencies.end(); ++it0) {
			for (auto it1 = it0->second.begin(); it1 != it0->second.end() ; ++it1) {
				vector<dataDependency> temp;
				for(unsigned int k = 0; k < it1->second.dataDependencies.size(); ++k) {
					if (it1->second.dataDependencies[k]._processNum != currentProcessID) {
						temp.push_back(dataDependency(it1->second.dataDependencies[k]._processNum, it1->second.dataDependencies[k]._virtualPage));
					}
				}
				if(!it1->second.dataDependencies.empty()) {
					assert(it1->second._ppage == MAX_PHYSICAL_MEMORY_PAGES);
				}
				it1->second.dataDependencies = temp;
			}
		}
		// remove yourself from swapBlockDependencies
		for (unsigned int i = 0; i < swapBlockDependencies.size(); ++i) {
			vector<dataDependency> temp;
			for (unsigned int j = 0; j < swapBlockDependencies[i].size(); ++j) {
				if (swapBlockDependencies[i][j]._processNum != currentProcessID) {
					temp.push_back(dataDependency(swapBlockDependencies[i][j]._processNum, swapBlockDependencies[i][j]._virtualPage));
				}
			}
			if (!swapBlockDependencies[i].empty() && temp.empty()) {
				// we're the last swap backed page dependency
				extraInfo * extra_ptr = &target->second._extraInfoMapPtr->eim[swapBlockDependencies[i][0]._virtualPage];
				freeSwapBlocks.push_back(extra_ptr->_blockNum);
				--(target->second.numSwapBlocksUsed);
			}  else if (temp.size() < swapBlockDependencies[i].size() ) {
				// process number is equal to what we want to destroy
				// free the reserved block!
				assert(!reservedSwapBlocks.empty());
				--(target->second.numSwapBlocksUsed);
				freeSwapBlocks.push_back(reservedSwapBlocks.front());
				reservedSwapBlocks.pop_front();
			}
			swapBlockDependencies[i] = temp;
		}
		while (target->second.numSwapBlocksUsed) {
			_(cout << "moving back reserved blocks" << endl;);
			assert(!reservedSwapBlocks.empty());
			freeSwapBlocks.push_back(reservedSwapBlocks.front());
			reservedSwapBlocks.pop_front();
			--(target->second.numSwapBlocksUsed);
		}
		_(
			cout << "MAX_NUM_SWAP_BLOCKS: " << MAX_NUM_SWAP_BLOCKS << endl;
			cout << "# of freeSwapBlocks: " << freeSwapBlocks.size() << endl;	
		)
		delete target->second._pageTablePtr;
		delete target->second._extraInfoMapPtr;
		mapOfProcesses.erase(target);
		_(
			cout << "ending vm_destroy" << endl;
		);
	} else {
		cerr << "ERROR in vm_destroy()" << endl;
		assert(0);
	}
}

/*
 * vm_map
 *
 * A request by the current process for the lowest invalid virtual page in
 * the process's arena to be declared valid.  On success, vm_map returns
 * the lowest address of the new virtual page.  vm_map returns nullptr if
 * the arena is full.
 *
 * If filename is nullptr, block is ignored, and the new virtual page is
 * backed by the swap file, is initialized to all zeroes (from the
 * application's perspective), and private (i.e., not shared with any other
 * virtual page).  In this case, vm_map returns nullptr if the swap file is
 * out of space.
 *
 * If filename is not nullptr, the new virtual page is backed by the specified
 * file at the specified block and is shared with other virtual pages that are
 * mapped to that file and block.  filename is a null-terminated C string and
 * must reside completely in the valid portion of the arena.  In this case,
 * vm_map returns nullptr if filename is not completely in the valid part of
 * the arena.
 * filename is specified relative to the pager's current working directory.
 */
void *vm_map(const char *filename, size_t block) {
	_(
		cout << "start vm_map: " << endl;
	);

	if (mapOfProcesses[currentProcessID]._vmArenaBound + 1 > VM_ARENA_SIZE/VM_PAGESIZE) {
		// Overflow Detected! No room.
		_(cout << "overflow detected\n";);
		return nullptr;
	}
	if (filename != nullptr && !validAddress((unsigned long) filename, currentProcessID)) {
		// filename is not within the valid arena
		_(cout << "filename is not within the valid arena? invalid address\n");
		return nullptr;
	}

	unsigned long returnAddr = ((uintptr_t) VM_ARENA_BASEADDR) + mapOfProcesses[currentProcessID]._vmArenaBound * VM_PAGESIZE;	// new base
	unsigned int virtualPage = mapOfProcesses[currentProcessID]._vmArenaBound;

			// increase this process's arena 
	if (filename == nullptr) { // swap-backed file.. ignore second argument!
		// checks the swap file to ensure that there is enough space to add the new block. ensure eager reservation
		if (freeSwapBlocks.empty()) {
			// no swap blocks left
			_(cout << "no free swapblocks available :(" << endl);
			return nullptr;
		}
		// reserve the swap block
		reservedSwapBlocks.push_back(freeSwapBlocks.front());
		freeSwapBlocks.pop_front();	// swap block is no longer free
		++mapOfProcesses[currentProcessID].numSwapBlocksUsed;

		page_table_entry_t * target_pte = &(mapOfProcesses[currentProcessID]._pageTablePtr->ptes[virtualPage]);
		extraInfo * target_extraInfo = &(mapOfProcesses[currentProcessID]._extraInfoMapPtr->eim[virtualPage]);
		// set pte info
		target_pte->ppage = 0;	// pinned page
		target_pte->read_enable = 1;
		target_pte->write_enable = 0;
		// set extraInfo
		target_extraInfo->_dirty = false;
		target_extraInfo->_reference = true;
		target_extraInfo->_isSwapBacked = true;
		target_extraInfo->_filename = "";
		target_extraInfo->_blockNum = MAX_NUM_SWAP_BLOCKS;
		physicalMemoryDependencies[0].push_back(dataDependency(currentProcessID, virtualPage));
	} else { // file-backed.. use second argument!
		page_table_entry_t * target_pte = &(mapOfProcesses[currentProcessID]._pageTablePtr->ptes[virtualPage]);
		extraInfo * target_extraInfo = &(mapOfProcesses[currentProcessID]._extraInfoMapPtr->eim[virtualPage]);
		// set pte info
		target_pte->ppage = MAX_PHYSICAL_MEMORY_PAGES;	// lives in file, not physical memory
		target_pte->read_enable = 0;
		target_pte->write_enable = 0;
		// set extraInfo
		target_extraInfo->_dirty = false;
		target_extraInfo->_reference = false;
		target_extraInfo->_isSwapBacked = false;
		target_extraInfo->_filename = "";
		target_extraInfo->_blockNum = block;
		unsigned int filenameVirtualPage = getVirtualPage((unsigned long) filename);	// find the virtual page that has this filename
		page_table_entry_t * filename_pte = &(mapOfProcesses[currentProcessID]._pageTablePtr->ptes[filenameVirtualPage]);
		extraInfo * filename_extraInfo = &(mapOfProcesses[currentProcessID]._extraInfoMapPtr->eim[filenameVirtualPage]);
		while (true) {
			if (filename_pte->ppage >= MAX_PHYSICAL_MEMORY_PAGES) {
				// the file we're referencing doesn't exists in physical memory,
				// so bring it in
				unsigned int openSpot = clockEvict();
				char * physical_memory_location = ((char *) vm_physmem + openSpot * VM_PAGESIZE);
				if(filename_extraInfo->_isSwapBacked) {
					_(cout << "getting filename from a swap_backed";);
					if(file_read(nullptr, filename_extraInfo->_blockNum, physical_memory_location) == -1) {
						freeClockBlocks.push_back(openSpot);
						_(cout << "file_read failed for swap_backed in file" << endl);
						return nullptr;
					}
					filename_pte->ppage = openSpot;
					transferDependencies(swapBlockDependencies[filename_extraInfo->_blockNum],physicalMemoryDependencies[filename_pte->ppage]);
				} else {
					// filebacked
					_(cout << "getting file_name from a file_backed";);
					if(file_read((const char *) filename_extraInfo->_filename.c_str(), filename_extraInfo->_blockNum, physical_memory_location) == -1) {
						freeClockBlocks.push_back(openSpot);
						_(cout << "file_read failed for file_backed in file" << endl);
						return nullptr;
					}
					filename_pte->ppage = openSpot;
					transferDependencies(fileDependencies[filename_extraInfo->_filename][filename_extraInfo->_blockNum].dataDependencies, physicalMemoryDependencies[filename_pte->ppage]);
				}
				// update fsm bits
				changeDependencies(physicalMemoryDependencies[filename_pte->ppage], filename_pte->ppage, 1, 0, 0, 1);
				evictClock.push_back(openSpot);
			} else {
				filename_pte->read_enable = 1;
				if(filename_extraInfo->_isSwapBacked) {
					filename_pte->write_enable = (filename_extraInfo->_dirty && (physicalMemoryDependencies[filename_pte->ppage].size() == 1)) ? 1 : 0;
				} else {
					filename_pte->write_enable = filename_extraInfo->_dirty ? 1 : 0;
				}
				changeDependencies(physicalMemoryDependencies[filename_pte->ppage], filename_pte->ppage, filename_pte->read_enable, filename_pte->write_enable, filename_extraInfo->_dirty, 1);
			}
			unsigned long offset = ((unsigned long) filename) - (uintptr_t) VM_ARENA_BASEADDR - filenameVirtualPage * VM_PAGESIZE;
			target_extraInfo->_filename += *((char *) vm_physmem + filename_pte->ppage * VM_PAGESIZE + offset);
			if (target_extraInfo->_filename.back() == '\0') {
				break;
			}
			++filename;
			if (!validAddress((unsigned long) filename, currentProcessID)) {
					// filename is not completely held within the scope of the arena
					_(cout << "filename is not completely held within the scope of the arena :(" << endl);
					return nullptr;
			} else if (filenameVirtualPage != getVirtualPage((unsigned long) filename)) {
				// change filename pte
				filenameVirtualPage = getVirtualPage((unsigned long) filename);
				filename_pte = &(mapOfProcesses[currentProcessID]._pageTablePtr->ptes[filenameVirtualPage]);
				filename_extraInfo = &(mapOfProcesses[currentProcessID]._extraInfoMapPtr->eim[filenameVirtualPage]);
			}
		} // end while
		if(fileDependencies[target_extraInfo->_filename][target_extraInfo->_blockNum]._ppage == MAX_PHYSICAL_MEMORY_PAGES) {
			fileDependencies[target_extraInfo->_filename][target_extraInfo->_blockNum].dataDependencies.push_back(dataDependency(currentProcessID, virtualPage));
		} else {
			unsigned int ppage = fileDependencies[target_extraInfo->_filename][target_extraInfo->_blockNum]._ppage;
			assert(fileDependencies[target_extraInfo->_filename][target_extraInfo->_blockNum].dataDependencies.empty());
			physicalMemoryDependencies[ppage].push_back(dataDependency(currentProcessID, virtualPage));
			auto it = danglingFiles.find(ppage);
			if(it != danglingFiles.end()) {
				assert(ppage == it->second._ppage);
				target_pte->ppage = ppage;
				target_pte->read_enable = it->second._reference ? 1 : 0;
				target_pte->write_enable = it->second._reference && it->second._dirty ? 1 : 0;
				// set extraInfo
				target_extraInfo->_dirty = it->second._dirty;
				target_extraInfo->_reference = it->second._reference;
				danglingFiles.erase(it);
			} else {
				page_table_entry_t * copy_pte = &(mapOfProcesses[physicalMemoryDependencies[ppage][0]._processNum]._pageTablePtr->ptes[physicalMemoryDependencies[ppage][0]._virtualPage]);
				extraInfo * copy_extraInfo = &(mapOfProcesses[physicalMemoryDependencies[ppage][0]._processNum]._extraInfoMapPtr->eim[physicalMemoryDependencies[ppage][0]._virtualPage]);
				target_pte->ppage = ppage;
				target_pte->read_enable = copy_pte->read_enable;
				target_pte->write_enable = copy_pte->write_enable;
				// set extraInfo
				target_extraInfo->_dirty = copy_extraInfo->_dirty;
				target_extraInfo->_reference = copy_extraInfo->_reference;
			}
		}
	} // end if-else
	_(
		cout << "finished vm_map" << endl;
	);
	++mapOfProcesses[currentProcessID]._vmArenaBound;
	return (void *) returnAddr;
}
