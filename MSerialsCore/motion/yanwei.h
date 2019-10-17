#ifndef ___YAN_WEI___
#define ___YAN_WEI___

#include "motion_card.h"
#include <vector>
#include <string>
#include <fstream>


#include "../../../third_party/yanwei/inc/IMC_Def.h"
#include "../../../third_party/yanwei/inc/IMCnet.h"
#include "../../../third_party/yanwei/inc/PackageFun.h"
#include "../../../third_party/yanwei/inc/ParamDef.h"
//#pragma comment(lib,"../../third_party/yanwei/inc/IMCnet.lib")

#define USE_PAUSE

#define X_AXIS_INV global::GetIns()->prj->x_axis_inv
#define Y_AXIS_INV global::GetIns()->prj->y_axis_inv

class Yanwei_IMCnet:public motion_card{
private:

	int ErrorCode = 0;
	std::string error_information;
	int card_num = 0;


	IMC_HANDLE m_handle = nullptr;
	//����
	short m_naxis = 0;

	//IO�ڱ���
	WR_MUL_DES m_motionLoc[16];
	WR_MUL_DES m_posDes[16];
	WR_MUL_DES m_encpDes[16];
	WR_MUL_DES m_errDes[16];
	WR_MUL_DES m_aioDes[16];
	WR_MUL_DES m_ginDes[2];
	WR_MUL_DES m_goutDes[3];
	WR_MUL_DES m_OtherDes[3];

	std::vector<std::string> m_net_information;

	BOOL IsError(IMC_STATUS status)
	{
		if (status != IMC_OK) {
			return TRUE;
		}
		else return FALSE;
		return TRUE;
	}

	IMC_STATUS GetParamMul(IMC_HANDLE handle, pWR_MUL_DES data, long ArrNum)
	{
		IMC_STATUS status = IMC_OK;
		do {
			status = IMC_GetMulParam(handle, data, static_cast<DWORD>(ArrNum));
			if (!IsError(status)) return status;
			Sleep(5);
		} while ((status == IMC_FIFO_FULL || status == IMC_TIME_OUT));
		return status;
	}

	WORD ReadVersion(IMC_HANDLE handle)
	{
		IMC_STATUS status = IMC_OK;
		WORD data = 0;
		do {
			status = IMC_GetParam16(handle, hwversionLoc, (short*)&data, 0);
			if (!IsError(status)) return data;
		} while ((status == IMC_FIFO_FULL || status == IMC_TIME_OUT));
		return data;
	}


	//��ʼ������
	void InitVars() {
		int i;

		

		for (i = 0; i<16; i++)
		{
			m_motionLoc[i].axis = i;
			m_motionLoc[i].addr = motionLoc;
			m_motionLoc[i].len = 2;


			m_posDes[i].axis = i;
			m_posDes[i].addr = curposLoc;
			m_posDes[i].len = 2;

			m_encpDes[i].axis = i;
			m_encpDes[i].addr = encpLoc;
			m_encpDes[i].len = 2;

			m_errDes[i].axis = i;
			m_errDes[i].addr = errorLoc;
			m_errDes[i].len = 1;

			m_aioDes[i].axis = i;
			m_aioDes[i].addr = aioLoc;
			m_aioDes[i].len = 1;
		}
		m_ginDes[0].axis = 0;
		m_ginDes[0].addr = gin1Loc;
		m_ginDes[0].len = 1;
		m_ginDes[1].axis = 0;
		m_ginDes[1].addr = gin2Loc;
		m_ginDes[1].len = 1;

		m_goutDes[0].axis = 0;
		m_goutDes[0].addr = gout1Loc;
		m_goutDes[0].len = 1;
		m_goutDes[1].axis = 0;
		m_goutDes[1].addr = gout2Loc;
		m_goutDes[1].len = 1;
		m_goutDes[2].axis = 0;
		m_goutDes[2].addr = gout3Loc;
		m_goutDes[2].len = 1;

		m_OtherDes[0].axis = 0;
		m_OtherDes[0].addr = stopinLoc;
		m_OtherDes[0].len = 1;
		m_OtherDes[1].axis = 0;
		m_OtherDes[1].addr = emstopLoc;
		m_OtherDes[1].len = 1;
		m_OtherDes[2].axis = 0;
		m_OtherDes[2].addr = hpauseLoc;
		m_OtherDes[2].len = 1;
	}



	void connect_card() {

		if (m_handle != NULL) {
			IMC_Close(m_handle);
			m_handle = NULL;
		}
		int id, net;
		IMC_STATUS status;
		size_t net_max = m_net_information.size();
                int id_max = 1;
		int try_time = 0;
		bool init_failed = true;
		for (id = 0; id < id_max; id++) {
			for (net = 0; net < net_max; net++)
			{
				status = IMC_Open(&m_handle, net, id);
				if (status != IMC_OK) {
					if (status == IMC_VERSION_ERROR) {
                                                error_information = "控制卡版本不对\n";
					}
					else
                                                error_information = "控制卡异常\n";
				}
				else {
					status  = IMC_GetParam16(m_handle, naxisLoc, &m_naxis, 0);
					status = NetConfig(m_naxis);
					if (IMC_OK != status)
					{
						break;
					}
					WORD ver = ReadVersion(m_handle);
                                        printf("version %d\n",ver);
					error_information = "";
					card_num++;
					init_failed = false;
					try_time++;
					goto endfun;
					
				}
			}
		}

	endfun:
		if (false == init_failed){
			std::cout << "yanwei init failed,try " <<try_time<< " times"<< std::endl;
		}


	}

	//��ʼ�����ƿ�����
	IMC_STATUS NetConfig(int Naxis)
	{
		IMC_STATUS status;
		long axis;
		status = SetParam16(m_handle, clearimcLoc, -1, 0, SEL_IFIFO);
		for (axis = 0; axis<Naxis; axis++) {
			status = SetParam16(m_handle, clearLoc, -1, axis, SEL_IFIFO);			//��������λ��ֵ��״̬,����clear����������ڵ�һ
			status = SetParam16(m_handle, errorLoc, 0, axis, SEL_IFIFO);			//
			status = SetParam16(m_handle, emstopLoc, 0, axis, SEL_IFIFO);			//
																					//	status = SetParam16(m_handle, sethomeLoc, -1, axis, SEL_IFIFO);		//�ѵ�ǰλ����Ϊԭ��
			status = SetParam16(m_handle, pathabsLoc, -1, axis, SEL_IFIFO);		//���ø�����·���岹������Ϊ��������
			status = SetParam16(m_handle, steptimeLoc, 400, axis, SEL_IFIFO);		//������������������ָ�����裬һ���ŷ�Ϊ20���ϣ�����Ϊ100����
			status = SetParam16(m_handle, dirtimeLoc, 400, axis, SEL_IFIFO);
			status = SetParam32(m_handle, accellimLoc, 0x7FFFFFFF, axis, SEL_IFIFO);//���ٶ����ƣ�������������
			status = SetParam32(m_handle, vellimLoc, 0x7FFFFFFF, axis, SEL_IFIFO);//�ٶ����ƣ�������������
			status = SetParam32(m_handle, mcsmaxvelLoc, 500000, axis, SEL_IFIFO);//������ϵ�㵽���˶�������ٶ�
			status = SetParam32(m_handle, pcsmaxvelLoc, 500000, axis, SEL_IFIFO);//������ϵ�㵽���˶�������ٶ�
			status = SetParam32(m_handle, mcsaccelLoc, 20000, axis, SEL_IFIFO);	//������ϵ���ٶ�
			status = SetParam32(m_handle, pcsaccelLoc, 20000, axis, SEL_IFIFO);	//������ϵ���ٶ�
			status = SetParam32(m_handle, mcsdecelLoc, 20000, axis, SEL_IFIFO);	//������ϵ���ٶ�		
			status = SetParam32(m_handle, pcsdecelLoc, 20000, axis, SEL_IFIFO);	//������ϵ���ٶ�

			status = SetParam32(m_handle, highvelLoc, 200000, axis, SEL_IFIFO);	//��������ĸ��ٶ�
			status = SetParam32(m_handle, lowvelLoc, 65536, axis, SEL_IFIFO);	//��������ĵ��ٶ�
			status = SetParam32(m_handle, homestposLoc, 0, axis, SEL_IFIFO);	//�������


			//只使能0轴的限位
			if(0 == axis)
			{
                            short Value_ = 0;
                                //默认为1 右方向
                                if(!isNegLimit)
                                {
                                    Value_ = (short)0x000A | (short)(1<<14);
                                }
                                else
                                {
                                    Value_ = (short)0x000A  | short(1<<15);
                                }

                                status = SetParam16(m_handle, aioctrLoc, Value_, axis, SEL_IFIFO);	//��IO���üĴ���,�͵�ƽ��Ч
			}
			else
				status = SetParam16(m_handle, aioctrLoc, (short)0x000A, axis, SEL_IFIFO);

			if (0)
				status = SetParam16(m_handle, encpctrLoc, (short)0x0004, axis, SEL_IFIFO);	//�����������üĴ���,bit15=1Ϊʹ���ڲ����ⷴ��������ο����ƿ�˵����
			else
				status = SetParam16(m_handle, encpctrLoc, (short)0x8004, axis, SEL_IFIFO);	//�����������üĴ���,bit15=1Ϊʹ���ڲ����ⷴ��������ο����ƿ�˵����

			//status = SetParam16(m_handle, aioctrLoc, 32, axis, SEL_IFIFO);
			status = SetParam16(m_handle, smoothLoc, 32, axis, SEL_IFIFO);		//�˶�ƽ������,�����S���߼Ӽ��ٵ�ƽ���̶�
			status = SetParam16(m_handle, settlewinLoc, 1, axis, SEL_IFIFO);
			status = SetParam16(m_handle, settlewinLoc, 0, axis, SEL_IFIFO);	//��ֹ����,�������ڴ�ֵ��iMC��������ֹ����Ϊ�ŷ���������һ��Ӧ����0
			status = SetParam16(m_handle, enaLoc, -1, axis, SEL_IFIFO);			//ʹ�ܸ��ᣬ�����Ƿ��������ʹ�ܣ�ena������д�����ֵ,�����������
			status = SetParam16(m_handle, exitfiltLoc, 0x10, axis, SEL_IFIFO);	//���������ʱ��error/=0�����᲻�˳�����(��ͣʱ�˳�)
			status = SetParam16(m_handle, stopfiltLoc, (short)0xFBFF, axis, SEL_IFIFO);	//�����κδ����ʱ�򣬸���ֹͣ����
			status = SetParam16(m_handle, haltstepLoc, 0, axis, SEL_IFIFO);
			status = SetParam16(m_handle, runLoc, -1, axis, SEL_IFIFO);			//���и���
		}
		status = SetParam16(m_handle, clrPFIFO1Loc, -1, 0, SEL_IFIFO);	//���PFIFO1����ָ��
		status = SetParam32(m_handle, pwaittime1Loc, 0, 0, SEL_IFIFO);	//����ȴ�������֮ǰδ��ɵĵȴ�ָ�����������͵�fifo��ָ��
		status = SetParam16(m_handle, startpath1Loc, -1, 0, SEL_IFIFO);		//���ò岹�˶��ռ�1��ָ��
		status = SetParam32(m_handle, pathacc1Loc, 1000, 0, SEL_IFIFO);		//·�����ٶ�
		status = SetParam32(m_handle, feedrate1Loc, 65536, 0, SEL_IFIFO);	//1:1�Ľ�������
		status = SetParam32(m_handle, segtgvel1Loc, 500000, 0, SEL_IFIFO);	//���öε������ٶ�
		status = SetParam32(m_handle, segendvel1Loc, 0, 0, SEL_IFIFO);		//���ö�ĩ���ٶ�

		status = SetParam16(m_handle, clrPFIFO2Loc, -1, 0, SEL_IFIFO);	//���PFIFO2����ָ��
		status = SetParam32(m_handle, pwaittime2Loc, 0, 0, SEL_IFIFO);	//����ȴ�������֮ǰδ��ɵĵȴ�ָ�����������͵�fifo��ָ��
		status = SetParam16(m_handle, startpath2Loc, -1, 0, SEL_IFIFO);		//���ò岹�˶��ռ�2
		status = SetParam32(m_handle, pathacc2Loc, 1000, 0, SEL_IFIFO);		//·�����ٶ�
		status = SetParam32(m_handle, feedrate2Loc, 65536, 0, SEL_IFIFO);	//1:1�Ľ�������
		status = SetParam32(m_handle, segtgvel2Loc, 500000, 0, SEL_IFIFO);	//���öε������ٶ�
		status = SetParam32(m_handle, segendvel2Loc, 0, 0, SEL_IFIFO);		//���ö�ĩ���ٶ�

		status = SetParam16(m_handle, eventsLoc, 0, 0, SEL_IFIFO);				//��������¼�ָ��,�����֮ǰ���¼�ָ�����ӽ�ȥ���¼�ָ����ԭ�е��¼�ָ��
		return status;
	}

	void Init() {
		NIC_INFO info;
		IMC_STATUS Status;
		int i, num;
		//�о�����
		Status = IMC_FindNetCard(&info, &num);
		if (Status == IMC_OK) {
			for (i = 0; i<num; i++) {
				m_net_information.push_back(std::string(info.description[i]));
			}
		}
		connect_card();
	}


public:
	//����
	Yanwei_IMCnet(int Method = 0)
		:error_information("")
	{
                isNegLimit = !Method;
		InitVars();
		Init();
	}
	virtual ~Yanwei_IMCnet() { // std::cout << "freeing Yanwei motion card" << std::endl;
	
	}

	void _close()
	{
		if (NULL == m_handle)
			return;
		for (int i = 0; i < m_naxis; i++)
		{
			SetParam16(m_handle, enaLoc, 0, i, SEL_QFIFO);
			SetParam16(m_handle, aioctrLoc, (short)0x0, i, SEL_IFIFO);	//��IO���üĴ���,�͵�ƽ��Ч
		}
		IMC_Close(m_handle);
	}

	void close() {
		std::cout << "free yan wei cotronl" << std::endl;
		_close();
	}
	
	const char* ClassName() {
		return "YanWei, Tip: With IO Mask Operation";
	}
	
	//override
	int Card_Quantity() {
		return card_num;
	}

#define YANWEI_IO_SEL			0
#define YANWEI_OUT_IO_SEL		1
#define YANWEI_AXIS_ERROR		2
#define YANWEI_AXIS_IO			3
#define YANWEI_AXIS_CMD_POS		4
#define YANWEI_AXIS_POS			5
#define YANWEI_AXIS_STATE               6
#define YANWEI_REG_ERROR		7
#define YANWEI_ORIGIN_BIT		(1<<2)
#define YANWEI_UP_LIMIT			(1<<0)
#define YANWEI_DOWN_LIMIT		(1<<1)
	//�������ڿ��ƿ�������������Ƕ�ȡʲô�ڣ��ڱ������ IO��ͨ��IO�� �ᣨ��������AXIO������������źţ�,��ȡ��еλ��
    int ReadInputBit(uint32 bit = 0, int second_sel = 0){
		if (NULL == m_handle) return 0;
		IMC_STATUS _Status = IMC_OK;
		switch (bit) {
		case YANWEI_IO_SEL:_Status = GetParamMul(m_handle, m_ginDes, 2); 
			if (IMC_OK == _Status) {
				return (int)(m_ginDes[second_sel].data[0]);
			}
			return 0;
			break;
		case YANWEI_OUT_IO_SEL:_Status = GetParamMul(m_handle, m_goutDes, 2); 
			if (IMC_OK == _Status) {
				return (int)(m_goutDes[second_sel].data[0]);
			}
			return 0;
			break;
		case YANWEI_AXIS_ERROR:_Status = GetParamMul(m_handle, m_errDes, 2); 
			if (IMC_OK == _Status) {
				return (int)(m_errDes[second_sel].data[0]);
			}
			return 0;
			break;
		case YANWEI_AXIS_IO:_Status = GetParamMul(m_handle, m_aioDes, m_naxis);
			if (IMC_OK == _Status) {
				return (int)(m_aioDes[second_sel].data[0]);
			}
			return 0;
			break;
		case YANWEI_AXIS_CMD_POS:_Status = GetParamMul(m_handle, m_posDes, m_naxis);
			if (IMC_OK == _Status) {
				int pos = m_posDes[second_sel].data[0] | m_posDes[second_sel].data[1] << 16;
				return pos;
			}
			return 0;
			break;
		//��ȡ��еλ��
		case YANWEI_AXIS_POS:_Status = GetParamMul(m_handle, m_encpDes, m_naxis); 
			if (IMC_OK == _Status) {
				int pos = m_encpDes[second_sel].data[0] | m_encpDes[second_sel].data[1] << 16;
				/**
				if (0 == m_naxis)
					return X_AXIS_INV * pos;
				else if (1 == m_naxis)
					return Y_AXIS_INV * pos;
					*/
				return pos;
			}
			return 0;
			break;
		case YANWEI_AXIS_STATE:_Status = GetParamMul(m_handle, m_motionLoc, m_naxis);
			if (IMC_OK == _Status) {
				int pos_ = m_motionLoc[second_sel].data[0] | m_motionLoc[second_sel].data[1] << 16;
				return pos_;
			}
			return 1;
			break;
		case YANWEI_REG_ERROR:_Status = GetParamMul(m_handle, m_errDes, m_naxis); 
			if (IMC_OK == _Status) {
				return static_cast<int>(m_errDes[second_sel].data[0]);
			}
			return 0; 
			break;
		default:return 0;
		} 
		if (IMC_OK != _Status){
			return static_cast<int>(m_ginDes[0].data[0]);
		}
		return 0;
		for (int i = 0; i < 4; i++){
			std::cout << " bit:" << m_ginDes[0].data[i];
		}
		std::cout<<std::endl;

    };
    //override
    void WriteOutput(uint32 bit){
		std::cout << "not support this operation" << std::endl;
    };

	void SetAxisZero(short Axis = 0) {
		if (m_handle == NULL)
			return;
		SetParam16(m_handle, clearLoc, -1, Axis, SEL_QFIFO);
	}


        void SetLimit(short Axis, long long Pos , int sel)
        {

            if (m_handle == NULL)
                    return;
            //默认为0,也就是右方向
            if(0!=sel)
            {
                SetParam32(m_handle, psoftlimLoc, Pos, Axis, SEL_QFIFO);
            }
            else
            {
                //右方向
                SetParam32(m_handle, nsoftlimLoc, Pos, Axis, SEL_QFIFO);

            }
        }



        void SetPosLimit(short Axis, long long Pos)
	{
		if (m_handle == NULL)
			return;
		SetParam32(m_handle, psoftlimLoc, Pos, Axis, SEL_QFIFO);
		
	}


/*

	void SetNegLimit(short Axis, long long Pos)
	{
		if (m_handle == NULL)
			return;
		SetParam32(m_handle, nsoftlimLoc, Pos, Axis, SEL_QFIFO);
	}
*/
	void WriteOutput(uint32 bit, uint32 level) {
		if (m_handle == NULL)
			return;
		static std::mutex mtx;
		std::lock_guard<std::mutex> lck(mtx);
		DWORD Tick = GetTickCount();
		IMC_STATUS _status = IMC_OK;
		do {
			//�����bug ���state_bit�滻��bit
			_status = IMC_SetParamBit(m_handle, gout1Loc, bit, level, 0, SEL_IFIFO);
			if (!IsError(_status)) return;
			if ((GetTickCount() - Tick) > 1000)
			{
				return;
			}
			Sleep(10);
		} while ((_status == IMC_FIFO_FULL || _status == IMC_TIME_OUT));
	};

	bool wait_input_timeout(uint32 sel_io, uint32 bit,clock_t timeout = 1000) {
		clock_t t = clock();
		for (; ((1 << bit)&(ReadInputBit(sel_io)));){
				if ((clock() - t) > timeout)
				return false;
			}
		return true;
	}


	bool wait_input_timeout_inv(uint32 sel_io, uint32 bit, clock_t timeout = 1000) {
		clock_t t = clock();
		for (; !((1 << bit)&(ReadInputBit(sel_io)));) {
			if ((clock() - t) > timeout)
				return false;
		}
		return true;
	}

	//0 �����˶��У���������Ǹ���dmc1380����
	int check_done(short Axis = 0) {
		return ReadInputBit(YANWEI_AXIS_STATE,Axis);
	}

	//�ȴ����ֹͣ
	bool wait_axis_done(short Axis = 0, long time_out = 2000) {
		Sleep(2);
		clock_t clk = clock();
		for (; 0 != ReadInputBit(YANWEI_AXIS_STATE, Axis);) {
			//std::cout << "�˶���" << std::endl;
			Sleep(2);
			if ((clock() - clk) > time_out)
			{
				return false;
			}
		}
		//std::cout << "�Ѿ�ֹͣ" << std::endl;
		return true;
	}


	//200ms加上去
	void v_move(int Axis = 0, int speed = 500, double Acc  =0.005,int fifo = SEL_QFIFO)
	{
		if (m_handle == NULL)
			return;
		int i = Axis;
		IMC_STATUS status = IMC_OK;	
		status = SetParam16(m_handle, emstopLoc, 0, i, SEL_QFIFO);			//���и���
																			//ʹ�������
		status = SetParam16(m_handle, enaLoc, -1, i, SEL_QFIFO);			//ʹ�ܸ��ᣬ�����Ƿ��������ʹ�ܣ�ena������д�����ֵ,�����������
																			//���������
		status = SetParam16(m_handle, runLoc, -1, i, SEL_QFIFO);			//���и���
		int acc = Acc * speed;
		status = SetParam32(m_handle, mcsaccelLoc, acc, Axis, SEL_IFIFO);	//������ϵ���ٶ�
		status = SetParam32(m_handle, pcsaccelLoc, acc, Axis, SEL_IFIFO);	//������ϵ���ٶ�
		status = SetParam32(m_handle, mcsdecelLoc, acc, Axis, SEL_IFIFO);	//������ϵ���ٶ�		
		status = SetParam32(m_handle, pcsdecelLoc, acc, Axis, SEL_IFIFO);	//������ϵ���ٶ�
		status = IMC_MoveVelocity(m_handle,speed,Axis,fifo); 

	}

	//��0�����Ѿ�ֹͣ��
//	int check_done(short Axis)
//	{
	
//	}

	void absolute_move(short Axis = 0, long long Dis = 50, double IntSpd = 10, double MaxSpd = 3000000, double Acc = 20) {
		if (NULL == m_handle)
			return;
		IMC_STATUS status = IMC_OK;
		status = SetParam16(m_handle, emstopLoc, 0, Axis, SEL_IFIFO);			//���и���											//ʹ�������
		status = SetParam16(m_handle, enaLoc, -1, Axis, SEL_IFIFO);			//ʹ�ܸ��ᣬ�����Ƿ��������ʹ�ܣ�ena������д�����ֵ,�����������																	//���������
		status = SetParam16(m_handle, runLoc, -1, Axis, SEL_IFIFO);			//���и���
		long acc = static_cast<long>(MaxSpd * Acc);
		status = SetParam32(m_handle, mcsmaxvelLoc, static_cast<long>(MaxSpd), Axis, SEL_IFIFO);//������ϵ�㵽���˶�������ٶ�
		status = SetParam32(m_handle, pcsmaxvelLoc, static_cast<long>(MaxSpd), Axis, SEL_IFIFO);//������ϵ�㵽���˶�������ٶ�
		status = SetParam32(m_handle, mcsaccelLoc, acc, Axis, SEL_IFIFO);	//������ϵ���ٶ�
		status = SetParam32(m_handle, pcsaccelLoc, acc, Axis, SEL_IFIFO);	//������ϵ���ٶ�
		status = SetParam32(m_handle, mcsdecelLoc, acc, Axis, SEL_IFIFO);	//������ϵ���ٶ�		
		status = SetParam32(m_handle, pcsdecelLoc, acc, Axis, SEL_IFIFO);	//������ϵ���ٶ�
		IMC_MoveAbsolute(m_handle, Dis, Axis, SEL_QFIFO);
	}

	void relative_move(short Axis = 0, long long Dis = 50, double IntSpd = 10, double MaxSpd = 3000000, double Acc = 1) {
		if (NULL == m_handle)
			return;
		IMC_STATUS status = IMC_OK;
		status = SetParam16(m_handle, emstopLoc, 0, Axis, SEL_IFIFO);			//���и���
																		//ʹ�������
		status = SetParam16(m_handle, enaLoc, -1, Axis, SEL_IFIFO);			//ʹ�ܸ��ᣬ�����Ƿ��������ʹ�ܣ�ena������д�����ֵ,�����������
																				//���������
		status = SetParam16(m_handle, runLoc, -1, Axis, SEL_IFIFO);			//���и���

                long acc = static_cast<long>(MaxSpd * Acc);
		status = SetParam32(m_handle, mcsmaxvelLoc, static_cast<long>(MaxSpd), Axis, SEL_IFIFO);//������ϵ�㵽���˶�������ٶ�
		status = SetParam32(m_handle, pcsmaxvelLoc, static_cast<long>(MaxSpd), Axis, SEL_IFIFO);//������ϵ�㵽���˶�������ٶ�
		status = SetParam32(m_handle, mcsaccelLoc, acc, Axis, SEL_IFIFO);	//������ϵ���ٶ�
		status = SetParam32(m_handle, pcsaccelLoc, acc, Axis, SEL_IFIFO);	//������ϵ���ٶ�
		status = SetParam32(m_handle, mcsdecelLoc, acc, Axis, SEL_IFIFO);	//������ϵ���ٶ�		
		status = SetParam32(m_handle, pcsdecelLoc, acc, Axis, SEL_IFIFO);	//������ϵ���ٶ�
		IMC_MoveRelative(m_handle, Dis, Axis, SEL_QFIFO);
	}

	void t_move(short Axis = 0, long long Dis = 50, double IntSpd = 10, double MaxSpd = 3000000, double Acc = 0.05) {
		if (m_handle == NULL)
			return;
		IMC_MoveAbsolute(m_handle,Dis,Axis,SEL_QFIFO);
		return;
		

		int i = Axis;
		static WR_MUL_DES pos1[16], pos2[16], mcsgo[16];
		IMC_STATUS status = IMC_OK;
		pos1[i].axis = i;
		pos1[i].addr = mcstgposLoc;
		pos1[i].len = 2;
		pos1[i].data[0] = Dis;
		pos1[i].data[1] = 0;
		pos2[i].axis = i;
		pos2[i].addr = mcstgposLoc;
		pos2[i].len = 2;
		pos2[i].data[0] = 0;
		pos2[i].data[1] = 0;
		mcsgo[i].axis = i;
		mcsgo[i].addr = mcsgoLoc;
		mcsgo[i].len = 1;
		mcsgo[i].data[0] = -1;

		status = SetParam16(m_handle, emstopLoc, 0, i, SEL_IFIFO);			//���и���
																				//ʹ�������
		status = SetParam16(m_handle, enaLoc, -1, i, SEL_IFIFO);			//ʹ�ܸ��ᣬ�����Ƿ��������ʹ�ܣ�ena������д�����ֵ,�����������
																				//���������
		status = SetParam16(m_handle, runLoc, -1, i, SEL_IFIFO);			//���и���
																	//���˶�
		status = SetParamMul(m_handle, pos1, i, SEL_QFIFO);
		status = SetParamMul(m_handle, mcsgo, i, SEL_QFIFO);
		std::cout << "Axis:" << Axis << " Move Forward:" << Dis << std::endl;
	};


	virtual void e_stop_ex(short Axis = 0) {
		//printf("card error\n");
		if (Axis < 0)
		{

                    for (int i = 0; i < 4; i++)
#ifdef USE_PAUSE
                                SetParam16(m_handle, mcsgoLoc, 0x0, i, SEL_QFIFO);
#else
				SetParam16(m_handle, emstopLoc, 0x10, i, SEL_QFIFO);
#endif
		}
		else
		{
#ifdef USE_PAUSE
                        SetParam16(m_handle, mcsgoLoc, 0x0, Axis, SEL_QFIFO);
#else
                        SetParam16(m_handle, emstopLoc, 0x10, Axis, SEL_QFIFO);
#endif
		}
	}

	void e_stop(short Axis = 0) {
		IMC_STATUS status = IMC_MoveVelocity(m_handle, 0, 0x0, SEL_QFIFO);


                if (Axis < 0)
		{
			for (int i = 0; i < 4; i++)
                        {
                                IMC_STATUS status = IMC_MoveVelocity(m_handle, 2000, i, SEL_QFIFO);
                                printf("%d\n",status);
                        }

			Sleep(20);
			for (int i = 0; i < 4; i++)
				status = IMC_MoveVelocity(m_handle, 0, i, SEL_QFIFO);
			Sleep(80);
			return;

			for (int i = 0; i < 4; i++)
				SetParam16(m_handle, emstopLoc, 0x10, Axis, SEL_QFIFO);
		}
		else
		{
			IMC_STATUS status = IMC_MoveVelocity(m_handle, 200, Axis, SEL_QFIFO);
			Sleep(20);
			status = IMC_MoveVelocity(m_handle, 0, Axis, SEL_QFIFO);
			Sleep(80);
			return;

			Sleep(100);
			SetParam16(m_handle, emstopLoc, 0x10, Axis, SEL_QFIFO);
		}
	};


	const char *Error_Infomation() {
		if (0 != error_information.length())
			return error_information.c_str();
		switch (ErrorCode) {
		case NoError_Find:return "No Error";
		case NoCard:return "No Card";
		case NoNetCard:return "No Net Card";
		case NoSure:return "Unknown Error";
		default:return "Unknown";
		}
	}

	private:
#define TIMER_OUT_NUM	2
		IMC_STATUS SetParam16(IMC_HANDLE handle, short loc, short data, long axis, int fifo)
		{			
			IMC_STATUS status = IMC_OK;
			do {
				status = IMC_SetParam16(handle, loc, data, axis, fifo);
				if (!IsError(status)) return status;
				Sleep(5);
			} while ((status == IMC_FIFO_FULL || status == IMC_TIME_OUT));
			return status;
		}

		IMC_STATUS SetParam32(IMC_HANDLE handle, short loc, long data, long axis, int fifo)
		{
			IMC_STATUS status = IMC_OK;
			do {
				status = IMC_SetParam32(handle, loc, data, axis, fifo);
				if (!IsError(status)) return status;
				Sleep(5);
			} while ((status == IMC_FIFO_FULL || status == IMC_TIME_OUT));
			return status;
		}

		IMC_STATUS SetParamMul(IMC_HANDLE handle, pWR_MUL_DES data, long ArrNum, int fifo)
		{
			IMC_STATUS status = IMC_OK;
			do {
				status = IMC_SetMulParam(handle, data, ArrNum, fifo);
				if (!IsError(status)) return status;
				Sleep(5);
			} while ((status == IMC_FIFO_FULL || status == IMC_TIME_OUT));
			return status;
		}
};

#endif
