//
// trie.h - trie definition
// Created: 23 May 1996
// Author:  Theppitak Karoonboonyanan
//

#ifndef TRIE_INC
#define TRIE_INC

#include "misc/typedefs.h"

#include "vmem/vmem.h"
#include "vmem/dataheap.h"  // solely for DataPtr type

typedef int32   State;  // index into double-array and TAIL
typedef DataPtr Index;  // index into data associating to keys

const Index ErrorIndex = ~0U;
const State RootStateBase  = 1;

enum EWalkResult {
    INTERNAL, TERMINAL, CRASH
};

// Alphabet set
typedef unsigned char Char;
const int  AlphabetSize = 256;
const Char Terminator   = 255;

const int MaxKeyLen = 256;

class SymbolSet {
public:
    // Constructors
    SymbolSet();
    SymbolSet(int nElements, ...);

    // Symbol List
    const Char* Symbols() const;

    // Basic Operations
    int  NElements() const;
    bool Contains(Char c) const;
    void MakeNull();

    // Binary Set Operators
    const SymbolSet& operator+=(Char c);
    const SymbolSet& operator-=(Char c);
/*
    const SymbolSet& operator+=(const SymbolSet& aSet);
    const SymbolSet& operator-=(const SymbolSet& aSet);
    const SymbolSet& operator*=(const SymbolSet& aSet);

    friend SymbolSet operator+(const SymbolSet& set1, Char c);
    friend SymbolSet operator-(const SymbolSet& set1, Char c);
    friend SymbolSet operator+(const SymbolSet& set1, const SymbolSet& set2);
    friend SymbolSet operator-(const SymbolSet& set1, const SymbolSet& set2);
    friend SymbolSet operator*(const SymbolSet& set1, const SymbolSet& set2);
*/

private:  // private data
    Char symbols[AlphabetSize+1];
    int  nSymbols;
};

inline const Char* SymbolSet::Symbols() const { return symbols; }
inline int  SymbolSet::NElements() const { return nSymbols; }
inline void SymbolSet::MakeNull()  { symbols[0] = 0; nSymbols = 0; }

///////////////////////////////////////////////////////////////////////////////
//  PART 1 : DOUBLE-ARRAY STRUCTURE
///////////////////////////////////////////////////////////////////////////////

// Cells in Double-Array Structure
struct DACell {
    State Base;
    State Check;
};
const int BaseOffset = 0;
const int CheckOffset = BaseOffset + sizeof(State);

//
// Double-Array Pool  (derived from VirtualMem class --See vmem.h)
// -----------------
// This class just shields up the more general VirtualMem class. It represents
// a virtual, dynamic-sized BASE and CHECK arrays.
//
class DoubleArray : public VirtualMem {
public:  // public functions
    DoubleArray(const char* fileName, int openModes);

    // BASE & CHECK referencings
    void SetBase(State s, State b);
    void SetCheck(State s, State c);

    // BASE & CHECK Read-Only Access
    State Base(State s);
    State Check(State s);
};

inline DoubleArray::DoubleArray(const char* fileName, int openModes)
: VirtualMem(fileName, openModes) {}

inline void DoubleArray::SetBase(State s, State b)
    { SetInt32(Pointer(s*sizeof(DACell) + BaseOffset), b); }
inline void DoubleArray::SetCheck(State s, State c)
    { SetInt32(Pointer(s*sizeof(DACell) + CheckOffset), c); }

inline State DoubleArray::Base(State s)
    { return Int32(Pointer(s*sizeof(DACell) + BaseOffset)); }
inline State DoubleArray::Check(State s)
    { return Int32(Pointer(s*sizeof(DACell) + CheckOffset)); }


//
// Branches
// --------
// A tree structure, represented in double-array structure, of the branching
// part of the trie. There are some improvements from Aoe's :
//
// 1. Terminal-nodes are allowed in the Branches structure. Terminal-nodes are
//    all nodes with incoming edge labeled Terminator, that is, a node s of
//    Branches is terminal if:
//
//          BASE[CHECK[s]] + Terminator = s
//
//    This can reduce some steps in retrievals: for words which are prefixes
//    of some other words, their last branches will be labeled Terminator.
//    Thus, pointers to TAIL blocks are no longer needed. We can immediately
//    store the associating data in such nodes.
//
// 2. The free-space list (G-link) is adapted to circular doubly-linked list.
//    This is accomplished by letting BASE of a free cell points to the
//    previous free cell (while CHECK points to next). Doing so can reduce the
//    time used to manipulate the list.
//
//    The entry point to the list is stored in BASE[0] and CHECK[0], and the
//    first cell available for use is BASE[1] and CHECK[1], which stores the
//    root node.
//
class Branches {
public:
    Branches(const char* fileName, int nIndices, int openModes);

    // File Status
    bool      IsSwapFileOpen();
    bool      IsSwapFileGood() const;
    bool      IsSwapFileBad()  const;
    bool      IsSwapFileFail() const;

    // Node info
    State     ParentOf(State s);
    int       OutDegree(State s);
    SymbolSet OutputSet(State s);
    bool      IsUsedNode(State s);
    bool      IsSeparateNode(State s);
    bool      IsTerminal(State s);
    State     TailState(State s);

    // Key's data access
    Index GetKeyData(State aSepNode);
    void  SetKeyData(State aSepNode, Index data);

    static State RootState(int indexNo);

    // Walking & Retrieval
    State       Walk(State s, Char c);
    EWalkResult WalkResult() const;

    // Insertion & Deletion
    State InsertBranch(State from, Char c);
    void  DeleteNode(State s);

    // For Storage Enumeration
    State TotalCells() const;

#ifdef TRIE_DEBUG
    void  AssertValid();
#endif  // TRIE_DEBUG

private:  // private const
    enum {
        unusedBase = 0
    };

private:  // private functions
    // free-list operations
    void  extendDAPool();
    void  removeFreeCell(State s);
    void  insertFreeCell(State s);

    bool  isVacantFor(const SymbolSet& aSet, State aBase);
    State findFreeBase(const SymbolSet& aSet);    // Aoe's X_CHECK
    void  relocateBase(State s, State newBase, State* pInspectedState = 0);

private:  // private data
    // the double-array structure
    DoubleArray da;
    State       totalCells;

    // walk result
    EWalkResult walkResult;
};

// File Status
inline bool Branches::IsSwapFileOpen() { return da.IsSwapFileOpen(); }
inline bool Branches::IsSwapFileGood() const { return da.IsSwapFileGood(); }
inline bool Branches::IsSwapFileBad()  const { return da.IsSwapFileBad();  }
inline bool Branches::IsSwapFileFail() const { return da.IsSwapFileFail(); }

inline State Branches::ParentOf(State s)        { return da.Check(s); }
inline bool  Branches::IsUsedNode(State s)      { return da.Check(s) > 0; }
inline bool  Branches::IsSeparateNode(State s)
    { return IsUsedNode(s) && da.Base(s) < 0; }
inline bool  Branches::IsTerminal(State s)
    { return IsUsedNode(s) && da.Base(ParentOf(s)) + Terminator == s; }

inline Index Branches::GetKeyData(State aSepNode)
    { return da.Base(aSepNode); }
inline void  Branches::SetKeyData(State aSepNode, Index data)
    { da.SetBase(aSepNode, data); }

inline State Branches::RootState(int indexNo)
    { return RootStateBase + indexNo; }

inline EWalkResult Branches::WalkResult() const  { return walkResult; }

inline State Branches::TotalCells() const { return totalCells; }


///////////////////////////////////////////////////////////////////////////////
//  PART 2 : TAIL HEAP
///////////////////////////////////////////////////////////////////////////////

//
// TailBlock
// ---------
// A data structure to transfer each TAIL element as a whole block.
// The block is composed of :
//    - suffix : string of remaining part of the keys in double-array structure
//    - data   : the data associating with each key
//
class TailBlock {
public:   // public functions
    // Constructors & Destructor
    TailBlock();
    TailBlock(const Char* suffix, Index data);
    ~TailBlock();

    // Information Accessing
    const Char* Suffix() const;
    Index       Data()   const;
    void SetSuffix(const Char* aSuffix);
    void SetData(Index aData);

private:  // private functions
    // never make a copy
    TailBlock(const TailBlock&);
    const TailBlock& operator=(const TailBlock&);

private:  // private data
    Char*  suffix;
    Index  data;
};

inline const Char* TailBlock::Suffix() const { return suffix; }
inline Index       TailBlock::Data()   const { return data;   }

//
// Tails
// -----
// The suffix part of the keys distinguished by the Branches structure.
// Blocks, each of which contains suffix and associating data, can be allocated,
// but, at present, cannot be freed. It needs further storage compaction stage.
// (See the reason for this in my document.)
//
// The single-step and multiple-step walk on the suffixes are also provided.
// To do so, the TAIL's State is defined as follows:
//
//    lower  8 bits - contains character offset in the suffix string
//    upper 24 bits - contains pointer to the beginning of the block
//
// thus completely describes a certain position in a certain suffix.
//
class Tails {
public:
    // Constructor
    Tails(const char* fileName, int openModes);

    // Swap file status
    bool        OpenSwapFile(const char* swapFileName, int iosModes);
    bool        CloseSwapFile();
    const char* SwapFileName() const;
    bool        IsSwapFileOpen();
    bool        IsSwapFileGood() const;
    bool        IsSwapFileBad()  const;
    bool        IsSwapFileFail() const;

    // Key data access
    Index GetKeyData(State t);
    void  SetKeyData(State t, Index data);

    // TAIL Block Access
    void  GetBlock(State t, TailBlock* pBlock);
    void  SetBlock(State t, const TailBlock& aBlock);

    // Walking & Retrieval
    Char        NextSymbol(State t);
    State       Walk(State t, Char c);
    State       Walk(State t, const Char* key);
    EWalkResult WalkResult() const;
    int         NWalked() const;
    bool        IsTerminal(State t);

    // Insertion & Deletion
    State AllocString(const Char* str, Index data);
    void  ChangeString(State t, const Char* str, Index data);
    void  FreeBlock(State t);

private:  // private constants
    // TAIL state representation
    enum {
        offsetBits = 8,
        offsetMask = ~((~0L) << offsetBits)
    };

    // TAIL block representation
    enum {
        dataOffset   = 0,
        suffixOffset = 4
    };

private:  // private functions
    // State decoding
    static Pointer blockBegin(State t);
    static int     charOffset(State t);

    // State encoding
    static State   makeTailState(Pointer blockBegin, int offset);

    // Block representation
    static Pointer dataBegin(State t);
    static Pointer suffixBegin(State t);

private:  // private data
    VirtualHeap dataHeap;

    // walk result
    EWalkResult walkResult; // set by both Walk() functions
    int         nWalked;    // set by Walk(State, const char*)
};

inline bool Tails::OpenSwapFile(const char* swapFileName, int iosModes)
    { return dataHeap.OpenSwapFile(swapFileName, iosModes); }
inline bool Tails::CloseSwapFile()        { return dataHeap.CloseSwapFile(); }
inline const char* Tails::SwapFileName() const
    { return dataHeap.SwapFileName(); }
inline bool Tails::IsSwapFileOpen()       { return dataHeap.IsSwapFileOpen(); }
inline bool Tails::IsSwapFileGood() const { return dataHeap.IsSwapFileGood(); }
inline bool Tails::IsSwapFileBad()  const { return dataHeap.IsSwapFileBad();  }
inline bool Tails::IsSwapFileFail() const { return dataHeap.IsSwapFileFail(); }

inline Pointer Tails::blockBegin(State t) { return t >> offsetBits; }
inline int     Tails::charOffset(State t) { return int(t & offsetMask); }

inline State   Tails::makeTailState(Pointer blockBegin, int offset)
    { return (blockBegin << offsetBits) | (offset & offsetMask); }

inline Pointer Tails::dataBegin(State t)   { return blockBegin(t) + dataOffset; }
inline Pointer Tails::suffixBegin(State t) { return blockBegin(t) + suffixOffset; }

// Key data access
inline Index Tails::GetKeyData(State t)
    { return dataHeap.UInt32(dataBegin(t)); }
inline void  Tails::SetKeyData(State t, Index data)
    { dataHeap.SetUInt32(dataBegin(t), data); }

inline Char Tails::NextSymbol(State t)
    { return dataHeap.Byte(suffixBegin(t) + charOffset(t)); }

inline EWalkResult Tails::WalkResult() const  { return walkResult; }
inline int         Tails::NWalked()    const  { return nWalked; }

inline bool Tails::IsTerminal(State t)
    { return dataHeap.Byte(suffixBegin(t) + charOffset(t) - 1) == Terminator; }

///////////////////////////////////////////////////////////////////////////////
//  PART 3 : THE TRIE (DOUBLE-ARRAY + TAIL HEAP)
///////////////////////////////////////////////////////////////////////////////

// helper functions
const Char* AppendTerminator(const Char* key);
const char* TruncateTerminator(const char* key);

//
// Trie (Multiple Index Trie)
// ----
// This class gives a general view on tries. The class users can insert keys,
// delete keys, check key inclusion, and retrieve the data associating
// with the key. Furthermore, the users also can walk along the trie with
// a key character by character. To do this, the State of the operation is
// defined so that the users can keep track of where they currently are.
//
// As named, the tries of this class can store more than one set of indices
// in the same storage. Each set is indicated by indexNo argument, of which
// value begins at zero.
//
// This class is implemented by using double-array structure with suffix
// compression. (See Aoe[1989] for the detail.) That is, the trie is composed
// of 2 structures:
//    - Branches : the branching part which distinguishes keys (it's a tree)
//    - Tails    : the remaining part of keys after the distinguishing position
//
// And, to indicate in which structure a State is, we use its sign as follows:
//    + : represents multi-nodes (i.e. nodes in the Branches structure)
//    - : represents single-nodes (i.e. nodes in the Tails structure)
//    0 : undefined
//
// Terminology:
//    - multi-node    : internal node in the Branches structure, which
//                      contains a pointer to the beginning pos of the
//                      sparse table row containing their child nodes
//    - single-node   : every node in the Tails structure
//    - separate-node : leaf node in the Branches structure, which contains
//                      a pointer to its remaining part in the Tails
//                      structure. (This can be distinguished from the pointer
//                      to table row by its sign.)
//    - terminal-node : node with the incoming edge labeled Terminator
//                      A terminal-node can be in both Branches and Tails.
//
// There is an improvement from Aoe's: the deletion algorithm is more complete.
// Instead of just mark the separate node of a key as unused, the other
// relevant nodes are also deleted.
//
class Trie {
public:
    typedef bool (*EnumFunc)(const Char* key, Index data);

    enum ErrorCode {
        NoError = 0,
        CantOpenBranches,
        CantOpenTails
    };

public:
    // constructor
    Trie(
        const char* branchesFileName,
        const char* tailsFileName,
        int nIndices,
        int openModes
    );

    // File Status
    ErrorCode ErrorStatus();

    // General Operations
    bool  InsertKey(int indexNo, const Char* key, Index dataIdx);
    bool  DeleteKey(int indexNo, const Char* key);
    Index Retrieve(int indexNo, const Char* key);

    // Walking & Retrieval
    State       StartState(int indexNo) const;
    State       Walk(State s, Char c);
    State       Terminate(State s);  // walk with Terminator
    State       Walk(State s, const Char* key);
    EWalkResult WalkResult() const;
    int         NWalked() const;   // call after Walk(State, const char*) only
    Index       GetKeyData(State s);

    // For Enumeration
    int       OutDegree(State s);
    SymbolSet OutputSet(State s);
    void      Enumerate(int indexNo, EnumFunc fn);

    // Storage Compaction
    void Compact();

private:  // private functions
    // Aoe's A_INSERT & B_INSERT
    void branchWithinBranches(State from, const Char* suffix, Index dataIdx);
    void branchWithinTail(State sepNode, const Char* suffix, Index dataIdx);

    bool enumKeys(State s, int level);

    // storage compaction
    void reallocTails();

private:  // private data
    // the trie structure
    Branches    branches;
    Tails       tails;

    // walk result
    EWalkResult walkResult;  // set by both Walk() functions
    int         nWalked;     // set by Walk(State, const Char*)

    // Enumerate func
    EnumFunc    enumFunc;
};

// constructor
inline Trie::Trie(
    const char* branchesFileName, const char* tailsFileName,
    int nIndices, int openModes
) :
    branches(branchesFileName, nIndices, openModes),
    tails(tailsFileName, openModes)
{ walkResult = INTERNAL; nWalked = 0; enumFunc = 0; }

inline State Trie::StartState(int indexNo) const
{
    return Branches::RootState(indexNo);
}
inline State       Trie::Terminate(State s)  { return Walk(s, Terminator); }
inline EWalkResult Trie::WalkResult() const  { return walkResult; }
inline int         Trie::NWalked()    const  { return nWalked; }

#endif // TRIE_INC

