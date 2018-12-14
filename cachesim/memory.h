#ifndef MEMORY_H_
#define MEMORY_H_
#include <stdio.h>

/*
  This file contains basic cache and block structures.
  It also defines some global constants
  for use in the classes and main program.
*/

extern unsigned int clockX;
extern unsigned int numMisses;
extern int cache_org;

//=================
//==  Constants
//=================
#define WORDS_PER_BLOCK 4
#define BLOCKS_IN_MEMORY 512
#define BLOCKS_IN_CACHE 8
#define NUM_OF_SET 4
#define BLOCKS_IN_SET 2 

#define CACHE_ACCESS_CYCLES 2
#define MM_ACCESS_CYCLES    100

#define WORD_OFFSET_BITS_FULLY  2
#define INDEX_BITS_FULLY        0
#define TAG_BITS_FULLY          30

#define WORD_OFFSET_MASK_FULLY  0x00000003
#define INDEX_MASK_FULLY        0x00000000
#define TAG_MASK_FULLY          0xFFFFFFFC

#define WORD_OFFSET_BITS_TWOWAY 2
#define INDEX_BITS_TWOWAY       2
#define TAG_BITS_TWOWAY         28

#define WORD_OFFSET_MASK_TWOWAY 0x00000003
#define INDEX_MASK_TWOWAY       0x0000000C
#define TAG_MASK_TWOWAY         0xFFFFFFF0

#define WORD_OFFSET_BITS_DIRECT 2
#define INDEX_BITS_DIRECT       3
#define TAG_BITS_DIRECT         27

#define WORD_OFFSET_MASK_DIRECT 0x00000003
#define INDEX_MASK_DIRECT       0x0000001C
#define TAG_MASK_DIRECT         0xFFFFFFE0
//-------------------------------------

// block
typedef struct {
  int tag;
  int data[WORDS_PER_BLOCK];
  int last_used; // initialize to 0 ;
  int valid; // initialize to 0;
} Block; 

// main memory
typedef struct {
  Block blocks[BLOCKS_IN_MEMORY];
} MainMem;

MainMem mm;

enum cache_org {DIRECT = 0, FULLY = 1, TWOWAY = 2};

// cache
typedef struct {
  Block cblocks[BLOCKS_IN_CACHE];
} Cache;

// memory
typedef struct {
  Cache myCache;
} Memory;

Memory m;

int getData (int address); // load
int getDataFromCache(int tag, int index, int word_offset, int num_of_sets);
void putData (int address, int value); // store
void putDataToCache(int tag, int index, int word_offset, int num_of_sets, int data);
void resetClock(void); // set timer to zero
void resetValidBits(void);
void showCacheAddress(); // show cache contents
void showCacheAddressHelper(int num_of_sets, int num_word_offset_bits, int num_index_bits);
void printCacheOrg (int org); // print cache org

#endif