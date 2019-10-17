/*



*/



#ifndef _CPPUTIL_HEAD_
#define _CPPUTIL_HEAD_

#include "MDeclSpec.h"
#include "MIntDef.h"
#include "IPType.h"
#include "MSerialsCpp.hpp"


#include <tuple>
#include <stdio.h>

namespace MSerials{

class LIntExport MRootObject{
public:
    virtual const char*ClassName(void) const = 0;
	int Revision(void) const { return 20180309; };
	int Version(void) const { return 2018; };
	const char* Creation(void) const { return "Author SK"; };
    virtual ~MRootObject(void){}
};

/****************************************************************************/
/*                           MBaseArray                                     */
/****************************************************************************/

class LIntExport MBaseArray : public MRootObject {
public:
	MBaseArray() : num(0), num_max(0) {} // Empty array
	MBaseArray(Mlong n, MBOOL single = FALSE);

	// virtual ~HBaseArray(void);

	virtual const char *ClassName(void) const = 0;
	Mlong Num(void) const { return num; }

protected:
	void Extend(Mlong last_index);
	virtual void NewLength(Mlong new_num, Mlong old_num) = 0;
	void EmptyArray() { num = num_max = 0; }
	void SingleArray();
	void NormalArray(Mlong n) { num = num_max = n; }
	void SetArray(Mlong n, Mlong nm) { num = n; num_max = nm; }
	Mlong AllocNum() const { return num_max; }
private:
	Mlong num;
	Mlong num_max;
	static  float incr;
	static  Mlong num_def;
};

/**

class LIntExport MTuple : public MBaseArray {
public:
	MTuple(void);
#if !defined(_TMS320C6X)
	MTuple(Mlong l);
#endif
	MTuple(int l);
	MTuple(float f);
	MTuple(double d);
	MTuple(const char *s);
	MTuple(const HCtrlVal &c);
	MTuple(const MTuple &in) :MBaseArray() { CopyTuple(in); }
	MTuple(Mlong length, const MTuple &value);
	MTuple(const MTuple &length, const MTuple &value);
	//MTuple(SpecialTuple d);

	~MTuple() { ClearTuple(); }
	MTuple &operator = (const MTuple& in);


	void CastFromHctuple(Hctuple *ctuple);
	void CastToHctuple(Hctuple &ctuple) const;

	MTuple       Sum(void) const;
	MTuple       Mean(void) const;
	MTuple       Deviation(void) const;
	MTuple       Median(void) const;
	MTuple       Getenv(void) const;
	MTuple       Max(void) const;
	MTuple       Min(void) const;
	MTuple       Rad(void) const;
	MTuple       Deg(void) const;
	MTuple       Real(void) const;
	MTuple       Int(void) const;
	MTuple       Round(void) const;
	MTuple       Chr(void) const;
	MTuple       Ord(void) const;
	MTuple       Chrt(void) const;
	MTuple       Ords(void) const;
	MTuple       Number(void) const;
	MTuple       Sqrt(void) const;
	MTuple       IsNumber(void) const;
	MTuple       Strlen(void) const;
	MTuple       Inverse(void) const;
	MTuple       Asin(void) const;
	MTuple       Acos(void) const;
	MTuple       Atan(void) const;
	MTuple       Log(void) const;
	MTuple       Log10(void) const;
	MTuple       Sin(void) const;
	MTuple       Cos(void) const;
	MTuple       Tan(void) const;
	MTuple       Sinh(void) const;
	MTuple       Cosh(void) const;
	MTuple       Tanh(void) const;
	MTuple       Exp(void) const;
	MTuple       Abs(void) const;
	MTuple       Sgn(void) const;
	MTuple       Rand(void) const;
	MTuple       Uniq(void) const;
	MTuple       Floor(void) const;
	MTuple       Ceil(void) const;
	MTuple       Sort(void) const;
	MTuple       SortIndex(void) const;
	MTuple       Cumul(void) const;
	MTuple       Atan2(const MTuple &op) const;
	MTuple       Pow(const MTuple &op) const;
	MTuple       Ldexp(const MTuple &op) const;
	MTuple       Fmod(const MTuple &op) const;
	MTuple       Min2(const MTuple &op) const;
	MTuple       Max2(const MTuple &op) const;
	MTuple       Find(const MTuple &op) const;
	MTuple       RegexpMatch(const MTuple &expression) const;
	MTuple       RegexpMatch(const char *expression) const;
	MTuple       RegexpReplace(const MTuple &expression, const MTuple &replace) const;
	MTuple       RegexpReplace(const char *expression, const char *replace) const;
	Mlong        RegexpTest(const MTuple &expression) const;
	Mlong        RegexpTest(const char *expression) const;
	MTuple       RegexpSelect(const MTuple &expression) const;
	MTuple       RegexpSelect(const char *expression) const;
	MTuple       Strchr(const MTuple &pattern) const;
	MTuple       Strchr(const char *pattern) const;
	MTuple       Strstr(const MTuple &pattern) const;
	MTuple       Strstr(const char *pattern) const;
	MTuple       Strrchr(const MTuple &pattern) const;
	MTuple       Strrchr(const char *pattern) const;
	MTuple       Strrstr(const MTuple &pattern) const;
	MTuple       Strrstr(const char *pattern) const;
	MTuple       StrBitSelect(Mlong index) const;
	MTuple       StrBitSelect(const MTuple &index) const;
	MTuple       ToString(const char *pattern) const;
	MTuple       ToString(const MTuple &pattern) const;
	MTuple       Split(const char *pattern) const;
	MTuple       Split(const MTuple &pattern) const;
	MTuple       Substring(const MTuple &index1, const MTuple &index2) const;
	MTuple       Substring(Mlong index1, Mlong index2) const;
	MTuple       Substring(const MTuple &index1, Mlong index2) const;
	MTuple       Substring(Mlong index1, const MTuple &index2) const;
	MTuple       Subset(const MTuple &index) const;
	MTuple       Remove(const MTuple &index) const;
	MTuple       SelectRank(Mlong index) const;
	MTuple       SelectRank(const MTuple &index) const;
	MBool        Xor(const MTuple &pattern) const;
	MTuple       TXor(const MTuple &val) const;
	MTuple       And(const MTuple &val) const;
	MTuple       Or(const MTuple &val) const;
	MTuple       Not(void) const;
	operator HCtrlVal(void) const;
	MTuple       operator () (Mlong min, Mlong max) const;
	MTuple       operator () (const MTuple &min, const MTuple &max) const;
	HCtrlVal    &operator [] (Mlong index);
	HCtrlVal     operator [] (Mlong index) const;
	HCtrlVal    &operator [] (const MTuple &index);
	HCtrlVal     operator [] (const MTuple &index) const;
	MTuple      &operator ++ (void); // nur fuer double und Mlong
	HBool        operator !  (void) const;
	MTuple       operator ~  (void) const;
	MTuple       operator << (const MTuple &val) const;
	MTuple       operator << (Mlong val) const;
	MTuple       operator >> (const MTuple &val) const;
	MTuple       operator >> (Mlong val) const;
	MTuple       operator +  (const MTuple &val) const;
	MTuple       operator +  (double val) const;
	MTuple       operator +  (int val) const;
#if !defined(_TMS320C6X)
	MTuple       operator +  (Mlong val) const;
#endif
	MTuple       operator +  (const char *val) const;
	MTuple       operator -  (const MTuple &val) const;
	MTuple       operator -  (double val) const;
	MTuple       operator -  (int val) const;
#if !defined(_TMS320C6X)
	MTuple       operator -  (Mlong val) const;
#endif
	MTuple       operator -  (void) const;
	MTuple       operator *  (const MTuple &val) const;
	MTuple       operator *  (double val) const;
	MTuple       operator *  (int val) const;
#if !defined(_TMS320C6X)
	MTuple       operator *  (Mlong val) const;
#endif
	MTuple       operator /  (const MTuple &val) const;
	MTuple       operator /  (double val) const;
	MTuple       operator /  (int val) const;
#if !defined(_TMS320C6X)
	MTuple       operator /  (Mlong val) const;
#endif
	MTuple       operator %  (const MTuple &val) const;
	MTuple       operator %  (Mlong val) const;
	MTuple      &operator += (const MTuple &val);
	MTuple      &operator += (double val);
	MTuple      &operator += (int val);
#if !defined(_TMS320C6X)
	MTuple      &operator += (Mlong val);
#endif
	MTuple      &operator += (HCtrlVal val);
	MBool        operator != (const MTuple &val) const;
	MBool        operator != (double val) const;
	MBool        operator != (int val) const;
#if !defined(_TMS320C6X)
	MBool        operator != (Mlong val) const;
#endif
	MTuple       operator ^  (const MTuple &val) const;
	MTuple       operator ^  (Mlong val) const;
	MTuple       operator |  (const MTuple &val) const;
	MTuple       operator |  (Mlong val) const;
	MTuple       operator &  (const MTuple &val) const;
	MTuple       operator &  (Mlong val) const;
	MBool        operator && (const MTuple &val) const;
	MBool        operator && (Mlong val) const;
	MBool        operator || (const MTuple &val) const;
	MBool        operator || (Mlong val) const;
	MBool        operator == (const MTuple &val) const;
	MBool        operator == (double val) const;
	MBool        operator == (int val) const;
#if !defined(_TMS320C6X)
	MBool        operator == (Mlong val) const;
#endif
	MBool        operator >= (const MTuple &val) const;
	MBool        operator >= (double val) const;
	MBool        operator >= (int val) const;
#if !defined(_TMS320C6X)
	MBool        operator >= (Mlong val) const;
#endif
	MBool        operator <= (const MTuple &val) const;
	MBool        operator <= (double val) const;
	MBool        operator <= (int val) const;
#if !defined(_TMS320C6X)
	MBool        operator <= (Mlong val) const;
#endif
	MBool        operator >  (const MTuple &val) const;
	MBool        operator >  (double val) const;
	MBool        operator >  (int val) const;
#if !defined(_TMS320C6X)
	MBool        operator >  (Mlong val) const;
#endif
	MBool        operator <  (const MTuple &val) const;
	MBool        operator <  (double val) const;
	MBool        operator <  (int val) const;
#if !defined(_TMS320C6X)
	MBool        operator <  (Mlong val) const;
#endif

	// friend operators
	LIntExport friend MTuple operator << (Mlong val1, const MTuple &val2);
	LIntExport friend MTuple operator >> (Mlong val1, const MTuple &val2);
	LIntExport friend MTuple operator + (double val1, const MTuple &val2);
	LIntExport friend MTuple operator + (int val1, const MTuple &val2);
	LIntExport friend MTuple operator + (Mlong val1, const MTuple &val2);
	LIntExport friend MTuple operator + (const char *val1, const MTuple &val2);
	LIntExport friend MTuple operator - (double val1, const MTuple &val2);
	LIntExport friend MTuple operator - (int val1, const MTuple &val2);
	LIntExport friend MTuple operator - (Mlong val1, const MTuple &val2);
	LIntExport friend MTuple operator * (double val1, const MTuple &val2);
	LIntExport friend MTuple operator * (int val1, const MTuple &val2);
	LIntExport friend MTuple operator * (Mlong val1, const MTuple &val2);
	LIntExport friend MTuple operator / (double val1, const MTuple &val2);
	LIntExport friend MTuple operator / (int val1, const MTuple &val2);
	LIntExport friend MTuple operator / (Mlong val1, const MTuple &val2);
	LIntExport friend MTuple operator % (Mlong val1, const MTuple &val2);
	LIntExport friend MTuple operator ^  (Mlong val1, const MTuple &val2);
	LIntExport friend MTuple operator | (Mlong val1, const MTuple &val2);
	LIntExport friend MTuple operator & (Mlong val1, const MTuple &val2);
	LIntExport friend HBool operator || (Mlong val1, const MTuple &val2);
	LIntExport friend HBool operator && (Mlong val1, const MTuple &val2);
	LIntExport friend HBool operator != (double val1, const MTuple &val2);
	LIntExport friend HBool operator != (int val1, const MTuple &val2);
	LIntExport friend HBool operator != (Mlong val1, const MTuple &val2);
	LIntExport friend HBool operator == (double val1, const MTuple &val2);
	LIntExport friend HBool operator == (int val1, const MTuple &val2);
	LIntExport friend HBool operator == (Mlong val1, const MTuple &val2);
	LIntExport friend HBool operator >= (double val1, const MTuple &val2);
	LIntExport friend HBool operator >= (int val1, const MTuple &val2);
	LIntExport friend HBool operator >= (Mlong val1, const MTuple &val2);
	LIntExport friend HBool operator <= (double val1, const MTuple &val2);
	LIntExport friend HBool operator <= (int val1, const MTuple &val2);
	LIntExport friend HBool operator <= (Mlong val1, const MTuple &val2);
	LIntExport friend HBool operator > (double val1, const MTuple &val2);
	LIntExport friend HBool operator > (int val1, const MTuple &val2);
	LIntExport friend HBool operator > (Mlong val1, const MTuple &val2);
	LIntExport friend HBool operator < (double val1, const MTuple &val2);
	LIntExport friend HBool operator < (int val1, const MTuple &val2);
	LIntExport friend HBool operator < (Mlong val1, const MTuple &val2);

	HBool        Continue(const MTuple &FinalValue, const MTuple &Increment);
	MTuple      &Append(const MTuple &t);
	MTuple       Concat(const MTuple &t) const;
	int          State(void) const { return state; }
	void         Reset(void) { ClearTuple(); tuple = 0; EmptyArray(); }
	const char  *ClassName(void) const { return "MTuple"; }
private:
	HCtrlVal    *tuple;       // values (array of Mlong/float/string)
	char        state;

	void  NewLength(Mlong new_num, Mlong old_num);
	void  ClearTuple();
	void  CopyTuple(const MTuple& in);

};



*/







}

#endif
