#ifndef __DATA_TYPE__
#define __DATA_TYPE__

#include "MCPPUtil.h"
#include <mutex>

using namespace MSerials;

class ATOM_BOOL:public MRootObject{
    private:
        bool _v;
        std::mutex _mtx;
    public:
    ATOM_BOOL(){_v = 0;}

    ATOM_BOOL(const ATOM_BOOL& a_b){ _v = a_b._v;}

    ATOM_BOOL& operator = ( const ATOM_BOOL& v){_v = v._v; return *this;}


};


#endif