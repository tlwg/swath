//
// vmem.cpp - implementation of a class for virtual memory
// Created: 24 May 1996
// Author:  Theppitak Karoonboonyanan
//

#include <string.h>
#include <limits.h>

#include "vmem/vmem.h"
#include "misc/porting.h"

long FileLength(fstream& fs)
{
    streampos prevPos = fs.tellg();
    fs.seekg(0L, ios::end);
    long len = fs.tellg();
    fs.seekg(prevPos, ios::beg);
    return len;
}

//////////////// implementation of PageMap class ////////////////
PageMap::PageMap(const char* swapFileName, ios_base::openmode iosModes)
: swapStream(swapFileName, iosModes | ios::binary) // | ios::bin) // ios::binary)
{
    this->swapFileName = new char[strlen(swapFileName)+1];
    strcpy(this->swapFileName, swapFileName);

    // get initial total pages
    if (swapStream.rdbuf()->is_open()) {
        totalPages = PageNumber(
            (FileLength(swapStream) + VirtualMem::PageSize() - 1)
                / VirtualMem::PageSize()
        );
    } else {
        totalPages = 0;
    }

    // initialize page & frame tables
    resetTables();
}

PageMap::~PageMap()
{
    if (IsSwapFileOpen()) {
        flushSwapFile();
    }
    delete swapFileName;
}

void PageMap::resetTables()
{
    int i;
    for (i = 0; i < MaxPages; i++) {
        pages[i].FrameNo  = NoFrame;
        pages[i].HitCount = 0;
    }
    for (i = 0; i < MaxFrames; i++) {
        frames[i].PageNo    = NoPage;
        frames[i].Frame     = (char*)0;
        frames[i].IsChanged = false;
    }
}

void PageMap::flushSwapFile()
{
    // flush all buffered frames and free all allocated blocks
    for (int i = 0; i < MaxFrames; i++) {
        if (frames[i].PageNo != NoPage && frames[i].IsChanged) {
            flushPage(frames[i].PageNo, frames[i].Frame);
        }
        delete frames[i].Frame;
    }

    resetTables();
}

// read the given page from swap file into frameBuf
void PageMap::retrievePage(PageNumber pageNo, char* frameBuf)
{
    ASSERT(pageNo < MaxPages);
    swapStream.clear();
    swapStream.seekg(long(pageNo)*VirtualMem::PageSize());
    swapStream.read(frameBuf, VirtualMem::PageSize());
    // fill the remainder (if any) of the frame with zeros
    int nRead = swapStream.gcount();
    memset(frameBuf + nRead, 0, VirtualMem::PageSize() - nRead);
}

// flush the given page from frameBuf out to swap file
void PageMap::flushPage(PageNumber pageNo, const char* frameBuf)
{
    ASSERT(pageNo < MaxPages);
    swapStream.clear();
    swapStream.seekp(long(pageNo) * VirtualMem::PageSize());
    swapStream.write(frameBuf, VirtualMem::PageSize());
}

// reserve a free frame, swapping out some page if needed
FrameNumber PageMap::findFreeFrame()
{
    FrameNumber f;
    // find if there is a frame f such that frames[f].PageNo == NoPage
    for (f = 0; f < MaxFrames; f++) {
        if (frames[f].PageNo == NoPage) {
            ASSERT(frames[f].Frame == (char*)0);
            ASSERT(frames[f].IsChanged == false);
            frames[f].Frame = new char[VirtualMem::PageSize()];
            return f;
        }
    }

    // no such frame, try swapping out
    // f := the frame with least HitCount
    unsigned minHit = UINT_MAX;
    for (FrameNumber i = 0; i < MaxFrames; i++) {
        unsigned h = pages[frames[i].PageNo].HitCount;
        if (h < minHit) {
            minHit = h;  f = i;
        }
    }
    // if the frame was changed, flush it
    if (frames[f].IsChanged) {
        flushPage(frames[f].PageNo, frames[f].Frame);
    }
    // cut links and make it a new frame
    pages[frames[f].PageNo].FrameNo = NoFrame;
    frames[f].PageNo    = NoPage;
    frames[f].IsChanged = false;

    return f;
}

// swap the page into a frame and return the frame swapped
FrameNumber PageMap::swapPageIn(PageNumber pageNo)
{
    ASSERT(pageNo < MaxPages);
    FrameNumber f = findFreeFrame();
    pages[pageNo].FrameNo = f;
    frames[f].PageNo = pageNo;
    retrievePage(pageNo, frames[f].Frame);

    // update total pages
    if (pageNo > totalPages)  totalPages = pageNo;

    return f;
}

// have the page been buffered in a frame and return the frame number
FrameNumber PageMap::validFrameNo(PageNumber pageNo)
{
    ASSERT(pageNo < MaxPages);
    FrameNumber f = pages[pageNo].FrameNo;
    return (f == NoFrame) ? swapPageIn(pageNo) : f;
}

// have the page been present in memory and return the pointer to it
const char* PageMap::PageFrame(PageNumber pageNo)
{
    ASSERT(pageNo < MaxPages);
    pages[pageNo].HitCount++;
    return frames[validFrameNo(pageNo)].Frame;
}

// have the page been present in memory and return the pointer to it
char* PageMap::PageFrameRef(PageNumber pageNo)
{
    ASSERT(pageNo < MaxPages);
    pages[pageNo].HitCount++;
    FrameNumber f = validFrameNo(pageNo);
    frames[f].IsChanged = true;
    return frames[f].Frame;
}


//////////////// implementation of VirtualMem class ////////////////

//
// binary data storage bottle-neck
//
uint16 VirtualMem::encodeUInt16(uint16 n)
{
    return IsHighEndianSystem() ? ReverseBytes16(n) : n;
}

uint32 VirtualMem::encodeUInt32(uint32 n)
{
    return IsHighEndianSystem() ? ReverseBytes32(n) : n;
}

int16  VirtualMem::encodeInt16(int16 n)
{
    return IsHighEndianSystem() ? int16(ReverseBytes16(uint16(n))) : n;
}

int32  VirtualMem::encodeInt32(int32 n)
{
    return IsHighEndianSystem() ? int32(ReverseBytes32(uint32(n))) : n;
}


uint16 VirtualMem::decodeUInt16(uint16 n)
{
    return IsHighEndianSystem() ? ReverseBytes16(n) : n;
}

uint32 VirtualMem::decodeUInt32(uint32 n)
{
    return IsHighEndianSystem() ? ReverseBytes32(n) : n;
}

int16  VirtualMem::decodeInt16(int16 n)
{
    return IsHighEndianSystem() ? int16(ReverseBytes16(uint16(n))) : n;
}

int32  VirtualMem::decodeInt32(int32 n)
{
    return IsHighEndianSystem() ? int32(ReverseBytes32(uint32(n))) : n;
}


//
// Memory Writings
//
void VirtualMem::SetByte(Pointer p, byte b)
{
    *(byte*)(pageFrameRef(pageNo(p)) + offset(p)) = b;
}

void VirtualMem::SetWord(Pointer p, word w)
{
    ASSERT(isWordBound(p));
    // p = wordBound(p);
    w = encodeUInt16(w);
    *(word*)(pageFrameRef(pageNo(p)) + offset(p)) = w;
}

void VirtualMem::SetDWord(Pointer p, dword d)
{
    ASSERT(isDWordBound(p));
    // p = dwordBound(p);
    d = encodeUInt32(d);
    *(dword*)(pageFrameRef(pageNo(p)) + offset(p)) = d;
}

void VirtualMem::SetInt8(Pointer p, int8 n)
{
    *(int8*)(pageFrameRef(pageNo(p)) + offset(p)) = n;
}

void VirtualMem::SetInt16(Pointer p, int16 n)
{           
    ASSERT(isWordBound(p));
    // p = wordBound(p);
    n = encodeInt16(n);
    *(int16*)(pageFrameRef(pageNo(p)) + offset(p)) = n;
}

void VirtualMem::SetInt32(Pointer p, int32 n)
{
    ASSERT(isDWordBound(p));
    // p = dwordBound(p);
    n = encodeInt32(n);
    *(int32*)(pageFrameRef(pageNo(p)) + offset(p)) = n;
}

void VirtualMem::SetUInt8(Pointer p, uint8 u)
{
    *(uint8*)(pageFrameRef(pageNo(p)) + offset(p)) = u;
}

void VirtualMem::SetUInt16(Pointer p, uint16 u)
{
    ASSERT(isWordBound(p));
    // p = wordBound(p);
    u = encodeUInt16(u);
    *(uint16*)(pageFrameRef(pageNo(p)) + offset(p)) = u;
}

void VirtualMem::SetUInt32(Pointer p, uint32 u)
{
    ASSERT(isDWordBound(p));
    // p = dwordBound(p);
    u = encodeUInt32(u);
    *(uint32*)(pageFrameRef(pageNo(p)) + offset(p)) = u;
}

void VirtualMem::SetPtr(Pointer p, Pointer ptr)
{
    ASSERT(isDWordBound(p));
    // p = dwordBound(p);
    ptr = encodeUInt32(ptr);
    *(uint32*)(pageFrameRef(pageNo(p)) + offset(p)) = ptr;
}

//
// Memory Readings
//
byte VirtualMem::Byte(Pointer p)
{
    return *(const byte*)(pageFrame(pageNo(p)) + offset(p));
}

word VirtualMem::Word(Pointer p)
{
    ASSERT(isWordBound(p));
    // p = wordBound(p);
    return decodeUInt16(*(const word*)(pageFrame(pageNo(p)) + offset(p)));
}

dword VirtualMem::DWord(Pointer p)
{
    ASSERT(isDWordBound(p));
    // p = dwordBound(p);
    return decodeUInt32(*(const dword*)(pageFrame(pageNo(p)) + offset(p)));
}

int8 VirtualMem::Int8(Pointer p)
{
    return *(const int8*)(pageFrame(pageNo(p)) + offset(p));
}

int16 VirtualMem::Int16(Pointer p)
{
    ASSERT(isWordBound(p));
    // p = wordBound(p);
    return decodeInt16(*(const int16*)(pageFrame(pageNo(p)) + offset(p)));
}

int32 VirtualMem::Int32(Pointer p)
{
    ASSERT(isDWordBound(p));
    // p = dwordBound(p);
    return decodeInt32(*(const int32*)(pageFrame(pageNo(p)) + offset(p)));
}

uint8 VirtualMem::UInt8(Pointer p)
{
    return *(const uint8*)(pageFrame(pageNo(p)) + offset(p));
}

uint16 VirtualMem::UInt16(Pointer p)
{
    ASSERT(isWordBound(p));
    // p = wordBound(p);
    return decodeUInt16(*(const uint16*)(pageFrame(pageNo(p)) + offset(p)));
}

uint32 VirtualMem::UInt32(Pointer p)
{
    ASSERT(isDWordBound(p));
    // p = dwordBound(p);
    return decodeUInt32(*(const uint32*)(pageFrame(pageNo(p)) + offset(p)));
}

Pointer VirtualMem::Ptr(Pointer p)
{
    ASSERT(isDWordBound(p));
    // p = dwordBound(p);
    return decodeUInt32(*(const uint32*)(pageFrame(pageNo(p)) + offset(p)));
}


// return the next doubleword-bound block after the place pointed by p
// which occupies a single page
Pointer VirtualMem::BlockStartingAt(Pointer p, int nSize)
{
    ASSERT(nSize < pageSize);
    p = dwordBound(p);
    PageNumber pgNo     = pageNo(p);
    PageOffset pgOffset = offset(p);
    if ((pgOffset + nSize) & (~offsetMask)) {  // page overflow
        // skip to next page
        pgNo++;
        pgOffset = 0;
    }
    return makePointer(pgNo, pgOffset);
}

// restriction: block must be within a single page (otherwise NULL is returned)
const char* VirtualMem::block(Pointer p, int nSize)
{
    p = BlockStartingAt(p, nSize);
    return pageFrame(pageNo(p)) + offset(p);
}

// similar to VirtualMem::block(), but the reference
char* VirtualMem::blockRef(Pointer p, int nSize)
{
    p = BlockStartingAt(p, nSize);
    return pageFrameRef(pageNo(p)) + offset(p);
}

int VirtualMem::BlockRead(Pointer p, char buff[], int nBytes)
{
    memcpy(buff, block(p, nBytes), nBytes);
    return nBytes;
}

int VirtualMem::BlockWrite(Pointer p, const char buff[], int nBytes)
{
    memcpy(blockRef(p, nBytes), buff, nBytes);
    return nBytes;
}


//////////////// implementation of VirtualHeap class ////////////////

VirtualHeap::VirtualHeap(const char* swapFileName, ios_base::openmode iosModes)
: VirtualMem(swapFileName, iosModes)
{
    if (TotalPages() == 0) {
        // init total size to 4 bytes (the total size itself)
        setTotalSize(4);
    }
}

Pointer VirtualHeap::NewBlock(int size)
{
    Pointer p = BlockStartingAt(Pointer(totalSize()), size);
    setTotalSize(p + size);
    return p;
}

void VirtualHeap::DeleteBlock(Pointer)
{
    // do nothing
}


#ifdef TEST_BED

#include <iostream.h>

static VirtualMem vm("vm1.vm", ios::in|ios::out);

int main()
{

    vm.SetUInt16(0,      0xabcd);
    vm.SetUInt32(16000,  0xcdba9876);
    vm.SetUInt16(6000,   0xcafe);
    vm.SetUInt16(10000,  0xface);
    vm.SetUInt16(6002,   0xfeed);
    vm.SetUInt32(4,      0xbad0beef);
    vm.SetUInt32(16020,  0xdeaddeaf);

    if (
        vm.UInt16(0)     == 0xabcd      &&
        vm.UInt32(16000) == 0xcdba9876  &&
        vm.UInt16(6000)  == 0xcafe      &&
        vm.UInt16(10000) == 0xface      &&
        vm.UInt16(6002)  == 0xfeed      &&
        vm.UInt32(4)     == 0xbad0beef  &&
        vm.UInt32(16020) == 0xdeaddeaf
    ) {
        cout << "OK." << endl;
    }

    return 0;
}

#endif // TEST_BED
