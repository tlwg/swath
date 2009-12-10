#include "worddef.h"

#define STACKSIZE 500

class WordStack {
private:
	wordStateType stack[STACKSIZE];
    short int top_idx;
public:
	WordStack();
	~WordStack();
	bool Push(wordStateType w);
	bool Pop();
	wordStateType Top();
	bool Empty();
};
