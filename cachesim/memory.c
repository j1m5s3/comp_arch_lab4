#include "memory.h"

unsigned int clockX;
unsigned int numMisses;
int cache_org;

void resetClock()
{
  clockX = 0;
  numMisses = 0;
}

void resetValidBits(void) {
  for (int i = 0; i < BLOCKS_IN_CACHE; i++) {
    m.myCache.cblocks[i].valid = 0;
  }
}

void printCacheOrg(int org)
{
  printf("Cache Organization: ");
  if (org == DIRECT)
    printf ("DIRECT MAPPED\n");
  else if (org == TWOWAY)
    printf ("TWO-WAY SET ASSOCIATIVE\n");
  else if (org == FULLY)
    printf ("FULLY ASSOCIATIVE\n");
  else
    printf ("ERROR: WRONG CACHE ORG\n");
}

// show cache contents: DIRECT or FULLY
void showCacheAddress()
{
  switch (cache_org) {
    case DIRECT:
      showCacheAddressHelper(BLOCKS_IN_CACHE, WORD_OFFSET_BITS_DIRECT, INDEX_BITS_DIRECT);
      break;
    case TWOWAY:
      showCacheAddressHelper(NUM_OF_SET, WORD_OFFSET_BITS_TWOWAY, INDEX_BITS_TWOWAY);
      break;
    case FULLY:
      showCacheAddressHelper(1, WORD_OFFSET_BITS_FULLY, INDEX_BITS_FULLY);
      break;
    default:
      printf ("ERROR: WRONG CACHE ORG\n");
      break;
  }
}

void showCacheAddressHelper(int num_of_sets, int num_word_offset_bits, int num_index_bits) {
  for (int j = 0; j < BLOCKS_IN_CACHE; j++) {
    printf("Address in block %d (set %d, way %d): ", j, j % num_of_sets, j / num_of_sets);
    for (int k = 0; k < WORDS_PER_BLOCK; k++) {
      int tag_part = m.myCache.cblocks[j].tag << (num_word_offset_bits + num_index_bits);
      int index_part = (j % num_of_sets) << num_word_offset_bits;
      int word_offset_part = k;
      int address = tag_part | index_part | word_offset_part;

      printf("%d ", address);
    }
    printf("lru: %d ", m.myCache.cblocks[j].last_used);
    printf("\n");
  }
}

int getData (int address)                // load
{
  int data;
  int word_offset;
  int index;
  int tag;
  
  switch (cache_org) {
    case DIRECT:
      word_offset = address & WORD_OFFSET_MASK_DIRECT;
      index = (address & INDEX_MASK_DIRECT) >> WORD_OFFSET_BITS_DIRECT;
      tag = (address & TAG_MASK_DIRECT) >> (WORD_OFFSET_BITS_DIRECT + INDEX_BITS_DIRECT);
      data = getDataFromCache(tag, index, word_offset, BLOCKS_IN_CACHE);
      break;
    case TWOWAY:
      word_offset = address & WORD_OFFSET_MASK_TWOWAY;
      index = (address & INDEX_MASK_TWOWAY) >> WORD_OFFSET_BITS_TWOWAY;
      tag = (address & TAG_MASK_TWOWAY) >> (WORD_OFFSET_BITS_TWOWAY + INDEX_BITS_TWOWAY);
      data = getDataFromCache(tag, index, word_offset, NUM_OF_SET);
      break;
    case FULLY:
      word_offset = address & WORD_OFFSET_MASK_FULLY;
      index = (address & INDEX_MASK_FULLY) >> WORD_OFFSET_BITS_FULLY;
      tag = (address & TAG_MASK_FULLY) >> (WORD_OFFSET_BITS_FULLY + INDEX_BITS_FULLY);
      data = getDataFromCache(tag, index, word_offset, 1);
      break;
    default:
      printf("ERROR: WRONG CACHE ORG\n");
      break;
  }

  return data;
}

int getDataFromCache(int tag, int index, int word_offset, int num_of_sets) {
  int lru_index = -1;
  int lru_counter = -1;
  int return_index = -1;

  /* Search in possible locations. */
  for (int possibleLocation = index; possibleLocation < BLOCKS_IN_CACHE; possibleLocation += num_of_sets) {
    if (m.myCache.cblocks[possibleLocation].tag == tag && m.myCache.cblocks[possibleLocation].valid) {
      return_index = possibleLocation;
      m.myCache.cblocks[possibleLocation].last_used = 0;
    } else {
      m.myCache.cblocks[possibleLocation].last_used += 1;
    }

    if (m.myCache.cblocks[possibleLocation].last_used > lru_counter) {
      lru_index = possibleLocation;
      lru_counter = m.myCache.cblocks[possibleLocation].last_used;
    }
  }

  if (return_index != -1) {
    /* Cache Hit */
    clockX += CACHE_ACCESS_CYCLES;
    return m.myCache.cblocks[return_index].data[word_offset];
  }

  /* Cache miss. Load from main memory. */
  numMisses++;
  clockX += MM_ACCESS_CYCLES;
  m.myCache.cblocks[lru_index].tag = tag;
  m.myCache.cblocks[lru_index].valid = 1;
  m.myCache.cblocks[lru_index].last_used = 0;
  return m.myCache.cblocks[lru_index].data[word_offset];
}

void putData (int address, int value)     // store
{
  int word_offset;
  int index;
  int tag;

  switch (cache_org) {
    case DIRECT:
      word_offset = address & WORD_OFFSET_MASK_DIRECT;
      index = (address & INDEX_MASK_DIRECT) >> WORD_OFFSET_BITS_DIRECT;
      tag = (address & TAG_MASK_DIRECT) >> (WORD_OFFSET_BITS_DIRECT + INDEX_BITS_DIRECT);
      putDataToCache(tag, index, word_offset, BLOCKS_IN_CACHE, value);
      break;
    case TWOWAY:
      word_offset = address & WORD_OFFSET_MASK_TWOWAY;
      index = (address & INDEX_MASK_TWOWAY) >> WORD_OFFSET_BITS_TWOWAY;
      tag = (address & TAG_MASK_TWOWAY) >> (WORD_OFFSET_BITS_TWOWAY + INDEX_BITS_TWOWAY);
      putDataToCache(tag, index, word_offset, NUM_OF_SET, value);
      break;
    case FULLY:
      word_offset = address & WORD_OFFSET_MASK_FULLY;
      index = (address & INDEX_MASK_FULLY) >> WORD_OFFSET_BITS_FULLY;
      tag = (address & TAG_MASK_FULLY) >> (WORD_OFFSET_BITS_FULLY + INDEX_BITS_FULLY);
      putDataToCache(tag, index, word_offset, 1, value);
      break;
    default:
      printf("ERROR: WRONG CACHE ORG\n");
      break;
  }
}

void putDataToCache(int tag, int index, int word_offset, int num_of_sets, int data) {
  int lru_index = -1;
  int lru_counter = -1;
  int write_index = -1;

  /* Search in possible locations. */
  for (int possibleLocation = index; possibleLocation < BLOCKS_IN_CACHE; possibleLocation += num_of_sets) {
    if (m.myCache.cblocks[possibleLocation].tag == tag && m.myCache.cblocks[possibleLocation].valid) {
      write_index = possibleLocation;
      m.myCache.cblocks[possibleLocation].last_used = 0;
    } else {
      m.myCache.cblocks[possibleLocation].last_used += 1;
    }

    if (m.myCache.cblocks[possibleLocation].last_used > lru_counter) {
      lru_index = possibleLocation;
      lru_counter = m.myCache.cblocks[possibleLocation].last_used;
    }
  }

  if (write_index != -1) {
    /* Cache Hit */
    clockX += CACHE_ACCESS_CYCLES;
    m.myCache.cblocks[write_index].data[word_offset] = data;

    /* Write through to memory */
    clockX += MM_ACCESS_CYCLES;
    return;
  }

  /* Cache miss. Write through to main memory and load from there. */
  numMisses++;
  clockX += 2 * MM_ACCESS_CYCLES;
  m.myCache.cblocks[lru_index].tag = tag;
  m.myCache.cblocks[lru_index].valid = 1;
  m.myCache.cblocks[lru_index].last_used = 0;
  m.myCache.cblocks[lru_index].data[word_offset] = data;
}