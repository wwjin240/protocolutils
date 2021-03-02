#pragma once
#include <vector>
#include <string>
#include <map>

using namespace std;


/**
 * Э��ͷ
 */
struct ProtocolHead {
	char _cvertion[64];   //�汾��
	char _author[32];     //����
	char _createtime[512];//����ʱ��
	char _copyInfo[512];  //��Ȩ��Ϣ

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
 * Э��汾ֵ 
 */
struct stParVersionValue {
	string _parVerName;  //�����İ汾��
	int    _parVerValue; //�����İ汾ֵ
};

/**
 * Э��汾����
 */
struct stVersionManger {
	int  _defaultVersion; //Ĭ�ϰ汾
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
 * ��������
 */
struct stBasePar {
	string _strParType; //��������
	string _strParName; //������
	bool   _IsConstRef; //�Ƿ���constref
	int    _version;    //�汾
	string _comment;    //ע��
	bool   _bWithDefault; //�Ƿ��Ĭ��ֵ
	string _strDefaultValue;
	int    _transNeedValue; //������Ҫֵ

	stBasePar()
	{
		_IsConstRef = false;
		_version = 1;
		_bWithDefault = false;
		_transNeedValue = -1;
	}

	//�Ƿ���������
	bool IsFillEnd()
	{
		if (_strParType.length() == 0 || _strParName.length() == 0)
			return false;
		return true;
	}

	//��ʼ��
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
 * �ṹ��ṹ
 */
struct stStructStruct {
	string             _name; //����
	vector<stBasePar>  _ParVec;//�����б�
	int                _index;//���
	int                _version;    //�汾
	string             _comment;//ע��
	bool               _bWithConstruct; //�Ƿ������
	int                _transNeedValue; //������Ҫֵ
	int    _startversion_ts; //��ʼ�汾��tsʹ��
	bool   _bBiaoZhuUsed;    //��עʹ��
	string _oldname;         //������

	string _inherited;    //��ĳ�ṹ�̳У�ֻ�ܵ��̳�

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
 * ö�ٽṹ
 */
struct stEnum {
	string          _name; //ö����
	vector<string>  _ParNameList; //�����б�
	vector<int>     _versionList; //�汾�б�
	vector<string>  _commentList;     //ע��
	vector<bool>    _keepOldList;    //�Ƿ񱣳־ɵ�,������ö���µ��㷨
	string             _comment;//ע��
	bool               _bNewVersion;
	vector<int>        _computedIdList; //����õ�id�б�

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
 * vector�ṹ
 */
struct stVector {
	string _name;      //����
	string _ParTName;  //ģ����
	int             _index;//���
	int             _version; //�汾

							  //////�����Ƿ��ǻ���ģ���������
	bool   _bBaseParT;
	string             _comment;//ע��
	bool   _bBiaoZhuUsed;    //��עʹ��
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
 * WJSVector��
 */
struct stWJSVector {
	string _name;      //����
	string _ParTName;  //ģ����
	int    _index;//���
	int    _version; //�汾

					 //////�����Ƿ��ǻ���ģ���������
	bool   _bBaseParT;
	bool   _bBiaoZhuUsed;    //��עʹ��

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
 * ������ṹ,����void��������
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
	string             _name;    //����
	vector<stBasePar>  _InParVec;//��������б�
	int    _version;    //�汾
	int    _userFuncId; //�û�ָ��id,Ĭ��ֵΪ0,��������Ч
	bool   _bNew;       //�Ƿ����µ�
	bool   _bDiscard;   //�Ƿ���
	string             _comment;//ע��
	bool   _bSCFlag;    //�Ƿ���SC��ʶ
	bool   _bMobaFlag;  //�Ƿ���MobaFlag
	string _strSCClassName; //����ǿ��SCģʽ
	int    _Layer;
	int    _SCSpliteType;   //SC�����е����� 0ͨ��,1:S,-1:C
	string    _part2;          //�Ӳ����ֽ�XX��

	int    _computedFuncId; //�������ĺ���id
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
 * ��ṹ
 */
struct stClass {
	stClass()
	{
		_bFuncAllUseNew = false;
	}
	string _strtype;//ͨ��������ֿ�֪�Ƿ��������ͻ��ˣ����ǿͻ��˵�������,Լ��request(C2S),callback(S2C)
	string _name;//����,
	string _servername; //����������
	string _clientname; //�ͻ�����
	vector<stFunc> _FuncVec; //�����б�
	bool   _bFuncAllUseNew;  //�Ƿ��������·�������

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
 * �����ռ�
 */
struct stNamespace {
	string _protocolname;//Э����
	string _protocolnamelower;//ȫСдЭ����
	string _servername;
	string _clientname;

	string _shortname;  //����

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
 * �洢�ļ���
 */
struct stSaveFileName {
	string _servername;
	string _clientname;
	string _servername2; //����iphone,Ϊ.m�ļ�
	string _clientname2; //����iphone,Ϊ.m�ļ�

	stSaveFileName()
	{
		_servername = "gen_server.h";
		_clientname = "gen_client.h";
		_servername2 = "gen_server.m";
		_clientname2 = "gen_client.m";
	}
};

/**
 * Э���,int��
 */
struct stProtocolNo {
	string _name;  //����
	int    _value; //ֵ

	stProtocolNo()
	{
		_value = 1;
	}
};

/**
 * ����Э��
 */
struct stRefProtocol {
	string _refProtocolName; //Э����
	string _includeheadnames; //����ͷ
	string _includeheadnamec;
	string _usingspacename;  //ʹ�������ռ�
	string _strPath;
	bool   _bNotCreateFile;  //�Ƿ������ļ�
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

//����һ��˳����
struct stPartManager
{
	string _name;
	string _comment;
	std::vector<string> _partList;
};

//c++��׼����
enum class eCPlusStandardType 	
{
	CPlus98,
	CPlus11,
	CPlus14,
	CPlus17,
	CPlus20,

	CPlusMax
};