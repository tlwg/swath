// maxwordseg.cpp: implementation of the MaxWordSeg class.
//
//////////////////////////////////////////////////////////////////////

#include "maxwordseg.h"
#include "wordstack.h"
#include <stdio.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MaxWordSeg::MaxWordSeg():AbsWordSeg()
{
	noAmbArea=0;
	score = new short int[MAXSEP];
}

MaxWordSeg::MaxWordSeg(const char *dataPath):AbsWordSeg(dataPath)
{
	noAmbArea=0;
	score = new short int[MAXSEP];
}


MaxWordSeg::~MaxWordSeg()
{
	delete[] score;
}


int MaxWordSeg::CreateSentence() //return number of ambiguous area.
{
int cntArea=1;
int stAmb, enAmb, stWord;
int i,idxSen;
bool singleWord;

	sepIdx=0;
	stAmb=0;
	if (IdxSep[0]>=0){
		enAmb=LinkSep[IdxSep[0]];
		stWord=1;
	}else{
		i=1;
		while ((i<len)&&(IdxSep[i]<0)) i++;
		
		if (i==len){
			stWord=i;
			enAmb=len;
		}else{
			stWord=i;
			enAmb=LinkSep[IdxSep[stWord]];
		}
	}
	while (enAmb<=len) {
		singleWord=true;
		for (;stWord<enAmb&&stWord<len;stWord++){
			if (IdxSep[stWord]<0) continue;
			if (LinkSep[IdxSep[stWord]]>enAmb) {
				singleWord=false;
				enAmb=LinkSep[IdxSep[stWord]];
			}
		}
		if (IdxSep[enAmb]<0){ //Checking for unknown
			while ((enAmb<len)&&(IdxSep[enAmb]<0)) enAmb++;
			singleWord=false;
		}
		if (singleWord) {
			SepData[2].Sep[sepIdx++]=enAmb;
		}else{
			idxSen=WordSegArea(stAmb,enAmb);
			saveSegment(idxSen,enAmb);
		}
		stAmb=enAmb;
		if (stAmb>=len) break;
		enAmb=LinkSep[IdxSep[stAmb]];
	}//end Loop while
	return 2;
}

void MaxWordSeg::saveSegment(int idxSen,int lastPoint)
{
int i=0;

	for (i=0;SepData[idxSen].Sep[i]<lastPoint;i++){
		SepData[2].Sep[sepIdx++]=SepData[idxSen].Sep[i];
	}
	if (SepData[idxSen].Sep[i]==lastPoint){
		SepData[2].Sep[sepIdx++]=SepData[idxSen].Sep[i];
	}

}

int MaxWordSeg::WordSegArea(int stSeg,int enSeg)
{
WordStack BackTrackStack;

short int senIdx=0,bestSenIdx=0,prevSenIdx=0,tmps; //tmps save score.
short int sepIdx=0,Idx=0,tmpidx;
short int nextSepIdx=0,curState;
short int scoreidx;
wordStateType wState;
short int bestScore=0;
bool foundUnk;
bool stopCreate;

    // ========================================
    // this loop for get the first sentecne
	//      and Create Backtrack point.....
    // ========================================
	Idx=stSeg;

	scoreidx=-1;
	SepData[0].Score=10000;
	SepData[1].Score=10000;
	while (Idx<enSeg){//(LinkSep[IdxSep[Idx]]!=enSeg){
        // found some words that start with Idx character
		if (IdxSep[Idx]>=0){
			if (LinkSep[IdxSep[Idx]+1]!=-1){
                wState.backState=Idx;
                wState.branchState=0;
                BackTrackStack.Push(wState);
			}
			SepData[senIdx].Sep[sepIdx++]=LinkSep[IdxSep[Idx]];
			tmps=(short int)++SepData[senIdx].Score;
			score[++scoreidx]=tmps;
	        Idx=LinkSep[IdxSep[Idx]];
		}else{
           	//at Idx there is no word in dictionary
           	while((Idx<enSeg) && (IdxSep[Idx]<0))
		   		Idx++;
            SepData[senIdx].Sep[sepIdx++]=Idx;
			SepData[senIdx].Score+=5;
			score[++scoreidx]=(short int)SepData[senIdx].Score;
		}
	}
	SepData[senIdx].Sep[sepIdx]=-1;
	bestScore=(short int)SepData[senIdx].Score;
    bestSenIdx=senIdx++;
    //================================================
	//create all posible sentences
	//using backtrack (use my stack not use recursive)
    //================================================
	int looptime=0;
    while (!BackTrackStack.Empty()){
//		printf("loop times :%d\n",++looptime);
		if (looptime++>200) 
			break;
		stopCreate=false;
		wState=BackTrackStack.Top();
		curState=wState.backState; //curState store the nth of character 
 		BackTrackStack.Pop();
		wState.branchState++;
        if ((curState=LinkSep[IdxSep[curState]+wState.branchState])!=-1)
			BackTrackStack.Push(wState);
		else
			continue;
		//create new sentence from prev sentence and save score to new sen.
		//change 1st parameter of copySepData from bestSenIdx to prevSenIdx
		nextSepIdx=copySepData(prevSenIdx,senIdx,wState.backState);
		scoreidx=nextSepIdx-1;
		SepData[senIdx].Score=(scoreidx<0)?10000:score[scoreidx];
		//loop for filling the rest sep point of new sentence
		while (curState!=enSeg){
			foundUnk=false;
            if (IdxSep[curState]<0){ //found unknown string.
                SepData[senIdx].Sep[nextSepIdx++]=curState;
				SepData[senIdx].Score++;
				score[++scoreidx]=(short int)SepData[senIdx].Score;
				while((curState<enSeg)&&(IdxSep[curState]<0)) curState++;
				foundUnk=true;
		        if (curState==enSeg) break;
			}else if (LinkSep[IdxSep[curState]+1]!=-1){
				//having another branchs 
				//then it should push backtrack state into Stack.
				wState.backState=curState;
				wState.branchState=0;
				BackTrackStack.Push(wState);
			}
			SepData[senIdx].Sep[nextSepIdx++]=curState;
			if (foundUnk){
				SepData[senIdx].Score+=5;
				foundUnk=false;
			}else
				SepData[senIdx].Score++;
			score[++scoreidx]=(short int)SepData[senIdx].Score;
			if (SepData[senIdx].Score>=bestScore-1){
				prevSenIdx=senIdx;
				stopCreate=true;
				break;
			}
            curState=LinkSep[IdxSep[curState]];
		} //finish create a new sentence.
		if (stopCreate) continue;
		SepData[senIdx].Sep[nextSepIdx]=enSeg;
		SepData[senIdx].Score+=(foundUnk)?5:1;
		SepData[senIdx].Sep[nextSepIdx+1]=-1;
		prevSenIdx=senIdx;
		if (SepData[senIdx].Score<bestScore){
			bestScore=(short int)SepData[senIdx].Score;
			tmpidx=bestSenIdx;
			bestSenIdx=senIdx;
			senIdx=tmpidx;
		}
	}
	return bestSenIdx;
}


