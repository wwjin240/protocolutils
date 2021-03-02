#pragma once
#include <vector>
#include <string>
#include <map>

using namespace std;


/**
 * 协议头
 */
struct ProtocolHead {
	char _cvertion[64];   //版本号
	char _author[32];     //作者
	char _createtime[512];//创建时间
	char _copyInfo[512];  //版权信息

	ProtocolHead()
	{
		sprintf(_cvertion, "//tool vertion:1.1 createtime:2010-02-27\n");
		sprintf(_author, "//author:wwjin\n");
		sprintf(_createtime, "//unknown\n");
		sprintf(_copyInfo, "//////////////copyright///////////\n");
	}

	void SetLua()
	{
		sprintf(_cvertion, "--tool vertion:1.1 createtime:2017-03-15\n");
		sprintf(_author, "--author:wwjin\n");
		sprintf(_createtime, "--unknown\n");
		sprintf(_copyInfo, "--////////////copyright///////////\n");
	}

	void SetPython()
	{
		sprintf(_cvertion, "#tool vertion:1.1 createtime:2017-11-24\n");
		sprintf(_author, "#author:wwjin\n");
		sprintf(_createtime, "#unknown\n");
		sprintf(_copyInfo, "#////////////copyright///////////\n");
	}
};

/*
 * 协议版本值 
 */
struct stParVersionValue {
	string _parVerName;  //参数的版本名
	int    _parVerValue; //参数的版本值
};

/**
 * 协议版本管理
 */
struct stVersionManger {
	int  _defaultVersion; //默认版本
	vector<stParVersionValue> _parVersionList;

	stVersionManger()
	{
		_defaultVersion = 1;
		//_protocolVersion = 1;
	}

	int GetVersion(const string& str)
	{
		for (size_t i = 0; i < _parVersionList.size(); i++) {
			if (_parVersionList[i]._parVerName.compare(str) == 0) {
				return _parVersionList[i]._parVerValue;
			}
		}

		return _defaultVersion;
	}

	int GetVersion(const char* str)
	{
		for (size_t i = 0; i < _parVersionList.size(); i++) {
			if (_parVersionList[i]._parVerName.compare(str) == 0) {
				return _parVersionList[i]._parVerValue;
			}
		}

		return _defaultVersion;
	}

	string GetVersionDesc(int version)
	{
		for (size_t i = 0; i < _parVersionList.size(); i++) {
			if (_parVersionList[i]._parVerValue == version)
				return _parVersionList[i]._parVerName;
		}

		if(version>=1)
		   return "defversion";

		return "";
	}

};

/**
 * 基本参数
 */
struct stBasePar {
	string _strParType; //参数类型
	string _strParName; //参数名
	bool   _IsConstRef; //是否是constref
	int    _version;    //版本
	string _comment;    //注释
	bool   _bWithDefault; //是否带默认值
	string _strDefaultValue;
	int    _transNeedValue; //传输需要值

	stBasePar()
	{
		_IsConstRef = false;
		_version = 1;
		_bWithDefault = false;
		_transNeedValue = -1;
	}

	//是否填完数据
	bool IsFillEnd()
	{
		if (_strParType.length() == 0 || _strParName.length() == 0)
			return false;
		return true;
	}

	//初始化
	void Init()
	{
		_strParType = "";
		_strParName = "";
		_IsConstRef = false;
		_comment = "";
		_transNeedValue = -1;
	}
	bool isInt64Type()
	{
		return _strParType == "__int64" || _strParType == "long long";
	}
};

/**
 * 结构体结构
 */
struct stStructStruct {
	string             _name; //名字
	vector<stBasePar>  _ParVec;//参数列表
	int                _index;//序号
	int                _version;    //版本
	string             _comment;//注释
	bool               _bWithConstruct; //是否带构造
	int                _transNeedValue; //传输需要值
	int    _startversion_ts; //起始版本，ts使用
	bool   _bBiaoZhuUsed;    //标注使用
	string _oldname;         //旧名字

	string _inherited;    //从某结构继承，只能单继承

	stStructStruct()
	{
		_bWithConstruct = false;
		_transNeedValue = -1;
		_startversion_ts = 0;
		_bBiaoZhuUsed = false;
	}

	bool   hasInt64Type()
	{
		for (size_t i = 0; i < _ParVec.size(); i++) {
			if (_ParVec[i].isInt64Type())
				return true;
		}
		return false;
	}

	bool IsHasTransNeedValuePar()
	{
		for (unsigned int i = 0; i < _ParVec.size(); i++) {
			if (_ParVec[i]._transNeedValue != -1) {
				return true;
			}
		}

		return false;

	}

	bool VersionIsOk()
	{
		for (unsigned int i = 1; i < _ParVec.size(); i++) {
			if (_ParVec[i]._version < _ParVec[i - 1]._version)
				return false;
		}
		return true;
	}
};

/**
 * 枚举结构
 */
struct stEnum {
	string          _name; //枚举名
	vector<string>  _ParNameList; //名字列表
	vector<int>     _versionList; //版本列表
	vector<string>  _commentList;     //注释
	vector<bool>    _keepOldList;    //是否保持旧的,用于新枚举下的算法
	string             _comment;//注释
	bool               _bNewVersion;
	vector<int>        _computedIdList; //计算好的id列表

	stEnum()
	{
		_bNewVersion = false;
	}

	bool VersionIsOk()
	{
		for (unsigned int i = 1; i < _versionList.size(); i++) {
			if (_versionList[i] < _versionList[i - 1])
				return false;
		}
		return true;
	}

	int GetMaxVersion() const
	{
		if (_versionList.size() > 0)
			return _versionList.back();

		return 0;

	}
};

/**
 * vector结构
 */
struct stVector {
	string _name;      //名称
	string _ParTName;  //模板名
	int             _index;//序号
	int             _version; //版本

							  //////引入是否是基本模板参数类型
	bool   _bBaseParT;
	string             _comment;//注释
	bool   _bBiaoZhuUsed;    //标注使用
	string _oldname;
	stVector()
	{
		_bBaseParT = false;
		_bBiaoZhuUsed = false;
	}
	bool isInt64Type()
	{
		return _ParTName == "__int64" || _ParTName == "long long";
	}

};

/**
 * WJSVector型
 */
struct stWJSVector {
	string _name;      //名称
	string _ParTName;  //模板名
	int    _index;//序号
	int    _version; //版本

					 //////引入是否是基本模板参数类型
	bool   _bBaseParT;
	bool   _bBiaoZhuUsed;    //标注使用

	stWJSVector()
	{
		_bBaseParT = false;
		_bBiaoZhuUsed = false;
	}

	bool isInt64Type()
	{
		return _ParTName == "__int64" || _ParTName == "long long";
	}

};

/**
 * 函数体结构,都是void返回类型
 */
struct stFunc {
	stFunc()
	{
		_userFuncId = 0;
		_bNew = false;
		_bDiscard = false;
		_bSCFlag = false;
		_bMobaFlag = false;
		_Layer = 0;
		_SCSpliteType = 0;
		_computedFuncId = 0;
	}
	string             _name;    //名字
	vector<stBasePar>  _InParVec;//输入参数列表
	int    _version;    //版本
	int    _userFuncId; //用户指定id,默认值为0,表明不生效
	bool   _bNew;       //是否是新的
	bool   _bDiscard;   //是否丢弃
	string             _comment;//注释
	bool   _bSCFlag;    //是否有SC标识
	bool   _bMobaFlag;  //是否有MobaFlag
	string _strSCClassName; //用于强制SC模式
	int    _Layer;
	int    _SCSpliteType;   //SC分离中的类型 0通用,1:S,-1:C
	string    _part2;          //子部分又叫XX区

	int    _computedFuncId; //计算过后的函数id
	bool   hasInt64Type()
	{
		for (size_t i = 0; i < _InParVec.size(); i++) {
			if (_InParVec[i].isInt64Type())
				return true;
		}
		return false;
	}
	bool VersionIsOk()
	{
		for (unsigned int i = 1; i < _InParVec.size(); i++) {
			if (_InParVec[i]._version < _InParVec[i - 1]._version)
				return false;
		}

		if (_InParVec.size() > 1) {
			if (_InParVec[_InParVec.size() - 1]._version > _version)
				return false;
		}
		return true;
	}
};

/**
 * 类结构
 */
struct stClass {
	stClass()
	{
		_bFuncAllUseNew = false;
	}
	string _strtype;//通过这个名字可知是服务器到客户端，还是客户端到服务器,约定request(C2S),callback(S2C)
	string _name;//名字,
	string _servername; //服务器端名
	string _clientname; //客户端名
	vector<stFunc> _FuncVec; //函数列表
	bool   _bFuncAllUseNew;  //是否函数都用新方法计算

	bool   hasInt64Type()
	{
		for (size_t i = 0; i < _FuncVec.size(); i++) {
			if (_FuncVec[i].hasInt64Type())
				return true;
		}
		return false;
	}
	int GetMaxLayer()
	{
		int tt_maxLayer = 0;
		for (size_t i = 0; i < _FuncVec.size(); i++) {
			if (_FuncVec[i]._Layer > tt_maxLayer) {
				tt_maxLayer = _FuncVec[i]._Layer;
			}
		}
		return tt_maxLayer;
	}
};

/**
 * 命名空间
 */
struct stNamespace {
	string _protocolname;//协议名
	string _protocolnamelower;//全小写协议名
	string _servername;
	string _clientname;

	string _shortname;  //短名

	vector<stEnum>          _EnumVec;
	vector<stStructStruct>  _StructVec;
	vector<stVector>        _VectorVec;
	vector<stClass>         _ClassVec;
	vector<stWJSVector>     _WJSVectorVec;

	int GetMaxIndex_StructAndVector() const {
		int t_index = -1;
		for (size_t i = 0; i < _StructVec.size(); i++) {
			if (_StructVec[i]._index > t_index)
				t_index = _StructVec[i]._index;
		}
		for (size_t i = 0; i < _VectorVec.size(); i++) {
			if (_VectorVec[i]._index > t_index)
				t_index = _VectorVec[i]._index;
		}
		for (size_t i = 0; i < _WJSVectorVec.size(); i++) {
			if (_WJSVectorVec[i]._index > t_index)
				t_index = _WJSVectorVec[i]._index;
		}
		return t_index;
	}


	stNamespace()
	{
	}
};

/**
 * 存储文件名
 */
struct stSaveFileName {
	string _servername;
	string _clientname;
	string _servername2; //用于iphone,为.m文件
	string _clientname2; //用于iphone,为.m文件

	stSaveFileName()
	{
		_servername = "gen_server.h";
		_clientname = "gen_client.h";
		_servername2 = "gen_server.m";
		_clientname2 = "gen_client.m";
	}
};

/**
 * 协议号,int型
 */
struct stProtocolNo {
	string _name;  //名称
	int    _value; //值

	stProtocolNo()
	{
		_value = 1;
	}
};

/**
 * 引用协议
 */
struct stRefProtocol {
	string _refProtocolName; //协议名
	string _includeheadnames; //包含头
	string _includeheadnamec;
	string _usingspacename;  //使用命名空间
	string _strPath;
	bool   _bNotCreateFile;  //是否不生成文件
	stRefProtocol()
	{
		_bNotCreateFile = false;

	}
};

enum class eProtocolWriterType
{
	eNone,
	CPlus,
	IPhone,
	Flash,
	Java,
	CSharp,
	Lua,
	JavaScript,
	TypeScript,
	Python,
	TSMiddle,

	eMax
};

//增加一个顺序版的
struct stPartManager
{
	string _name;
	string _comment;
	std::vector<string> _partList;
};

//c++标准类型
enum class eCPlusStandardType 	
{
	CPlus98,
	CPlus11,
	CPlus14,
	CPlus17,
	CPlus20,

	CPlusMax
};