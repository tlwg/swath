// longwordseg.cpp: implementation of the LongWordSeg class.
//
//////////////////////////////////////////////////////////////////////

#include "wordstack.h"
#include "longwordseg.h"
#include "dictpath.h"
#include <string.h>
#include <stdio.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LongWordSeg::LongWordSeg():AbsWordSeg()
{
	MyDict=trie_new_from_file("swathdic.tri");
}

LongWordSeg::LongWordSeg(const char *dataPath):AbsWordSeg()
{
	char *triePath=new char[strlen(dataPath)+30];
	sprintf(triePath,"%s/swathdic.tri",dataPath);
	MyDict=trie_new_from_file(triePath);
	delete[] triePath;
}

LongWordSeg::~LongWordSeg()
{
	trie_free(MyDict);
}

int LongWordSeg::CreateSentence()
{
WordStack BackTrackStack;

short int senIdx=0;
short int sepIdx=0,Idx=0;
short int nextSepIdx=0,curState;

wordStateType wState;
    // ========================================
    // this loop for get the first sentecne
	//      and Create Backtrack point.....
    // ========================================

	while (Idx<len){//(LinkSep[IdxSep[Idx]]!=len){
        // found some words that start with Idx character
		if (IdxSep[Idx]>=0){
			if (LinkSep[IdxSep[Idx]+1]!=-1){
                wState.backState=Idx;
                wState.branchState=0;
                BackTrackStack.Push(wState);
			}
			SepData[senIdx].Sep[sepIdx++]=LinkSep[IdxSep[Idx]];
            if (LinkSep[IdxSep[Idx]]==len)
				break;
	        Idx=LinkSep[IdxSep[Idx]];
		}else{
           	//at Idx there is no word in dictionary
           	while((Idx<len) && (IdxSep[Idx]<0))
		   		Idx++;
            SepData[senIdx].Sep[sepIdx++]=Idx;
			if (Idx==len){
				break;
			}
		}
	}
    if (SepData[senIdx].Sep[sepIdx-1]==len){
		SepData[senIdx].Sep[sepIdx]=-1;
	}else{
		SepData[senIdx].Sep[sepIdx++]=len;
		SepData[senIdx].Sep[sepIdx]=-1;
	}
    return 0;
}
