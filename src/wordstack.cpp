#include "wordstack.h"

WordStack::WordStack(){
	top_idx = -1;
}

WordStack::~WordStack(){

}

bool WordStack::Push(wordStateType w){
	if (top_idx < STACKSIZE-1){
		top_idx++;
		stack[top_idx]=w;
		return true;
	}
	return false;
}

bool WordStack::Pop(){
	if (top_idx>=0){
		top_idx--;
		return true;
	}
	return false;
}

bool WordStack::Empty(){
	if (top_idx < 0)
		return true;
	else
		return false;
}

wordStateType WordStack::Top(){
wordStateType w;

    if (top_idx >= 0)
		return stack[top_idx];
	else{
		w.backState=-1;
		w.branchState=0;
		return w;
	}
}
