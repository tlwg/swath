//
// dataheap.h - variable-sized data heap on VirtualMem
// Created: 31 Jul 1996 (splitted from dict.h)
// Author:  Theppitak Karoonboonyanan
//

#ifndef DATAHEAP_INC
#define DATAHEAP_INC

#include "vmem/vmem.h"

////////////////
//  Contents  //
////////////////

class DataHeap;
class HeapAssociated;
class CharBlock;


//
// DataHeap
// --------
//
typedef uint32 DataPtr;

class DataHeap : public VirtualHeap {
public:
    DataHeap(const char* swapFileName, int iosModes);
};

inline DataHeap::DataHeap(const char* swapFileName, int iosModes)
: VirtualHeap(swapFileName, iosModes) {}


//
// HeapAssociated
// --------------
//
class HeapAssociated {
public:
    DataHeap& GetDataHeap() const { return dataHeap; }
    Pointer   GetEntryPtr() const { return entry; }

    void      Attach(Pointer aEntry);
    Pointer   Detach();

    void      Delete();        // delete the object in heap

protected:
    // never directly instantiate this class
    HeapAssociated(DataHeap& aHeap);                  // Default c-tor
    HeapAssociated(DataHeap& aHeap, Pointer aEntry);  // Associating c-tor

    void    Create(int size);  // to be called by derived class' creating c-tor

private:
    DataHeap& dataHeap;
    Pointer   entry;
};

inline HeapAssociated::HeapAssociated(DataHeap& aHeap)
: dataHeap(aHeap), entry(0)  {}

inline HeapAssociated::HeapAssociated(DataHeap& aHeap, Pointer aEntry)
: dataHeap(aHeap), entry(aEntry)  {}

inline void    HeapAssociated::Attach(Pointer aEntry)  { entry = aEntry; }
inline Pointer HeapAssociated::Detach()
{
    Pointer prvEntry = entry;
    entry = 0;
    return prvEntry;
}


//
// CharBlock
// ---------
//

class CharBlock : public HeapAssociated {
public:
    static int PrecalcSize(const char* str);

public:
    CharBlock(DataHeap& aHeap);                   // default c-tor
    CharBlock(DataHeap& aHeap, const char* str);  // creating c-tor
    CharBlock(DataHeap& aHeap, Pointer aEntry);   // associating c-tor

    int  BlockSize();

    const char* GetString(char buff[], int buffSize);
    void        SetString(const char* str);
};


#endif  // DATAHEAP_INC
