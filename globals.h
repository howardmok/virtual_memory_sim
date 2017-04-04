/*
 * globals.h
 *
 * Header file for globals used in the pager 
 */
#ifdef DEBUG
#define _(x) x
#include <iostream>
#else
#define _(x)
#endif

#ifndef _GLOBALS_H_
#define _GLOBALS_H_



#include <cassert>
#include <unordered_map>
#include <deque>
#include <string.h>
#include <vector>
#include "vm_arena.h"
#include "vm_pager.h"

// Globals for max constants for memory
unsigned int MAX_NUM_SWAP_BLOCKS;
unsigned int MAX_PHYSICAL_MEMORY_PAGES;

// deque indicating which swap blocks are available for use
std::deque<unsigned int> freeSwapBlocks;
// deque indicating which swap blocks are reserved for use
std::deque<unsigned int> reservedSwapBlocks;

/*************************************
 *                                   *
 * Extra data for page_table_entries *
 *                                   *
 *************************************/

// Extra Info to go along with each each page_table_entry
struct extraInfo {
	std::string _filename;				// name of the file, if file-backed
	unsigned int _dirty : 1;			// 0 for clean, 1 for dirty
	unsigned int _reference : 1;		// 0 for nonresident, 1 for resident
	unsigned int _isSwapBacked : 1;		// 0 for file backed; 1 for swap backed
	unsigned int _blockNum;				// block backing this file 
	extraInfo() :  _filename(""), _dirty(0), _reference(0), _isSwapBacked(0), _blockNum(MAX_NUM_SWAP_BLOCKS) {};
};

struct extraInfoMap {
	std::unordered_map<unsigned int, extraInfo> eim; // map is accessed by using a virtualPage #
};

/********************************************
 *                                          *
 * Globals tracking data used for processes *
 *                                          *
 ********************************************/

struct processData {
	page_table_t * _pageTablePtr;
	extraInfoMap * _extraInfoMapPtr;

	unsigned int numSwapBlocksUsed;

	unsigned int _vmArenaBound;

	processData() : _pageTablePtr(nullptr), _extraInfoMapPtr(nullptr), numSwapBlocksUsed(0), _vmArenaBound(0) {}; 
};

// process id of the currently running process
pid_t currentProcessID;

// a map of processes to their process data. 
// this will be filled once a process calls fork
// to create a new process
std::unordered_map<pid_t, processData> mapOfProcesses;

/************************************************
 *                                              *
 * Data used for Clock Replacement and Eviction *
 *                                              *
 ************************************************/

std::deque<unsigned int> freeClockBlocks; // things that were evicted, but never added back to memory
std::deque<unsigned int> evictClock; // carries unsigned int's that refer to a ppage

/**************************************
 *                                    *
 * Globals tracking data dependencies *
 *        (in physmem && disk)        *
 *                                    *
 **************************************/

struct dataDependency {
	pid_t _processNum;				// pid
	unsigned long _virtualPage;
	dataDependency(pid_t pN, unsigned long pageNum) : _processNum(pN), _virtualPage(pageNum) {};
};

//the first level of vectors are for each process 
//the second level of vectors are pages that the process currently has running.
std::vector<std::vector<dataDependency>> physicalMemoryDependencies; 
// use blocknum to index into it
std::vector<std::vector<dataDependency>> swapBlockDependencies; // this is for when a swap block gets evicted and dependencies exist

struct fileDependency {
	std::vector<dataDependency> dataDependencies;
	unsigned int _ppage;
	fileDependency() : _ppage(MAX_PHYSICAL_MEMORY_PAGES) {};
};
// key: filename, value: map 
	// key: blockNum, value: dataDependency
std::unordered_map<std::string, std::unordered_map<unsigned int, fileDependency>> fileDependencies;

// data to to keep track of dangling files in vm_physmem
struct forgottenFiles {
	std::string _filename;
	unsigned int _blockNum;
	unsigned int _ppage;
	unsigned int _dirty : 1;
	unsigned int _reference : 1;
	forgottenFiles() : _blockNum(MAX_NUM_SWAP_BLOCKS), _ppage(MAX_PHYSICAL_MEMORY_PAGES), _dirty(0), _reference(0) {};
	forgottenFiles(std::string f, unsigned int bN, unsigned int pp, unsigned int d, unsigned int r) : _filename(f), _blockNum(bN), _ppage(pp), _dirty(d), _reference(r) {};
};

// maps ppages to forgotten files
std::unordered_map<unsigned int, forgottenFiles> danglingFiles;

/**********************
 *                    *
 *  Helper Functions  *
 *                    *
 **********************/

// function to check a lot of things for sanity
_(
	void sanity() {
		std::cout << "beginning sanity check" << std::endl;
		assert(evictClock.size() < MAX_PHYSICAL_MEMORY_PAGES);
		// check physicalMemoryDependencies
		for (unsigned int i = 0; i < MAX_PHYSICAL_MEMORY_PAGES; i++) {
			if(!physicalMemoryDependencies[i].empty()) {
				page_table_entry_t * check_pte = &(mapOfProcesses[physicalMemoryDependencies[i][0]._processNum]._pageTablePtr->ptes[physicalMemoryDependencies[i][0]._virtualPage]);
				extraInfo * check_extra = &(mapOfProcesses[physicalMemoryDependencies[i][0]._processNum]._extraInfoMapPtr->eim[physicalMemoryDependencies[i][0]._virtualPage]);
				for (unsigned int j = 0; j < physicalMemoryDependencies[i].size(); j++) {
					page_table_entry_t * pte_ptr = &(mapOfProcesses[physicalMemoryDependencies[i][j]._processNum]._pageTablePtr->ptes[physicalMemoryDependencies[i][j]._virtualPage]);
					extraInfo * e_ptr = &(mapOfProcesses[physicalMemoryDependencies[i][j]._processNum]._extraInfoMapPtr->eim[physicalMemoryDependencies[i][j]._virtualPage]);
					assert(check_pte->ppage == pte_ptr->ppage);
					assert(check_pte->read_enable == pte_ptr->read_enable);
					assert(check_pte->write_enable == pte_ptr->write_enable);
					assert(check_extra->_dirty == e_ptr->_dirty);
					assert(check_extra->_reference == e_ptr->_reference);
					assert(check_extra->_blockNum == e_ptr->_blockNum);
					assert(check_extra->_isSwapBacked == e_ptr->_isSwapBacked);
					assert(check_extra->_filename == e_ptr->_filename);
				}
			}
		} // end for()
		// check fileDependencies
		for (auto it0 = fileDependencies.begin(); it0 != fileDependencies.end(); ++it0) {
			std::cout << "filename: " <<it0->first << ":\n";
			for(auto it1 = it0->second.begin(); it1 != it0->second.end() ; ++it1) {
				if(!it1->second.dataDependencies.empty()) {
					page_table_entry_t * check_pte = &(mapOfProcesses[it1->second.dataDependencies[0]._processNum]._pageTablePtr->ptes[it1->second.dataDependencies[0]._virtualPage]);
					extraInfo * check_extra = &(mapOfProcesses[it1->second.dataDependencies[0]._processNum]._extraInfoMapPtr->eim[it1->second.dataDependencies[0]._virtualPage]);
					for(unsigned int k = 0; k < it1->second.dataDependencies.size(); ++k) {
						page_table_entry_t * pte_ptr = &(mapOfProcesses[it1->second.dataDependencies[k]._processNum]._pageTablePtr->ptes[it1->second.dataDependencies[k]._virtualPage]);
						extraInfo * e_ptr = &(mapOfProcesses[it1->second.dataDependencies[k]._processNum]._extraInfoMapPtr->eim[it1->second.dataDependencies[k]._virtualPage]);
						assert(MAX_PHYSICAL_MEMORY_PAGES == check_pte->ppage && check_pte->ppage == pte_ptr->ppage);
						assert(!check_pte->read_enable && check_pte->read_enable == pte_ptr->read_enable);
						assert(!check_pte->write_enable && check_pte->write_enable == pte_ptr->write_enable);
						assert(!check_extra->_dirty && check_extra->_dirty == e_ptr->_dirty);
						assert(!check_extra->_reference && check_extra->_reference == e_ptr->_reference);
						assert(check_extra->_blockNum == e_ptr->_blockNum);
						assert(!check_extra->_isSwapBacked && !e_ptr->_isSwapBacked);
						assert(check_extra->_filename == e_ptr->_filename);
					}
				} else if(!physicalMemoryDependencies[it1->second._ppage].empty()){
					assert(it1->second._ppage == mapOfProcesses[physicalMemoryDependencies[it1->second._ppage][0]._processNum]._pageTablePtr->ptes[physicalMemoryDependencies[it1->second._ppage][0]._virtualPage].ppage);
				}
			}
		}
		// check swapBlockDependencies
		for (unsigned int i = 0; i < MAX_NUM_SWAP_BLOCKS; ++i) {
			if(!swapBlockDependencies[i].empty()) {
				page_table_entry_t * check_pte = &(mapOfProcesses[swapBlockDependencies[i][0]._processNum]._pageTablePtr->ptes[swapBlockDependencies[i][0]._virtualPage]);
				extraInfo * check_extra = &(mapOfProcesses[swapBlockDependencies[i][0]._processNum]._extraInfoMapPtr->eim[swapBlockDependencies[i][0]._virtualPage]);
				for(unsigned int j = 0; j < swapBlockDependencies[i].size() ; ++j) {
					page_table_entry_t * pte_ptr = &(mapOfProcesses[swapBlockDependencies[i][j]._processNum]._pageTablePtr->ptes[swapBlockDependencies[i][j]._virtualPage]);
					extraInfo * e_ptr = &(mapOfProcesses[swapBlockDependencies[i][j]._processNum]._extraInfoMapPtr->eim[swapBlockDependencies[i][j]._virtualPage]);
					assert(MAX_PHYSICAL_MEMORY_PAGES == check_pte->ppage && check_pte->ppage == pte_ptr->ppage);
					assert(!check_pte->read_enable && check_pte->read_enable == pte_ptr->read_enable);
					assert(!check_pte->write_enable && check_pte->write_enable == pte_ptr->write_enable);
					assert(!check_extra->_dirty && check_extra->_dirty == e_ptr->_dirty);
					assert(!check_extra->_reference && check_extra->_reference == e_ptr->_reference);
					assert(check_extra->_blockNum == e_ptr->_blockNum);
					assert(check_extra->_isSwapBacked && e_ptr->_isSwapBacked);
					assert(check_extra->_filename == e_ptr->_filename);
				}
			}
		}
		// check danglingFiles
		for(unsigned int i = 0; i < danglingFiles.size(); ++i) {
			for(unsigned int j = 1; j < danglingFiles.size(); ++j) {
				assert(danglingFiles[i]._ppage < MAX_PHYSICAL_MEMORY_PAGES);
				assert(danglingFiles[j]._ppage < MAX_PHYSICAL_MEMORY_PAGES);
				assert(danglingFiles[i]._ppage != danglingFiles[j]._ppage);
			}
		}
	std::cout << "done with sanity check" << std::endl << std::endl;
	} // end sanity
)

// copy over a page to a new target location
void copy_page(char * src, char * dest) {
	for (unsigned int i = 0; i < VM_PAGESIZE; i++) {
		*(dest + i) = *(src + i);
	}
}


bool validAddress(unsigned long address, pid_t pid) {
	return (address >= VM_ARENA_BASEPAGE && ((address / VM_PAGESIZE) - VM_ARENA_BASEPAGE) < mapOfProcesses[pid]._vmArenaBound);
}

// returns the vpn given a virtual address
unsigned long getVirtualPage(unsigned long virtualAddress) {
	assert(validAddress(virtualAddress, currentProcessID));
	return (virtualAddress / VM_PAGESIZE) - VM_ARENA_BASEPAGE;
}

// swaps the contents of one dependency vector to another
void transferDependencies(std::vector<dataDependency> &src, std::vector<dataDependency> &dest) {
	// transferDependencies copies the contents of src into dest
	assert(!src.empty());
	assert(dest.empty());
	// copy over to swapBlockDependencies
	src.swap(dest);
}

// takes in a src vector, reads through all entries of that vector and updates all corresponding data.
void changeDependencies(std::vector<dataDependency> &src, unsigned int ppage_num, unsigned int read_bit, unsigned int write_bit, unsigned int dirty_bit, unsigned int reference_bit) {
	// change the bits for the dependency vector passed
	assert(!src.empty());
	for (unsigned int i = 0; i < src.size(); i++) {
		processData *target = &mapOfProcesses[src[i]._processNum];
		target->_pageTablePtr->ptes[src[i]._virtualPage].ppage = ppage_num;
		target->_pageTablePtr->ptes[src[i]._virtualPage].read_enable = read_bit;
		target->_pageTablePtr->ptes[src[i]._virtualPage].write_enable = write_bit;
		target->_extraInfoMapPtr->eim[src[i]._virtualPage]._dirty = dirty_bit;
		target->_extraInfoMapPtr->eim[src[i]._virtualPage]._reference = reference_bit;
	}
	if(!mapOfProcesses[src[0]._processNum]._extraInfoMapPtr->eim[src[0]._virtualPage]._isSwapBacked) {
		extraInfo * temp = &mapOfProcesses[src[0]._processNum]._extraInfoMapPtr->eim[src[0]._virtualPage];
		fileDependencies[temp->_filename][temp->_blockNum]._ppage = ppage_num;
	}
}

_(
	void print_contents() {
		std::deque<unsigned int> tempClock;
		unsigned int clockSize = evictClock.size();
		std::cout << "\n\nprinting the contents of clock: " << std::endl;
		while(!evictClock.empty()) {
			std::cout << evictClock.front() << " ";
			tempClock.push_back(evictClock.front());
			evictClock.pop_front();
		}
		while (!tempClock.empty()){
			evictClock.push_back(tempClock.front());
			tempClock.pop_front();
		}
		std::cout << std::endl;
		std::cout << "clock is done printing" << std::endl;

		std::cout << "\ncurrentProcessID: " << currentProcessID << std::endl;
		std::cout << "physicalMemoryDependencies: ------------- " << std::endl;
		for (unsigned int i = 0; i < MAX_PHYSICAL_MEMORY_PAGES; i++) {
			std::cout << "ppage: " << i << " -> ";
			for (unsigned int j = 0; j < physicalMemoryDependencies[i].size(); j++) {
				page_table_entry_t * pte_ptr = &(mapOfProcesses[physicalMemoryDependencies[i][j]._processNum]._pageTablePtr->ptes[physicalMemoryDependencies[i][j]._virtualPage]);
				extraInfo * e_ptr = &(mapOfProcesses[physicalMemoryDependencies[i][j]._processNum]._extraInfoMapPtr->eim[physicalMemoryDependencies[i][j]._virtualPage]);
				std::cout << " processNum: " <<  physicalMemoryDependencies[i][j]._processNum << " virtualPage: " << physicalMemoryDependencies[i][j]._virtualPage << " ppage: " << pte_ptr->ppage << " r: " << pte_ptr->read_enable << " w: " << pte_ptr->write_enable << " d: " << e_ptr->_dirty << " ref: " << e_ptr->_reference << " block: " << e_ptr->_blockNum << " || ";
			}
			std::cout << std::endl;
		} // end for()
		std::cout << "fileDependencies: ------------- " << std::endl;
		for (auto it0 = fileDependencies.begin(); it0 != fileDependencies.end(); ++it0) {
			std::cout << "filename: " <<it0->first << ":\n";
			for(auto it1 = it0->second.begin(); it1 != it0->second.end() ; ++it1) {
				std::cout << "blockNum " << it1->first << " && ppage: " << it1->second._ppage << " -> ";
				for(unsigned int k = 0; k < it1->second.dataDependencies.size(); ++k) {
					page_table_entry_t * pte_ptr = &(mapOfProcesses[it1->second.dataDependencies[k]._processNum]._pageTablePtr->ptes[it1->second.dataDependencies[k]._virtualPage]);
					extraInfo * e_ptr = &(mapOfProcesses[it1->second.dataDependencies[k]._processNum]._extraInfoMapPtr->eim[it1->second.dataDependencies[k]._virtualPage]);
					std::cout << "<processNum: " << it1->second.dataDependencies[k]._processNum << ",virtualPage: " << it1->second.dataDependencies[k]._virtualPage << " ppage: " << pte_ptr->ppage << " r: " << pte_ptr->read_enable << " w: " << pte_ptr->write_enable << " d: " << e_ptr->_dirty << " ref: " << e_ptr->_reference << " block: " << e_ptr->_blockNum << "> ";
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
		std::cout << "swapBlockDependencies: ------------- " << std::endl;
		for (unsigned int i = 0; i < 3; ++i) {
			std::cout << i << ": " << std::endl;
			for(unsigned int j = 0; j < swapBlockDependencies[i].size() ; ++j) {
				page_table_entry_t * pte_ptr = &(mapOfProcesses[swapBlockDependencies[i][j]._processNum]._pageTablePtr->ptes[swapBlockDependencies[i][j]._virtualPage]);
				extraInfo * e_ptr = &(mapOfProcesses[swapBlockDependencies[i][j]._processNum]._extraInfoMapPtr->eim[swapBlockDependencies[i][j]._virtualPage]);
				std::cout << "<_processNum: " << swapBlockDependencies[i][j]._processNum << ", virtualPage: " << swapBlockDependencies[i][j]._virtualPage << " ppage: " << pte_ptr->ppage << " r: " << pte_ptr->read_enable << " w: " << pte_ptr->write_enable << " d: " << e_ptr->_dirty << " ref: " << e_ptr->_reference << " block: " << e_ptr->_blockNum << "> ";
			}
			std::cout << std::endl;
		}
		std::cout << "danglingFiles: --------------" << std::endl;
		for(unsigned int i = 0; i < danglingFiles.size(); ++i) {
			std::cout << danglingFiles[i]._filename << " blockNum: " << danglingFiles[i]._blockNum << " ppage: " << danglingFiles[i]._ppage << " dirty: " << danglingFiles[i]._dirty << " reference: " << danglingFiles[i]._reference << "\n";
		}
		std::cout << "\ndone printing contents \n\n";
	} // end print_contents()
)

void clockEvictHelper(bool invalidate, std::vector<dataDependency> dependencies) {
	for(unsigned int i = 0; i < dependencies.size(); ++i) {
		processData *victim = &mapOfProcesses[dependencies[i]._processNum];
		if(invalidate) {
			assert(victim->_pageTablePtr->ptes[dependencies[i]._virtualPage].ppage > 0);
			assert(victim->_pageTablePtr->ptes[dependencies[i]._virtualPage].ppage < MAX_PHYSICAL_MEMORY_PAGES);
			assert(!victim->_pageTablePtr->ptes[dependencies[i]._virtualPage].read_enable && !victim->_pageTablePtr->ptes[dependencies[i]._virtualPage].write_enable);
			victim->_pageTablePtr->ptes[dependencies[i]._virtualPage].ppage = MAX_PHYSICAL_MEMORY_PAGES;
			victim->_extraInfoMapPtr->eim[dependencies[i]._virtualPage]._dirty = false;
		} else {
			assert(victim->_pageTablePtr->ptes[dependencies[i]._virtualPage].read_enable);
			victim->_pageTablePtr->ptes[dependencies[i]._virtualPage].read_enable = false;
			victim->_pageTablePtr->ptes[dependencies[i]._virtualPage].write_enable = false;
			victim->_extraInfoMapPtr->eim[dependencies[i]._virtualPage]._reference = false;
		}
	}
} // end clockEvictHelper()

unsigned int clockEvict() {
	_(std::cout << "starting clockEvict\n";);
	// returns evictClock.size() + 1 if clock is not full
	// else, returns the block that got freed
	if(!freeClockBlocks.empty()) {
		unsigned int evict = freeClockBlocks.front();
		freeClockBlocks.pop_front();
		_(std::cout << "passing back evict: " << evict << std::endl;);
		return evict;
	}
	if (evictClock.size() < MAX_PHYSICAL_MEMORY_PAGES - 1) {
		assert(physicalMemoryDependencies[evictClock.size()+1].empty());
		// No reason to evict because clock is not full yet
		return evictClock.size() + 1;
	} //endif
	assert(evictClock.size() > 0);
	assert(evictClock.size() == MAX_PHYSICAL_MEMORY_PAGES - 1);
	while (true) { // loop through evictClock 
		unsigned int possibleEvict = evictClock.front();
		evictClock.pop_front();
		if(physicalMemoryDependencies[possibleEvict].empty()) {
			auto it = danglingFiles.find(possibleEvict);
			if(it != danglingFiles.end()) { // ppage is a danglingFile
				if(it->second._dirty && !it->second._reference) { // dangingFile is unreferenced and dirty
					_(
						std::cout << "file was dirty. writing back to disk" << std::endl;
					);
					assert(it->second._ppage != MAX_PHYSICAL_MEMORY_PAGES);
					file_write(it->second._filename.c_str(), it->second._blockNum, (char *) vm_physmem + it->second._ppage * VM_PAGESIZE);
					fileDependencies[it->second._filename][it->second._blockNum]._ppage = MAX_PHYSICAL_MEMORY_PAGES;
					danglingFiles.erase(it);
					return possibleEvict;
				} else if(it->second._reference) { // danglingFile is referenced 
					it->second._reference = 0;
					evictClock.push_back(possibleEvict);
				} else { // dangling file is unreferenced and not dirty
					fileDependencies[it->second._filename][it->second._blockNum]._ppage = MAX_PHYSICAL_MEMORY_PAGES;
					danglingFiles.erase(it);
					return possibleEvict;
				}
			} else { // if the ppage isn't a dangling file, it just returns the ppage
				return possibleEvict;
			} // end if-else
		} else {
			extraInfo * temp = &mapOfProcesses[physicalMemoryDependencies[possibleEvict][0]._processNum]._extraInfoMapPtr->eim[physicalMemoryDependencies[possibleEvict][0]._virtualPage];

			if (temp->_reference == 0) { // top of clock is unreferenced
				_(std::cout << "unreferenced. evicting\n";);
				// evict this!
				if (temp->_dirty) {
					char * physical_memory_location = ((char *) vm_physmem + possibleEvict * VM_PAGESIZE);
					if (temp->_isSwapBacked){ // is swap backed
						file_write(nullptr, temp->_blockNum, physical_memory_location);
					} else { // is filebacked
						file_write( (const char*) temp->_filename.c_str() , temp->_blockNum, physical_memory_location);
					}
				}
				clockEvictHelper(true, physicalMemoryDependencies[possibleEvict]);
				if(temp->_isSwapBacked) { // evicted file is swap backed
					_(
						std::cout << "evicting swapbacked\nthis swapback block is: " << temp->_blockNum << std::endl;
						std::cout << "page is " << possibleEvict << std::endl;
						_(
							std::cout << "MAX_NUM_SWAP_BLOCKS: " << MAX_NUM_SWAP_BLOCKS << std::endl;
							std::cout << "# of freeSwapBlocks: " << freeSwapBlocks.size() << std::endl;
						)
					);
					assert(temp->_blockNum != MAX_NUM_SWAP_BLOCKS);
					transferDependencies(physicalMemoryDependencies[possibleEvict], swapBlockDependencies[temp->_blockNum]);
				} else { // evicted file is filebacked
					_(
						std::cout << "evicting filebacked\n";
					);
					fileDependencies[temp->_filename][temp->_blockNum]._ppage = MAX_PHYSICAL_MEMORY_PAGES;
					transferDependencies(physicalMemoryDependencies[possibleEvict], fileDependencies[temp->_filename][temp->_blockNum].dataDependencies);
				}
				_(
					std::cout << "passing back possibleEvict1: " << possibleEvict << std::endl;
				);
				return possibleEvict;
			} else { // top of clock is referenced
				_(std::cout << "referenced. unreferencing\n";);
				assert(temp->_reference == 1);
				assert(possibleEvict > 0 && possibleEvict < MAX_PHYSICAL_MEMORY_PAGES);
				clockEvictHelper(false, physicalMemoryDependencies[possibleEvict]);
				assert(temp->_reference == 0);
				evictClock.push_back(possibleEvict);
			} // end if-else
		} //end if-else
	} // end while
} // end clockEvict()

#endif /* _GLOBALS_H */