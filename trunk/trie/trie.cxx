//
// trie.cpp - trie implementation
// Created: 30 May 1996
// Author:  Theppitak Karoonboonyanan
//

#include <string.h>
#include <stdarg.h>

#include "trie/trie.h"
#include "misc/fileutil.h"

//////////////// Implementation of SymbolSet class ////////////////
SymbolSet::SymbolSet()
{
    MakeNull();
}

SymbolSet::SymbolSet(int nElements, ...)
{
    ASSERT(nElements <= AlphabetSize);
    va_list args;
    va_start(args, nElements);

    int i;
    for (i = 0; i < nElements; i++) {
        symbols[i] = Char(va_arg(args, int));
    }
    symbols[i] = 0;

    va_end(args);

    nSymbols = nElements;
}

bool SymbolSet::Contains(Char c) const
{
    return strchr((char*)symbols, c) != NULL;
}

const SymbolSet& SymbolSet::operator+=(Char c)
{
    symbols[nSymbols++] = c;
    symbols[nSymbols] = 0;
    return *this;
}

const SymbolSet& SymbolSet::operator-=(Char c)
{
    Char* p = (Char*)strchr((char*)symbols, c);
    if (p != 0) {
        // shift the following chars
        while (*p) {
            *p = *(p+1);
            p++;
        }
        nSymbols--;
    }
    return *this;
}


///////////////////////////////////////////////////////////////////////////////
//  PART 1 : DOUBLE-ARRAY STRUCTURE
///////////////////////////////////////////////////////////////////////////////

//////////////// Implementation of DoubleArray class ////////////////

//////////////// Implementation of Branches class ////////////////
Branches::Branches(const char* fileName, int nIndices, ios_base::openmode openModes)
: da(fileName, openModes)
{
    totalCells = (da.TotalPages() * da.PageSize()) / sizeof(DACell);
    if (totalCells == 0) {
        // init header cell
        da.SetBase(0, 0);
        da.SetCheck(0, 0);
        // init root states
        for (int i = 0; i < nIndices; i++) {
            da.SetBase(RootState(i), RootState(i) + nIndices);
            da.SetCheck(RootState(i), 0);
        }
        // so, totalCells is now...
        totalCells = 1 + nIndices;

        extendDAPool();
    }
    walkResult = INTERNAL;
#ifdef TRIE_DEBUG
    AssertValid();
#endif  // TRIE_DEBUG
}

// return number of edges that point out from node s
// i.e. number of c such that CHECK[BASE[s] + c] = s
// [Aoe's N(SET_LIST(s))]
int Branches::OutDegree(State s)
{
    int   outdegree = 0;
    State b = da.Base(s);
    ASSERT(b > 0);
    for (int c = 0; c < AlphabetSize; c++) {
        if (da.Check(b + c) == s)  { outdegree++; }
    }
    return outdegree;
}

// return the set of those symbols on the output edges of the node s
// [Aoe's SET_LIST(s)]
SymbolSet Branches::OutputSet(State s)
{
    SymbolSet theSet;
    State b = da.Base(s);
    ASSERT(b > 0);
    for (int c = 0; c < AlphabetSize; c++) {
        if (da.Check(b + c) == s)  { theSet += c; }
    }
    return theSet;
}

// return:
//    s is separate node  -> the equivalent TAIL state of s
//    otherwise           -> s
State Branches::TailState(State s)
{
    State t = da.Base(s);
    return (t < 0) ? t : s;
}

// walking & retrieval
// behavior on walkResult:
//    CRASH    - returns the state before crashing
//    TERMINAL - returns the terminal state
//    INTERNAL - returns the entered state
State Branches::Walk(State s, Char c)
{
    ASSERT(s > 0);
    State t = da.Base(s) + c;
    if (t < totalCells && da.Check(t) == s) {
        walkResult = (c == Terminator) ? TERMINAL : INTERNAL;
        return t;
    } else {
        walkResult = CRASH;
        return s;
    }
}

// extend the double-array pool for a page, with free-list initialized
void Branches::extendDAPool()
{
    int CellsPerPage = da.PageSize() / sizeof(DACell);
    ASSERT(da.PageSize() % sizeof(DACell) == 0);
    // add cells up to page end
    int cellsToAdd = int(CellsPerPage - (totalCells % CellsPerPage));

    // init new free-list
    State i;
    for (i = totalCells; i < totalCells + cellsToAdd - 1; i++) {
        da.SetCheck(i, -(i+1));
        da.SetBase(i+1, -i);
    }

    // merge the new circular list to the old
    da.SetCheck(-da.Base(0), -totalCells);
    da.SetBase(totalCells, da.Base(0));
    da.SetCheck(i, 0);
    da.SetBase(0, -i);

    totalCells += cellsToAdd;
}

void Branches::removeFreeCell(State s)
{
    ASSERT(s > 0);

    // ensure existence of the free cell
    while (totalCells <= s) {
        extendDAPool();
    }

    ASSERT(da.Check(s) <= 0);
    ASSERT(da.Base(s) <= 0);
    da.SetCheck(-da.Base(s), da.Check(s));
    da.SetBase(-da.Check(s), da.Base(s));
}

void Branches::insertFreeCell(State s)
{
    ASSERT(s > 0);
    if (s < totalCells) {
        // let i be the first free cell which i > s
        State i = -da.Check(0);
        ASSERT(i >= 0);
        while (i != 0 && i < s) {
            i = -da.Check(i);
            ASSERT(i >= 0);
        }

        // insert s before i
        da.SetCheck(s, -i);
        da.SetBase(s, da.Base(i));
        da.SetCheck(-da.Base(i), -s);
        da.SetBase(i, -s);
    }
}

// check if the place beginning from s is vacant for placing
// all the symbols in aSet
bool Branches::isVacantFor(const SymbolSet& aSet, State aBase)
{
    for (const Char* p = aSet.Symbols(); *p; p++) {
        if (IsUsedNode(aBase + *p))  { return false; }
    }
    return true;
}

// search for a BASE beginning index such that all the cells to be occupied
// by the symbols in aSet are free
// Note: For optimal space usage, the first symbol in aSet should be
//       the smallest value.
// [Aoe's X_CHECK(LIST)]
State Branches::findFreeBase(const SymbolSet& aSet)
{
    Char  firstSymbol = aSet.Symbols()[0];
    for (;;) {  // retry loop
        State s;
        // s := first free cell such that s > first symbol
        for (;;) {  // retry loop
            s = -da.Check(0);
            ASSERT(s >= 0);
            while (s != 0 && s <= State(firstSymbol)) {
                s = -da.Check(s);
                ASSERT(s >= 0);
            }
            if (s != 0)  { break; }
            extendDAPool();
        }

        // search for next free cell in which aSet fits
        while (s != 0 && !isVacantFor(aSet, s - firstSymbol)) {
            s = -da.Check(s);
            ASSERT(s == 0 || s >= State(firstSymbol));
        }
        if (s != 0)  { return s - firstSymbol; }
        extendDAPool();
    }
}

void Branches::relocateBase(State s, State newBase, State* pInspectedState)
{
    State oldBase = da.Base(s);
    ASSERT(oldBase > 0);
    SymbolSet outputs(OutputSet(s));
    ASSERT(isVacantFor(outputs, newBase));

    for (const Char* p = outputs.Symbols(); *p; p++) {
        State oldNext = oldBase + *p;
        State newNext = newBase + *p;

        // give newNext node to s and copy BASE
        ASSERT(newNext >= totalCells || da.Check(newNext) <= 0);
        removeFreeCell(newNext);
        da.SetCheck(newNext, s);  // mark owner
        da.SetBase(newNext, da.Base(oldNext)); // copy value

        // oldNext node is now moved to newNext
        // so, all cells belonging to oldNext
        // must be given to newNext
        State nextBase = da.Base(oldNext);
        // preventing cases of TAIL pointer & terminal node
        if (nextBase > 0 && *p != Terminator) {
            for (int c = 0; c < AlphabetSize; c++) {
                if (da.Check(nextBase + c) == oldNext) {
                    da.SetCheck(nextBase + c, newNext);
                }
            }
        }
        // And, if inspected state is oldNext, change it to newNext
        if (pInspectedState && *pInspectedState == oldNext) {
            *pInspectedState = newNext;
        }

        // free oldNext node
        insertFreeCell(oldNext);
    }

    // finally, make BASE[s] point to newBase
    da.SetBase(s, newBase);
}

#ifdef TRIE_DEBUG
void Branches::AssertValid()
{
    // check circular joining
    ASSERT(-da.Base(0) < totalCells);
    ASSERT(da.Check(-da.Base(0)) == 0);

    State s = 0;
    do {
        State t = -da.Check(s);
        ASSERT(t >= 0);
        ASSERT(t < totalCells);
        // Check back pointer
        ASSERT(da.Base(t) == -s);
        // The cells in between must not be free
        for (State u = s + 1; u < t; u++) {
            ASSERT(da.Check(u) >= 0);
            ASSERT(da.Check(u) < totalCells);
            if (!IsSeparateNode(u)) {
                ASSERT(IsTerminal(u) || da.Base(u) < totalCells);
            }
            // detect no-branch node (whose BASE may be incorrect)
            if (u > RootState(4) && !IsSeparateNode(u) && !IsTerminal(u)) {
                ASSERT(OutDegree(u) > 0);
            }
        }
        s = t;
    } while (s != 0);
}
#endif // TRIE_DEBUG

State Branches::InsertBranch(State from, Char c)
{
    ASSERT(da.Check(from) >= 0);
    State to;
    if (da.Base(from) == unusedBase || IsSeparateNode(from) || IsTerminal(from))
    {
        // BASE[from] isn't pointing to a BASE, so just find a brand-new one
        State newBase = findFreeBase(SymbolSet(1, c));
        da.SetBase(from, newBase);
        to = newBase + c;
    } else {
        // the cell is free for use?
        to = da.Base(from) + c;
        if (IsUsedNode(to)) {
            // no, prepare place for 'to'
            // determine work amount and minimize it
            State overlapped = ParentOf(to);
            ASSERT(da.Check(overlapped) >= 0);
            ASSERT(da.Base(overlapped) > 0);
            if (OutDegree(from) < OutDegree(overlapped)) {
                // relocate 'from'
                SymbolSet outputs(OutputSet(from));
                outputs += c;
                State newBase = findFreeBase(outputs);
                relocateBase(from, newBase);
                // let 'to' be the new cell reallocated
                to = newBase + c;
            } else {
                // relocate 'overlapped' so that 'to' is free
                // Note that since the relocated state is not 'from',
                // so it can be possibly changed, and thus must be inspected
                State newBase = findFreeBase(OutputSet(overlapped));
                relocateBase(overlapped, newBase, &from);
            }
        }
    }

    // take and occupy it
    ASSERT(da.Check(to) <= 0);
    removeFreeCell(to);
    da.SetBase(to, unusedBase);
    da.SetCheck(to, from);

    return to;
}

void Branches::DeleteNode(State s)
{
    insertFreeCell(s);
}


///////////////////////////////////////////////////////////////////////////////
//  PART 2 : TAIL HEAP
///////////////////////////////////////////////////////////////////////////////

//////////////// Implementation of TailBlock class ////////////////
TailBlock::TailBlock()
{
    suffix = (Char*)0;
    data   = 0;
}

TailBlock::TailBlock(const Char* aSuffix, Index aData)
{
    suffix = (Char*)0;
    SetSuffix(aSuffix);
    SetData(aData);
}

TailBlock::~TailBlock()
{
    delete suffix;
}

void TailBlock::SetSuffix(const Char* aSuffix)
{
    delete suffix;
    suffix = new Char[strlen((const char*)aSuffix)+1];
    strcpy((char*)suffix, (const char*)aSuffix);
}

void TailBlock::SetData(Index aData)
{
    data = aData;
}


//////////////// Implementation of Tails class ////////////////
// constructor
Tails::Tails(const char* fileName, ios_base::openmode openModes)
: dataHeap(fileName, openModes)
{
    walkResult = INTERNAL;
}

// TAIL Block Access
void Tails::GetBlock(State t, TailBlock* pBlock)
{
    pBlock->SetData(GetKeyData(t));

    Char  buffer[MaxKeyLen];
    Char* pBuf = buffer;
    Pointer p = suffixBegin(t);
    for (Char c = dataHeap.Byte(p); c != 0; c = dataHeap.Byte(++p)) {
        *pBuf++ = c;
    }
    *pBuf = 0;
    pBlock->SetSuffix(buffer);
}

void Tails::SetBlock(State t, const TailBlock& aBlock)
{
    SetKeyData(t, aBlock.Data());

    const Char* suffix = aBlock.Suffix();
    dataHeap.BlockWrite(
        suffixBegin(t), (const char*)suffix, strlen((const char*)suffix) + 1
    );
}

// Walking & Retrieval
State Tails::Walk(State t, Char c)
{
    int offset = charOffset(t);
    if (dataHeap.Byte(suffixBegin(t) + offset) == c) {
        walkResult = (c == Terminator) ? TERMINAL : INTERNAL;
        return makeTailState(blockBegin(t), ++offset);
    } else {
        walkResult = CRASH;
        return t;
    }
}

// key must be ended with a Terminator
// on return: WalkResult will equal either TERMINAL or CRASH only
State Tails::Walk(State t, const Char* key)
{
    nWalked = 0;
    walkResult = CRASH;  // if not set to TERMINAL, it CRASHes

    Pointer pSuffix = suffixBegin(t);
    int offset = charOffset(t);
    while (dataHeap.Byte(pSuffix + offset) == *key) {
        offset++; nWalked++;
        if (*key == Terminator) {
            walkResult = TERMINAL;
            break;
        }
        key++;
    }

    return makeTailState(blockBegin(t), offset);
}

// Insertion & Deletion
State Tails::AllocString(const Char* str, Index data)
{
    int blockSize = suffixOffset + strlen((const char*)str) + 1;
    Pointer p = dataHeap.NewBlock(blockSize);

    // fill out the block
    dataHeap.SetUInt32(p, data);
    dataHeap.BlockWrite(
        p + suffixOffset, (const char*)str, strlen((const char*)str) + 1
    );

    return makeTailState(p, 0);
}

void Tails::ChangeString(State t, const Char* str, Index data)
{
    SetKeyData(t, data);
    dataHeap.BlockWrite(
        suffixBegin(t), (const char*)str, strlen((const char*)str) + 1
    );
}

void Tails::FreeBlock(State t)
{
    dataHeap.DeleteBlock(blockBegin(t));
}


///////////////////////////////////////////////////////////////////////////////
//  PART 3 : THE TRIE (DOUBLE-ARRAY + TAIL)
///////////////////////////////////////////////////////////////////////////////

// A helping function: append a terminator char to the key
// and return pointer to an internal static buffer containing the result
const Char* AppendTerminator(const Char* key)
{
    static Char  buff[MaxKeyLen+2];

    strcpy((char*)buff, (const char*)key);
    int len = strlen((char*)buff);
    buff[len++] = Terminator;
    buff[len] = 0;

    return buff;
}

const char* TruncateTerminator(const char* key)
{
    static char  buff[MaxKeyLen+1];

    strcpy(buff, key);
    if (buff[0]) {
        buff[strlen(buff)-1] = '\0';
    }
    return buff;
}

//////////////// Implementation of Trie class ////////////////

Trie::ErrorCode Trie::ErrorStatus()
{
    if (!branches.IsSwapFileOpen()) {
        return CantOpenBranches;
    }
    if (!tails.IsSwapFileOpen()) {
        return CantOpenTails;
    }
    return NoError;
}

int Trie::OutDegree(State s)
{
    if (s > 0) {
        // if s is separate node, it's equivalent to a TAIL state
        return branches.IsSeparateNode(s) ? 1 : branches.OutDegree(s);
    }
    return (s < 0) ? 1 : 0;
}

SymbolSet Trie::OutputSet(State s)
{
    // if s is separate node, jump to TAIL first
    if (s > 0 && branches.IsSeparateNode(s)) {
        s = branches.TailState(s);
    }

    if (s > 0) {
        return branches.OutputSet(s);
    } else if (s < 0) {
        return SymbolSet(1, tails.NextSymbol(-s));
    } else {
        return SymbolSet();
    }
}

//
// Trie::Walk() with a transition character
//
State Trie::Walk(State s, Char c)
{
    // if s is separate node, jump to TAIL first
    if (s > 0 && branches.IsSeparateNode(s)) {
        s = branches.TailState(s);
    }

    // walk according to state type
    if (s > 0) {
        s = branches.Walk(s, c);
        walkResult = branches.WalkResult();
    } else if (s < 0) {
        s = -tails.Walk(-s, c);
        walkResult = tails.WalkResult();
    } else {
        // crash
        walkResult = CRASH;
    }

    return s;
}

//
// Trie::Walk() with a key as string
// returns the farthest state that the key can reach
// behavior on walkResult: (similar to Branches::Walk() and Tails::Walk())
//    CRASH    - returns the state before crashing
//    TERMINAL - returns the terminal state
//    INTERNAL - impossible
//
State Trie::Walk(State s, const Char* key)
{
    nWalked = 0;
    const Char* p = AppendTerminator(key);

    // walk through multi-nodes
    if (s > 0) {
        while (!branches.IsSeparateNode(s)) {
            s = branches.Walk(s, *p);
            walkResult = branches.WalkResult();
            if (walkResult == CRASH)  return s;
            p++; nWalked++;
            if (walkResult == TERMINAL)  return s;
        }
        s = branches.TailState(s);
    }

    // walk through single-nodes
    if (s < 0) {
        s = -tails.Walk(-s, p);
        walkResult = tails.WalkResult();
        nWalked += tails.NWalked();
    } else {
        walkResult = CRASH;
    }

    return s;
}

Index Trie::GetKeyData(State s)
{
    if (s > 0) {
        return branches.IsTerminal(s) ? branches.GetKeyData(s) : ErrorIndex;
    } else if (s < 0) {
        return tails.IsTerminal(-s) ? tails.GetKeyData(-s) : ErrorIndex;
    } else {
        return ErrorIndex;
    }
}

// Aoe's A_INSERT
void Trie::branchWithinBranches(State from, const Char* suffix, Index dataIdx)
{
    State newState = branches.InsertBranch(from, *suffix);
    if (*suffix == Terminator) {
        // it's an immediate terminal node
        branches.SetKeyData(newState, dataIdx);
    } else {
        // link it to a TAIL block
        branches.SetKeyData(newState, -tails.AllocString(suffix+1, dataIdx));
    }

#ifdef TRIE_DEBUG
    branches.AssertValid();
#endif  // TRIE_DEBUG
}

// Aoe's B_INSERT
void Trie::branchWithinTail(State sepNode, const Char* suffix, Index dataIdx)
{
    ASSERT(branches.IsSeparateNode(sepNode));

    // get previous TAIL info
    State oldTail = -branches.TailState(sepNode);
    TailBlock oldTailBlock;
    tails.GetBlock(oldTail, &oldTailBlock);

    // insert chain of common nodes
    const Char* p = oldTailBlock.Suffix();
    State s = sepNode;
    while (*p == *suffix) {
        s = branches.InsertBranch(s, *p);
        p++;  suffix++;
    }

    // insert back the remaining of old TAIL block
    if (*p == Terminator) {
        // free old tail block
        tails.FreeBlock(oldTail);
        // insert immediate terminal node in branches
        State t = branches.InsertBranch(s, Terminator);
        branches.SetKeyData(t, oldTailBlock.Data());
    } else {
        // cut remaining TAIL and insert back
        State t = branches.InsertBranch(s, *p);
        tails.ChangeString(oldTail, p+1, oldTailBlock.Data());
        branches.SetKeyData(t, -oldTail);
    }

    // then, insert the new branch
    branchWithinBranches(s, suffix, dataIdx);
}

bool Trie::InsertKey(int indexNo, const Char* key, Index dataIdx)
{
    State s = StartState(indexNo);
    const Char* p = AppendTerminator(key);

    // walk through multi-nodes
    do {
        s = branches.Walk(s, *p);
        switch (branches.WalkResult()) {
            case CRASH: {
                // A_INSERT(s, p);
                branchWithinBranches(s, p, dataIdx);
                return true;
            }

            case TERMINAL: {
                ASSERT(*p == Terminator);
                //duplicated key, overwrite data index
                branches.SetKeyData(s, dataIdx);
                return true;
            }
        }
        p++;
    } while (!branches.IsSeparateNode(s));

    // continue walking on single-nodes
    State t = -tails.Walk(-branches.TailState(s), p);
    switch (tails.WalkResult()) {
        case CRASH: {
            // B_INSERT(s, p);
            branchWithinTail(s, p, dataIdx);
            return true;
        }

        case TERMINAL: {
            ASSERT(p[tails.NWalked()-1] == Terminator);
            // duplicated key, overwrite data index
            tails.SetKeyData(-t, dataIdx);
            return true;
        }
    }

    ASSERT(false);
    return false;
}

bool Trie::DeleteKey(int indexNo, const Char* key)
{
    State s = StartState(indexNo);
    const Char* p = AppendTerminator(key);

    // walk to separate or terminal node
    do {
        s = branches.Walk(s, *p);
        switch (branches.WalkResult()) {
            case CRASH:    return false;
            case TERMINAL: goto   DELETE_BRANCHES;
        }
        p++;
    } while (!branches.IsSeparateNode(s));

    // for separate node, delete TAIL block if key exists
    {
        State t = -branches.TailState(s);
        tails.Walk(t, p);
        switch (tails.WalkResult()) {
            case CRASH:    return false;
            case TERMINAL: tails.FreeBlock(t); break;
            default:       ASSERT(false);
        }
    }

DELETE_BRANCHES:
    // delete relevant branches
    do {
        State parent = branches.ParentOf(s);
        branches.DeleteNode(s);
        s = parent;
    } while (s != StartState(indexNo) && branches.OutDegree(s) == 0);

    return true;
}

Index Trie::Retrieve(int indexNo, const Char* key)
{
    State s = Walk(StartState(indexNo), key);
    return (walkResult == TERMINAL) ? GetKeyData(s) : ErrorIndex;
}

bool Trie::enumKeys(State s, int level)
{
    static Char theWord[256];
    SymbolSet outputs(OutputSet(s));
    for (const Char* p = outputs.Symbols(); *p; p++) {
        theWord[level] = *p;
        State t = Walk(s, *p);
        ASSERT(WalkResult() != CRASH);
        if (WalkResult() == TERMINAL) {
            theWord[level] = '\0';
            return enumFunc && enumFunc(theWord, GetKeyData(t));
        } else {
            if (!enumKeys(t, level+1)) {
                return false;
            }
        }
    }
    return true;
}

void Trie::Enumerate(int indexNo, EnumFunc fn)
{
    enumFunc = fn;
    enumKeys(StartState(indexNo), 0);
}

// reallocate all TAIL  blocks to remove garbage in TAIL heap
void Trie::reallocTails()
{
    const char* oldTailsName = tails.SwapFileName();
    const char* tmpTailsName = tmpnam(NULL);

    Tails* pNewTails = new Tails(tmpTailsName, ios::in | ios::out);

    // read all TAIL blocks and allocate them in the new TAIL heap
    for (State s = StartState(0); s < branches.TotalCells(); s++) {
        if (branches.IsSeparateNode(s)) {
            TailBlock aTailBlock;
            tails.GetBlock(-branches.TailState(s), &aTailBlock);
            State t = -pNewTails->AllocString(
                aTailBlock.Suffix(), aTailBlock.Data()
            );
            branches.SetKeyData(s, t);
        }
    }

    tails.CloseSwapFile();
    pNewTails->CloseSwapFile();
    delete pNewTails;

    // replace the old file with the temp file
    remove(oldTailsName);
    movefile(tmpTailsName, oldTailsName);
    tails.OpenSwapFile(oldTailsName, ios::in | ios::out);
}

// Storage Compaction
void Trie::Compact()
{
    reallocTails();
    //branches.removeExtraFreeCells();
}


///////////////////////////////////////////////////////////////////////////////
//  PART 4 : TEST BED
///////////////////////////////////////////////////////////////////////////////

#ifdef TRIE_TEST

#include <iomanip.h>

static void testInsertDelete(Trie* pTrie)
{
    pTrie->InsertKey(0, (Char*)"progress", 1);
    pTrie->InsertKey(0, (Char*)"programme", 2);
    pTrie->InsertKey(0, (Char*)"produce", 3);
    pTrie->InsertKey(0, (Char*)"producer", 4);
    pTrie->InsertKey(0, (Char*)"pro", 5);

    pTrie->DeleteKey(0, (Char*)"producer");
    pTrie->DeleteKey(0, (Char*)"produce");
    pTrie->DeleteKey(0, (Char*)"progress");    

    pTrie->InsertKey(0, (Char*)"pass", 6);
}

bool showKey(const Char* key, Index data)
{
    cout << '(' << (const char*)key << ',' << data << ')' << endl;
    return true;
}

void testQuery(Trie* pTrie)
{
    char  buff1[256];
    Index data;

    for (;;) {
        cout << "> " << flush;
        cin >> buff1;
        if (strcmp(buff1, "i") == 0) {
            cin >> buff1 >> data;
            if (pTrie->InsertKey(0, (Char*)buff1, data)) {
                cout << '(' << buff1 << ',' << data << ") inserted." << endl;
            } else {
                cout << "Can't insert (" << buff1 << ',' << data << ")" << endl;
            }
        } else if (strcmp(buff1, "d") == 0) {
            cin >> buff1;
            if (pTrie->DeleteKey(0, (Char*)buff1)) {
                cout << '(' << buff1 << ") deleted." << endl;
            } else {
                cout << "Can't delete (" << buff1 << ")" << endl;
            }
        } else if (strcmp(buff1, "r") == 0) {
            cin >> buff1;
            data = pTrie->Retrieve(0, (Char*)buff1);
            if (data != ErrorIndex) {
                cout << data << endl;
            } else {
                cout << "Can't retrieve (" << buff1 << ")" << endl;
            }
        } else if (strcmp(buff1, "l") == 0) {
            pTrie->Enumerate(0, showKey);
        } else if (strcmp(buff1, "c") == 0) {
            pTrie->Compact();
        } else if (strcmp(buff1, "q") == 0) {
            break;
        } else {
            cout << "Unknown command" << endl;
        }
        cin.getline(buff1, 256);
    }
}

void testLargeDict(Trie* pTrie)
{
    char buff[120];
    cout << "Source: " << flush;
    cin >> buff;

    ifstream source(buff);
    Index idx = 0;
    while (source.getline(buff, sizeof buff)) {
        cout << buff << endl;
        pTrie->InsertKey(0, (const Char*)buff, idx);
        idx++;
    }
}

const int nIndices = 4;

void testMultiIndex(Trie* pTrie)
{
    ifstream inputs[nIndices];

    // open input files
    char buff[120];
    int  i;
    for (i = 0; i < nIndices; i++) {
        for (;;) {
            cout << "Source #" << i+1 << ": " << flush;
            cin >> buff;

            inputs[i].open(buff);
            if (inputs[i].rdbuf()->is_open())  { break; }

            cout << "Cannot open " << buff << endl;
        }
    }

    // insert keys
    Index idx = 0;
    for (;;) {
        for (i = 0; i < nIndices; i++) {
            if (!inputs[i].getline(buff, sizeof buff))  { return; }
            if (pTrie->Retrieve(i, (const Char*)buff) == ErrorIndex) {
                cout << setw(3) << i+1 << ": ";
                cout << setw(i+1) << " " << buff << endl;
                pTrie->InsertKey(i, (const Char*)buff, idx);
            }
        }
        idx++;
    }
}

int main()
{
    Trie* pTrie = new Trie(
        "testtrie.br", "testtrie.tl", nIndices, ios::in | ios::out
    );

//    testInsertDelete(pTrie);
//    testQuery(pTrie);
//    testLargeDict(pTrie);
    testMultiIndex(pTrie);

    delete pTrie;

    return 0;
}

#endif // TRIE_TEST
