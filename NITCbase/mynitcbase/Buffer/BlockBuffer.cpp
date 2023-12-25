#include "BlockBuffer.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

// the declarations for these functions can be found in "BlockBuffer.h"

BlockBuffer::BlockBuffer(int blockNum)
{
    // initialise this.blockNum with the argument
    this->blockNum = blockNum;
}

BlockBuffer::BlockBuffer(char blocktype){
    // allocate a block on the disk and a buffer in memory to hold the new block of
    // given type using getFreeBlock function and get the return error codes if any.
	// * NOTE: this line should be changed later
	int blockType = blocktype == 'R' ? REC : UNUSED_BLK; 
 
	int blockNum = getFreeBlock(blockType);
	if (blockNum < 0 || blockNum >= DISK_BLOCKS) {
		printf("Some Error Has Occured\n");
		
		return;
	}


	int bufferIndex = StaticBuffer::getFreeBuffer(blockNum);
	if (bufferIndex < 0 || bufferIndex >= BUFFER_CAPACITY) {
        printf("Some Error Has Occured\n");
		return;
	}
  
		
    // set the blockNum field of the object to that of the allocated block
    // number if the method returned a valid block number,
    // otherwise set the error code returned as the block number.

	this->blockNum = blockNum;
  

    // (The caller must check if the constructor allocatted block successfully
    // by checking the value of block number field.)
}


// calls the parent class constructor
RecBuffer::RecBuffer(int blockNum) : BlockBuffer::BlockBuffer(blockNum) {}

RecBuffer::RecBuffer() : BlockBuffer('R'){}

// load the block header into the argument pointer
int BlockBuffer::getHeader(HeadInfo *head)
{
    // read the block at this.blockNum into the buffer
    unsigned char *buffer;
    
    // reading the buffer block from cache
    // Disk::readBlock(buffer, this->blockNum);
    int ret = loadBlockAndGetBufferPtr(&buffer);
    if (ret != SUCCESS)
        return ret;

    // populate the numEntries, numAttrs and numSlots fields in *head
    memcpy(&head->pblock, buffer + 4, 4);
    memcpy(&head->lblock, buffer + 8, 4);
    memcpy(&head->rblock, buffer + 12, 4);
    memcpy(&head->numEntries, buffer + 16, 4);
    memcpy(&head->numAttrs, buffer + 20, 4);
    memcpy(&head->numSlots, buffer + 24, 4);

    return SUCCESS;
}

// load the record at slotNum into the argument pointer
int RecBuffer::getRecord(union Attribute *record, int slotNum)
{
    // get the header using this.getHeader() function
    HeadInfo head;
    BlockBuffer::getHeader(&head);

    int attrCount = head.numAttrs;
    int slotCount = head.numSlots;

    // read the block at this.blockNum into a buffer
    unsigned char *buffer;
    // Disk::readBlock(buffer, this->blockNum);
    int ret = loadBlockAndGetBufferPtr(&buffer);
    if (ret != SUCCESS)
        return ret;

    /* record at slotNum will be at offset HEADER_SIZE + slotMapSize + (recordSize * slotNum)
       - each record will have size attrCount * ATTR_SIZE
       - slotMap will be of size slotCount
    */
    int recordSize = attrCount * ATTR_SIZE;
    unsigned char *slotPointer = buffer + (32 + slotCount + (recordSize * slotNum)); // calculate buffer + offset

    // load the record into the rec data structure
    memcpy(record, slotPointer, recordSize);

    return SUCCESS;
}

int RecBuffer::setRecord(union Attribute *rec, int slotNum) {
    unsigned char *bufferPtr;
    /* get the starting address of the buffer containing the block
       using loadBlockAndGetBufferPtr(&bufferPtr). */
       int ret=loadBlockAndGetBufferPtr(&bufferPtr);
       if(ret!=SUCCESS){
        return ret;
       }
   

    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.
        HeadInfo head;
        getHeader(&head);
    /* get the header of the block using the getHeader() function */
        int NumofAtrs=head.numAttrs;
    // get number of attributes in the block.
        int  NumofSlots=head.numSlots;
    // get the number of slots in the block.
        if(slotNum<0 || slotNum>NumofSlots){
            return E_OUTOFBOUND;
        }
      
    // if input slotNum is not in the permitted range return E_OUTOFBOUND.
    
    /* offset bufferPtr to point to the beginning of the record at required
       slot. the block contains the header, the slotmap, followed by all
       the records. so, for example,
       record at slot x will be at bufferPtr + HEADER_SIZE + (x*recordSize)
       copy the record from `rec` to buffer using memcpy
       (hint: a record will be of size ATTR_SIZE * numAttrs)
    */
   unsigned char *buf=bufferPtr+HEADER_SIZE+NumofSlots+(slotNum*ATTR_SIZE*NumofAtrs);
   memcpy(buf,rec,ATTR_SIZE*NumofAtrs);
  


    ret=StaticBuffer::setDirtyBit(this->blockNum);

    if (ret != SUCCESS) {
		printf("some error\n");
		return ret;
	}
    // update dirty bit using setDirtyBit()

    /* (the above function call should not fail since the block is already
       in buffer and the blockNum is valid. If the call does fail, there
       exists some other issue in the code) */
    return SUCCESS;
    // return SUCCESS
}

// load the record at slotNum into the argument pointer
// int RecBuffer::setRecord(union Attribute *record, int slotNum)
// {
//     // get the header using this.getHeader() function
//     HeadInfo head;
//     BlockBuffer::getHeader(&head);

//     int attrCount = head.numAttrs;
//     int slotCount = head.numSlots;

//     // read the block at this.blockNum into a buffer
//     unsigned char *buffer;
//     // Disk::readBlock(buffer, this->blockNum);
//     int ret = loadBlockAndGetBufferPtr(&buffer);
//     if (ret != SUCCESS)
//         return ret;

//     /* record at slotNum will be at offset HEADER_SIZE + slotMapSize + (recordSize * slotNum)
//        - each record will have size attrCount * ATTR_SIZE
//        - slotMap will be of size slotCount
//     */
//     int recordSize = attrCount * ATTR_SIZE;
//     unsigned char *slotPointer = buffer + (32 + slotCount + (recordSize * slotNum)); // calculate buffer + offset

//     // load the record into the rec data structure
//     memcpy(slotPointer, record, recordSize);

//     Disk::writeBlock(buffer, this->blockNum);

//     return SUCCESS;
// }

/*
Used to load a block to the buffer and get a pointer to it.
NOTE: this function expects the caller to allocate memory for the argument (is this so?)
    - in the function, it is simply pointing the buffer pointer to already alocated
    memory, thus it does not require the memory allocated
*/

int BlockBuffer::loadBlockAndGetBufferPtr(unsigned char **buffPtr)
{
    // check whether the block is already present in the buffer
    // using StaticBuffer.getBufferNum()
    int bufferNum = StaticBuffer::getBufferNum(this->blockNum);
    if (bufferNum == E_OUTOFBOUND)
        return E_OUTOFBOUND;

    if (bufferNum == E_BLOCKNOTINBUFFER)
    { // the block is not present in the buffer
        bufferNum = StaticBuffer::getFreeBuffer(this->blockNum);

        // no free space found in the buffer (currently)
        // or some other error occurred in the process
        if (bufferNum == E_OUTOFBOUND || bufferNum == FAILURE)
            return bufferNum;

        Disk::readBlock(StaticBuffer::blocks[bufferNum], this->blockNum);
    }else{
       
        for(int i=0;i<BUFFER_CAPACITY;i++){
            StaticBuffer::metainfo[i].timeStamp++;
            
        }
         StaticBuffer::metainfo[bufferNum].timeStamp=0;
    }

    // store the pointer to this buffer (blocks[bufferNum]) in *buffPtr
    *buffPtr = StaticBuffer::blocks[bufferNum];

    return SUCCESS;
}

/* used to get the slotmap from a record block
NOTE: this function expects the caller to allocate memory for `*slotMap`
*/
int RecBuffer::getSlotMap(unsigned char *slotMap)
{
    unsigned char *bufferPtr;

    // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr().
    int ret = loadBlockAndGetBufferPtr(&bufferPtr);
    if (ret != SUCCESS)
        return ret;

    // get the header of the block using getHeader() function
    RecBuffer recordBlock (this->blockNum);
    struct HeadInfo head;
    recordBlock.getHeader(&head);

    int slotCount = head.numSlots;

    // get a pointer to the beginning of the slotmap in memory by offsetting HEADER_SIZE
    unsigned char *slotMapInBuffer = bufferPtr + HEADER_SIZE;

    // copy the values from `slotMapInBuffer` to `slotMap` (size is `slotCount`)
    for (int slot = 0; slot < slotCount; slot++) 
        *(slotMap+slot)= *(slotMapInBuffer+slot);

    return SUCCESS;
}


int BlockBuffer::setHeader(struct HeadInfo *head){

    unsigned char *bufferPtr;
    // get the starting address of the buffer containing the block using
    // loadBlockAndGetBufferPtr(&bufferPtr).
    int ret=loadBlockAndGetBufferPtr(&bufferPtr);
    if (ret!=SUCCESS){
        return ret;
    }
     

    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    // cast bufferPtr to type HeadInfo*
    struct HeadInfo *bufferHeader = (struct HeadInfo *)bufferPtr;

    bufferHeader->numAttrs=head->numAttrs;
    bufferHeader->numSlots=head->numSlots;
    bufferHeader->numEntries=head->numEntries;
    bufferHeader->blockType=head->blockType;
    bufferHeader->lblock=head->lblock;
    bufferHeader->rblock=head->rblock;
    bufferHeader->pblock=head->pblock;
    // copy the fields of the HeadInfo pointed to by head (except reserved) to
    // the header of the block (pointed to by bufferHeader)
    //(hint: bufferHeader->numSlots = head->numSlots )
     ret=StaticBuffer::setDirtyBit(this->blockNum);
    if (ret!=SUCCESS){
        return ret;
    }
     
    // update dirty bit by calling StaticBuffer::setDirtyBit()
    // if setDirtyBit() failed, return the error code
    return SUCCESS;
    // return SUCCESS;
}

int BlockBuffer::setBlockType(int blockType){

    unsigned char *bufferPtr;
    /* get the starting address of the buffer containing the block
       using loadBlockAndGetBufferPtr(&bufferPtr). */
       int ret =loadBlockAndGetBufferPtr(&bufferPtr);
       if(ret!=SUCCESS){
        return ret;
       }

    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    // store the input block type in the first 4 bytes of the buffer.
    // (hint: cast bufferPtr to int32_t* and then assign it)
    // *((int32_t *)bufferPtr) = blockType;
    *((int32_t *)bufferPtr)=blockType;
    StaticBuffer::blockAllocMap[this->blockNum]=blockType;
    // update the StaticBuffer::blockAllocMap entry corresponding to the
    // object's block number to `blockType`.
    ret=StaticBuffer::setDirtyBit(this->blockNum);
    if (ret!=SUCCESS){
        return ret;
    }
    // update dirty bit by calling StaticBuffer::setDirtyBit()
    // if setDirtyBit() failed
        // return the returned value from the call
    return SUCCESS;
    // return SUCCESS
}
int BlockBuffer::getFreeBlock(int blockType){

    // iterate through the StaticBuffer::blockAllocMap and find the block number
    // of a free block in the disk.

    int blknum=0;
    for(blknum=0;blknum<DISK_BLOCKS;blknum++){
        if(StaticBuffer::blockAllocMap[blknum]==UNUSED_BLK){
            break;
        }
    }
    if (blknum==DISK_BLOCKS){
        return E_DISKFULL;
    }
   
    // if no block is free, return E_DISKFULL.
    this->blockNum=blknum;
    // set the object's blockNum to the block number of the free block.
   int blk= StaticBuffer::getFreeBuffer(this->blockNum);
 
    // find a free buffer using StaticBuffer::getFreeBuffer() .
    HeadInfo head;
    head.pblock=-1;
    head.lblock=-1;
    head.rblock=-1;
    head.numAttrs=0;
    head.numSlots=0;
    head.numEntries=0;
    // initialize the header of the block passing a struct HeadInfo with values
    // pblock: -1, lblock: -1, rblock: -1, numEntries: 0, numAttrs: 0, numSlots: 0
    // to the setHeader() function.

    setHeader(&head);
     
    // update the block type of the block to the input block type using setBlockType().
    setBlockType(blockType);

    return blknum;
    // return block number of the free block.
}

int RecBuffer::setSlotMap(unsigned char *slotMap){
    unsigned char * bufferPtr;
    int ret=loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret!=SUCCESS){
        return ret;
    }
    HeadInfo head;
    getHeader(&head);
    int numSlots=head.numSlots;
    bufferPtr=bufferPtr+HEADER_SIZE;
    memcpy(bufferPtr,slotMap,numSlots);

    ret=StaticBuffer::setDirtyBit(this->blockNum);
    if(ret!=SUCCESS){
        return ret;
    }
    return SUCCESS;
}       

int BlockBuffer::getBlockNum(){
    return this->blockNum;
}


void BlockBuffer::releaseBlock(){

    // if blockNum is INVALID_BLOCK (-1), or it is invalidated already, do nothing
    int blockNum=getBlockNum();
    int bufferNum;
    if(blockNum==INVALID_BLOCKNUM || StaticBuffer::blockAllocMap[blockNum]==UNUSED_BLK){
        return;
    }
        bufferNum=StaticBuffer::getBufferNum(blockNum);
        if(bufferNum!=E_BLOCKNOTINBUFFER){
            StaticBuffer::metainfo[bufferNum].free=true;

        }
        StaticBuffer::blockAllocMap[blockNum]=UNUSED_BLK;
        this->blockNum=INVALID_BLOCKNUM;
    
    // else
        /* get the buffer number of the buffer assigned to the block
           using StaticBuffer::getBufferNum().
           (this function return E_BLOCKNOTINBUFFER if the block is not
           currently loaded in the buffer)
            */

        // if the block is present in the buffer, free the buffer
        // by setting the free flag of its StaticBuffer::tableMetaInfo entry
        // to true.

        // free the block in disk by setting the data type of the entry
        // corresponding to the block number in StaticBuffer::blockAllocMap
        // to UNUSED_BLK.

        // set the object's blockNum to INVALID_BLOCK (-1)
}

int compareAttrs(Attribute attr1, Attribute attr2, int attrType) {
    return attrType == NUMBER ? 
        (attr1.nVal < attr2.nVal ? -1 : (attr1.nVal > attr2.nVal ? 1 : 0)) : 
        strcmp(attr1.sVal, attr2.sVal) ;
    // // return attrType;
}


