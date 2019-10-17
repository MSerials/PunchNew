// Preferences.h: interface for the CPreferences class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREFERENCES_H__4133CB8C_D2DA_4B24_9A3E_66C33ABA291E__INCLUDED_)
#define AFX_PREFERENCES_H__4133CB8C_D2DA_4B24_9A3E_66C33ABA291E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4996)
//#pragma warning(disable:2079)
//#pragma warning(disable:2228)

#include <afxcoll.h>  // for CStringList
#include <afxtempl.h>
#include <iostream>
#include <vector>
using namespace std;
#pragma warning(disable:4477)
#pragma warning(disable:4244)

#define C1            52845
#define C2            22719
#define KEY           1315
#define SHIFT         120
//ini macros
#define ITEM_ROW       8
#define ITEM_COL       10
//ini macros

#define PARA_ALL            0xffffffff
#define PARA_PRJ            (0x1<<0)
#define PARA_IMAGE			(0x1<<1)
#define PARA_IO				(0x1<<2)
#define PARA_SN				(0x1<<3)

/*
win_tool wt;
CString AppPath = wt.AppPath();
CString SysPath = AppPath + L"\\" + L"sys";
sys.SetIniDir(SysPath, false);
if (0 == sys.SetIniFile(L"PrgParameter.ini"))
{
sys.initData();
sys.SaveParaFile(PARA_PRJ|PARA_IO);
}
sys.LoadParaFile(PARA_PRJ | PARA_IO);
*/

#ifdef __AFX_H__

class CPreferences  
{
public:
	CPreferences() {};
	virtual ~CPreferences() {};
	
//***********************************************************************************************************
	//all private variables
private:
	//stores pathname of ini file to read/write
	CString path = L"";
	CString dir = L"";
	//all keys are of this time
	struct key
	{
		//list of values in key
		CArray<CString, CString> values; 
		//corresponding list of value names
		CArray<CString, CString> names;
	};
	//list of keys in ini
	CArray<key, key> keys; 
	//corresponding list of key names
	CArray<CString, CString> names; 		
	//all private functions
private:
	//overloaded to take CString
	istream & getline(istream & is, CString & str) {
		char buf[2048];
		is.getline(buf, 2048);
		str = buf;
		return is;
	};

	//returns index of specified value, in the specified key, or -1 if not found
	int FindValue(int keynum, CString valuename) {
		if (keynum == -1)
			return -1;
		int valuenum = 0;
		while (valuenum < keys[keynum].names.GetSize() && keys[keynum].names[valuenum] != valuename)
			valuenum++;
		if (valuenum == keys[keynum].names.GetSize())
			return -1;
		return valuenum;
	};

	//returns index of specified key, or -1 if not found
	int FindKey(CString keyname) {
		int keynum = 0;
		while (keynum < keys.GetSize() && names[keynum] != keyname)
			keynum++;
		if (keynum == keys.GetSize())
			return -1;
		return keynum;
	};

	//public variables
public:
	//will contain error info if one occurs
	//ended up not using much, just in ReadFile and GetValue
	CString error;
	//public functions
public:
	CString AppPath()
	{
		CString AppPath;
		::GetModuleFileName(GetModuleHandle(NULL), AppPath.GetBuffer(300), 300);
		AppPath.ReleaseBuffer();
		AppPath = AppPath.Left(AppPath.ReverseFind('\\'));
		return AppPath;
	}
	//***************************************
	bool SetIniDir(CString filedir, bool bmodule = true) {
		if (bmodule)
			dir = GetModulePath() + "\\" + filedir;
		else
			dir = filedir;
		return CreateMultipleDirectory(dir);
	};

	//
	void CreateIniFile(CString fileName) {
		CString strCurDir;
		strCurDir = dir + L"\\" + fileName;
		HANDLE hf = CreateFile(strCurDir, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, NULL, NULL);
		CloseHandle(hf);
		path = strCurDir;
	};

	void DeleteIniFile(CString fileName) {
		CString strCurDir;
		strCurDir = dir + "\\" + fileName;
		DeleteFile(strCurDir);
	};

	void CopyIniFile(CString srcName, CString dstName) {
		CString strOldPath, strNewPath;
		strOldPath = dir + "\\" + srcName;
		strNewPath = dir + "\\" + dstName;
		CopyFile(strOldPath, strNewPath, FALSE);
	};

	
	int SetIniFile(CString fileName) {
		if (L"" == dir)
		{
			AfxMessageBox(L"请先设置文件目录!");
			return -1;
		}
		else
		{
			path = dir + L"\\" + fileName;
		}
		if (!PathFileExists(path))
		{
			HANDLE hf = CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, NULL, NULL);
			CloseHandle(hf);
			return 0;
		}
		return 1;
	};


	CString GetAllFileName(CString suffix, int &nCnt, TCHAR separ = '|') {
		CString sFileName;
		CFileFind finder;
		CString rStr;
		CString strWildcard;
		rStr.Empty();
		nCnt = 0;
		strWildcard = dir + L"\\*" + suffix;
		BOOL bWorking = finder.FindFile(strWildcard);

		if (dir == L"")
		{
			AfxMessageBox(L"请先设置文件目录!");
			return L"";
		}
		while (bWorking)
		{
			sFileName.Empty();
			bWorking = finder.FindNextFile();
			if (finder.IsDots()) continue;
			sFileName = finder.GetFileName();
			sFileName = sFileName.Left(sFileName.ReverseFind('.')) + separ;
			rStr += sFileName;
			nCnt++;
		}
		rStr.TrimRight(separ);
		return rStr;
	};//such as: suffix=".ini" separ='|'

	CString SplitString(CString srcStr, int nIndex, TCHAR separ = '|') {
		int i, len = 0;
		CString strSplit;
		srcStr.TrimRight(separ);
		for (i = 0; i<srcStr.GetLength(); i++)
		{
			if (srcStr[i] == separ)  len++;
		}
		if ((nIndex>len) || (srcStr == L"")) return L"";
		AfxExtractSubString(strSplit, srcStr, nIndex, separ);
		return strSplit;
	};
	
	CString GetModulePath() {
		TCHAR szModulePath[MAX_PATH] = { 0 };
		CString strCurDir;
		::GetModuleFileName(NULL, szModulePath, _MAX_PATH);
		strCurDir = szModulePath;
		strCurDir = strCurDir.Left(strCurDir.ReverseFind(TEXT('\\')));
		return strCurDir;
	}

	CString GetFilePath() {
		return path;
	};

	CString GetFileDir() {
		return dir;
	};

	bool    CreateMultipleDirectory(CString szPath) {
		if (szPath == _T("")) return false;

		CString strDir(szPath);

		if (strDir.GetAt(strDir.GetLength() - 1) != _T('\\'))
		{
			strDir += _T('\\');
		}
		std::vector<CString> vPath;
		CString strTemp;
		bool bSuccess = false;

		for (int i = 0; i<strDir.GetLength(); ++i)
		{
			if (strDir.GetAt(i) != _T('\\'))
			{
				strTemp += strDir.GetAt(i);
			}
			else
			{
				vPath.push_back(strTemp);
				strTemp += _T('\\');

			}
		}

		std::vector<CString>::const_iterator vIter;
		for (vIter = vPath.begin(); vIter != vPath.end(); vIter++)
		{
			bSuccess = CreateDirectory(*vIter, NULL) ? true : false;
		}
		return bSuccess;
	};

	int     SelFolder(CWnd *pWnd, CString &strFolder) {
		HWND hParent = pWnd->m_hWnd;
		LPMALLOC lpMalloc;
		strFolder.Empty();

		if (::SHGetMalloc(&lpMalloc) != NOERROR) return 0;

		TCHAR szDisplayName[_MAX_PATH];
		TCHAR szBuffer[_MAX_PATH];
		BROWSEINFO browseInfo;
		browseInfo.hwndOwner = hParent;
		browseInfo.pidlRoot = NULL; // set root at Desktop
		browseInfo.pszDisplayName = szDisplayName;
		browseInfo.lpszTitle = L"Select a folder";
		browseInfo.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
		browseInfo.lpfn = NULL;
		browseInfo.lParam = 0;

		LPITEMIDLIST lpItemIDList;
		if ((lpItemIDList = ::SHBrowseForFolder(&browseInfo)) != NULL)
		{
			// Get the path of the selected folder from the    item ID list.
			if (::SHGetPathFromIDList(lpItemIDList, szBuffer))
			{
				// At this point, szBuffer contains the path the user chose.
				if (szBuffer[0] == '\0') return 0;

				// We have a path in szBuffer! Return it.
				strFolder = szBuffer;
				return 1;
			}
			else return 1; // strResult is empty

			lpMalloc->Free(lpItemIDList);
			lpMalloc->Release();
		}

		return 1;
	};


	void    OpenIniFile() {
		::ShellExecute(NULL, L"open", path, NULL, NULL, SW_SHOW);
	};
	
	CString Encrypt(CString Source, WORD Key = KEY) {
		CString Result, str;
		int i, j;
		Result = Source; // 初始化结果字符串
		for (i = 0; i<Source.GetLength(); i++) // 依次对字符串中各字符进行操作
		{
			Result.SetAt(i, Source.GetAt(i) ^ (Key >> 8)); // 将密钥移位后与字符异或
			Key = ((BYTE)Result.GetAt(i) + Key)*C1 + C2; // 产生下一个密钥
		}
		Source = Result; // 保存结果
		Result.Empty(); // 清除结果
		for (i = 0; i<Source.GetLength(); i++) // 对加密结果进行转换
		{
			j = (BYTE)Source.GetAt(i); // 提取字符
									   // 将字符转换为两个字母保存
			str = L"12"; // 设置str长度为2
			str.SetAt(0, SHIFT + j / 26);//这里将65改大点的数例如256，密文就会变乱码，效果更好，相应的，解密处要改为相同的数
			str.SetAt(1, SHIFT + j % 26);
			Result += str;
		}
		return Result;
	}; // 加密函数


	CString Decrypt(CString Source, WORD Key = KEY) {
		CString Result, str;
		int i, j;

		Result.Empty(); // 清除结果
		for (i = 0; i < Source.GetLength() / 2; i++) // 将字符串两个字母一组进行处理
		{
			j = ((BYTE)Source.GetAt(2 * i) - SHIFT) * 26;//相应的，解密处要改为相同的数		
			j += (BYTE)Source.GetAt(2 * i + 1) - SHIFT;
			str = "1"; // 设置str长度为1
			str.SetAt(0, j);
			Result += str; // 追加字符，还原字符串
		}
		Source = Result; // 保存中间结果
		for (i = 0; i<Source.GetLength(); i++) // 依次对字符串中各字符进行操作
		{
			Result.SetAt(i, (BYTE)Source.GetAt(i) ^ (Key >> 8)); // 将密钥移位后与字符异或
			Key = ((BYTE)Source.GetAt(i) + Key)*C1 + C2; // 产生下一个密钥
		}
		return Result;
	
	}; // 解密函数
	
	//gets value of [keyname] valuename = 
	//overloaded to return CString, int, and double,
	//returns "", or 0 if key/value not found.  Sets error member to show problem
	CString GetValue(CString keyname, CString valuename) {
		int keynum = FindKey(keyname), valuenum = FindValue(keynum, valuename);

		if (keynum == -1)
		{
			error = L"没有此项" + keyname;
			return L"";
		}

		if (valuenum == -1)
		{
			error = L"没有此项" + valuename;
			return L"";
		}
		return keys[keynum].values[valuenum];
	};

	int		GetValueI(CString keyname, CString valuename) {
		return _tstoi(GetValue(keyname, valuename));
	};

	double  GetValueF(CString keyname, CString valuename) {
		return _tstof(GetValue(keyname, valuename));
	};

	bool	GetValuebool(CString keyname, CString valuename) {
		return (GetValue(keyname, valuename) == L"0") ? false : true;
	};
	CString GetValueStr(CString keyname, CString valuename) {
		return GetValue(keyname, valuename);
	};
	//sets value of [keyname] valuename =.
	//specify the optional parameter as false (0) if you do not want it to create
	//the key if it doesn't exist. Returns true if data entered, false otherwise
	//overloaded to accept CString, int, and double
	bool SetValue(CString keyname, CString valuename, CString value, bool create = 1){
		int keynum = FindKey(keyname), valuenum = 0;
		//find key
		if (keynum == -1) //if key doesn't exist
		{
			if (!create) //and user does not want to create it,
				return 0; //stop entering this key
			names.SetSize(names.GetSize() + 1);
			keys.SetSize(keys.GetSize() + 1);
			keynum = names.GetSize() - 1;
			names[keynum] = keyname;
		}

		//find value
		valuenum = FindValue(keynum, valuename);
		if (valuenum == -1)
		{
			if (!create)
				return 0;
			keys[keynum].names.SetSize(keys[keynum].names.GetSize() + 1);
			keys[keynum].values.SetSize(keys[keynum].names.GetSize() + 1);
			valuenum = keys[keynum].names.GetSize() - 1;
			keys[keynum].names[valuenum] = valuename;
		}
		keys[keynum].values[valuenum] = value;
		return 1;
	};

	bool SetValueI(CString keyname, CString valuename, int value, bool create = 1) {
		CString temp;
		temp.Format(L"%d", value);
		return SetValue(keyname, valuename, temp, create);
	};

	bool SetValueF(CString keyname, CString valuename, double value, bool create = 1) {
		CString temp;
		temp.Format(L"%e", value);
		return SetValue(keyname, valuename, temp, create);
	};

	bool SetValuebool(CString keyname, CString valuename, bool value, bool create = 1) {
		CString temp;
		if (value == false)
			temp = L"0";
		else
			temp = L"1";
		return SetValue(keyname, valuename, temp, create);
	};

	bool SetValueStr(CString keyname, CString valuename, CString value, bool create = 1) {
		return SetValue(keyname, valuename, value, create);
	};

	//returns true if successful, false otherwise
	bool ReadFile() {
		CFile file;
		CFileStatus status;
		if (!file.GetStatus(path, status))
			return 0;
		std::ifstream inifile;
		CString readinfo;
		inifile.open(path);
		int curkey = -1, curval = -1;
		if (true == inifile.fail())
		{
			error = "打开文件出错.";
			return 0;
		}
		CString keyname, valuename, _value;
		CString temp;
		while (getline(inifile, readinfo))
		{
			if (readinfo != "")
			{
				if (readinfo[0] == '[' && readinfo[readinfo.GetLength() - 1] == ']') //if a section heading
				{
					//去掉[]
					keyname = readinfo;
					keyname.TrimLeft('[');
					keyname.TrimRight(']');
				}
				else //if a value
				{
					valuename = readinfo.Left(readinfo.Find(L"="));
					_value = readinfo.Right(readinfo.GetLength() - valuename.GetLength() - 1);
					SetValue(keyname, valuename, _value);
				}
			}
		}
		inifile.close();
		return 1;
	};
	//writes data stored in class to ini file
	void WriteFile() {
		FILE *fp;
		fp = _tfopen(path, L"wt");
		if (fp == (FILE*)NULL)
		{
			return;
		}
		for (int keynum = 0; keynum <= names.GetUpperBound(); keynum++)
		{
			if (keys[keynum].names.GetSize() != 0)
			{
				_ftprintf(fp, _T("[%s]\n"), names[keynum]);
				for (int valuenum = 0; valuenum <= keys[keynum].names.GetUpperBound(); valuenum++)
				{
					CString strtemp = keys[keynum].names[valuenum];
					CString strtemp1 = keys[keynum].values[valuenum];
					_ftprintf(fp, _T("%s=%s"), strtemp, strtemp1);

					if (valuenum != keys[keynum].names.GetUpperBound())
						_ftprintf(fp, _T("\n"));
					else
						if (keynum < names.GetSize())
							_ftprintf(fp, _T("\n"));
				}
				if (keynum < names.GetSize())
					_ftprintf(fp, _T("\n"));
			}
		}
		fclose(fp);
	};

	//deletes all stored ini data
	void Reset() {
		keys.SetSize(0);
		names.SetSize(0);
	};

	//returns number of keys currently in the ini
	int GetNumKeys() {
		return keys.GetSize();
	};
	//returns number of values stored for specified key
	int GetNumValues(CString keyname) {
		int keynum = FindKey(keyname);
		if (keynum == -1)
			return -1;
		else
			return keys[keynum].names.GetSize();
	};
	
	//deletes specified value
	//returns true if value existed and deleted, false otherwise
	bool DeleteValue(CString keyname, CString valuename) {
		return false;
	};
	//deletes specified key and all values contained within
	//returns true if key existed and deleted, false otherwise
	bool DeleteKey(CString keyname) {
		return false;
	};

	//For Project Parameter list
	//Image parameter
	//Parameter


	void LoadParaFile(UINT item) { //item=IMAGE|LOAD|PRJ.....
		UINT mask = item;

		if (ReadFile())
		{
			////////////////////////////////////
			if (PARA_PRJ == (mask&PARA_PRJ))
			{
				//m_strCurrentProjectName = GetValue(L"projectSetting",L"CurrentProject");

			}
			if (PARA_IMAGE == (mask&PARA_IMAGE))
			{
				Model_Name = GetValue(L"IMAGE_SETTING", L"Model_Name");
				Detect_ROI.r1 = GetValueF(L"IMAGE_SETTING", L"ROIR1");
				Detect_ROI.c1 = GetValueF(L"IMAGE_SETTING", L"ROIC1");
				Detect_ROI.r2 = GetValueF(L"IMAGE_SETTING", L"ROIR2");
				Detect_ROI.c2 = GetValueF(L"IMAGE_SETTING", L"ROIC2");

				Model_ROI.r1 = GetValueF(L"IMAGE_SETTING", L"MROIR1");
				Model_ROI.c1 = GetValueF(L"IMAGE_SETTING", L"MROIC1");
				Model_ROI.r2 = GetValueF(L"IMAGE_SETTING", L"MROIR2");
				Model_ROI.c2 = GetValueF(L"IMAGE_SETTING", L"MROIC2");
			
				xAxis_Ratio = GetValueF(L"IMAGE_SETTING", L"XAXISPIXRATIO");
				yAxis_Ratio = GetValueF(L"IMAGE_SETTING", L"YAXISPIXRATIO");

				xAxis_Distance_Ratio = GetValueF(L"IMAGE_SETTING", L"XRATIO");
				yAxis_Distance_Ratio = GetValueF(L"IMAGE_SETTING", L"YRATIO");

				xAxis_cam_dis = GetValueF(L"IMAGE_SETTING", L"XAXISCAMDIS");
				yAxis_cam_dis = GetValueF(L"IMAGE_SETTING", L"YAXISCAMDIS");
				xAxis_Limit = GetValueI(L"IMAGE_SETTING", L"XAISLIMIT");
			

				xAxis_Speed = GetValueI(L"IMAGE_SETTING", L"XSPD");
				yAxis_Speed = GetValueI(L"IMAGE_SETTING", L"YSPD");

				threshold_value = GetValueF(L"IMAGE_SETTING", L"THRES_VAL");
				distance_object = GetValueF(L"IMAGE_SETTING", L"OBJ_DIS");

				distance_to_border = GetValueF(L"IMAGE_SETTING", L"BORDER_DIS");

				Camera_Angle = GetValueF(L"IMAGE_SETTING", L"CAM_ANGLE");
				xAxis_cam_distance_pls = GetValueI(L"IMAGE_SETTING", L"X_CAM_DISTANC_PLS");
				yAxis_cam_distance_pls = GetValueI(L"IMAGE_SETTING", L"Y_CAM_DISTANC_PLS");

				//DoubleCheck = GetValueI(L"IMAGE_SETTING", L"DOUBLE_CHECK");
				Pos_Accurcy = GetValueF(L"IMAGE_SETTING", L"POS_ACC");
				X_Axis_Limit = GetValueI(L"IMAGE_SETTING", L"XAXISLMT");
				Lines_Method = GetValueI(L"IMAGE_SETTING", L"LINESMETHOD");
				x_axis_inv = GetValueI(L"IMAGE_SETTING", L"X_AXIS_INV");
				y_axis_inv = GetValueI(L"IMAGE_SETTING", L"Y_AXIS_INV");

			}

			if (PARA_IO == (mask&PARA_IO))
			{

				//m_mode1 = GetValueI(L"IOCARD_SETTING", L"mode1");
			}

		}
	}

	void SaveParaFile(UINT item) {
		UINT mask = item;
		////////////////////////////////////
		if (PARA_PRJ == (mask&PARA_PRJ))
		{
			//m_strCurrentProjectName = GetValue(L"projectSetting",L"CurrentProject");

		}

		if (PARA_IMAGE == (mask&PARA_IMAGE))
		{
			SetValue(L"IMAGE_SETTING", L"Model_Name", Model_Name);
			SetValueF(L"IMAGE_SETTING", L"ROIR1", Detect_ROI.r1);
			SetValueF(L"IMAGE_SETTING", L"ROIC1", Detect_ROI.c1);
			SetValueF(L"IMAGE_SETTING", L"ROIR2", Detect_ROI.r2);
			SetValueF(L"IMAGE_SETTING", L"ROIC2", Detect_ROI.c2);

			SetValueF(L"IMAGE_SETTING", L"MROIR1", Model_ROI.r1);
			SetValueF(L"IMAGE_SETTING", L"MROIC1", Model_ROI.c1);
			SetValueF(L"IMAGE_SETTING", L"MROIR2", Model_ROI.r2);
			SetValueF(L"IMAGE_SETTING", L"MROIC2", Model_ROI.c2);

			SetValueF(L"IMAGE_SETTING", L"XRATIO", xAxis_Distance_Ratio);
			SetValueF(L"IMAGE_SETTING", L"YRATIO", yAxis_Distance_Ratio);
			
			SetValueF(L"IMAGE_SETTING", L"XAXISPIXRATIO", xAxis_Ratio);
			SetValueF(L"IMAGE_SETTING", L"YAXISPIXRATIO", yAxis_Ratio);

			SetValueF(L"IMAGE_SETTING", L"XAXISCAMDIS", xAxis_cam_dis);
			SetValueF(L"IMAGE_SETTING", L"YAXISCAMDIS", yAxis_cam_dis);
			SetValueI(L"IMAGE_SETTING", L"XAISLIMIT", xAxis_Limit);

			SetValueI(L"IMAGE_SETTING", L"XSPD", xAxis_Speed);
			SetValueI(L"IMAGE_SETTING", L"YSPD", yAxis_Speed);

			SetValueF(L"IMAGE_SETTING", L"THRES_VAL", threshold_value);
			SetValueF(L"IMAGE_SETTING", L"OBJ_DIS", distance_object);

			SetValueF(L"IMAGE_SETTING", L"BORDER_DIS", distance_to_border);
			

			SetValueF(L"IMAGE_SETTING", L"CAM_ANGLE", Camera_Angle);
			
			SetValueI(L"IMAGE_SETTING", L"X_CAM_DISTANC_PLS", xAxis_cam_distance_pls);
			SetValueI(L"IMAGE_SETTING", L"Y_CAM_DISTANC_PLS", yAxis_cam_distance_pls);

			//SetValueI(L"IMAGE_SETTING", L"DOUBLE_CHECK", DoubleCheck);
			SetValueF(L"IMAGE_SETTING", L"POS_ACC", Pos_Accurcy);
			SetValueI(L"IMAGE_SETTING", L"XAXISLMT", X_Axis_Limit);
			SetValueI(L"IMAGE_SETTING", L"LINESMETHOD", Lines_Method);
			SetValueI(L"IMAGE_SETTING", L"X_AXIS_INV", x_axis_inv);
			SetValueI(L"IMAGE_SETTING", L"Y_AXIS_INV", y_axis_inv);
		
		}
		if (PARA_IO == (mask&PARA_IO))
		{

			//SetValueI(L"IOCARD_SETTING", L"mode1", m_mode1);


		}
		WriteFile();
	}


	void initData() {};


	struct _Rect
	{
		double r1;
		double c1;
		double r2;
		double c2;
	};


	int Lines_Method = 0;

	//IMAGE #define PARA_IMAGE			(0x1<<1)
	
	//图像y轴方向代表的像素距离
	double yAxis_Ratio = 0.93;
	//图像x轴方向代表的像素距离
	double xAxis_Ratio = 0.93;

	//由于安装导致，可能有些伺服电机方向放反
	int x_axis_inv = 1;
	int y_axis_inv = 1;

	//一个脉冲代表的距离 mm/pul
	double yAxis_Distance_Ratio = 0.001;
	//x轴方向一个脉冲代表的距离
	double xAxis_Distance_Ratio = 0.001;
	//mm,相机和冲压床的距离
	double xAxis_cam_dis = 30;
	double yAxis_cam_dis = 890;

	int xAxis_Limit = 0;
	//以脉冲计的y轴到相机的距离，如果用浮点做，会导致误差有点大。
	int xAxis_cam_distance_pls = 1000;
	int yAxis_cam_distance_pls = 900;
	int yAxis_Speed = 3000000;
	int xAxis_Speed = 8000000;
	//相机由于安装不平的时候产生的角度
	double Camera_Angle = 0.0;
	//二值化
	double threshold_value = 180;
	//利用膨胀来检测防止冲压距离太近
	double distance_object = 2;

	double distance_to_border = 1;
	//载入model的图片名
	CString Model = L"";

	bool isCabli = false;
	//检测双面的
	//int DoubleCheck = 0;
	//还差3mm停止的时候就开始冲压
	double Pos_Accurcy = 4.5;
	int X_Axis_Limit = -100000;

	//冲压限制次数,-1代表无限制
	int Press_Limit = -1;
	//#define PARA_PRJ            (0x1<<0)
	CString m_Prj_Vector;
	CString m_Prj_Name;
	int m_Prj_Index;

	CString m_password;			//密码

	

	//IMAGE 检测的范围
	CString Model_Name;
	_Rect Detect_ROI,Model_ROI;

	//IO卡 #define PARA_IO				(0x1<<2)
	double Tacc;          
	int waves;
	int stVel;
	int MaxVel;
	
	int m_mode1;








};
//extern CPreferences *ini;
#endif // !defined(AFX_PREFERENCES_H__4133CB8C_D2DA_4B24_9A3E_66C33ABA291E__INCLUDED_)

#endif
