#pragma once
#include <QSettings>
#include <iostream>
#include <string>
//#include "global.h"

#ifndef __SETTINGS_FILES__
#define __SETTINGS_FILES__

#define _VERSION       "20190902"

#if 1
#define PARA_SET	(1)
#define PARA_PRJ	(1<<1)
#define PARA_IMAGE	(1<<2)
#define PARA_IO		(1<<3)

#define PARA_ALL	(PARA_SET | PARA_PRJ | PARA_IMAGE |PARA_IO)   //ALL -1

#define STR_SET     "PARA_SET"
#define STR_PRJ     "PARA_PRJ"
#define STR_IMAGE   "IMAGE_SETTING"
#define STR_IO      "PARA_IO"

#define EXCEL_DIR "../ExcelDir/"
#define NG_IMAGE_DIR "../NG_IMAGE_DIR/"


#define PRJ_PATH "Prj/PrgParameter.ini"
#define MODEL_NAME_BY_USER  "请输入模具名称（非必须）"


#ifndef MASK_METHOD
#define MASK_METHOD


#define __VERSION__	_VERSION
#define __VENDOR__	"FuryAI"



//对于网口控制卡，ReadInPut函数的参数代表的是读取什么口，口被分类成 IO（通用IO） 轴（报警），和AXIO（轴的上下限信号）,返回值则是状态
//以下两组定义需要组合去用

//第二组定义
//从轴信号点读出来的错误信号,以这个标准为以后的信号标准
//对应YANWEI_AXIS_IO组
#define AXIS_NoError			0x0
#define AXIS_POS_LIMIT			(1<<0)
#define AXIS_NEG_LIMIT			(1<<1)
#define AXIS_ORG				(1<<2)
#define AXIS_PRO				(1<<3)
#define AXIS_ALM				(1<<4)
#define AXIS_E_STOP				(1<<5)
//未知的错误提示
#define PSOF					0x3
#define NSOF					0x4
//对应#define YANWEI_IO_SEL的输入
#define IN_PUNCH_ORIGIN			(1<<0)
#define X_AXIS_ORIGIN_SPEED		Preference::GetIns()->prj->xAxis_Speed
#define X_AXIS_MOTOR			0
#define Y_AXIS_MOTOR			1
#define OUT_PUNCH_CYL			(1)
#define OUT_BLOW				(1<<3)
#define ON   1
#define OFF  0
#endif


#define CAMERAMODE                              0
#define DIGITALMODE                             1
#define X_RATIO                                 Preference::GetIns()->prj->xAxis_Ratio
#define Y_RATIO                                 Preference::GetIns()->prj->yAxis_Ratio
#define X_DIS_PULS_RATIO                        Preference::GetIns()->prj->xAxis_Distance_Ratio
#define Y_DIS_PULS_RATIO                        Preference::GetIns()->prj->yAxis_Distance_Ratio
#define X_CAM_DISTANCE                          Preference::GetIns()->prj->xAxis_cam_dis
#define Y_CAM_DISTANCE                          -Preference::GetIns()->prj->yAxis_cam_dis
#define X_CAM_DISTANCE_PLS                      Preference::GetIns()->prj->xAxis_cam_distance_pls
#define Y_CAM_DISTANCE_PLS_EX                   Preference::GetIns()->prj->yAxis_cam_distance_pls
#define ADD_BOUND_SIZE                          static_cast<int>(Preference::GetIns()->prj->distance_object)
#define ADD_BOUND                               cv::Size(ADD_BOUND_SIZE,ADD_BOUND_SIZE)
#define Get_Obj_Threshold                       static_cast<int>(Preference::GetIns()->prj->threshold_value)
#define X_AXIS_SPEED                            Preference::GetIns()->prj->xAxis_Speed
#define Y_AXIS_SPEED                            Preference::GetIns()->prj->yAxis_Speed
#define CAMERA_ANGLE                            Preference::GetIns()->prj->Camera_Angle
#define MODEL_GAP                               Preference::GetIns()->prj->distance_object
#define CHECK_R1                                Preference::GetIns()->prj->Detect_ROI.r1
#define CHECK_C1                                Preference::GetIns()->prj->Detect_ROI.c1
#define CHECK_R2                                Preference::GetIns()->prj->Detect_ROI.r2
#define CHECK_C2                                Preference::GetIns()->prj->Detect_ROI.c2
#define MODEL_R1                                Preference::GetIns()->prj->Model_ROI.r1
#define MODEL_C1                                Preference::GetIns()->prj->Model_ROI.c1
#define MODEL_R2                                Preference::GetIns()->prj->Model_ROI.r2
#define MODEL_C2                                Preference::GetIns()->prj->Model_ROI.c2
#define POS_ACCURCY                             Preference::GetIns()->prj->Pos_Accurcy
#define POS_ACCURCYY                            Preference::GetIns()->prj->Pos_Accurcyy
#define BOARD_DISTANCE                          Preference::GetIns()->prj->distance_to_border
#define X_AXIS_LIMIT                            Preference::GetIns()->prj->X_Axis_Limit
#define LINE_METHOD                             Preference::GetIns()->prj->Lines_Method
#define MODE_INDEX                              Preference::GetIns()->prj->CameraRes
#define VERTICAL_SKIP_RATIO                     Preference::GetIns()->prj->Vertical_Skip_Ratio
#define RETURNRATIO                             Preference::GetIns()->prj->Return_Ratio
#define PUNCH_LIMIT                             Preference::GetIns()->prj->Limit_Counter
#define PUNCH_COUNTER                           Preference::GetIns()->prj->Current_Counter
#define DXFPATH                                 Preference::GetIns()->prj->DxfPath
#define PUNCHMODE                               Preference::GetIns()->prj->ModeSelect
#define MODELD                                  Preference::GetIns()->prj->Model_Diameter
#define MODELW                                  Preference::GetIns()->prj->Model_Width
#define MODELH                                  Preference::GetIns()->prj->Model_Height
#define DXFWL                                   Preference::GetIns()->prj->DxfWLower
#define DXFWU                                   Preference::GetIns()->prj->DxfWUpper
#define DXFHL                                   Preference::GetIns()->prj->DxfHLower
#define DXFHU                                   Preference::GetIns()->prj->DxfHUpper
#define PRJSAVE                                 Preference::GetIns()->prj->WriteSettings();
#define 左方向为原点                              Preference::GetIns()->prj->OriginIsLeft
#define CAMERAGAMMA                             Preference::GetIns()->prj->Gamma
#define DOUBLEMODEL                             Preference::GetIns()->prj->DoubleModel

//0 2048x1516
//1 1600x1200
//2 1280x1024
//3 1024x768







//X轴世界坐标误差的精度
//#define X_AXIS_PUNCH_PRECISION                  (X_RATIO*1.2)
//Y轴世界坐标误差的精度
//#define Y_AXIS_PUNCH_PRECISION                  (Y_RATIO*1.2)


/*
请配合 Ini类文件使用
使用方法，在初始化地方如下例子即可
//系统配置文件
Preference::GetIns()->sys->SetFilePos(QString("sys.ini"));
//项目配置文件
QString Path = Preference::GetIns()->sys->Project_Name + "/prj.ini";
Preference::GetIns()->prj->SetFilePos(Path);
*/

class Ini
{ 
private:
	void Init() {
		InitData();
		int SEL = PARA_ALL;
        if (0 == strcmp("sys", m_class_name.c_str()))
		{
            SEL = PARA_PRJ | PARA_IO | PARA_SET;
		}
		WriteSettings(SEL);
	}

    std::string m_class_name;
    QSettings *settings;
    QString str_set,str_prj,str_img,str_io;

public:
    Ini(std::string class_name = "Preference")
		: settings(nullptr)
        ,m_class_name (class_name)
	{
		str_set = STR_SET; str_set += "/";
		str_prj = STR_PRJ; str_prj += "/";
		str_img = STR_IMAGE; str_img += "/";
		str_io = STR_IO; str_io += "/";
		InitData();
	}

	~Ini() { delete settings; }

    const char* Class_Name() { return m_class_name.c_str(); }

	void SetFilePos(QString str) {
        if (nullptr != settings) delete settings;
		settings = new QSettings(str, QSettings::IniFormat);
		ReadSettings(PARA_ALL);
	}


public:
	void WriteSettings(int SEL = PARA_ALL)
	{

		if (SEL & PARA_SET)
		{
			
			//表示文件存在
			settings->setValue(str_set + "sign", 1);
			//settings->endGroup();
		}

		if (SEL & PARA_PRJ)
		{
        //	settings->setValue(str_prj + "PRJ_NAME", Project_Name);
		}

		if (SEL & PARA_IMAGE)
		{
            settings->setValue(str_img + "Model_Name", Model_Name);
            settings->setValue(str_img + "DXFPATH", DxfPath);
            settings->setValue(str_img + "MODESEL", ModeSelect);
            settings->setValue(str_img + "MODELDIAMETER", Model_Diameter);
            settings->setValue(str_img + "MODELWIDTH", Model_Width);
            settings->setValue(str_img + "MODELHEIGHT", Model_Height);
            settings->setValue(str_img + "DXFWL", DxfWLower);
            settings->setValue(str_img + "DXFWU", DxfWUpper);
            settings->setValue(str_img + "DXFHL", DxfHLower);
            settings->setValue(str_img + "DXFHU", DxfHUpper);
            settings->setValue(str_img + "ROIR1", Detect_ROI.r1);
            settings->setValue(str_img + "ROIC1", Detect_ROI.c1);
            settings->setValue(str_img + "ROIR2", Detect_ROI.r2);
            settings->setValue(str_img + "ROIC2", Detect_ROI.c2);
            settings->setValue(str_img + "MROIR1", Model_ROI.r1);
            settings->setValue(str_img + "MROIC1", Model_ROI.c1);
            settings->setValue(str_img + "MROIR2", Model_ROI.r2);
            settings->setValue(str_img + "MROIC2", Model_ROI.c2);
            settings->setValue(str_img + "XRATIO", xAxis_Distance_Ratio);
            settings->setValue(str_img + "YRATIO", yAxis_Distance_Ratio);
            settings->setValue(str_img + "XAXISPIXRATIO", xAxis_Ratio);
            settings->setValue(str_img + "YAXISPIXRATIO", yAxis_Ratio);
            settings->setValue(str_img + "XAXISCAMDIS", xAxis_cam_dis);
            settings->setValue(str_img + "YAXISCAMDIS", yAxis_cam_dis);
            settings->setValue(str_img + "X_CAM_DISTANC_PLS", xAxis_cam_distance_pls);
            settings->setValue(str_img + "Y_CAM_DISTANC_PLS", yAxis_cam_distance_pls);
            settings->setValue(str_img + "XAXISLMT", X_Axis_Limit);
            settings->setValue(str_img + "CAMRES", CameraRes);
            settings->setValue(str_img + "XSPD", xAxis_Speed);
            settings->setValue(str_img + "YSPD", yAxis_Speed);
            settings->setValue(str_img + "THRES_VAL", threshold_value);
            settings->setValue(str_img + "OBJ_DIS", distance_object);
            settings->setValue(str_img + "BORDER_DIS", distance_to_border);
            settings->setValue(str_img + "CAM_ANGLE", Camera_Angle);
            settings->setValue(str_img + "VERSKIPRATIO", Vertical_Skip_Ratio);
            settings->setValue(str_img + "RETURNRATIO", Return_Ratio);
            settings->setValue(str_img + "POS_ACC", Pos_Accurcy);
            settings->setValue(str_img + "POS_ACCY", Pos_Accurcyy);
            settings->setValue(str_img + "LINESMETHOD", Lines_Method);
            settings->setValue(str_img + "ORGLEFT", OriginIsLeft);
            settings->setValue(str_img + "DOUBLEMODEL", DoubleModel);
            settings->setValue(str_img + "LIMITCOUNTER", Limit_Counter);
            settings->setValue(str_img + "CURRENTCOUNTER", Current_Counter);
            settings->setValue(str_img + "GAMMA",Gamma);
            settings->setValue(str_img + "VER", Version);
		}


		if (SEL & PARA_IO)
		{

		}
	}



	void ReadSettings(int SEL = PARA_ALL)
	{
		if (SEL & PARA_SET)
		{
			//表示文件存在
			int Sign = settings->value(str_set+"sign", 0).toInt();
			if (1 != Sign) {
				Init();
//				DLOG(LOG_INFO, "ini file not found");
				return;
			}
			//settings->endGroup();
		}


		if (SEL & PARA_PRJ)
		{
			//settings->beginGroup(STR_PRJ);
            //Project_Name = settings->value(str_prj+"PRJ_NAME", QString::fromLocal8Bit("PrgParameter")).toString();
			//settings->endGroup();
		}


		if (SEL & PARA_IMAGE)
		{
            Model_Name = settings->value(str_img + "Model_Name").toString();
            DxfPath = settings->value(str_img + "DXFPATH").toString();
            ModeSelect = settings->value(str_img + "MODESEL").toInt();
            Model_Diameter = settings->value(str_img + "MODELDIAMETER",37.2).toDouble();
            Model_Width = settings->value(str_img + "MODELWIDTH",30).toDouble();
            Model_Height = settings->value(str_img + "MODELHEIGHT",20).toDouble();
            DxfWLower = settings->value(str_img + "DXFWL",1).toDouble();
            DxfWUpper = settings->value(str_img + "DXFWU",100).toDouble();
            DxfHLower = settings->value(str_img + "DXFHL",1).toDouble();
            DxfHUpper = settings->value(str_img + "DXFHU",100).toDouble();
            Detect_ROI.r1 = settings->value(str_img + "ROIR1",0).toDouble();
            Detect_ROI.c1 = settings->value(str_img + "ROIC1",0).toDouble();
            Detect_ROI.r2 = settings->value(str_img + "ROIR2",0).toDouble();
            Detect_ROI.c2 = settings->value(str_img + "ROIC2",0).toDouble();
            Model_ROI.r1 = settings->value(str_img + "MROIR1",0).toDouble();
            Model_ROI.c1 = settings->value(str_img + "MROIC1",0).toDouble();
            Model_ROI.r2 = settings->value(str_img + "MROIR2",0).toDouble();
            Model_ROI.c2 = settings->value(str_img + "MROIC2",0).toDouble();
            xAxis_Distance_Ratio = settings->value(str_img + "XRATIO",0.001).toDouble();
            yAxis_Distance_Ratio = settings->value(str_img + "YRATIO",0.001).toDouble();
            xAxis_Ratio = settings->value(str_img + "XAXISPIXRATIO",0.822).toDouble();
            yAxis_Ratio = settings->value(str_img + "YAXISPIXRATIO",0.822).toDouble();
            xAxis_cam_dis = settings->value(str_img + "XAXISCAMDIS",0).toDouble();
            yAxis_cam_dis = settings->value(str_img + "YAXISCAMDIS",0).toDouble();
            xAxis_cam_distance_pls = settings->value(str_img + "X_CAM_DISTANC_PLS",0).toDouble();
            yAxis_cam_distance_pls = settings->value(str_img + "Y_CAM_DISTANC_PLS",0).toDouble();
            X_Axis_Limit = settings->value(str_img + "XAXISLMT", 0).toInt();
            CameraRes = settings->value(str_img + "CAMRES", 2).toInt();
            xAxis_Speed = settings->value(str_img + "XSPD",3000000).toInt();
            yAxis_Speed = settings->value(str_img + "YSPD",8000000).toInt();
            threshold_value = settings->value(str_img + "THRES_VAL",180).toDouble();
            distance_object = settings->value(str_img + "OBJ_DIS",0).toDouble();
            distance_to_border = settings->value(str_img + "BORDER_DIS", 3).toInt();
            Camera_Angle = settings->value(str_img + "CAM_ANGLE", 0.0).toDouble();
            Vertical_Skip_Ratio = settings->value(str_img + "VERSKIPRATIO", 1.02).toDouble();
            Return_Ratio = settings->value(str_img + "RETURNRATIO", 0.75).toDouble();
            Pos_Accurcy = settings->value(str_img + "POS_ACC", 37.5).toDouble();
            Pos_Accurcyy = settings->value(str_img + "POS_ACCY", 37.5).toDouble();
            Lines_Method = settings->value(str_img + "LINESMETHOD", 0).toInt();
            OriginIsLeft = settings->value(str_img + "ORGLEFT", 0).toInt();
            Limit_Counter = settings->value(str_img + "LIMITCOUNTER", 0).toInt();
            DoubleModel = settings->value(str_img + "DOUBLEMODEL", 0).toInt();
            Current_Counter = settings->value(str_img + "CURRENTCOUNTER", 0).toInt();
            Gamma = settings->value(str_img + "GAMMA",1.33).toDouble();
            Version = settings->value(str_img + "VER", 0).toString();
		}


		if (SEL & PARA_IO)
		{

		}



	}


	//无法读取配置文件的时候初始化参数
	void InitData() {

	}

    struct _Rect
    {
        double r1;
        double c1;
        double r2;
        double c2;
    };


//    int CamWidth[8] = {2048,1600,1280,1024,800,640,320,320};
 //   int CamHeight[8]={1516,1200,1024,768,600,480,240,240};

    int CamWidth[8] = {1600,1280,1024,800,640,320,320,320};
    int CamHeight[8]={1184,960,819,600,480,240,240,240};

    //伽马默认值为1.33
    double Gamma = 1.33;
    //数控还是视觉模式
    int ModeSelect = 0;
    //冲压限制
    int Limit_Counter = 0;
    //已经冲压多少个
    int Current_Counter = 0;
    //视觉模式下排布选择
    int Lines_Method = 0;
    //根据要求更改的，保存下直径参数
    double Model_Diameter = 35.0;
    //根据要求更改的，保存下直径参数
    double Model_Width = 20.0;
    double Model_Height = 20.0;

    double DxfWLower = 1;
    double DxfWUpper = 100;
    double DxfHLower = 1;
    double DxfHUpper = 100;

    //设置像素
    int CameraRes = 2;
    int OriginIsLeft = 0;
    //图像y轴方向代表的像素距离
    double yAxis_Ratio = 0.6;
    //图像x轴方向代表的像素距离
    double xAxis_Ratio = 0.6;
    //一个脉冲代表的距离 mm/pul
    double yAxis_Distance_Ratio = 0.001;
    //x轴方向一个脉冲代表的距离
    double xAxis_Distance_Ratio = 0.001;
    //mm,相机和冲压床的距离
    double xAxis_cam_dis = 30;
    double yAxis_cam_dis = 890;
    //以脉冲计的y轴到相机的距离，如果用浮点做，会导致误差有点大。
    int xAxis_cam_distance_pls = 1000;
    int yAxis_cam_distance_pls = 900;
    int yAxis_Speed = 3000000;
    int xAxis_Speed = 8000000;
    //相机由于安装不平的时候产生的角度
    double Camera_Angle = 0.0;
    double Vertical_Skip_Ratio = 1.02;
    double Return_Ratio = 0.75;
    //二值化
    double threshold_value = 180;
    //利用膨胀来检测防止冲压距离太近
    double distance_object = 1;
    //边界离模具的距离，防止冲破边界用的
    double distance_to_border = 3;
    //载入model的图片名
    QString Model = "";
    //应该没用了
    QString Version = _VERSION;
    //还差20mm停止的时候就开始冲压
    double Pos_Accurcy = 20;
    double Pos_Accurcyy = 20;//s
    int X_Axis_Limit = 0;
    //冲压限制次数,-1代表无限制
    int Press_Limit = -1;

    int DoubleModel = 0;
    //IMAGE 检测的范围
    QString Model_Name = "";
    //数控模式下dxf文件的路径
    QString DxfPath = "";
    _Rect Detect_ROI,Model_ROI;

};


class Preference
{
public:
	Preference() { 
		sys = new Ini("sys");
		prj = new Ini("prj");
    }
	~Preference() { delete sys; delete prj; }
	static Preference *GetIns() { static Preference ini; return &ini; }

	Ini *sys;
	Ini *prj;
};

#endif

#endif
