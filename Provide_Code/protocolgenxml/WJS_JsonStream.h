#pragma once
#include <string>
#include <stdio.h>


using namespace std;

//Json值
struct stJsonObject;
struct stJsonArray;
struct stJsonValue
{
	int    _type; //0普通string,1表array,2表json对象
	string _v;
	stJsonObject* _pObject;
	stJsonArray*  _pArray;	
	stJsonValue()
	{
		_type = 0;
		_pObject = NULL;
		_pArray = NULL;	
	}
};

//{k:v,k:v}
//json对象
struct stJsonObject
{
	string _k;       //键
	stJsonValue _jv; //值
	stJsonObject* _pnext;
	bool  _bEnterToChild; //是否优先进入子项
	stJsonObject()
	{
		_pnext = NULL;
		_bEnterToChild = false;
	}
};



//Json数组
struct stJsonArray
{
	stJsonValue* _pv;
	unsigned int  _ArrayLen;
	stJsonArray()
	{
		_pv = NULL;
		_ArrayLen = 0;
	}

};

//二进制的值显示为 binary_len

//Json流
class WJS_JsonStream
{
public:
	WJS_JsonStream() { _pBeginJsonObject = new stJsonObject(); };
	~WJS_JsonStream() {
		Free();
	};

	////////////
private:
	void WriteJsonArrayToFile(FILE* fp,const stJsonArray& jsonArray) const
	{
		if (!jsonArray._pv)
			return;
		WriteJsonValueToFile(fp,*jsonArray._pv);
	}

	void WriteJsonValueToFile(FILE* fp,const stJsonValue& jsonValue) const
	{
		//0普通string,1表array,2表json对象
		switch (jsonValue._type)
		{
		case 0:
		{
			string tt_s = "\"";
			fwrite(tt_s.c_str(), tt_s.size(), 1, fp);
			fwrite(jsonValue._v.c_str(), jsonValue._v.length(), 1, fp);
			tt_s = "\"";
			fwrite(tt_s.c_str(), tt_s.size(), 1, fp);
		}
			break;
		case 1:
		{
			string tt_s = "[";
			fwrite(tt_s.c_str(), tt_s.size(), 1, fp);
			if (jsonValue._pArray)
			{
				for (unsigned int i = 0; i < jsonValue._pArray->_ArrayLen; i++)
				{
					if (i != 0)
					{
						tt_s = ", ";
						fwrite(tt_s.c_str(), tt_s.size(), 1, fp);
					}
					WriteJsonArrayToFile(fp, *(jsonValue._pArray + i));
				}
			}



			tt_s = "]";
			fwrite(tt_s.c_str(), tt_s.size(), 1, fp);

		}
			break;
		case 2:
		{
			StartWriteJsonObjectToFile(fp, jsonValue._pObject);

		}
			break;
		default:
			break;
		}

	}

	void WriteJsonObjectToFile(FILE* fp, const stJsonObject* pJsonObject) const
	{
		if (!pJsonObject)
			return;
		if (pJsonObject->_k.length() == 0)
			return;

		fwrite(pJsonObject->_k.c_str(), pJsonObject->_k.length(), 1, fp);
		string tt_s = " : ";
		fwrite(tt_s.c_str(), tt_s.size(), 1, fp);

		WriteJsonValueToFile(fp,pJsonObject->_jv);

		if (pJsonObject->_pnext)
		{
			tt_s = ", ";
			fwrite(tt_s.c_str(), tt_s.size(), 1, fp);
			WriteJsonObjectToFile(fp, pJsonObject->_pnext);
		}
			
	}

	void StartWriteJsonObjectToFile(FILE* fp, const stJsonObject* pJsonObject) const
	{
		string tt_s = "{";
		fwrite(tt_s.c_str(), tt_s.size(), 1, fp);

		WriteJsonObjectToFile(fp, pJsonObject);

		tt_s = "}";
		fwrite(tt_s.c_str(), tt_s.size(), 1, fp);
	}

public:
	void WriteStreamToFile(FILE* fp) const
	{
		if (!fp)
			return;
		StartWriteJsonObjectToFile(fp, _pBeginJsonObject);
	}
public:
	///////////////
	void Free()
	{
		if (_pBeginJsonObject)
		{
			FreeJsonObject(*_pBeginJsonObject);

			delete _pBeginJsonObject;
			_pBeginJsonObject = NULL;
		}
		
	}
	void CreateFirst()
	{
		if (_pBeginJsonObject)
			return;
		_pBeginJsonObject = new stJsonObject();
	}
private:
	void FreeJsonObject(stJsonObject& JsonObject)
	{
		if (JsonObject._pnext)
		{
			FreeJsonObject(*JsonObject._pnext);
			delete JsonObject._pnext;
			JsonObject._pnext = NULL;
		}

		FreeJsonValue(JsonObject._jv);
	}

	void FreeJsonValue(stJsonValue& JsonValue)
	{
		//0普通string,1表array,2表json对象
		switch (JsonValue._type)
		{
		case 1:
		{
			if (JsonValue._pArray)
			{
				for (unsigned int i = 0; i < JsonValue._pArray->_ArrayLen; i++)
				{
					FreeJsonArray(*(JsonValue._pArray+i));
				}	
				delete[] JsonValue._pArray;
				JsonValue._pArray = NULL;
			}
		}
		break;
		case 2:
		{
			if (JsonValue._pObject)
			{
				FreeJsonObject(*JsonValue._pObject);
				delete JsonValue._pObject;
				JsonValue._pObject = NULL;
			}
		}
		break;
		default:
			break;
		}
	}

	void FreeJsonArray(stJsonArray& JsonArray)
	{
		if (JsonArray._pv)
		{
			FreeJsonValue(*JsonArray._pv);
			delete JsonArray._pv;
			JsonArray._pv = NULL;
		}

	}

public:
	//////////////
	stJsonValue* CreateJsonValue_String(const string& value)
	{
		stJsonValue* tt_pJsonValue = new stJsonValue();
		if (tt_pJsonValue)
		{
			tt_pJsonValue->_type = 0;
			tt_pJsonValue->_v = value;
			return  tt_pJsonValue;
		}

		return NULL;
	}
	stJsonArray* CreateJsonArray(unsigned int num)
	{
		stJsonArray* tt_pJsonArray = new stJsonArray[num];
		if (tt_pJsonArray)
			tt_pJsonArray->_ArrayLen = num;
		return tt_pJsonArray;
	}

	stJsonObject* CreateJsonObject(const string& strkey, int vtype,bool bEnterToChild)
	{
		stJsonObject* tt_pJsonObject = new stJsonObject();
		if (tt_pJsonObject)
		{
			tt_pJsonObject->_k = strkey;
			tt_pJsonObject->_jv._type = vtype;
			tt_pJsonObject->_bEnterToChild = bEnterToChild;
			return tt_pJsonObject;
		}
		return NULL;
	}

	///////////
	//得到当前的操作JsonObject
	stJsonObject* GetCurOpJsonObject(stJsonObject* pJsonObject)
	{
		if (!pJsonObject)
			return NULL;

		if (pJsonObject->_bEnterToChild)
		{
			if (pJsonObject->_jv._pObject)
				return GetCurOpJsonObject(pJsonObject->_jv._pObject);

			return pJsonObject;
		}

		if (pJsonObject->_pnext)
			return GetCurOpJsonObject(pJsonObject->_pnext);

		return pJsonObject;
	}

	void WriteValueToJsonObject(stJsonObject* pJsonObject, const string& strValue)
	{
		stJsonObject* tt_pCurJsonObject = GetCurOpJsonObject(pJsonObject);
		if (tt_pCurJsonObject)
			tt_pCurJsonObject->_jv._v = strValue;
	}
	void WriteArrayValueToJsonObject(stJsonObject* pJsonObject, stJsonArray* pArray)
	{
		stJsonObject* tt_pCurJsonObject = GetCurOpJsonObject(pJsonObject);
		if (tt_pCurJsonObject)
			tt_pCurJsonObject->_jv._pArray = pArray;
	}

	void WriteValue(const string& strValue)
	{
		if (!_pBeginJsonObject)
			return;
		WriteValueToJsonObject(_pBeginJsonObject, strValue);
	}

	void WriteArrayValue(stJsonArray* pArray)
	{
		if (!_pBeginJsonObject)
			return;
		WriteArrayValueToJsonObject(_pBeginJsonObject,pArray);
	}

	stJsonObject* WriteNewKey(const string& strkey, int vtype, bool bEnterToChild)
	{
		stJsonObject* tt_pJsonObject = CreateJsonObject(strkey, vtype, bEnterToChild);
		AddJsonObject(tt_pJsonObject);
		return tt_pJsonObject;
	}

	void AddJsonObject(stJsonObject* pSrcJsonObject)
	{
		if (pSrcJsonObject && _pBeginJsonObject)
		{
			if (_pBeginJsonObject->_k.length() == 0 && (!_pBeginJsonObject->_pnext))
			{
				_pBeginJsonObject->_k = pSrcJsonObject->_k;
				_pBeginJsonObject->_jv = pSrcJsonObject->_jv;
				_pBeginJsonObject->_bEnterToChild = pSrcJsonObject->_bEnterToChild;
				_pBeginJsonObject->_pnext = pSrcJsonObject->_pnext;
			}
			else
				AddToTargetJsonObject(pSrcJsonObject, _pBeginJsonObject);
		}
	}

	void EnterToChildEnd(stJsonObject* pJsonObject)
	{
		if (!pJsonObject)
			return;

		if (pJsonObject->_bEnterToChild)
			pJsonObject->_bEnterToChild = false;

	}

	void AppendJsonStream(WJS_JsonStream& jns)
	{
		stJsonObject*   tt_pJsonObject = jns._pBeginJsonObject;
		jns._pBeginJsonObject = NULL;
		AddJsonObject(tt_pJsonObject);
	}
private:
	//加入到目标对象
	void AddToTargetJsonObject(stJsonObject* pSrcJsonObject, stJsonObject* pTargetJsonObject)
	{
		if (!pTargetJsonObject)
			return;
		//优先进入子项
		if (pTargetJsonObject->_bEnterToChild)
		{
			if (!pTargetJsonObject->_jv._pObject)
			{
				pTargetJsonObject->_jv._type = 2;
				pTargetJsonObject->_jv._pObject = pSrcJsonObject;
				return;
			}
			AddToTargetJsonObject(pSrcJsonObject, pTargetJsonObject->_jv._pObject);
			return;
		}

		//进入兄弟项
		if (!pTargetJsonObject->_pnext)
		{
			pTargetJsonObject->_pnext = pSrcJsonObject;
			return;
		}
		AddToTargetJsonObject(pSrcJsonObject, pTargetJsonObject->_pnext);
	}

public:
	///////////////
	string ToString(int v)
	{
		char tt_ch[64];
		sprintf(tt_ch, "%d", v);
		return tt_ch;
	}
	string ToString(unsigned int v)
	{
		char tt_ch[64];
		sprintf(tt_ch, "%d", v);
		return tt_ch;
	}
	string ToString(short v)
	{
		char tt_ch[64];
		sprintf(tt_ch, "%d", v);
		return tt_ch;
	}
	string ToString(unsigned short v)
	{
		char tt_ch[64];
		sprintf(tt_ch, "%d", v);
		return tt_ch;
	}
	string ToString(__int64 v)
	{
		char tt_ch[64];
		sprintf(tt_ch, "%lld", v);
		return tt_ch;
	}
	string ToString(bool v)
	{
		if (v)
			return "true";
		else
			return "false";
	}
	string ToString(float v)
	{
		char tt_ch[64];
		sprintf(tt_ch, "%.2f", v);
		return tt_ch;
	}
	string ToString(double v)
	{
		char tt_ch[64];
		sprintf(tt_ch, "%.2f", v);
		return tt_ch;
	}
	string ToString(long v)
	{
		char tt_ch[64];
		sprintf(tt_ch, "%d", v);
		return tt_ch;
	}
	string ToString(unsigned long v)
	{
		char tt_ch[64];
		sprintf(tt_ch, "%d", v);
		return tt_ch;
	}
	string ToString(unsigned char v)
	{
		char tt_ch[64];
		sprintf(tt_ch, "%c", v);
		return tt_ch;
	}
	string ToString(char v)
	{
		char tt_ch[64];
		sprintf(tt_ch, "%c", v);
		return tt_ch;
	}


public:
	stJsonObject*         _pBeginJsonObject;

};

class IWrite_JsonStream
{
public:
	virtual void NotifyWriteJsonStream(const WJS_JsonStream& jsonStream) = 0;
};

inline WJS_JsonStream& operator << (WJS_JsonStream& is, int v)
{
	is.WriteValue(is.ToString(v));
	return is;
}
inline WJS_JsonStream& operator << (WJS_JsonStream& is, unsigned int v)
{
	is.WriteValue(is.ToString(v));
	return is;
}
inline WJS_JsonStream& operator << (WJS_JsonStream& is, __int64 v)
{
	is.WriteValue(is.ToString(v));
	return is;
}
inline WJS_JsonStream& operator << (WJS_JsonStream& is, short v)
{
	is.WriteValue(is.ToString(v));
	return is;
}
inline WJS_JsonStream& operator << (WJS_JsonStream& is, unsigned short v)
{
	is.WriteValue(is.ToString(v));
	return is;
}
inline WJS_JsonStream& operator << (WJS_JsonStream& is, bool v)
{
	is.WriteValue(is.ToString(v));
	return is;
}
inline WJS_JsonStream& operator << (WJS_JsonStream& is, float v)
{
	is.WriteValue(is.ToString(v));
	return is;
}
inline WJS_JsonStream& operator << (WJS_JsonStream& is, double v)
{
	is.WriteValue(is.ToString(v));
	return is;
}
inline WJS_JsonStream& operator << (WJS_JsonStream& is, long v)
{
	is.WriteValue(is.ToString(v));
	return is;
}
inline WJS_JsonStream& operator << (WJS_JsonStream& is, unsigned long v)
{
	is.WriteValue(is.ToString(v));
	return is;
}
inline WJS_JsonStream& operator << (WJS_JsonStream& is, char v)
{
	is.WriteValue(is.ToString(v));
	return is;
}
inline WJS_JsonStream& operator << (WJS_JsonStream& is, unsigned char v)
{
	is.WriteValue(is.ToString(v));
	return is;
}
inline WJS_JsonStream& operator << (WJS_JsonStream& is, const string& v)
{
	is.WriteValue(v);
	return is;
}