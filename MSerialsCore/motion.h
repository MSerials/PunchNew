#pragma once 
#include <vector>

//#include "motion/lesai_dmc1380.h"
#include "motion/motion_card.h"
#include "motion\yanwei.h"


class motion
{
    private:
		void_card * vc = nullptr;
		std::vector<motion_card*> m_motion_card;
		int current_card_idx = -1;
		int method = 0;
    

    motion(int _method = 0){
          method = _method;
		  vc = new void_card();
		  
    }

public:
	static motion * GetIns() { static motion _m; return &_m; }
        int init(int method = 0) {

		//DMC_1380 *dmc_1380card = new DMC_1380(method);
		//vc = new void_card();
		Yanwei_IMCnet *yanwei_IMCnet = new Yanwei_IMCnet(method);
		if (0 != yanwei_IMCnet->Card_Quantity()) {
			m_motion_card.push_back(yanwei_IMCnet);
		}
		else
		{
			std::cout << yanwei_IMCnet->Error_Infomation() << std::endl;
			delete yanwei_IMCnet;
		}
		if (m_motion_card.size() > 0) {
			current_card_idx = 0;
                }

		for (auto mc : m_motion_card)
		{
			std::cout <<"Card Vendor:"<<mc->ClassName()<<std::endl;
		}
                return static_cast<int>(m_motion_card.size());
	}

	motion_card* CurrentCard() {
		try {
			return m_motion_card.at(current_card_idx);
		}
		catch (std::out_of_range e) {
			if (vc == nullptr) vc = new void_card();
			return vc;
		}
	}

	void select_card(int idx) {
		current_card_idx = idx;
	}

	size_t card_type_quantity()
	{
		return m_motion_card.size();
	}
};
