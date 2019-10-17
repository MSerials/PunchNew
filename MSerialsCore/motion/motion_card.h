#pragma once
#include <iostream>
#pragma warning(disable:4100)
#ifndef __MOTION_CARD__
#define __MOTION_CARD__

#ifndef BDAQCALL
    #ifndef _WIN64
    typedef signed int int32;
    typedef unsigned int uint32;
    #else
    typedef signed long int32;
    typedef unsigned long uint32;
    #endif
#endif

#define NoError_Find 0
#define NoCard       1
#define NoNetCard    2
#define NoSure       3

class motion_card
{
public:
        int isNegLimit = 0;
        motion_card(int Method = 0) {
            isNegLimit = Method;
            std::cout << "init parent motion card" << std::endl;
}
virtual ~motion_card() { };// std::cout << "freeing parent motion card" << std::endl; }

	virtual const char* ClassName() { return "motion"; }

	virtual void SetAxisZero(short Axis = 0) = 0;

	int Card_Quantity() { return 0; }
    //override
    virtual int ReadInputBit(uint32 bit = 0, int second_sel = 0) = 0;
    //override
    virtual void WriteOutput(uint32 bit) = 0;
	//override
	virtual void WriteOutput(uint32 bit, uint32 level) = 0;


	virtual bool wait_input_timeout(uint32 sel_io, uint32 bit, clock_t timeout = 1000) { printf("for special motion card use\n"); return false; };

	virtual bool wait_axis_done(short Axis = 0, long time_out = 2000) { return false; }

	virtual bool wait_input_timeout_inv(uint32 sel_io, uint32 bit, clock_t timeout = 1000) { printf("for special motion card use\n"); return false; }

	//0 是在运动中，这个函数是根据dmc1380来的
	virtual int check_done(short Axis = 0) = 0;

	//速度运行
	virtual void v_move(int axis = 0, int speed = 500, double Acc = 20,int fifo = 0) = 0;

	virtual void absolute_move(short Axis = 0, long long Dis = 50, double IntSpd = 10, double MaxSpd = 3000000, double Acc = 0.05) = 0;

	virtual void relative_move(short Axis = 0, long long Dis = 50, double IntSpd = 10, double MaxSpd = 3000000, double Acc = 0.05) = 0;

	virtual void t_move(short Axis = 0, long long Dis = 50, double IntSpd = 10, double MaxSpd = 10000, double Acc = 0.05) = 0;

	virtual void e_stop(short Axis = 0) = 0;

	virtual void e_stop_ex(short Axis = 0) = 0;

        virtual void SetLimit(short Axis, long long Pos , int sel) = 0;

	virtual void SetPosLimit(short Axis = 0, long long Pos = -1000000) = 0;

        //virtual void SetNegLimit(short Axis = 0, long long Pos = -100000) = 0;

	virtual void close() = 0;

	virtual const char* Error_Infomation() { return "can not find card"; }

  //  virtual void 
};


class void_card :public motion_card {
private:
        int pos[32] = { 0 };
public:
	void_card(int Method = 0) {// std::cout << "init void card" << std::endl; 
	};
	virtual ~void_card() {// std::cout << "freeing void motion card" << std::endl; 
	};

	const char* ClassName() { 
		return "PlaceHolder"; 
	}

	int Card_Quantity() { 
		printf("card error\n");
		return 0; 
	}

	void SetAxisZero(short Axis = 0) {
		//printf("card error set axiszero\n");
	}

	bool wait_axis_done(short Axis = 0, long time_out = 2000) { 
//		printf("card error wait done error\n");
		return true;
		return false; 
	}


        void SetLimit(short Axis, long long Pos , int sel)
        {

        }

	void SetPosLimit(short Axis, long long Pos)
	{

	}

        //void SetNegLimit(short Axis, long long Pos)
        //{

        //}

	//override
	int  ReadInputBit(uint32 bit = 0, int second_sel = 0) {
                if(bit == 6)
                    return 0;

		switch (second_sel)
		{
                case 0: std::cout <<"pos is" << pos[second_sel]<< std::endl; return pos[second_sel];
                case 1: std::cout <<"pos is" << pos[second_sel]<< std::endl; return pos[second_sel];
		default:
			return 0;
		}
                bit = 5;
		
                //printf("card error read input error\n");
		return 0;
	}
	//override
	void WriteOutput(uint32 bit) {
		//printf("card error write error\n");
	}
	//override
	virtual void WriteOutput(uint32 bit, uint32 level) {
		//printf("card error write error2\n");
	}

	virtual bool wait_input_timeout(uint32 sel_io, uint32 bit, clock_t timeout = 1000) {
		//printf("card error\n");
		return false;
	}


	virtual bool wait_input_timeout_inv(uint32 sel_io, uint32 bit, clock_t timeout = 1000) {
		//printf("card error\n");
		return false;
	}

	//0 是在运动中，这个函数是根据dmc1380来的
	virtual int check_done(short Axis = 0) {
		//printf("card error\n");
		return 0;
	};

	virtual void v_move(int axis = 0, int speed = 500, double Acc = 20, int fifo = 0)
	{
                printf(" moving speed is %d Axis %d %f %d\n",speed, axis,Acc, fifo);
		if (0 == axis)
		{
			pos[0] += speed/5000;
		}
		if (1 == axis)
		{
			pos[1] += speed/5000;
		}
		Sleep(1);
		//printf("card error\n");
	}


	void absolute_move(short Axis = 0, long long Dis = 50, double IntSpd = 10, double MaxSpd = 3000000, double Acc = 0.05) {
		//printf("card error\n");
	}

	void relative_move(short Axis = 0, long long Dis = 50, double IntSpd = 10, double MaxSpd = 3000000, double Acc = 0.05) {
		//printf("card error\n");
	}


	virtual void t_move(short Axis = 0, long long Dis = 50, double IntSpd = 10, double MaxSpd = 10000, double Acc = 0.05) {
		//printf("card error\n");
	};

	virtual void e_stop(short Axis = 0){
		//printf("card error\n");
	}


	virtual void e_stop_ex(short Axis = 0) {
		//printf("card error\n");
	}

	void close() {
		//printf("card error\n");
	}
	const char* Error_Infomation() { return "can not find card"; }
};



#endif
