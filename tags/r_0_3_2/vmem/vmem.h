//
// vmem.h - declaration of a class for virtual memory
// Created: 23 May 1996
// Author:  Theppitak Karoonboonyanan
//
// Modification log:
//   22 Jul 1996 - add class VirtualHeap
//

#ifndef VMEM_INC
#define VMEM_INC

#include "misc/typedefs.h"
#include <fstream>

using namespace std;

//////////////
// Contents //
//////////////
class VirtualMem;
class VirtualHeap;

//////////////////////
// General TypeDefs //
//////////////////////
typedef uint32 Pointer;
typedef uint16 PageNumber;
typedef uint16 PageOffset;

typedef int    FrameNumber;

const FrameNumber NoFrame = -1;
const PageNumber  NoPage  = PageNumber(~0);

//
// Page Map Table
// --------------
// For quick access to the memory block allocated for a page, the page number
// is used to index into a "page map table" to check if the page has already
// been swapped in. If so, the block is immediately used. If not, the page
// must be swapped in first.
//
// To manage the blocks efficiently, an intermediate "frame map table" is
// defined so that all blocks are registered in this table, and the "pointers"
// from the page map table will indirectly point to their block through
// entries in this frame map table.
//
//         page table                frame table
//      +--------------+          +-------+-------+
//      |              |   +----->|       |       |         page block
//      |              |   |  +-------    |    --------->+--------------+
//      +------+-------+   |  |   +-------+-------+      |              |
//      |Other |    -------+  |   |               |      |              |
//      |Attr. |       |<-----+   |               |      :              :
//      +------+-------+          +---------------+      |              |
//      |              |          |               |      |              |
//      :              :          :               :      +--------------+
//
//

struct PageMapEntry {
    FrameNumber FrameNo;
    unsigned    HitCount;
};

struct FrameMapEntry {
    PageNumber PageNo;
    char*      Frame;
    bool       IsChanged;
};

class PageMap {
public:  // public types & constants
    enum {
        MaxPages  = 8192,
        MaxFrames = 3 // 16
    };

public:  // public functions
    PageMap(const char* swapFileName, ios_base::openmode iosModes);
    ~PageMap();

    // swap file status & operations (same meaning as corresponding ios funcs)
    bool        OpenSwapFile(const char* swapFileName, ios_base::openmode iosModes);
    bool        CloseSwapFile();
    const char* SwapFileName() const;
    bool        IsSwapFileOpen();
    bool        IsSwapFileGood() const;
    bool        IsSwapFileBad()  const;
    bool        IsSwapFileFail() const;

    // total pages being used
    PageNumber TotalPages() const;

    // page access & preparation (returns pointer to page beginning)
    const char* PageFrame(PageNumber pageNo);
    char*       PageFrameRef(PageNumber pageNo);

private:  // private functions
    void        resetTables();
    FrameNumber validFrameNo(PageNumber pageNo);  // prepare valid frame
    void        retrievePage(PageNumber pageNo, char* frameBuff);
    void        flushPage(PageNumber pageNo, const char* frameBuff);
    void        flushSwapFile();
    FrameNumber findFreeFrame();
    FrameNumber swapPageIn(PageNumber pageNo);

private:  // private data
    char*         swapFileName;
    fstream  swapStream;
    PageNumber    totalPages;
    PageMapEntry  pages[MaxPages];
    FrameMapEntry frames[MaxFrames];
};

// swap file status & operations
inline bool PageMap::OpenSwapFile(const char* swapFileName, ios_base::openmode iosModes)
{
    swapStream.open(swapFileName, iosModes|ios_base::binary);
    return (bool)swapStream.rdbuf()->is_open();
}
inline bool PageMap::CloseSwapFile()
    { flushSwapFile(); swapStream.close(); return !swapStream.fail(); }
inline const char* PageMap::SwapFileName() const  { return swapFileName; }
inline bool PageMap::IsSwapFileOpen()
    { return swapStream.rdbuf()->is_open(); }
inline bool PageMap::IsSwapFileGood() const { return swapStream.good(); }
inline bool PageMap::IsSwapFileBad()  const { return swapStream.bad();  }
inline bool PageMap::IsSwapFileFail() const { return swapStream.fail(); }

// total pages being used
inline PageNumber PageMap::TotalPages() const  { return totalPages; }

//
// Virtual Memory
// --------------
//
// [History]: This class was originally for speeding up disk access operations;
// that is, it should have been named DiskCache or so on. But, for the sake of
// syntactic convinience in using it, it was then reconceptualized as a virtual
// memory. The idea was just in reverse: instead of viewing it as data
// retrieval from the disk, we can imagine a large block of memory we wish to
// use, which is then stored in the disk file, but all its parts are always
// ready for us to access, by some magic. And by setting some parameters, disk
// caching is its side-effect.
//
// The organization of the class is similar to that of virtual memory in
// operating systems (See Deitel, Operating Systems, Addison-Wesley, 1990)
//
// An implementation detail is that, to avoid accessing blocks which overlap
// the boundary between any two pages, page size must be multiple of doubleword
// size, and when accessing words or doublewords, their address must be
// word-aligned and doubleword-aligned respectively. And, when accessing a
// large block, it must not lie in more than one page.
//
// Another enhancement is for data portability. Since there are two different
// systems of storing integers: high-endian (such as Motorola 68xxx, Sun Sparc)
// and low-endian (such as Intel 80x86 and Pentium), we must select one to
// use with our swap files, so that they can be transferred between systems.
// In this implementation, we choose low-endian because the program is intended
// to be run upon Intel family microprocessors.
//

class VirtualMem {
public:  // public functions
    VirtualMem(const char* swapFileName, ios_base::openmode iosModes);

    // Swap file status (same meaning as the corresponding ios funcs)
    bool        OpenSwapFile(const char* swapFileName, ios_base::openmode iosModes);
    bool        CloseSwapFile();
    const char* SwapFileName() const;
    bool        IsSwapFileOpen();
    bool        IsSwapFileGood() const;
    bool        IsSwapFileBad()  const;
    bool        IsSwapFileFail() const;

    // Parameters
    static int PageSize();

    // total pages being used
    PageNumber TotalPages() const;

    // Memory Writing
    void SetByte  (Pointer p, byte   b);
    void SetWord  (Pointer p, word   w);
    void SetDWord (Pointer p, dword  d);
    void SetInt8  (Pointer p, int8   n);
    void SetInt16 (Pointer p, int16  n);
    void SetInt32 (Pointer p, int32  n);
    void SetUInt8 (Pointer p, uint8  u);
    void SetUInt16(Pointer p, uint16 u);
    void SetUInt32(Pointer p, uint32 u);
    void SetPtr   (Pointer p, Pointer ptr);

    // Memory Reading
    byte    Byte(Pointer p);
    word    Word  (Pointer p);
    dword   DWord (Pointer p);
    int8    Int8  (Pointer p);
    int16   Int16 (Pointer p);
    int32   Int32 (Pointer p);
    uint8   UInt8 (Pointer p);
    uint16  UInt16(Pointer p);
    uint32  UInt32(Pointer p);
    Pointer Ptr   (Pointer p);

    // Block Allocation
    Pointer BlockStartingAt(Pointer p, int nSize);

    // Block Access
    int BlockRead(Pointer p, char buf[], int nBytes);
    int BlockWrite(Pointer p, const char buf[], int nBytes);

private:  // private types
    enum {
        // offset extractions
        offsetBits = 12,    // 4KB per page
        offsetMask = ~((~0L)<<offsetBits),
        pageSize   = (1L)<<offsetBits,

        // offset alignments
        wordBoundMask  = (~0L)<<1,
        dwordBoundMask = (~0L)<<2
    };

private:  // private functions
    // address conversions
    static PageNumber pageNo(Pointer p);
    static PageOffset offset(Pointer p);
    static Pointer    makePointer(PageNumber pgNo, PageOffset offset);

    // address alignments
    static bool    isWordBound(Pointer p);
    static bool    isDWordBound(Pointer p);
    static Pointer wordBound(Pointer p);
    static Pointer dwordBound(Pointer p);

    // binary data storage bottle-neck
    static uint16 encodeUInt16(uint16 n);
    static uint32 encodeUInt32(uint32 n);
    static int16  encodeInt16(int16 n);
    static int32  encodeInt32(int32 n);

    static uint16 decodeUInt16(uint16 n);
    static uint32 decodeUInt32(uint32 n);
    static int16  decodeInt16(int16 n);
    static int32  decodeInt32(int32 n);

    // page preparations for memory access
    const char* pageFrame(PageNumber pageNo);
    char*       pageFrameRef(PageNumber pageNo);

    // virtual memory block preparations
    const char* block(Pointer p, int nSize);
    char*       blockRef(Pointer p, int nSize);

private:  // private data
    PageMap  pgMap;
};

// constructor
inline VirtualMem::VirtualMem(const char* swapFileName, ios_base::openmode iosModes)
    : pgMap(swapFileName, iosModes) {}

// parameters
inline int VirtualMem::PageSize()  { return pageSize; }

// swap file status
inline bool VirtualMem::OpenSwapFile(const char* swapFileName, ios_base::openmode iosModes)
    { return pgMap.OpenSwapFile(swapFileName, iosModes); }
inline bool VirtualMem::CloseSwapFile()        { return pgMap.CloseSwapFile(); }
inline const char* VirtualMem::SwapFileName() const
    { return pgMap.SwapFileName(); }
inline bool VirtualMem::IsSwapFileOpen() { return pgMap.IsSwapFileOpen(); }
inline bool VirtualMem::IsSwapFileGood() const {return pgMap.IsSwapFileGood();}
inline bool VirtualMem::IsSwapFileBad()  const {return pgMap.IsSwapFileBad(); }
inline bool VirtualMem::IsSwapFileFail() const {return pgMap.IsSwapFileFail();}

// total pages being used
inline PageNumber VirtualMem::TotalPages() const { return pgMap.TotalPages(); }

// pointer calculations
inline PageNumber VirtualMem::pageNo(Pointer p)
    { return PageNumber(p >> offsetBits); }
inline PageOffset VirtualMem::offset(Pointer p)
    { return PageOffset(p & offsetMask); }
inline Pointer VirtualMem::makePointer(PageNumber pgNo, PageOffset offset)
    { return Pointer((pgNo << offsetBits) | (offset & offsetMask)); }

// returns next word-aligned address
inline bool VirtualMem::isWordBound(Pointer p)
    { return (p & ~wordBoundMask) == 0; }
inline bool VirtualMem::isDWordBound(Pointer p)
    { return (p & ~dwordBoundMask) == 0; }
inline Pointer VirtualMem::wordBound(Pointer p)
    { return (p + sizeof(word) - 1) & wordBoundMask; }
// returns next doubleword-aligned address
inline Pointer VirtualMem::dwordBound(Pointer p)
    { return (p + sizeof(dword) - 1) & dwordBoundMask; }

// page frame preparations
inline const char* VirtualMem::pageFrame(PageNumber pageNo)
    { return pgMap.PageFrame(pageNo); }

inline char* VirtualMem::pageFrameRef(PageNumber pageNo)
    { return pgMap.PageFrameRef(pageNo); }


//
// Virtual Heap
// ------------
// Provides a heap of variable-sized blocks upon VirtualMem
// Blocks can be allocated, but, at present, cannot be freed.
//
// The first 4 bytes of the heap is reserved for keeping the total size of the
// heap being used.
//

class VirtualHeap : public VirtualMem {
public:
    VirtualHeap(const char* swapFileName, ios_base::openmode iosModes);

    Pointer NewBlock(int size);
    void    DeleteBlock(Pointer p);

private:
    uint32  totalSize();
    void    setTotalSize(uint32 s);
};

inline uint32 VirtualHeap::totalSize()            { return UInt32(0); }
inline void   VirtualHeap::setTotalSize(uint32 s) { SetUInt32(0, s); }


#endif // VMEM_INC
