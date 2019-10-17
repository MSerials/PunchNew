#pragma once


#ifndef __LESAI_DMC1380__
#define __LESAI_DMC1380__

#include "../../third_party/dmc1380/leisai_dmc1380/Dmc1380.h"
#include "motion_card.h"

class DMC_1380:public motion_card
{
private:
	int method = 0;
public:
	DMC_1380(int _method = 0) { 
		method = _method; 
		std::cout << "dmc1380 initialing card" << std::endl;
		//d1000_board_init();
	}
    virtual ~DMC_1380() { std::cout << "closing dmc1380 card" << std::endl; }
	virtual uint32 ReadInputBit(uint32 bit) {
		//return d1000_in_bit(bit);
		return 0;
	};
	virtual void WriteOutput(uint32 bit) { return; }
};


#endif