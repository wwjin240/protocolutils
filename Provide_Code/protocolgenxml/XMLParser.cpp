#include "XMLParser.h"

//基本数据类型
string g_strBaseType[] = { "void","int","unsigned int","short","unsigned short","char","byte","BYTE","bool","BOOL", \
"DWORD","float","long","unsigned long","string","__int64","wjs_char32","wjs_char64","wjs_char128","wjs_char256",\
"wjs_char512","wjs_char1024","long long","unsigned char","wjs_utf8","u16string","wstring" };

XMLParser::XMLParser()
	:m_bHCPP(false)
	,m_bUsePartProtocol(false)
	, m_bExceptMoba(true)
	, m_nStructVecCount(0)
	, m_bAndroidPeer(false)
{
	m_baseTypeDefaultMap.insert(make_pair("int","0"));
	m_baseTypeDefaultMap.insert(make_pair("unsigned int", "0"));
	m_baseTypeDefaultMap.insert(make_pair("short", "0"));
	m_baseTypeDefaultMap.insert(make_pair("unsigned short", "0"));
	m_baseTypeDefaultMap.insert(make_pair("byte", "0"));
	m_baseTypeDefaultMap.insert(make_pair("BYTE", "0"));

	m_baseTypeDefaultMap.insert(make_pair("bool", "false"));
	m_baseTypeDefaultMap.insert(make_pair("BOOL", "FALSE"));
	m_baseTypeDefaultMap.insert(make_pair("DWORD", "0"));
	m_baseTypeDefaultMap.insert(make_pair("float", "0"));
	m_baseTypeDefaultMap.insert(make_pair("long", "0"));
	m_baseTypeDefaultMap.insert(make_pair("unsigned long", "0"));
	m_baseTypeDefaultMap.insert(make_pair("__int64", "0"));
	m_baseTypeDefaultMap.insert(make_pair("long long", "0"));
	m_baseTypeDefaultMap.insert(make_pair("unsigned char", "0"));
}

XMLParser::~XMLParser()
{

}
void XMLParser::Free_()
{
	m_NamespaceVec.clear();
	m_nStructVecCount = 0;
	m_strErrorReason = "";
	m_PartManageMap.clear();
	m_PartManageList.clear();
	m_YuanPartList.clear();

}
bool XMLParser::ParserXMLFile(const string& strFileName)
{
	stNamespace tt_namespace;
	m_NamespaceVec.push_back(tt_namespace);

	TiXmlDocument doc(strFileName.c_str());
	bool bLoadOK = doc.LoadFile();
	if (!bLoadOK) //载入是否成功
		return false;

	//doc.Print( stdout );

	TiXmlNode* pNode = 0;
	TiXmlElement* pElement = 0;

	//解析根节点
	pNode = doc.RootElement();
	pElement = pNode->ToElement();

	string strValue = pNode->ValueStr();
	if (strValue.compare("protocol") != 0)
		return false;

	const char* pch = pElement->Attribute("name");
	if (!pch)
		return false;
	m_NamespaceVec[0]._protocolname.append(pch);

	m_NamespaceVec[0]._protocolnamelower = m_NamespaceVec[0]._protocolname;
	ToLower(m_NamespaceVec[0]._protocolnamelower);



	pNode = pNode->FirstChildElement();
	if (!pNode)
		return false;
	pElement = pNode->ToElement();
	strValue = pNode->ValueStr();

	while (true) {
		//协议号
		if (strValue.compare("protocolNo") == 0) {
			if (!ParserProtocolNo(pNode, pElement)) {
#ifdef _DEBUG
				_Logger.Log(eLogTypeDebug, "ParserProtocolNo failed");
#endif
				printf("ParserProtocolNo failed\n");
				return false;
			}
		}
		else if (strValue.compare("namespacename") == 0) //命名空间
		{
			if (!ParserNamespace(pNode, pElement)) {
#ifdef _DEBUG
				_Logger.Log(eLogTypeDebug, "ParserNamespace failed");
#endif
				printf("ParserNamespace failed\n");
				return false;
			}
		}
		else if (strValue.compare("protocolRef") == 0)//协议引用
		{
			if (!ParserProtocolRef(pNode, pElement)) {
#ifdef _DEBUG
				_Logger.Log(eLogTypeDebug, "ParserProtocolRef failed");
#endif
				printf("ParserProtocolRef failed\n");
				return false;
			}
		}
		else if (strValue.compare("VManger") == 0)//协议版本
		{
			if (!ParserVersionManger(pNode, pElement)) {
#ifdef _DEBUG
				_Logger.Log(eLogTypeDebug, "ParserVersionManger failed");
#endif
				printf("ParserVersionManger failed\n");
				return false;
			}
		}
		else if (strValue.compare("PartManager") == 0)//协议版本
		{
			if (!ParserPartManager(pNode, pElement)) {
#ifdef _DEBUG
				_Logger.Log(eLogTypeDebug, "ParserPartManager failed");
#endif
				printf("ParserPartManager failed\n");
				return false;
			}
		}
		else if (strValue.compare("enum") == 0) //枚举
		{
			if (!ParserEnum(pNode, pElement)) {
				return false;
			}
		}
		else if (strValue.compare("struct") == 0) //结构体
		{
			if (!ParserStruct(pNode, pElement)) {
#ifdef _DEBUG
				_Logger.Log(eLogTypeDebug, "ParserStruct failed");
#endif
				printf("ParserStruct failed\n");
				return false;
			}
		}
		else if (strValue.compare("sequence") == 0) {
			if (!ParserVector(pNode, pElement)) {
#ifdef _DEBUG
				_Logger.Log(eLogTypeDebug, "ParserVector failed");
#endif
				printf("ParserVector failed\n");
				return false;
			}
		}
		else if (strValue.compare("sequence2") == 0) {
			if (!ParserWJSVector(pNode, pElement)) {
#ifdef _DEBUG
				_Logger.Log(eLogTypeDebug, "ParserWJSVector failed");
#endif
				printf("ParserWJSVector failed\n");
				return false;
			}
		}
		else if (strValue.compare("request") == 0) {
			if (!ParserClass(pNode, pElement, true)) {
#ifdef _DEBUG
				_Logger.Log(eLogTypeDebug, "ParserClass request failed");
#endif
				printf("ParserClass request failed\n");
				return false;
			}
		}
		else if (strValue.compare("callback") == 0) {
			if (!ParserClass(pNode, pElement, false)) {
#ifdef _DEBUG
				_Logger.Log(eLogTypeDebug, "ParserClass callback failed");
#endif
				printf("ParserClass callback failed\n");
				return false;
			}
		}
		else {
#ifdef _DEBUG
			_Logger.Log(eLogTypeDebug, "无法解析 failed");
#endif
			printf("无法解析 failed\n");
			return false;
		}

		//命名空间名
		pNode = pNode->NextSiblingElement();
		if (pNode) {
			pElement = pNode->ToElement();
			strValue = pNode->ValueStr();
		}
		else {
			//先检测是否已经读完
			return true;
		}

	}
#ifdef _DEBUG
	_Logger.Log(eLogTypeDebug, "未成功解析 failed");
#endif
	printf("未成功解析 failed\n");
	return false;
}

bool XMLParser::ParserNamespace(TiXmlNode* pNode, TiXmlElement* pElement)
{
	const char* pch = 0;
	if (!pElement)
		return false;

	pch = pElement->Attribute("value1");
	if (!pch) {
		printf("namesapce not has valuel\n");
		return false;
	}


	m_NamespaceVec[0]._servername.append(pch);

	pch = pElement->Attribute("value2");
	if (!pch) {
		printf("namesapce not has value2\n");
		return false;
	}

	m_NamespaceVec[0]._clientname.append(pch);

	pch = pElement->Attribute("value3");
	if (pch) {
		if (strcmp(pch, "hcpp") == 0)
			m_bHCPP = true;
	}

	pch = pElement->Attribute("shortname");
	if (pch) {
		m_NamespaceVec[0]._shortname = pch;
	}

	return true;
}

bool XMLParser::ParserProtocolNo(TiXmlNode* pNode, TiXmlElement* pElement)
{
	const char* pch = 0;
	if (!pElement)
		return false;

	pch = pElement->Attribute("name");
	if (!pch) {
		printf("protocolno not has name\n");
		return false;
	}

	m_ProtocolNo._name.append(pch);

	pch = pElement->Attribute("value");
	if (!pch) {
		printf("protocolno not has value\n");
		return false;
	}


	m_ProtocolNo._value = atoi(pch);

	return true;
}

bool XMLParser::ParserProtocolRef(TiXmlNode* pNode, TiXmlElement* pElement)
{
	const char* pch = 0;
	if (!pElement)
		return false;

	//pch=pElement->Attribute("name");
	//if(!pch)
	//	return false;
	//m_ProtocolNo._name.append(pch);

	stRefProtocol tt_RefProtocol;

	pch = pElement->Attribute("value1");
	if (!pch) {
		printf("protocolref has no value1\n");
		return false;
	}

	tt_RefProtocol._refProtocolName.append(pch);

	pch = pElement->Attribute("value2");
	if (!pch) {
		printf("protocolref has no value2\n");
		return false;
	}

	tt_RefProtocol._includeheadnames.append(pch);
	pch = pElement->Attribute("value3");
	if (!pch) {
		printf("protocolref has no value3\n");
		return false;
	}

	tt_RefProtocol._includeheadnamec.append(pch);
	pch = pElement->Attribute("value4");
	if (!pch) {
		printf("protocolref has no value4\n");
		return false;
	}

	tt_RefProtocol._usingspacename.append(pch);

	pch = pElement->Attribute("path");
	if (pch)
		tt_RefProtocol._strPath.append(pch);

	pch = pElement->Attribute("NotCreateFile");
	if (pch)
		tt_RefProtocol._bNotCreateFile = true;

	m_RefProtocolList.push_back(tt_RefProtocol);

	RefProtocolProcess();
	return true;
}

bool XMLParser::ParserVersionManger(TiXmlNode* pNode, TiXmlElement* pElement) //解析协议版本信息
{
	const char* pch = 0;
	if (!pElement)
		return false;

	//名称
	pch = pElement->Attribute("name");
	if (!pch)
		return false;


	TiXmlNode* tt_pChild = pNode->FirstChildElement();
	if (!tt_pChild)
		return false;

	TiXmlElement* tt_pElement = tt_pChild->ToElement();
	pch = tt_pElement->Attribute("name");
	if (!pch)
		return false;


	while (tt_pChild) {
		tt_pElement = tt_pChild->ToElement();
		pch = tt_pElement->Attribute("name");
		if (!pch)
			return false;

		string strname = pch;
		pch = tt_pElement->Attribute("value");
		if (!pch)
			return false;

		if (strname.compare("defversion") == 0) {
			m_VersionManger._defaultVersion = atoi(pch);
		}
		else {
			stParVersionValue tt_par;
			tt_par._parVerName = strname;
			tt_par._parVerValue = atoi(pch);
			m_VersionManger._parVersionList.push_back(tt_par);
		}

		tt_pChild = tt_pChild->NextSiblingElement();
		if (!tt_pChild)
			break;
	}

	return true;
}

bool XMLParser::ParserPartManager(TiXmlNode* pNode, TiXmlElement* pElement)
{
	const char* pch = 0;
	if (!pElement)
		return false;

	//名称
	pch = pElement->Attribute("name");
	if (!pch)
		return false;

	string tt_PartMangeName = pch;

	string t_comment;
	pch = pElement->Attribute("comment");
	if (pch) {
		t_comment.append(pch);
	}

	TiXmlNode* tt_pChild = pNode->FirstChildElement();
	if (!tt_pChild)
		return false;

	TiXmlElement* tt_pElement = tt_pChild->ToElement();
	pch = tt_pElement->Attribute("name");
	if (!pch)
		return false;

	while (tt_pChild) {
		tt_pElement = tt_pChild->ToElement();
		pch = tt_pElement->Attribute("name");
		if (!pch)
			return false;

		string strname = pch;
		AddToPartManage(tt_PartMangeName, strname,t_comment);
		

		tt_pChild = tt_pChild->NextSiblingElement();
		if (!tt_pChild)
			break;
	}

	return true;
}

#pragma region 解析枚举
bool XMLParser::ParserEnum(TiXmlNode* pNode, TiXmlElement* pElement)
{
	const char* pch = 0;
	if (!pElement)
		return false;
	stEnum tt_enum;

	pch = pElement->Attribute("name");
	if (!pch) {
		printf("enum has no name\n");
		return false;
	}

	tt_enum._name.append(pch);

	pch = pElement->Attribute("comment");
	if (pch) {
		tt_enum._comment.append(pch);
	}

	pch = pElement->Attribute("New");
	if (pch) {
		tt_enum._bNewVersion = true;
	}


	TiXmlNode* tt_pChild = pNode->FirstChildElement();
	if (!tt_pChild)
		return false;

	TiXmlElement* tt_pElement = tt_pChild->ToElement();
	pch = tt_pElement->Attribute("name");
	if (!pch) {
		printf("%s sub has no name\n", tt_enum._name.c_str());
		return false;
	}

	while (tt_pChild) {

		tt_pElement = tt_pChild->ToElement();
		pch = tt_pElement->Attribute("name");
		if (!pch) {
			printf("%s sub has no name\n", tt_enum._name.c_str());
			return false;
		}

		string tt_str2;
		tt_str2.append(pch);
		tt_enum._ParNameList.push_back(tt_str2);

		pch = tt_pElement->Attribute("comment");
		if (pch) {
			tt_enum._commentList.push_back(pch);
		}
		else
			tt_enum._commentList.push_back("");

		////////////增加版本
		pch = tt_pElement->Attribute("vertion");
		if (pch) {
			tt_enum._versionList.push_back(m_VersionManger.GetVersion(pch));
		}
		else {
			pch = tt_pElement->Attribute("version");
			if (pch) {
				tt_enum._versionList.push_back(m_VersionManger.GetVersion(pch));
			}
			else {
				//没有取默认
				tt_enum._versionList.push_back(m_VersionManger._defaultVersion);
			}
		}

		pch = tt_pElement->Attribute("KeepOld");
		if (pch)
			tt_enum._keepOldList.push_back(true);
		else
			tt_enum._keepOldList.push_back(false);

		tt_enum._computedIdList.push_back(-1);


		tt_pChild = tt_pChild->NextSiblingElement();
		if (!tt_pChild)
			break;
	}

	if (!tt_enum.VersionIsOk()) {
		printf("%s version error\n", tt_enum._name.c_str());
		return false;
	}

	m_NamespaceVec[0]._EnumVec.push_back(tt_enum);
	return true;
}
#pragma endregion

#pragma region 解析结构体
bool XMLParser::ParserStruct(TiXmlNode* pNode, TiXmlElement* pElement)
{
	const char* pch = 0;
	if (!pElement)
		return false;
	stStructStruct tt_struct;

	pch = pElement->Attribute("name");
	if (!pch) {
		printf("struct has no name\n");
		return false;
	}
	tt_struct._name.append(pch);
	if (!IsAllAttributeRecongnize_Struct(pElement, tt_struct._name))
		return false;

	pch = pElement->Attribute("oldname");
	if (pch)
		tt_struct._oldname.append(pch);

	pch = pElement->Attribute("comment");
	if (pch) {
		tt_struct._comment.append(pch);
	}

	pch = pElement->Attribute("construct");
	if (pch) {
		tt_struct._bWithConstruct = true;
	}
	else {
		pch = pElement->Attribute("consruct");
		if (pch) {
			tt_struct._bWithConstruct = true;
		}
	}


	pch = pElement->Attribute("vertion");
	if (pch) {
		tt_struct._version = m_VersionManger.GetVersion(pch);
	}
	else {
		pch = pElement->Attribute("version");
		if (pch) {
			tt_struct._version = m_VersionManger.GetVersion(pch);
		}
		else {
			//没有取默认
			tt_struct._version = m_VersionManger._defaultVersion;
		}
	}

	pch = pElement->Attribute("startversion_ts");
	if (pch) {
		tt_struct._startversion_ts = m_VersionManger.GetVersion(pch);
	}

	pch = pElement->Attribute("inherited");
	if (pch) {
		tt_struct._inherited = pch;
	}

	TiXmlNode* tt_pChild = pNode->FirstChildElement();
	if (!tt_pChild)
		return false;

	stBasePar tt_basepar1;
	TiXmlElement* tt_pElement = tt_pChild->ToElement();
	pch = tt_pElement->Attribute("name");
	if (!pch) {
		printf("%s par has no name\n", tt_struct._name.c_str());
		return false;
	}
	tt_basepar1._strParName.append(pch);
	if (!IsAllAttributeRecongnize_StructPar(tt_pElement, tt_struct._name, tt_basepar1._strParName))
		return false;

	pch = tt_pElement->Attribute("type");
	if (!pch) {
		printf("%s:%s  has no type\n", tt_struct._name.c_str(), tt_basepar1._strParName.c_str());
		return false;
	}

	tt_basepar1._strParType.append(pch);

	pch = tt_pElement->Attribute("default");
	if (pch) {
		tt_basepar1._bWithDefault = true;
		tt_basepar1._strDefaultValue.append(pch);
	}


	pch = tt_pElement->Attribute("comment");
	if (pch) {
		tt_basepar1._comment.append(pch);
	}

	pch = tt_pElement->Attribute("transneedvalue");
	if (pch) {
		tt_basepar1._transNeedValue = atoi(pch);
	}


	//检测是否是有效的类型
	if (!IsValidType(tt_basepar1._strParType)) {
#ifdef _DEBUG
		_Logger.Logf(eLogTypeDebug, "ParserStruct 无效类型 %s", tt_basepar1._strParType.c_str());
#endif
		printf("ParserStruct 无效类型 %s\n", tt_basepar1._strParType.c_str());
		return false;
	}
	TestChangeInt64ToLongLong(tt_basepar1._strParType);

	//不是基础类型，也不是枚举
	TestBaseParIsConstRef(tt_basepar1);

	////////////增加版本
	pch = tt_pElement->Attribute("vertion");
	if (pch) {
		tt_basepar1._version = m_VersionManger.GetVersion(pch);
	}
	else {
		pch = tt_pElement->Attribute("version");
		if (pch) {
			tt_basepar1._version = m_VersionManger.GetVersion(pch);
		}
		else {
			//没有取默认
			tt_basepar1._version = m_VersionManger._defaultVersion;
		}
	}


	tt_struct._ParVec.push_back(tt_basepar1);

	while (true) {
		tt_pChild = tt_pChild->NextSiblingElement();
		if (!tt_pChild)
			break;

		stBasePar tt_basepar2;

		tt_pElement = tt_pChild->ToElement();
		pch = tt_pElement->Attribute("name");
		if (!pch) {
			printf("%s par has no name\n", tt_struct._name.c_str());
			return false;
		}

		tt_basepar2._strParName.append(pch);
		if (!IsAllAttributeRecongnize_StructPar(tt_pElement, tt_struct._name, tt_basepar2._strParName))
			return false;

		pch = tt_pElement->Attribute("type");
		if (!pch) {
			printf("%s:%s  has no type\n", tt_struct._name.c_str(), tt_basepar2._strParName.c_str());
			return false;
		}

		tt_basepar2._strParType.append(pch);

		pch = tt_pElement->Attribute("comment");
		if (pch) {
			tt_basepar2._comment.append(pch);
		}

		pch = tt_pElement->Attribute("default");
		if (pch) {
			tt_basepar2._bWithDefault = true;
			tt_basepar2._strDefaultValue.append(pch);
		}

		pch = tt_pElement->Attribute("transneedvalue");
		if (pch) {
			tt_basepar2._transNeedValue = atoi(pch);
		}

		//检测是否是有效的类型
		if (!IsValidType(tt_basepar2._strParType)) {
#ifdef _DEBUG
			_Logger.Logf(eLogTypeDebug, "ParserStruct 无效类型 %s", tt_basepar2._strParType.c_str());
#endif
			printf("ParserStruct 无效类型 %s\n", tt_basepar2._strParType.c_str());
			return false;
		}
		TestChangeInt64ToLongLong(tt_basepar2._strParType);

		TestBaseParIsConstRef(tt_basepar2);

		////////////增加版本
		pch = tt_pElement->Attribute("vertion");
		if (pch) {
			tt_basepar2._version = m_VersionManger.GetVersion(pch);
		}
		else {
			pch = tt_pElement->Attribute("version");
			if (pch) {
				tt_basepar2._version = m_VersionManger.GetVersion(pch);
			}
			else {
				//没有取默认
				tt_basepar2._version = m_VersionManger._defaultVersion;
			}
		}

		tt_struct._ParVec.push_back(tt_basepar2);
	}

	tt_struct._index = m_nStructVecCount++;

	if (!tt_struct.VersionIsOk()) {
		printf("%s  version error", tt_struct._name.c_str());
		return false;
	}

	if (tt_struct._ParVec.size() > 0 && tt_struct._ParVec[tt_struct._ParVec.size() - 1]._version > tt_struct._version) {
		printf("%s version error", tt_struct._name.c_str());
		return false;
	}

	//如果结构体有构造增加默认值
	if (tt_struct._bWithConstruct) {
		for (auto& r : tt_struct._ParVec) {
			if (!r._bWithDefault && r._strDefaultValue == "") {
				r._strDefaultValue = GetBaseTypeDefault(r._strParType);
				if (r._strDefaultValue.length() > 0) {
					r._bWithDefault = true;
				}
			}
		}
	}

	m_NamespaceVec[0]._StructVec.push_back(tt_struct);
	if (tt_struct._oldname.length() > 0) {
		tt_struct._name = tt_struct._oldname;
		m_NamespaceVec[0]._StructVec.push_back(tt_struct);
	}
	return true;
}
bool XMLParser::IsAllAttributeRecongnize_Struct(TiXmlElement* pElement, const string& structName)
{
	if (!pElement)
		return false;
	static std::set<std::string> s_sets;
	if (s_sets.size() == 0) {
		s_sets.insert("name");
		s_sets.insert("comment");
		s_sets.insert("construct");
		s_sets.insert("consruct");
		s_sets.insert("vertion");
		s_sets.insert("version");
		s_sets.insert("startversion_ts");
		s_sets.insert("oldname");
		s_sets.insert("inherited");
	}

	const TiXmlAttribute* pAttr = pElement->FirstAttribute();
	while (pAttr) {
		string s_name = string(pAttr->Name());
		auto iter = s_sets.find(s_name);
		if (iter == s_sets.end()) {
			printf("struct[%s]: cantparser attr[%s]\n", structName.c_str(), s_name.c_str());
#ifdef _DEBUG
			_Logger.Logf(eLogTypeDebug, "struct[%s]: cantparser attr[%s]\n", structName.c_str(), s_name.c_str());
#endif
			return false;
		}
		pAttr = pAttr->Next();
	}
	return true;
}
bool XMLParser::IsAllAttributeRecongnize_StructPar(TiXmlElement* pElement, const string& structName, const string& parName)
{
	if (!pElement)
		return false;
	static std::set<std::string> s_sets;
	if (s_sets.size() == 0) {
		s_sets.insert("name");
		s_sets.insert("type");
		s_sets.insert("default");
		s_sets.insert("defalut");
		s_sets.insert("defalue");
		s_sets.insert("comment");
		s_sets.insert("transneedvalue");
		s_sets.insert("vertion");
		s_sets.insert("version");
	}

	const TiXmlAttribute* pAttr = pElement->FirstAttribute();
	while (pAttr) {
		string s_name = string(pAttr->Name());
		auto iter = s_sets.find(s_name);
		if (iter == s_sets.end()) {
			printf("struct[%s],par[%s]: cantparser attr[%s]\n", structName.c_str(), parName.c_str(), s_name.c_str());
#ifdef _DEBUG
			_Logger.Logf(eLogTypeDebug, "struct[%s],par[%s]: cantparser attr[%s]\n", structName.c_str(), parName.c_str(), s_name.c_str());
#endif
			return false;
		}
		pAttr = pAttr->Next();
	}
	return true;
}
#pragma endregion

#pragma region 解析Vector
bool XMLParser::ParserVector(TiXmlNode* pNode, TiXmlElement* pElement)
{
	const char* pch = 0;
	if (!pElement)
		return false;

	stVector tt_vector;

	pch = pElement->Attribute("name");
	if (!pch) {
		printf("vector has no name\n");
		return false;
	}

	tt_vector._name.append(pch);

	pch = pElement->Attribute("oldname");
	if (pch)
		tt_vector._oldname.append(pch);

	pch = pElement->Attribute("comment");
	if (pch)
		tt_vector._comment.append(pch);

	pch = pElement->Attribute("element");
	if (!pch) {
		printf("%s has no element\n", tt_vector._name.c_str());
		return false;
	}


	tt_vector._ParTName.append(pch);
	tt_vector._index = m_nStructVecCount++;

	//检测是否是有效的类型
	if (!IsValidType(tt_vector._ParTName)) {
#ifdef _DEBUG
		_Logger.Logf(eLogTypeDebug, "ParserVector 无效类型 %s", tt_vector._ParTName.c_str());
#endif
		printf("ParserVector 无效类型 %s\n", tt_vector._ParTName.c_str());
		return false;
	}
	TestChangeInt64ToLongLong(tt_vector._ParTName);

	//if(tt_vector._ParTName != "string")
	//{
	tt_vector._bBaseParT = IsBaseType(tt_vector._ParTName);
	//}



	////////////增加版本
	pch = pElement->Attribute("vertion");
	if (pch) {
		tt_vector._version = m_VersionManger.GetVersion(pch);
	}
	else {
		pch = pElement->Attribute("version");
		if (pch) {
			tt_vector._version = m_VersionManger.GetVersion(pch);
		}
		else {
			//没有取默认
			tt_vector._version = m_VersionManger._defaultVersion;
		}
	}



	m_NamespaceVec[0]._VectorVec.push_back(tt_vector);
	if (tt_vector._oldname.length() > 0) {
		tt_vector._name = tt_vector._oldname;
		m_NamespaceVec[0]._VectorVec.push_back(tt_vector);
	}

	return true;

}

bool XMLParser::ParserWJSVector(TiXmlNode* pNode, TiXmlElement* pElement)
{
	const char* pch = 0;
	if (!pElement)
		return false;

	stWJSVector tt_WJSVector;

	pch = pElement->Attribute("name");
	if (!pch) {
		printf("wjsvector has no name\n");
		return false;
	}

	tt_WJSVector._name.append(pch);

	pch = pElement->Attribute("element");
	if (!pch) {
		printf("%s has no element\n", tt_WJSVector._name.c_str());
		return false;
	}


	tt_WJSVector._ParTName.append(pch);
	tt_WJSVector._index = m_nStructVecCount++;

	//检测是否是有效的类型
	if (!IsValidType(tt_WJSVector._ParTName)) {
#ifdef _DEBUG
		_Logger.Logf(eLogTypeDebug, "ParserWJSVector 无效类型 %s", tt_WJSVector._ParTName.c_str());
#endif
		printf("ParserWJSVector 无效类型 %s\n", tt_WJSVector._ParTName.c_str());
		return false;
	}
	TestChangeInt64ToLongLong(tt_WJSVector._ParTName);


	tt_WJSVector._bBaseParT = IsBaseType(tt_WJSVector._ParTName);

	////////////增加版本
	pch = pElement->Attribute("vertion");
	if (pch) {
		tt_WJSVector._version = m_VersionManger.GetVersion(pch);
	}
	else {
		pch = pElement->Attribute("version");
		if (pch) {
			tt_WJSVector._version = m_VersionManger.GetVersion(pch);
		}
		else {
			//没有取默认
			tt_WJSVector._version = m_VersionManger._defaultVersion;
		}
	}


	m_NamespaceVec[0]._WJSVectorVec.push_back(tt_WJSVector);

	return true;
}
#pragma endregion

#pragma region 解析类
bool XMLParser::ParserClass(TiXmlNode* pNode, TiXmlElement* pElement, bool brequest)
{
	const char* pch = 0;
	if (!pElement)
		return false;

	stClass tt_class;
	pch = pElement->Attribute("name");
	if (!pch) {
		printf("class has no name\n");
		return false;
	}

	tt_class._name.append(pch);
	if (!IsAllAttributeRecongnize_Class(pElement, tt_class._name))
		return false;

	pch = pElement->Attribute("FuncNew");
	if (pch) {
		tt_class._bFuncAllUseNew = true;
	}

	string tt_part = "";
	pch = pElement->Attribute("part");
	if (pch) {
		tt_part = pch;
	}

	if (brequest) {
		tt_class._strtype = "request";
	}
	else {
		tt_class._strtype = "callback";
	}

	if (brequest) {
		tt_class._servername = tt_class._name;
		tt_class._servername.append("callback");

		tt_class._clientname = tt_class._name;
		tt_class._clientname.append("send");
	}
	else {
		tt_class._servername = tt_class._name;
		tt_class._servername.append("send");

		tt_class._clientname = tt_class._name;
		tt_class._clientname.append("callback");
	}

	if (!ParserClassMethod(pNode, pElement, tt_class, tt_part))
		return false;


	if (tt_part.length() > 0) {
		m_bUsePartProtocol = true;
		//确认是否将此区下的函数都置为丢弃
		if (!TestPartIsOk(tt_part)) {
			for (auto& r : tt_class._FuncVec) {
				r._bDiscard = true;
			}
		}

		for (auto& m : m_NamespaceVec[0]._ClassVec) {
			if (m._strtype == tt_class._strtype) {
				for (auto& r : tt_class._FuncVec) {
					m._FuncVec.push_back(r);
				}
				return true;
			}
		}

	}
	m_NamespaceVec[0]._ClassVec.push_back(tt_class);
	return true;
}
bool XMLParser::ParserClassMethod(TiXmlNode* pNode, TiXmlElement* pElement, stClass& rClass, const string& part)
{
	const char* pch = 0;
	if (!pElement)
		return false;

	TiXmlNode* tt_pChild = pNode->FirstChildElement();
	if (!tt_pChild)
		return false;

	TiXmlElement* tt_pElement = tt_pChild->ToElement();
	pch = tt_pElement->Attribute("name");
	if (!pch) {
		printf("%s:method has no name\n", rClass._name.c_str());
		return false;
	}


	/////////////////

	while (tt_pChild) {
		AddToYuanPartList(part);
		stFunc tt_func2;
		tt_func2._part2 = part;

		tt_pElement = tt_pChild->ToElement();
		pch = tt_pElement->Attribute("name");
		if (!pch) {
			printf("%s:method has no name\n", rClass._name.c_str());
			return false;
		}
		tt_func2._name.append(pch);

		if (!IsAllAttributeRecongnize_Func(tt_pElement, tt_func2._name))
			return false;

		pch = tt_pElement->Attribute("comment");
		if (pch) {
			tt_func2._comment.append(pch);
		}

		//if(tt_func2._name == "ClickedByOhterPeople")
		//{
		//	int a;
		//	a=10;
		//}

		pch = tt_pElement->Attribute("vertion");
		if (pch) {
			tt_func2._version = m_VersionManger.GetVersion(pch);
		}
		else {
			pch = tt_pElement->Attribute("version");
			if (pch) {
				tt_func2._version = m_VersionManger.GetVersion(pch);
			}
			else {
				//没有取默认
				tt_func2._version = m_VersionManger._defaultVersion;
			}
		}

		pch = tt_pElement->Attribute("FuncId");
		if (pch) {
			tt_func2._userFuncId = atoi(pch);
			//if(tt_func2._userFuncId>65535)
			//	tt_func2._userFuncId=0;
		}

		pch = tt_pElement->Attribute("New");
		if (pch) {
			tt_func2._bNew = true;
		}
		else if (rClass._bFuncAllUseNew) {
			tt_func2._bNew = true;
		}

		pch = tt_pElement->Attribute("KeepOld");
		if (pch) {
			tt_func2._bNew = false;
		}

		pch = tt_pElement->Attribute("Discard");
		if (pch) {
			tt_func2._bDiscard = true;
		}

		pch = tt_pElement->Attribute("SCFlag");
		if (pch) {
			tt_func2._bSCFlag = true;
		}

		pch = tt_pElement->Attribute("MobaFlag");
		if (pch) {
			tt_func2._bMobaFlag = true;
			if (m_bExceptMoba)
				tt_func2._bDiscard = true;
		}
		pch = tt_pElement->Attribute("Layer");
		if (pch) {
			tt_func2._Layer = atoi(pch);
		}
		pch = tt_pElement->Attribute("SCSplite");
		if (pch) {
			if (strcmp(pch, "s") == 0 || strcmp(pch, "S") == 0)
				tt_func2._SCSpliteType = 1;
			else if (strcmp(pch, "c") == 0 || strcmp(pch, "C") == 0)
				tt_func2._SCSpliteType = -1;
		}


		if (!ParserMethod(tt_pChild, tt_pElement, tt_func2)) {
#ifdef _DEBUG
			_Logger.Log(eLogTypeDebug, "ParserMethod failed");
#endif
			printf("ParserMethod failed\n");
			return false;
		}

		rClass._FuncVec.push_back(tt_func2);

		tt_pChild = tt_pChild->NextSiblingElement();
		if (!tt_pChild) //结束
			break;
	}

	return true;
}
bool XMLParser::ParserMethod(TiXmlNode* pNode, TiXmlElement* pElement, stFunc& rFunc)
{
	const char* pch = 0;
	if (!pElement)
		return false;

	TiXmlNode* tt_pChild = pNode->FirstChildElement();
	if (!tt_pChild)
		return true; //没有子节点，说明是无参方法

					 //stBasePar tt_basepar1;
	TiXmlElement* tt_pElement = tt_pChild->ToElement();
	pch = tt_pElement->Attribute("name");
	if (!pch) {
		printf("%s par has no", rFunc._name.c_str());
		return false;
	}

	while (tt_pChild) {

		stBasePar tt_basepar2;

		tt_pElement = tt_pChild->ToElement();
		pch = tt_pElement->Attribute("name");
		if (!pch) {
			printf("%s par has no", rFunc._name.c_str());
			return false;
		}
		tt_basepar2._strParName.append(pch);
		if (!IsAllAttributeRecongnize_FuncPar(tt_pElement, rFunc._name, tt_basepar2._strParName))
			return false;

		pch = tt_pElement->Attribute("type");
		if (!pch) {
			printf("%s:%s has no type", rFunc._name.c_str(), tt_basepar2._strParName.c_str());
			return false;
		}

		tt_basepar2._strParType.append(pch);

		pch = tt_pElement->Attribute("comment");
		if (pch) {
			tt_basepar2._comment.append(pch);
		}

		//检测是否是有效的类型
		if (!IsValidType(tt_basepar2._strParType)) {
#ifdef _DEBUG
			_Logger.Logf(eLogTypeDebug, "ParserMethod 无效类型 %s", tt_basepar2._strParType.c_str());
#endif
			printf("ParserMethod 无效类型 %s\n", tt_basepar2._strParType.c_str());
			return false;
		}
		TestChangeInt64ToLongLong(tt_basepar2._strParType);

		TestBaseParIsConstRef(tt_basepar2);

		////////////增加版本
		pch = tt_pElement->Attribute("vertion");
		if (pch) {
			tt_basepar2._version = m_VersionManger.GetVersion(pch);
		}
		else {
			pch = tt_pElement->Attribute("version");
			if (pch) {
				tt_basepar2._version = m_VersionManger.GetVersion(pch);
			}
			else {
				//没有取默认
				tt_basepar2._version = m_VersionManger._defaultVersion;
			}
		}



		rFunc._InParVec.push_back(tt_basepar2);

		tt_pChild = tt_pChild->NextSiblingElement();
		if (!tt_pChild)
			break;

	}

	if (!rFunc.VersionIsOk()) {
		printf("%s  version error", rFunc._name.c_str());
		return false;
	}

	return true;
}

bool XMLParser::IsAllAttributeRecongnize_Class(TiXmlElement* pElement, const string& className)
{
	if (!pElement)
		return false;
	static std::set<std::string> s_sets;
	if (s_sets.size() == 0) {
		s_sets.insert("name");
		s_sets.insert("comment");
		s_sets.insert("callback");
		s_sets.insert("request");
		s_sets.insert("part");
		s_sets.insert("FuncNew");
	}

	const TiXmlAttribute* pAttr = pElement->FirstAttribute();
	while (pAttr) {
		string s_name = string(pAttr->Name());
		auto iter = s_sets.find(s_name);
		if (iter == s_sets.end()) {
			printf("class[%s]: cantparser attr[%s]\n", className.c_str(), s_name.c_str());
#ifdef _DEBUG
			_Logger.Logf(eLogTypeDebug, "class[%s]: cantparser attr[%s]\n", className.c_str(), s_name.c_str());
#endif
			return false;
		}
		pAttr = pAttr->Next();
	}
	return true;
}


bool XMLParser::IsAllAttributeRecongnize_Func(TiXmlElement* pElement, const string& funcName)
{
	if (!pElement)
		return false;
	static std::set<std::string> s_sets;
	if (s_sets.size() == 0) {
		s_sets.insert("name");
		s_sets.insert("comment");
		s_sets.insert("vertion");
		s_sets.insert("version");
		s_sets.insert("FuncId");
		s_sets.insert("New");
		s_sets.insert("KeepOld");
		s_sets.insert("Discard");
		s_sets.insert("SCFlag");
		s_sets.insert("MobaFlag");
		s_sets.insert("Layer");
		s_sets.insert("SCSplite");
	}

	const TiXmlAttribute* pAttr = pElement->FirstAttribute();
	while (pAttr) {
		string s_name = string(pAttr->Name());
		auto iter = s_sets.find(s_name);
		if (iter == s_sets.end()) {
			printf("method[%s]: cantparser attr[%s]\n", funcName.c_str(), s_name.c_str());
#ifdef _DEBUG
			_Logger.Logf(eLogTypeDebug, "method[%s]: cantparser attr[%s]\n", funcName.c_str(), s_name.c_str());
#endif
			return false;
		}
		pAttr = pAttr->Next();
	}
	return true;
}

bool XMLParser::IsAllAttributeRecongnize_FuncPar(TiXmlElement* pElement, const string& funcName, const string& parName)
{
	if (!pElement)
		return false;
	static std::set<std::string> s_sets;
	if (s_sets.size() == 0) {
		s_sets.insert("name");
		s_sets.insert("type");
		s_sets.insert("comment");
		s_sets.insert("vertion");
		s_sets.insert("version");
		s_sets.insert("default");
	}

	const TiXmlAttribute* pAttr = pElement->FirstAttribute();
	while (pAttr) {
		string s_name = string(pAttr->Name());
		auto iter = s_sets.find(s_name);
		if (iter == s_sets.end()) {
			printf("method[%s],par[%s]: cantparser attr[%s]\n", funcName.c_str(), parName.c_str(), s_name.c_str());
#ifdef _DEBUG
			_Logger.Logf(eLogTypeDebug, "method[%s],par[%s]: cantparser attr[%s]\n", funcName.c_str(), parName.c_str(), s_name.c_str());
#endif
			return false;
		}
		pAttr = pAttr->Next();
	}
	return true;
}
#pragma endregion


#pragma region 类型判定
bool XMLParser::IsValidType(const string& strtypename)
{
	if (IsBaseType(strtypename))
		return true;

	if (IsExType_enum(strtypename))
		return true;

	if (IsExType_struct(strtypename))
		return true;

	if (IsExType_vector(strtypename))
		return true;

	if (IsExType_WJSVector(strtypename))
		return true;

	return false;
}
bool XMLParser::IsBaseType(const string& strtypename)
{
	for (int i = 0; i < sizeof(g_strBaseType) / sizeof(string); i++) {
		if (g_strBaseType[i].compare(strtypename) == 0)
			return true;
	}

	return false;
}
bool XMLParser::IsExType_enum(const string& strtypename)
{
	if (m_NamespaceVec.size() == 0)
		return false;
	vector<stEnum> pEnum = m_NamespaceVec[m_NamespaceVec.size() - 1]._EnumVec;

	for (auto& r : pEnum) {
		if (r._name.compare(strtypename) == 0)
			return true;
	}

	return false;
}
bool XMLParser::IsExType_struct(const string& strtypename)
{
	if (m_NamespaceVec.size() == 0)
		return false;
	vector<stStructStruct> pstruct = m_NamespaceVec[m_NamespaceVec.size() - 1]._StructVec;
	for (auto& r : pstruct) {
		if (r._name.compare(strtypename) == 0)
			return true;
	}

	return false;
}
bool XMLParser::IsExType_vector(const string& strtypename)
{
	if (m_NamespaceVec.size() == 0)
		return false;
	vector<stVector> pVector = m_NamespaceVec[m_NamespaceVec.size() - 1]._VectorVec;

	for (auto& r : pVector) {
		if (r._name.compare(strtypename) == 0)
			return true;
	}

	return false;
}
bool XMLParser::IsExType_WJSVector(const string& strtypename) //是否是另外版本的类vector型
{
	if (m_NamespaceVec.size() == 0)
		return false;
	vector<stWJSVector> pVector = m_NamespaceVec[m_NamespaceVec.size() - 1]._WJSVectorVec;

	for (auto& r : pVector) {
		if (r._name.compare(strtypename) == 0)
			return true;
	}

	return false;
}
#pragma endregion

#pragma region 测试区
bool    XMLParser::TestPartIsOk(const string& strPart)
{
	//本身非区
	if (strPart.length() == 0)
		return true;

	//使用全区方案
	if (m_UsePartManager.length() == 0)
		return true;

	auto iter = m_PartManageMap.find(m_UsePartManager);
	if (iter != m_PartManageMap.end()) {
		if (iter->second.find(strPart) != iter->second.end())
			return true;
	}

	return false;
}
void    XMLParser::TestChangeInt64ToLongLong(string& strOldType)
{
	if (m_bAndroidPeer) {
		if (strOldType.compare("__int64") == 0) {
			strOldType = "long long";
		}
	}

}
void   XMLParser::TestBaseParIsConstRef(stBasePar& pPar)
{
	//增加string类型，也使用constref
	if (pPar._strParType.compare("string") == 0) {
		pPar._IsConstRef = true;
		return;
	}
	//不是基础类型，也不是枚举
	if (!IsBaseType(pPar._strParType) && (!IsExType_enum(pPar._strParType))) {
		pPar._IsConstRef = true;
	}
}
#pragma endregion


#pragma region 工具型函数
void XMLParser::ToLower(string &str)
{
	for (size_t i = 0; i < str.size(); i++) {
		char c = str.at(i);
		if (c >= 'A'&&c <= 'Z') {
			c = tolower(c);
			str[i] = c;
		}
	}
}
string XMLParser::ToLowerV2(string &str)
{
	for (size_t i = 0; i < str.size(); i++) {
		char c = str.at(i);
		if (c >= 'A'&&c <= 'Z') {
			c = tolower(c);
			str[i] = c;
		}
	}
	return str;
}
string XMLParser::GetUTF8(LPCTSTR lpInput)
{
	string t_str;
	UTF8Encode(lpInput,t_str);
	return t_str;
}
void XMLParser::UTF8Encode(LPCTSTR lpInput, std::string &strOutput)
{
	int				nOut;
	const WCHAR*	pInput;
	char*			pTest;

	if ((0 == lpInput) || (0 == _tcslen(lpInput))) {
		strOutput.clear();
		return;
	}


	WCHAR*	pBuffer;
	//多字节到宽字节需要空间
	nOut = MultiByteToWideChar(CP_ACP, 0, lpInput, -1, NULL, 0);
	nOut++;

	pBuffer = new WCHAR[nOut];
	MultiByteToWideChar(CP_ACP, 0, lpInput, -1, pBuffer, nOut);
	pInput = pBuffer;


	//宽字节变化到多字节
	nOut = WideCharToMultiByte(CP_UTF8, 0, pInput, -1, NULL, 0, NULL, NULL);
	nOut++;
	pTest = new char[nOut];
	WideCharToMultiByte(CP_UTF8, 0, pInput, -1, pTest, nOut, NULL, NULL);

	strOutput = pTest;

	delete[]pTest;
	delete[]pBuffer;
}

//UTF8解码
void XMLParser::UTF8Decode(const char *pInput, string &strOutput)
{
	int			nOut;
	WCHAR*		pBuffer;

	if ((0 == pInput) || (0 == strlen(pInput))) {
		strOutput.clear();
		return;
	}

	//多字节转宽字节，需要多少空间
	nOut = MultiByteToWideChar(CP_UTF8, 0, pInput, -1, NULL, 0);
	nOut++;

	pBuffer = new WCHAR[nOut];
	MultiByteToWideChar(CP_UTF8, 0, pInput, -1, pBuffer, nOut);


	//再将宽字节转多字节
	char*		pTest;
	nOut = WideCharToMultiByte(CP_ACP, 0, pBuffer, -1, NULL, 0, NULL, NULL);
	nOut++;

	pTest = new char[nOut];
	nOut = WideCharToMultiByte(CP_ACP, 0, pBuffer, -1, pTest, nOut, NULL, NULL);

	strOutput = pTest;
	delete[]pTest;
	delete[]pBuffer;
}
int    XMLParser::GetBaseTypeNum()
{
	return sizeof(g_strBaseType) / sizeof(string);
}
void   XMLParser::AddToPartManage(const string& partManageName, const string& partName, const string& comment)
{
	m_PartManageMap[partManageName].insert(partName);


	for (auto& r : m_PartManageList) {
		if (r._name == partManageName) {
			r._partList.push_back(partName);
			return;
		}
	}

	stPartManager t;
	t._name = partManageName;
	t._comment = comment;
	t._partList.push_back(partName);
	m_PartManageList.push_back(t);

}

void   XMLParser::AddToYuanPartList(const string& partName)
{
	if (partName.length() == 0)
		return;

	for (auto& r : m_YuanPartList) {
		if (r == partName)
			return;
	}

	m_YuanPartList.push_back(partName);

}
#pragma region

bool XMLParser::IsBaseType_WriterType(const string& strTypeName, eProtocolWriterType writerType)
{
	if (writerType == eProtocolWriterType::IPhone) {
		for (int i = 0; i < GetBaseTypeNum(); i++) {
			if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
				return true;

			}
		}
	}
	else if (writerType == eProtocolWriterType::Java) {
		for (int i = 0; i < GetBaseTypeNum(); i++) {
			if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
				return true;
			}
		}
	}
	else if (writerType == eProtocolWriterType::Lua) {
		for (int i = 0; i < GetBaseTypeNum(); i++) {
			if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
				return true;
			}
		}
	}
	else if (writerType == eProtocolWriterType::Python) {
		for (int i = 0; i < GetBaseTypeNum(); i++) {
			if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
				return true;
			}
		}
	}
	else if (writerType == eProtocolWriterType::Flash) {
		for (int i = 0; i < GetBaseTypeNum(); i++) {
			if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
				return true;

			}
		}
	}
	else if (writerType == eProtocolWriterType::CSharp) {
		for (int i = 0; i < GetBaseTypeNum(); i++) {
			if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
				return true;
			}
		}
	}

	return false;
}

string  XMLParser::GetBaseTypeDefault(const std::string& typeName)
{
	auto iter = m_baseTypeDefaultMap.find(typeName);
	if (iter != m_baseTypeDefaultMap.end())
		return  iter->second;

	return "";
}