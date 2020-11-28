/* @author: D.Wei */
#ifndef __GUARD_PTR_H__
#define __GUARD_PTR_H__

#include <stdlib.h>

enum {NONE,ARRAY};
template<typename T, int type = NONE>
class GuardPtr
{
public:
    GuardPtr(T*& ptr):_ptr(ptr){};
    ~GuardPtr()
    {
        if (_ptr){
	    	switch(type){
	    	case ARRAY: delete [] _ptr;
	    		break;
	    	default: delete _ptr;
	    		break;
	    	}
	        _ptr = NULL;
        }
    }

private:
    T*& _ptr;
};

class AutoFree
{
public:
    AutoFree(void* ptr):_ptr(ptr){};
    ~AutoFree()
    {
        if (_ptr){
	    	free(_ptr);
        }
    }

private:
    void* _ptr;
};


#endif
