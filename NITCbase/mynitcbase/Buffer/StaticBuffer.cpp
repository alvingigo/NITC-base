#include "StaticBuffer.h"

// the declarations for this class can be found at "StaticBuffer.h"

unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];
unsigned char StaticBuffer::blockAllocMap[DISK_BLOCKS];

StaticBuffer::StaticBuffer(){
    unsigned char buffer[BLOCK_SIZE];
    int blockAllocMapSlot=0;
    for(int blknum=0;blknum<4;blknum++){
        Disk::readBlock(buffer,blknum);
        for (int slot = 0; slot < BLOCK_SIZE; slot++, blockAllocMapSlot++)
			StaticBuffer::blockAllocMap[blockAllocMapSlot] = buffer[slot];
    }
   
    // initialise all blocks as free


    for (int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++) {
        metainfo[bufferIndex].free = true;
        metainfo[bufferIndex].dirty=false;
        metainfo[bufferIndex].blockNum=-1;
        metainfo[bufferIndex].timeStamp=-1;
    }
}

/*
At this stage, we are not writing back from the buffer to the disk since we are
not modifying the buffer. So, we will define an empty destructor for now. In
subsequent stages, we will implement the write-back functionality here.
*/
StaticBuffer::~StaticBuffer() {
   for (int blockIndex = 0, blockAllocMapSlot = 0; blockIndex < 4; blockIndex++) {
		unsigned char buffer [BLOCK_SIZE];

		for (int slot = 0; slot < BLOCK_SIZE; slot++, blockAllocMapSlot++) 
			buffer[slot] = blockAllocMap[blockAllocMapSlot];

		Disk::writeBlock(buffer, blockIndex);
	}



    for (int bufferIndex=0;bufferIndex<BUFFER_CAPACITY;bufferIndex++){
        if(metainfo[bufferIndex].free=false && metainfo[bufferIndex].dirty==true){
            Disk::writeBlock(blocks[bufferIndex],metainfo[bufferIndex].blockNum);
        }
    }


}

// int StaticBuffer::getFreeBuffer(int blockNum) {
//     if (blockNum < 0 || blockNum > DISK_BLOCKS) return E_OUTOFBOUND;

//     int allocatedBuffer = 0;

//     // iterate through all the blocks in the StaticBuffer
//     // find the first free block in the buffer (check metainfo)
//     // assign allocatedBuffer = index of the free block
//     for (; allocatedBuffer < BUFFER_CAPACITY; allocatedBuffer++)
//         if (metainfo[allocatedBuffer].free) break;

//     if (allocatedBuffer == BUFFER_CAPACITY) return FAILURE;

//     metainfo[allocatedBuffer].free = false;
//     metainfo[allocatedBuffer].blockNum = blockNum;

//     return allocatedBuffer;
// }


int StaticBuffer::getFreeBuffer(int blockNum){
    // Check if blockNum is valid (non zero and less than DISK_BLOCKS)
    // and return E_OUTOFBOUND if not valid.
    if (blockNum < 0 || blockNum > DISK_BLOCKS)
        return E_OUTOFBOUND;

    // increase the timeStamp in metaInfo of all occupied buffers.
    int  alocbuf=0;
    for(alocbuf=0;alocbuf<BUFFER_CAPACITY;alocbuf++){
        if(metainfo[alocbuf].free==false){
            metainfo[alocbuf].timeStamp++;
        }
    }

    // let bufferNum be used to store the buffer number of the free/freed buffer.
    int bufferNum=-1;

    // iterate through metainfo and check if there is any buffer free
    for(int i=0;i<BUFFER_CAPACITY;i++){
        if(metainfo[i].free){
            bufferNum=i;
            break;
        }
    }
    int largetime=-1;
    // if a free buffer is available, set bufferNum = index of that free buffer.
    if(bufferNum==-1){
        for(int i=0;i<BUFFER_CAPACITY;i++){
            if (metainfo[i].timeStamp>largetime){
                largetime=metainfo[i].timeStamp;
                bufferNum=i;
            }
        }
        if(metainfo[bufferNum].dirty){
            Disk::writeBlock(StaticBuffer::blocks[bufferNum],metainfo[bufferNum].blockNum);
        }
    }

    // if a free buffer is not available,
    //     find the buffer with the largest timestamp
    //     IF IT IS DIRTY, write back to the disk using Disk::writeBlock()
    //     set bufferNum = index of this buffer
    metainfo[bufferNum].free=false;
    metainfo[bufferNum].dirty=false;
    metainfo[bufferNum].blockNum=blockNum;
    metainfo[bufferNum].timeStamp=0;
    // update the metaInfo entry corresponding to bufferNum with
    // free:false, dirty:false, blockNum:the input block number, timeStamp:0.
    return bufferNum;
    // return the bufferNum.
}

/* Get the buffer index where a particular block is stored
   or E_BLOCKNOTINBUFFER otherwise
*/
int StaticBuffer::getBufferNum(int blockNum) {
    // Check if blockNum is valid (between zero and DISK_BLOCKS)
    // and return E_OUTOFBOUND if not valid.
    if (blockNum < 0 || blockNum >= DISK_BLOCKS) return E_OUTOFBOUND;

    // find and return the bufferIndex which corresponds to blockNum (check metainfo)
    for (int bufferBlock = 0; bufferBlock < BUFFER_CAPACITY; bufferBlock++){
        if (metainfo[bufferBlock].free == false 
            && metainfo[bufferBlock].blockNum == blockNum) return bufferBlock;
    }

    // if block is not in the buffer
    return E_BLOCKNOTINBUFFER;
}


int StaticBuffer::setDirtyBit(int blockNum){
    // find the buffer index corresponding to the block using getBufferNum().
    int buffernum=StaticBuffer::getBufferNum(blockNum);

    if (buffernum==E_BLOCKNOTINBUFFER){
        return E_BLOCKNOTINBUFFER;
    }
    // if block is not present in the buffer (bufferNum = E_BLOCKNOTINBUFFER)
    //     return E_BLOCKNOTINBUFFER
    if(buffernum==E_OUTOFBOUND){
        return E_OUTOFBOUND;

    }
    // if blockNum is out of bound (bufferNum = E_OUTOFBOUND)
    //     return E_OUTOFBOUND
    metainfo[buffernum].dirty=true;
    // else
    //     (the bufferNum is valid)
    //     set the dirty bit of that buffer to true in metainfo
    return SUCCESS;
    // return SUCCESS
}