#include "../../stdafx.h"

// Preferences.cpp: implementation of the CPreferences class.
//
//////////////////////////////////////////////////////////////////////


#include "Preferences.h"
#include <fstream>

#pragma warning(disable:4996)

using namespace std;
//CPreferences *ini = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
////////////////////////////////////////////////////////////////////// 

CPreferences::CPreferences() 
{
	path = L"";
	dir = L"";
	Reset();
	initData();
}

CPreferences::~CPreferences()
{
	
}

//constructor, can specify pathname here instead of using SetPath later
void CPreferences::CreateIniFile(CString fileName)
{       
	CString strCurDir;	 
	strCurDir  = dir + L"\\" + fileName; 
	HANDLE hf = CreateFile(strCurDir, GENERIC_WRITE, FILE_SHARE_READ, NULL,CREATE_NEW, NULL, NULL);
	CloseHandle(hf);
	path = strCurDir;
}

void CPreferences::DeleteIniFile(CString fileName)
{
	CString strCurDir;	 
	strCurDir  = dir + "\\" + fileName; 
	DeleteFile(strCurDir);
}

void CPreferences::CopyIniFile(CString srcName, CString dstName)
{
	CString strOldPath,strNewPath;	 
	strOldPath  = dir + "\\" + srcName; 
	strNewPath  = dir + "\\" + dstName; 
	CopyFile(strOldPath,strNewPath,FALSE);
}

int CPreferences::SetIniFile(CString fileName)
{
	
	if (dir == "")
	{
		AfxMessageBox(L"���������ļ�Ŀ¼!");
		return -1;
	}
	else
	{
		path = dir + L"\\" + fileName;
	}
	if (!PathFileExists(path))
	{
		HANDLE hf = CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,CREATE_NEW, NULL, NULL);
		CloseHandle(hf);
		
		return 0;
	}
	return 1;
}

CString CPreferences::GetAllFileName(CString suffix, int &nCnt, TCHAR separ)
{
	CString sFileName;
	CFileFind finder;	
	CString rStr;
	CString strWildcard;
	rStr.Empty();
	
	nCnt=0;
	strWildcard = dir + L"\\*" + suffix;		
	BOOL bWorking = finder.FindFile(strWildcard);
	
	if (dir == L"")
	{
		AfxMessageBox(L"���������ļ�Ŀ¼!");
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
}

CString CPreferences::SplitString(CString srcStr, int nIndex, TCHAR separ/* = */)
{
	int i, len =0;
	CString strSplit;
	srcStr.TrimRight(separ);
	for(i=0; i<srcStr.GetLength(); i++)
	{
		if(srcStr[i] == separ)  len++;
	}
	
	if ((nIndex>len) || (srcStr==L"")) return L"";
	AfxExtractSubString(strSplit, srcStr, nIndex, separ);
	return strSplit;
}

/////////////////////////////////////////////////////////////////////
// Public Functions
/////////////////////////////////////////////////////////////////////

//sets path of ini file to read and write from
bool CPreferences::SetIniDir(CString filedir, bool bmodule/* =true */)
{
	if (bmodule)
		dir = GetModulePath() + "\\" + filedir;
	else
		dir = filedir;
	
	return CreateMultipleDirectory(dir);
}

//reads ini file specified using CIniFile::SetPath()
//returns true if successful, false otherwise
bool CPreferences::ReadFile()
{
	CFile file;
	CFileStatus status;
	if (!file.GetStatus(path,status))
		return 0;
	ifstream inifile;
	CString readinfo;
	inifile.open(path);
	int curkey = -1, curval = -1;
	if (inifile.fail())
	{
		error = "���ļ�����.";
		return 0;
	}
	CString keyname, valuename, _value;
	CString temp;
	while (getline(inifile,readinfo))
	{
		if (readinfo != "")
		{
			if (readinfo[0] == '[' && readinfo[readinfo.GetLength()-1] == ']') //if a section heading
			{
				//ȥ��[]
				keyname = readinfo;
				keyname.TrimLeft('[');
				keyname.TrimRight(']');
			}
			else //if a value
			{
				valuename = readinfo.Left(readinfo.Find(L"="));
				_value = readinfo.Right(readinfo.GetLength()-valuename.GetLength()-1);
				SetValue(keyname,valuename,_value);
			}
		}
	}
	inifile.close();
	return 1;
}

//writes data stored in class to ini file
void CPreferences::WriteFile()
{
	FILE *fp;
	fp=_tfopen(path,L"wt");
	if(fp==(FILE*)NULL)
	{
		return ;
	}
	for (int keynum = 0; keynum <= names.GetUpperBound(); keynum++)
	{
		if (keys[keynum].names.GetSize() != 0)
		{
			_ftprintf(fp,_T("[%s]\n"),names[keynum]);
			for (int valuenum = 0; valuenum <= keys[keynum].names.GetUpperBound(); valuenum++)
			{
				CString strtemp=keys[keynum].names[valuenum];
				CString strtemp1=keys[keynum].values[valuenum];
				_ftprintf(fp,_T("%s=%s"),strtemp,strtemp1);
				
				if (valuenum != keys[keynum].names.GetUpperBound())
					_ftprintf(fp,_T("\n"));
				else
					if (keynum < names.GetSize())
						_ftprintf(fp,_T("\n"));
			}
			if (keynum < names.GetSize())
				_ftprintf(fp,_T("\n"));
		}
	}
	fclose(fp);
}

//deletes all stored ini data
void CPreferences::Reset()
{
	keys.SetSize(0);
	names.SetSize(0);
}

//returns number of keys currently in the ini
int CPreferences::GetNumKeys()
{
	return keys.GetSize();
}

//returns number of values stored for specified key, or -1 if key found
int CPreferences::GetNumValues(CString keyname)
{
	int keynum = FindKey(keyname);
	if (keynum == -1)
		return -1;
	else
		return keys[keynum].names.GetSize();
}

//gets value of [keyname] valuename = 
//overloaded to return CString, int, and double
CString CPreferences::GetValue(CString keyname, CString valuename)
{
	int keynum = FindKey(keyname), valuenum = FindValue(keynum,valuename);
	
	if (keynum == -1)
	{
		error = L"û�д���"+keyname;
		return L"";
	}
	
	if (valuenum == -1)
	{
		error = L"û�д���"+valuename;
		return L"";
	}
	return keys[keynum].values[valuenum];
}

//gets value of [keyname] valuename = 
//overloaded to return CString, int, and double
int CPreferences::GetValueI(CString keyname, CString valuename)
{
	return _tstoi(GetValue(keyname,valuename));
}

//gets value of [keyname] valuename = 
//overloaded to return CString, int, and double
double CPreferences::GetValueF(CString keyname, CString valuename)
{
	return _tstof(GetValue(keyname, valuename));
}

CString CPreferences::GetValueStr(CString keyname, CString valuename)
{
	return GetValue(keyname, valuename);
}
bool CPreferences::GetValuebool(CString keyname,CString valuename)
{
	return (GetValue(keyname,valuename)==L"0")?false:true;
}


//sets value of [keyname] valuename =.
//specify the optional paramter as false (0) if you do not want it to create
//the key if it doesn't exist. Returns true if data entered, false otherwise
//overloaded to accept CString, int, and double
bool CPreferences::SetValue(CString keyname, CString valuename, CString value, bool create)
{
	int keynum = FindKey(keyname), valuenum = 0;
	//find key
	if (keynum == -1) //if key doesn't exist
	{
		if (!create) //and user does not want to create it,
			return 0; //stop entering this key
		names.SetSize(names.GetSize()+1);
		keys.SetSize(keys.GetSize()+1);
		keynum = names.GetSize()-1;
		names[keynum] = keyname;
	}
	
	//find value
	valuenum = FindValue(keynum,valuename);
	if (valuenum == -1)
	{
		if (!create)
			return 0;
		keys[keynum].names.SetSize(keys[keynum].names.GetSize()+1);
		keys[keynum].values.SetSize(keys[keynum].names.GetSize()+1);
		valuenum = keys[keynum].names.GetSize()-1;
		keys[keynum].names[valuenum] = valuename;
	}
	keys[keynum].values[valuenum] = value;
	return 1;
}

//sets value of [keyname] valuename =.
//specify the optional paramter as false (0) if you do not want it to create
//the key if it doesn't exist. Returns true if data entered, false otherwise
//overloaded to accept CString, int, and double
bool CPreferences::SetValueI(CString keyname, CString valuename, int value, bool create)
{
	CString temp;
	temp.Format(L"%d",value);
	return SetValue(keyname, valuename, temp, create);
}

//sets value of [keyname] valuename =.
//specify the optional paramter as false (0) if you do not want it to create
//the key if it doesn't exist. Returns true if data entered, false otherwise
//overloaded to accept CString, int, and double
bool CPreferences::SetValueF(CString keyname, CString valuename, double value, bool create)
{
	CString temp;
	temp.Format(L"%e",value);
	return SetValue(keyname, valuename, temp, create);
}

bool CPreferences::SetValueStr(CString keyname, CString valuename,CString value,bool create)
{
	return SetValue(keyname, valuename, value, create);
}
bool CPreferences::SetValuebool(CString keyname,CString valuename,bool value,bool create)
{
	CString temp;
	if(value==false)
		temp=L"0";
	else
		temp=L"1";
	return SetValue(keyname, valuename, temp, create);
}

/////////////////////////////////////////////////////////////////////
// Private Functions
/////////////////////////////////////////////////////////////////////

//returns index of specified key, or -1 if not found
int CPreferences::FindKey(CString keyname)
{
	int keynum = 0;
	while ( keynum < keys.GetSize() && names[keynum] != keyname)
		keynum++;
	if (keynum == keys.GetSize())
		return -1;
	return keynum;
}

//returns index of specified value, in the specified key, or -1 if not found
int CPreferences::FindValue(int keynum, CString valuename)
{
	if (keynum == -1)
		return -1;
	int valuenum = 0;
	while (valuenum < keys[keynum].names.GetSize() && keys[keynum].names[valuenum] != valuename)
		valuenum++;
	if (valuenum == keys[keynum].names.GetSize())
		return -1;
	return valuenum;
}

//overloaded from original getline to take CString
istream & CPreferences:: getline(istream & is, CString & str)
{
    char buf[2048];
    is.getline(buf,2048);
    str = buf;
    return is;
}


void CPreferences::OpenIniFile()
{
	::ShellExecute(NULL,L"open",path,NULL,NULL,SW_SHOW);
}

CString CPreferences::GetFilePath()
{
	return path;
}

CString CPreferences::GetFileDir()
{
	return dir;
}

CString CPreferences::GetModulePath()
{
	TCHAR szModulePath[MAX_PATH] = {0};        
	CString strCurDir;
	
	::GetModuleFileName(NULL,szModulePath,_MAX_PATH);  	
	strCurDir  = szModulePath;  
	strCurDir  = strCurDir.Left(strCurDir.ReverseFind(TEXT('\\')));
	return strCurDir;
}

bool CPreferences::CreateMultipleDirectory(CString szPath)
{
	if (szPath == _T("")) return false;
	
    CString strDir(szPath);
	
    if (strDir.GetAt(strDir.GetLength()-1)!=_T('\\'))
    {
        strDir += _T('\\');
    }
    std::vector<CString> vPath;
    CString strTemp;
    bool bSuccess = false;
	
    for (int i=0;i<strDir.GetLength();++i)
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
}


int CPreferences::SelFolder(CWnd *pWnd, CString &strFolder)
{
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
    browseInfo.ulFlags = BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS;
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
}
CString CPreferences::Encrypt(CString Source, WORD Key) // ���ܺ���
{
	CString Result,str;
	int i,j;
	
	Result=Source; // ��ʼ������ַ���
	for(i=0; i<Source.GetLength(); i++) // ���ζ��ַ����и��ַ����в���
	{
		Result.SetAt(i, Source.GetAt(i)^(Key>>8)); // ����Կ��λ�����ַ����
		Key = ((BYTE)Result.GetAt(i)+Key)*C1+C2; // ������һ����Կ
	}
	Source=Result; // ������
	Result.Empty(); // ������
	for(i=0; i<Source.GetLength(); i++) // �Լ��ܽ������ת��
	{
		j=(BYTE)Source.GetAt(i); // ��ȡ�ַ�
		// ���ַ�ת��Ϊ������ĸ����
		str=L"12"; // ����str����Ϊ2
		str.SetAt(0, SHIFT+j/26);//���ｫ65�Ĵ���������256�����ľͻ�����룬Ч�����ã���Ӧ�ģ����ܴ�Ҫ��Ϊ��ͬ����
		str.SetAt(1, SHIFT+j%26);
		Result += str;
	}
	return Result;
}


CString CPreferences::Decrypt(CString Source, WORD Key) // ���ܺ���
{
	CString Result,str;
	int i,j;
	
	Result.Empty(); // ������
	for(i=0; i < Source.GetLength()/2; i++) // ���ַ���������ĸһ����д���
	{
		j = ((BYTE)Source.GetAt(2*i)-SHIFT)*26;//��Ӧ�ģ����ܴ�Ҫ��Ϊ��ͬ����		
		j += (BYTE)Source.GetAt(2*i+1)-SHIFT;
		str="1"; // ����str����Ϊ1
		str.SetAt(0, j);
		Result+=str; // ׷���ַ�����ԭ�ַ���
	}
	Source=Result; // �����м���
	for(i=0; i<Source.GetLength(); i++) // ���ζ��ַ����и��ַ����в���
	{
		Result.SetAt(i, (BYTE)Source.GetAt(i)^(Key>>8)); // ����Կ��λ�����ַ����
		Key = ((BYTE)Source.GetAt(i)+Key)*C1+C2; // ������һ����Կ
	}
	return Result;
}


void CPreferences::LoadParaFile(UINT item)
{
	UINT mask = item;

	if (ReadFile())
	{
		////////////////////////////////////
		if (PARA_PRJ == (mask&PARA_PRJ))
		{
			//m_strCurrentProjectName = GetValue(L"projectSetting",L"CurrentProject");
			m_Prj_Vector = GetValue(L"projectSetting", L"PRJ_VECTOR");
			m_Prj_Index =GetValueI(L"projectSetting", L"PRJ_IDX");
			m_isAlertStop = GetValueI(L"projectSetting", L"ALMSTOP");
			m_isNotMark = GetValueI(L"projectSetting", L"NOMARK");

			m_idCamera				= GetValueI(L"projectSetting", L"CAMERAID");
			m_nTriggerOutForBack	= GetValueI(L"projectSetting", L"TRGOUTBACK");
			m_nTriggerOutForFront	= GetValueI(L"projectSetting", L"TRGOUTFRONT");
			m_nTriggerOutForFinish	= GetValueI(L"projectSetting", L"TRGOUTFINISH");
			m_nSetTimerIntervals	= GetValueI(L"projectSetting", L"TIMERINTERVALS");
			m_nSnapTimeDelay		= GetValueI(L"projectSetting", L"SANPDELAY");
			m_shutter				= GetValueF(L"projectSetting", L"SHUTTER");
			m_markdelay				= GetValueI(L"projectSetting", L"MARKDELAY");
			m_direction				= GetValueI(L"projectSetting", L"DIR");

			m_password				= GetValue(L"projectSetting", L"PWD");
			m_nTotal				= GetValueI(L"projectSetting", L"TOTAL_QTY");
			m_nUp					= GetValueI(L"projectSetting", L"UP_QTY");
			m_nDown					= GetValueI(L"projectSetting", L"DOWN_QTY");
			m_nLeft					= GetValueI(L"projectSetting", L"LEFT_QTY");
			m_nRight				= GetValueI(L"projectSetting", L"RIGHT_QTY");
			m_nVoid					= GetValueI(L"projectSetting", L"VOID_QTY");
		}
		if (PARA_IMAGE == (mask&PARA_IMAGE))
		{
			m_nESCPixel = GetValueI(L"IMAGE_SETTING", L"EscPixel");
			m_idThreshNoise = GetValueI(L"IMAGE_SETTING", L"idThreshNoise");
			m_idOfThreshold = GetValueI(L"IMAGE_SETTING", L"idOfThreshold");
			m_idFixThreshold = GetValueI(L"IMAGE_SETTING", L"idFixThreshold");
			m_iThreshNoiseLow = GetValueI(L"IMAGE_SETTING", L"iThreshNoiseLow");
			m_iThreshNoiseHi = GetValueI(L"IMAGE_SETTING", L"iThreshNoiseHi");
			m_iCanBeRecognisedLenMin = GetValueI(L"IMAGE_SETTING", L"RECOGNISED_LEN_MIN");
			m_iCanBeRecognisedLenMax = GetValueI(L"IMAGE_SETTING", L"RECOGNISED_LEN_MAX");
			m_iExistMax = GetValueI(L"IMAGE_SETTING", L"EXIST_PX_MAX");
			m_iExistMin = GetValueI(L"IMAGE_SETTING", L"EXIST_PX_MIN");
			m_iExistHeightMax = GetValueI(L"IMAGE_SETTING", L"HEIGHT_MAX");
			m_iExistHeightMin = GetValueI(L"IMAGE_SETTING", L"HEIGHT_MIN");
			m_iExistWidthMax = GetValueI(L"IMAGE_SETTING", L"WIDTH_MAX");
			m_iExistWidthMin = GetValueI(L"IMAGE_SETTING", L"WIDTH_MIN");
			m_bDisplayImageEffect = GetValueI(L"IMAGE_SETTING", L"EFFECT_KIND");
			m_nWHRatio = (float)GetValueF(L"IMAGE_SETTING", L"WHRATIO");
			m_imageDealMode = GetValueI(L"IMAGE_SETTING", L"m_imagedealmode");
			m_blocksize = GetValueI(L"IMAGE_SETTING", L"BLOCKSIZE");
			m_threshold2 = GetValueI(L"IMAGE_SETTING", L"THRESHOLD2");


			//˫���˲�����������

			d = GetValueI(L"IMAGE_SETTING", L"biFilterd");
			sigmaColor = GetValueF(L"IMAGE_SETTING", L"biFilterc");
			sigmaSpace = GetValueF(L"IMAGE_SETTING", L"biFilters");

			top_inv.r1 = GetValueF(L"IMAGE_SETTING", L"TINVR1");
			top_inv.c1 = GetValueF(L"IMAGE_SETTING", L"TINVC1");
			top_inv.r2 = GetValueF(L"IMAGE_SETTING", L"TINVR2");
			top_inv.c2 = GetValueF(L"IMAGE_SETTING", L"TINVC2");
			bottom_inv.r1 = GetValueF(L"IMAGE_SETTING", L"BINVR1");
			bottom_inv.c1 = GetValueF(L"IMAGE_SETTING", L"BINVC1");
			bottom_inv.r2 = GetValueF(L"IMAGE_SETTING", L"BINVR2");
			bottom_inv.c2 = GetValueF(L"IMAGE_SETTING", L"BINVC2");
			top_char.r1 = GetValueF(L"IMAGE_SETTING", L"TCHARR1");
			top_char.c1 = GetValueF(L"IMAGE_SETTING", L"TCHARC1");
			top_char.r2 = GetValueF(L"IMAGE_SETTING", L"TCHARR2");
			top_char.c2 = GetValueF(L"IMAGE_SETTING", L"TCHARC2");
			bottom_char.r1 = GetValueF(L"IMAGE_SETTING", L"BCHARR1");
			bottom_char.c1 = GetValueF(L"IMAGE_SETTING", L"BCHARC1");
			bottom_char.r2 = GetValueF(L"IMAGE_SETTING", L"BCHARR2");
			bottom_char.c2 = GetValueF(L"IMAGE_SETTING", L"BCHARC2");
			top_knife.r1 = GetValueF(L"IMAGE_SETTING", L"TKNIFER1");
			top_knife.c1 = GetValueF(L"IMAGE_SETTING", L"TKNIFEC1");
			top_knife.r2 = GetValueF(L"IMAGE_SETTING", L"TKNIFER2");
			top_knife.c2 = GetValueF(L"IMAGE_SETTING", L"TKNIFEC2");
			bottom_knife.r1 = GetValueF(L"IMAGE_SETTING", L"BKNIFER1");
			bottom_knife.c1 = GetValueF(L"IMAGE_SETTING", L"BKNIFEC1");
			bottom_knife.r2 = GetValueF(L"IMAGE_SETTING", L"BKNIFER2");
			bottom_knife.c2 = GetValueF(L"IMAGE_SETTING", L"BKNIFEC2");
			top_unbolt.r1 = GetValueF(L"IMAGE_SETTING", L"TUBOLTR1");
			top_unbolt.c1 = GetValueF(L"IMAGE_SETTING", L"TUBOLTC1");
			top_unbolt.r2 = GetValueF(L"IMAGE_SETTING", L"TUBOLTR2");
			top_unbolt.c2 = GetValueF(L"IMAGE_SETTING", L"TUBOLTC2");
			bottom_unbolt.r1 = GetValueF(L"IMAGE_SETTING", L"BUBOLTR1");
			bottom_unbolt.c1 = GetValueF(L"IMAGE_SETTING", L"BUBOLTC1");
			bottom_unbolt.r2 = GetValueF(L"IMAGE_SETTING", L"BUBOLTR2");
			bottom_unbolt.c2 = GetValueF(L"IMAGE_SETTING", L"BUBOLTC2");


			TopPara.thresholdvalue = GetValueF(L"IMAGE_SETTING", L"TPTHRES");
			TopPara.minarea = GetValueF(L"IMAGE_SETTING", L"TPMINAREA");
			TopPara.maxarea = GetValueF(L"IMAGE_SETTING", L"TPMAXAREA");
			BottomPara.thresholdvalue = GetValueF(L"IMAGE_SETTING", L"BPTHRES");
			BottomPara.minarea = GetValueF(L"IMAGE_SETTING", L"BPMINAREA");
			BottomPara.maxarea = GetValueF(L"IMAGE_SETTING", L"BPMAXAREA");

			Tacc = GetValueF(L"IMAGE_SETTING", L"TACC");
			MaxVel = GetValueI(L"IMAGE_SETTING", L"MAXVEL");
			waves = GetValueI(L"IMAGE_SETTING", L"WAVES");
			m_ChainLength = GetValueI(L"IMAGE_SETTING", L"CHAINLEN");
			

			TopPara.knife_threshold = GetValueF(L"IMAGE_SETTING", L"TKNIFETHRES");
			TopPara.minlen = GetValueF(L"IMAGE_SETTING", L"TKNIFEMNLEN");
			TopPara.maxlen = GetValueF(L"IMAGE_SETTING", L"TKNIFEMXLEN");
			BottomPara.knife_threshold = GetValueF(L"IMAGE_SETTING", L"BKNIFETHRES");
			BottomPara.minlen = GetValueF(L"IMAGE_SETTING", L"BKNIFEMNLEN");
			BottomPara.maxlen = GetValueF(L"IMAGE_SETTING", L"BKNIFEMXLEN");

			TopPara.match_acc = GetValueF(L"IMAGE_SETTING", L"TMATCHACC");
			BottomPara.match_acc = GetValueF(L"IMAGE_SETTING", L"BMATCHACC");
		}



		if (PARA_IO == (mask&PARA_IO))
		{

			m_mode1 = GetValueI(L"IOCARD_SETTING", L"mode1");
			g_RailLenght = GetValueI(L"IOCARD_SETTING", L"g_RailLenght");
			IN_Start_Camera_SENSOR = GetValueI(L"IOCARD_SETTING", L"IN_Start_Camera_SENSOR");
			IN_CHECK_BREAK = GetValueI(L"IOCARD_SETTING", L"IN_CHECK_BREAK");
			IN_Check_UP_TIPPEDUP = GetValueI(L"IOCARD_SETTING", L"IN_Check_UP_TIPPEDUP");
			IN_CYL_Magnet_SENSOR = GetValueI(L"IOCARD_SETTING", L"IN_CYL_Magnet_SENSOR");
			IN_Check_BT_TIPPEDUP = GetValueI(L"IOCARD_SETTING", L"IN_Check_BT_TIPPEDUP");
			IN_START_BTN = GetValueI(L"IOCARD_SETTING", L"IN_START_BTN");
			IN_BTN_STOP = GetValueI(L"IOCARD_SETTING", L"IN_BTN_STOP");
			IN_Check_Double_Sword2 = GetValueI(L"IOCARD_SETTING", L"IN_Check_Double_Sword2");
			IN_CYL_Mark3_UP_SENSOR = GetValueI(L"IOCARD_SETTING", L"IN_CYL_Mark3_UP_SENSOR");
			IN_CHECK_SENSOR = GetValueI(L"IOCARD_SETTING", L"IN_CHECK_SENSOR");
			IN_DIED_KNOED_SENSOR = GetValueI(L"IOCARD_SETTING", L"IN_DIED_KNOED_SENSOR");
			IN_Check_Double_Sword1 = GetValueI(L"IOCARD_SETTING", L"IN_Check_Double_Sword1");
			IN_CYL_Mark1_UP_SENSOR = GetValueI(L"IOCARD_SETTING", L"IN_CYL_Mark1_UP_SENSOR");
			IN_Foot_Switch = GetValueI(L"IOCARD_SETTING", L"IN_Foot_Switch");
			IN_CYL_Mark2_UP_SENSOR = GetValueI(L"IOCARD_SETTING", L"IN_CYL_Mark2_UP_SENSOR");
			IN_CHECK_SENSOR1 = GetValueI(L"IOCARD_SETTING", L"IN_CHECK_SENSOR1");
			IN_CYL_Press_UP_SENSOR = GetValueI(L"IOCARD_SETTING", L"IN_CYL_Press_UP_SENSOR");
			IN_ESTOP_BTN = GetValueI(L"IOCARD_SETTING", L"IN_ESTOP_BTN");
			IN_Check_UP_DaJi = GetValueI(L"IOCARD_SETTING", L"IN_Check_UP_DaJi");
			IN_Check_BT_DaJi = GetValueI(L"IOCARD_SETTING", L"IN_Check_BT_DaJi");
			IN_CYL_Push_UP_SENSOR = GetValueI(L"IOCARD_SETTING", L"IN_CYL_Push_UP_SENSOR");

			g_UpTippedPos = GetValueI(L"IOCARD_SETTING", L"g_UpTippedPos");
			g_BtTippedPos = GetValueI(L"IOCARD_SETTING", L"g_BtTippedPos");
			g_UpCameraPos = GetValueI(L"IOCARD_SETTING", L"g_UpCameraPos");
			g_BtDaJiPos = GetValueI(L"IOCARD_SETTING", L"g_BtDaJiPos");
			g_UpDaJiPos = GetValueI(L"IOCARD_SETTING", L"g_UpDaJiPos");
			g_DoubleKnifePos = GetValueI(L"IOCARD_SETTING", L"g_DoubleKnifePos");
			g_DoubleKnifePos1 = GetValueI(L"IOCARD_SETTING", L"g_DoubleKnifePos1");
			g_BtCameraPos = GetValueI(L"IOCARD_SETTING", L"g_BtCameraPos");
			g_Mark1CylinderPos = GetValueI(L"IOCARD_SETTING", L"g_Mark1CylinderPos");
			g_Mark2CylinderPos = GetValueI(L"IOCARD_SETTING", L"g_Mark2CylinderPos");
			g_MarkPos = GetValueI(L"IOCARD_SETTING", L"g_MarkPos");
			g_ThirdDiedKnodePos = GetValueI(L"IOCARD_SETTING", L"g_ThirdDiedKnodePos");

		}
	
	}

}

void CPreferences::SaveParaFile(UINT item)
{
	UINT mask = item;
	////////////////////////////////////
	if (PARA_PRJ==(mask&PARA_PRJ))
	{
		//m_strCurrentProjectName = GetValue(L"projectSetting",L"CurrentProject");
		SetValue(L"projectSetting", L"PRJ_VECTOR", m_Prj_Vector);
		SetValueI(L"projectSetting", L"PRJ_IDX", m_Prj_Index);
		SetValueI(L"projectSetting", L"ALMSTOP", m_isAlertStop);
		SetValueI(L"projectSetting", L"NOMARK", m_isNotMark);
		
		

		SetValueI(L"projectSetting", L"CAMERAID", m_idCamera);
		SetValueI(L"projectSetting", L"TRGOUTBACK", m_nTriggerOutForBack);
		SetValueI(L"projectSetting", L"TRGOUTFRONT", m_nTriggerOutForFront);
		SetValueI(L"projectSetting", L"TRGOUTFINISH", m_nTriggerOutForFinish);
		SetValueI(L"projectSetting", L"TIMERINTERVALS", m_nSetTimerIntervals);
		SetValueI(L"projectSetting", L"SANPDELAY", m_nSnapTimeDelay);
		SetValueF(L"projectSetting", L"SHUTTER", m_shutter);
		SetValueI(L"projectSetting", L"MARKDELAY", m_markdelay);
		SetValueI(L"projectSetting", L"DIR", m_direction);
		
		SetValue(L"projectSetting", L"PWD", m_password);
		SetValueI(L"projectSetting", L"TOTAL_QTY", m_nTotal);
		SetValueI(L"projectSetting", L"UP_QTY", m_nUp);
		SetValueI(L"projectSetting", L"DOWN_QTY", m_nDown);
		SetValueI(L"projectSetting", L"LEFT_QTY", m_nLeft);
		SetValueI(L"projectSetting", L"RIGHT_QTY", m_nRight);
		SetValueI(L"projectSetting", L"VOID_QTY", m_nVoid);
	}

	if(PARA_IMAGE==(mask&PARA_IMAGE))
	{
		SetValueI(L"IMAGE_SETTING",L"EscPixel",m_nESCPixel);
		SetValueI(L"IMAGE_SETTING",L"idThreshNoise",m_idThreshNoise);	//��������ʽ
		SetValueI(L"IMAGE_SETTING",L"idOfThreshold",m_idOfThreshold);	//��ֵ���㷨ѡ��
		SetValueI(L"IMAGE_SETTING",L"idFixThreshold",m_idFixThreshold );	//��ֵ��
		SetValueI(L"IMAGE_SETTING", L"iThreshNoiseLow", m_iThreshNoiseLow);	//������������
		SetValueI(L"IMAGE_SETTING", L"iThreshNoiseHi", m_iThreshNoiseHi);	//������������
		SetValueI(L"IMAGE_SETTING", L"RECOGNISED_LEN_MIN", m_iCanBeRecognisedLenMin); //������������������
		SetValueI(L"IMAGE_SETTING", L"RECOGNISED_LEN_MAX", m_iCanBeRecognisedLenMax);		//�������������
		SetValueI(L"IMAGE_SETTING", L"EXIST_PX_MAX", m_iExistMax); //���������
		SetValueI(L"IMAGE_SETTING", L"EXIST_PX_MIN", m_iExistMin);  //�����ʾͼ��ԭͼ�����ͼ��
		SetValueI(L"IMAGE_SETTING", L"HEIGHT_MAX", m_iExistHeightMax);
		SetValueI(L"IMAGE_SETTING", L"HEIGHT_MIN", m_iExistHeightMin);
		SetValueI(L"IMAGE_SETTING", L"WIDTH_MAX", m_iExistWidthMax);
		SetValueI(L"IMAGE_SETTING", L"WIDTH_MIN", m_iExistWidthMin);
		SetValueI(L"IMAGE_SETTING", L"EFFECT_KIND", m_bDisplayImageEffect);
		SetValueF(L"IMAGE_SETTING", L"WHRATIO", m_nWHRatio);
		SetValueI(L"IMAGE_SETTING", L"m_imagedealmode", m_imageDealMode);
		SetValueI(L"IMAGE_SETTING", L"BLOCKSIZE", m_blocksize);
		SetValueI(L"IMAGE_SETTING", L"THRESHOLD2", m_threshold2);



							//˫���˲�����������
	
		SetValueI(L"IMAGE_SETTING", L"biFilterd", d);
		SetValueF(L"IMAGE_SETTING", L"biFilterc", sigmaColor);
		SetValueF(L"IMAGE_SETTING", L"biFilters", sigmaSpace);

		SetValueF(L"IMAGE_SETTING", L"TINVR1", top_inv.r1);
		SetValueF(L"IMAGE_SETTING", L"TINVC1", top_inv.c1);
		SetValueF(L"IMAGE_SETTING", L"TINVR2", top_inv.r2);
		SetValueF(L"IMAGE_SETTING", L"TINVC2", top_inv.c2);
		SetValueF(L"IMAGE_SETTING", L"BINVR1", bottom_inv.r1);
		SetValueF(L"IMAGE_SETTING", L"BINVC1", bottom_inv.c1);
		SetValueF(L"IMAGE_SETTING", L"BINVR2", bottom_inv.r2);
		SetValueF(L"IMAGE_SETTING", L"BINVC2", bottom_inv.c2);
		SetValueF(L"IMAGE_SETTING", L"TCHARR1", top_char.r1);
		SetValueF(L"IMAGE_SETTING", L"TCHARC1", top_char.c1);
		SetValueF(L"IMAGE_SETTING", L"TCHARR2", top_char.r2);
		SetValueF(L"IMAGE_SETTING", L"TCHARC2", top_char.c2);
		SetValueF(L"IMAGE_SETTING", L"BCHARR1", bottom_char.r1);
		SetValueF(L"IMAGE_SETTING", L"BCHARC1", bottom_char.c1);
		SetValueF(L"IMAGE_SETTING", L"BCHARR2", bottom_char.r2);
		SetValueF(L"IMAGE_SETTING", L"BCHARC2", bottom_char.c2);
		SetValueF(L"IMAGE_SETTING", L"TKNIFER1", top_knife.r1);
		SetValueF(L"IMAGE_SETTING", L"TKNIFEC1", top_knife.c1);
		SetValueF(L"IMAGE_SETTING", L"TKNIFER2", top_knife.r2);
		SetValueF(L"IMAGE_SETTING", L"TKNIFEC2", top_knife.c2);
		SetValueF(L"IMAGE_SETTING", L"BKNIFER1", bottom_knife.r1);
		SetValueF(L"IMAGE_SETTING", L"BKNIFEC1", bottom_knife.c1);
		SetValueF(L"IMAGE_SETTING", L"BKNIFER2", bottom_knife.r2);
		SetValueF(L"IMAGE_SETTING", L"BKNIFEC2", bottom_knife.c2);
		SetValueF(L"IMAGE_SETTING", L"TUBOLTR1", top_unbolt.r1);
		SetValueF(L"IMAGE_SETTING", L"TUBOLTC1", top_unbolt.c1);
		SetValueF(L"IMAGE_SETTING", L"TUBOLTR2", top_unbolt.r2);
		SetValueF(L"IMAGE_SETTING", L"TUBOLTC2", top_unbolt.c2);
		SetValueF(L"IMAGE_SETTING", L"BUBOLTR1", bottom_unbolt.r1);
		SetValueF(L"IMAGE_SETTING", L"BUBOLTC1", bottom_unbolt.c1);
		SetValueF(L"IMAGE_SETTING", L"BUBOLTR2", bottom_unbolt.r2);
		SetValueF(L"IMAGE_SETTING", L"BUBOLTC2", bottom_unbolt.c2);


		SetValueF(L"IMAGE_SETTING", L"TPTHRES", TopPara.thresholdvalue);
		SetValueF(L"IMAGE_SETTING", L"TPMINAREA", TopPara.minarea);
		SetValueF(L"IMAGE_SETTING", L"TPMAXAREA", TopPara.maxarea);
		SetValueF(L"IMAGE_SETTING", L"BPTHRES", BottomPara.thresholdvalue);
		SetValueF(L"IMAGE_SETTING", L"BPMINAREA", BottomPara.minarea);
		SetValueF(L"IMAGE_SETTING", L"BPMAXAREA", BottomPara.maxarea);

		SetValueF(L"IMAGE_SETTING", L"TACC", Tacc);
		SetValueI(L"IMAGE_SETTING", L"MAXVEL", MaxVel);
		SetValueI(L"IMAGE_SETTING", L"WAVES",waves);
		SetValueI(L"IMAGE_SETTING", L"CHAINLEN", m_ChainLength);

		SetValueF(L"IMAGE_SETTING", L"TKNIFETHRES", TopPara.knife_threshold);
		SetValueF(L"IMAGE_SETTING", L"TKNIFEMNLEN", TopPara.minlen);
		SetValueF(L"IMAGE_SETTING", L"TKNIFEMXLEN", TopPara.maxlen);
		SetValueF(L"IMAGE_SETTING", L"BKNIFETHRES", BottomPara.knife_threshold);
		SetValueF(L"IMAGE_SETTING", L"BKNIFEMNLEN", BottomPara.minlen);
		SetValueF(L"IMAGE_SETTING", L"BKNIFEMXLEN", BottomPara.maxlen);


		SetValueF(L"IMAGE_SETTING", L"TMATCHACC", TopPara.match_acc);
		SetValueF(L"IMAGE_SETTING", L"BMATCHACC", BottomPara.match_acc);


	}
	if(PARA_IO==(mask&PARA_IO))
	{

		SetValueI(L"IOCARD_SETTING", L"mode1", m_mode1);
		SetValueI(L"IOCARD_SETTING", L"g_RailLenght", g_RailLenght);
		SetValueI(L"IOCARD_SETTING", L"IN_Start_Camera_SENSOR", IN_Start_Camera_SENSOR);
		SetValueI(L"IOCARD_SETTING", L"IN_CHECK_BREAK", IN_CHECK_BREAK);
		SetValueI(L"IOCARD_SETTING", L"IN_Check_UP_TIPPEDUP", IN_Check_UP_TIPPEDUP);
		SetValueI(L"IOCARD_SETTING", L"IN_CYL_Magnet_SENSOR", IN_CYL_Magnet_SENSOR);
		SetValueI(L"IOCARD_SETTING", L"IN_Check_BT_TIPPEDUP", IN_Check_BT_TIPPEDUP);
		SetValueI(L"IOCARD_SETTING", L"IN_START_BTN", IN_START_BTN);
		SetValueI(L"IOCARD_SETTING", L"IN_BTN_STOP", IN_BTN_STOP);
		SetValueI(L"IOCARD_SETTING", L"IN_Check_Double_Sword2", IN_Check_Double_Sword2);
		SetValueI(L"IOCARD_SETTING", L"IN_CYL_Mark3_UP_SENSOR", IN_CYL_Mark3_UP_SENSOR);
		SetValueI(L"IOCARD_SETTING", L"IN_CHECK_SENSOR", IN_CHECK_SENSOR);
		SetValueI(L"IOCARD_SETTING", L"IN_DIED_KNOED_SENSOR", IN_DIED_KNOED_SENSOR);
		SetValueI(L"IOCARD_SETTING", L"IN_Check_Double_Sword1", IN_Check_Double_Sword1);
		SetValueI(L"IOCARD_SETTING", L"IN_CYL_Mark1_UP_SENSOR", IN_CYL_Mark1_UP_SENSOR);
		SetValueI(L"IOCARD_SETTING", L"IN_Foot_Switch", IN_Foot_Switch);
		SetValueI(L"IOCARD_SETTING", L"IN_CYL_Mark2_UP_SENSOR", IN_CYL_Mark2_UP_SENSOR);
		SetValueI(L"IOCARD_SETTING", L"IN_CHECK_SENSOR1", IN_CHECK_SENSOR1);
		SetValueI(L"IOCARD_SETTING", L"IN_CYL_Press_UP_SENSOR", IN_CYL_Press_UP_SENSOR);
		SetValueI(L"IOCARD_SETTING", L"IN_ESTOP_BTN", IN_ESTOP_BTN);
		SetValueI(L"IOCARD_SETTING", L"IN_Check_UP_DaJi", IN_Check_UP_DaJi);
		SetValueI(L"IOCARD_SETTING", L"IN_Check_BT_DaJi", IN_Check_BT_DaJi);
		SetValueI(L"IOCARD_SETTING", L"IN_CYL_Push_UP_SENSOR", IN_CYL_Push_UP_SENSOR); 

		SetValueI(L"IOCARD_SETTING", L"g_UpTippedPos", g_UpTippedPos);
		SetValueI(L"IOCARD_SETTING", L"g_BtTippedPos", g_BtTippedPos);
		SetValueI(L"IOCARD_SETTING", L"g_UpCameraPos", g_UpCameraPos);
		SetValueI(L"IOCARD_SETTING", L"g_BtDaJiPos", g_BtDaJiPos);
		SetValueI(L"IOCARD_SETTING", L"g_UpDaJiPos", g_UpDaJiPos);
		SetValueI(L"IOCARD_SETTING", L"g_DoubleKnifePos", g_DoubleKnifePos);
		SetValueI(L"IOCARD_SETTING", L"g_DoubleKnifePos1", g_DoubleKnifePos1);
		SetValueI(L"IOCARD_SETTING", L"g_BtCameraPos", g_BtCameraPos);
		SetValueI(L"IOCARD_SETTING", L"g_Mark1CylinderPos", g_Mark1CylinderPos);
		SetValueI(L"IOCARD_SETTING", L"g_Mark2CylinderPos", g_Mark2CylinderPos);
		SetValueI(L"IOCARD_SETTING", L"g_MarkPos", g_MarkPos);
		SetValueI(L"IOCARD_SETTING", L"g_ThirdDiedKnodePos", g_ThirdDiedKnodePos);

	}
	
	//if(PARA_SN==(mask&PARA_SN))
	//{
	//	SetValue(L"PRJ_SN",L"SN",m_password); 
	//}
	WriteFile();
}


void CPreferences::initData()
{
	//#define PARA_PRJ            (0x1<<0)
	m_Prj_Vector = L"example";
	m_Prj_Index = 0;
	m_ChainLength = 49;
	m_isAlertStop = 0;
	m_isNotMark = 0;

	m_shutter = 2500.0;
	m_idCamera = 0;
	m_nTriggerOutForBack = 0;
	m_nTriggerOutForFront = 0;
	m_nTriggerOutForFinish = 0;
	m_nSetTimerIntervals = 10;
	m_nSnapTimeDelay = 1; //����ֵ�������������ӳ���35ms
	m_markdelay = 700;
	m_password = "";			//����

	m_nTotal = 0;
	m_nUp = 0;
	m_nDown = 0;
	m_nLeft = 0;
	m_nRight = 0;
	m_nVoid = 0;	//ͳ���������Ҽ�⵽����Ŀ��û����Ʒ����Ŀ	

					//IMAGE #define PARA_IMAGE			(0x1<<1)
	m_imageDealMode = 0; //OTSU
	m_nESCPixel = 0;
	m_idThreshNoise = 0;		//��������ʽ
	m_idOfThreshold = 0;		//��ֵ���㷨ѡ��
	m_idFixThreshold = 0;
	m_iThreshNoiseLow = 50;		//������ͨ��������
	m_iThreshNoiseHi = 600000;		//������ͨ��������
	m_iCanBeRecognisedLenMin = 2; //ʶ�����������
	m_iCanBeRecognisedLenMax = 800; //ʶ�����������
	m_iExistMax = 200000;			//���������������
	m_iExistMin = 600;			//���������������
	m_iExistHeightMax = 2000;		//������ڸ߶���������
	m_iExistHeightMin = 20;		//������ڸ߶���������
	m_iExistWidthMax = 2000;		//������ڿ����������
	m_iExistWidthMin = 20;		//������ڿ����������
	m_bDisplayImageEffect = 0;  //�����ʾͼ��ԭͼ�����ͼ��
	m_nWHRatio = 1.5;			//ͼ��ĳ����
	m_blocksize = 75;			//����Ӧ��ֵ����С
								//��Ӧ���õĽ����ROI
	m_threshold2 = 30;

	//˫���˲�����������
	d = 9;
	sigmaColor = 20;
	sigmaSpace = 5;


	m_mode1 = 0;

	Tacc = 0.05;
	MaxVel = 52000;
	waves = 16000;//һ��������Ŀ



	g_UpTippedPos = -66;
	//�ײ�������
	g_BtTippedPos = -72;
	//�������λ��
	g_UpCameraPos = 0;
	//�ײ����λ��
	g_BtDaJiPos = 3;
	//�������λ��
	g_UpDaJiPos = 3;
	//����˫����λ��
	g_DoubleKnifePos = 5;
	//�ײ�˫����λ��
	g_DoubleKnifePos1 = -4;
	//�ײ����λ��
	g_BtCameraPos = 8;
	//Mark1����λ��  
	g_Mark1CylinderPos = 11;
	//Mark2����λ��
	g_Mark2CylinderPos = 16;
	//Markλ��
	g_MarkPos = 33;
	//���������Ӧ��
	g_ThirdDiedKnodePos = -21;// = -21;
	//���쳤��
	g_RailLenght = 48;
	//�������ո�Ӧ��
	IN_Start_Camera_SENSOR = 1;
	//��ϼ���Ӧ��
	IN_CHECK_BREAK = 3;
	//Ԥ��ɸѡ���ϴ���͹�������
	IN_Check_UP_TIPPEDUP = 6;
	//�����������޸�Ӧ��
	IN_CYL_Magnet_SENSOR = 7;
	//Ԥ��ɸѡ���ϴ���͹�������
	IN_Check_BT_TIPPEDUP = 8;
	//������ť
	IN_START_BTN = 16;
	//ֹͣ��ť
	IN_BTN_STOP = 17;
	//�ײ�˫������Ӧ��
	IN_Check_Double_Sword2 = 18;
	//Mark3��������
	IN_CYL_Mark3_UP_SENSOR = 19;
	//�����Ӧ������
	IN_CHECK_SENSOR = 25;
	//���������Ӧ��
	IN_DIED_KNOED_SENSOR = 25;
	//����˫������Ӧ��
	IN_Check_Double_Sword1 = 21;
	//Mark1���׸�Ӧ��
	IN_CYL_Mark1_UP_SENSOR = 22;
	//���ϰ�ť���߽�̤����
	IN_Foot_Switch = 23;
	//Mark2�������޸�Ӧ��
	IN_CYL_Mark2_UP_SENSOR = 24;
	//�����Ӧ������
	IN_CHECK_SENSOR1 = 20;
	//��ѹ�������޸�Ӧ��
	IN_CYL_Press_UP_SENSOR = 26;
	//��ͣ��ť
	IN_ESTOP_BTN = 27;
	//���������Ӧ��
	IN_Check_UP_DaJi = 28;
	//�ײ������Ӧ��
	IN_Check_BT_DaJi = 29;
	//�����������޸�Ӧ��
	IN_CYL_Push_UP_SENSOR = 30;

	top_inv.c1 = top_inv.r1 = 0.0;
	top_inv.c2 = top_inv.r2 = 300;
	bottom_inv = top_char = bottom_char = top_knife = bottom_knife = top_unbolt = bottom_unbolt = top_inv;

	TopPara.thresholdvalue = 245;
	TopPara.minarea = 2500;
	TopPara.maxarea = 35000;

	TopPara.knife_threshold = 235;
	TopPara.minlen = 300.0;
	TopPara.maxlen = 460.0;

	TopPara.match_acc = 0.5;

	BottomPara = TopPara;
}