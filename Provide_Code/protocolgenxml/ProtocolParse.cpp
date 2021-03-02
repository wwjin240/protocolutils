//#ifdef TIXML_USE_STL
//#include <iostream>
//#include <sstream>
//using namespace std;
//#else
//#include <stdio.h>
//#endif

//#if defined( WIN32 ) && defined( TUNE )
//#include <crtdbg.h>
//_CrtMemState startMemState;
//_CrtMemState endMemState;
//#endif

#include "ProtocolParse.h"
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
//#include "../tinyxml/tinyxml.h"

#include "md5.h"
#include "CCreateComment.h"
#include <set>


extern string g_ProgressId;
extern bool g_bCreateCopy;
extern bool g_bCreateCopy2;
extern string g_strBaseType[];
extern vector<string> g_AllTempFilesList;

ProtocolParse::ProtocolParse(const string& strPathName)
	:ProtocolParse_Writer(strPathName, eProtocolWriterType::eNone)
	,m_bOnlyParseSelf(false)
{
	m_bOnlyParseSelf = true;
}

ProtocolParse::~ProtocolParse()
{

}

void  ProtocolParse::RefProtocolProcess()
{
	if (m_bOnlyParseSelf)
		return;
	ProtocolParse_Writer::RefProtocolProcess();
}

bool ProtocolParse::IsValidType(const string& strtypename)
{
	if (m_bOnlyParseSelf)
		return true;

	return XMLParser::IsValidType(strtypename);
}

void    ProtocolParse::WriterToXML(const string& saveFileName)
{
	TiXmlDocument* pDocument = nullptr;
	TiXmlDeclaration* pDeclaration = nullptr;

	//先创建Document
	pDocument = new TiXmlDocument();
	if (!pDocument) 
		return;


	pDeclaration = new TiXmlDeclaration("1.0", "UTF-8", "");
	if (!pDeclaration)
		return;

	pDocument->LinkEndChild(pDeclaration);

	//创建根节点
	TiXmlElement *pRoot = new TiXmlElement("protocol");
	if (!pRoot)
		return;
	if (m_NamespaceVec.size() > 0) {
		pRoot->SetAttribute("name", m_NamespaceVec[0]._protocolname);		
	}
	else {
		pRoot->SetAttribute("name", "GateWay2Client");
	}
	

	//关联XML文档，成为XML文档的根节点
	pDocument->LinkEndChild(pRoot);

	char t_ch64[64];
	
	//创建孩子节点
	TiXmlElement* pProtocolNo = new TiXmlElement("protocolNo");
	if (pProtocolNo) {
		pProtocolNo->SetAttribute("name", m_ProtocolNo._name);	
		sprintf_s(t_ch64, 64, "%d", m_ProtocolNo._value);
		pProtocolNo->SetAttribute("value", t_ch64);
		pRoot->LinkEndChild(pProtocolNo);
	}

	if (m_NamespaceVec.size() > 0) {
		auto& r = m_NamespaceVec[0];
		TiXmlElement* pNamespace = new TiXmlElement("namespacename");
		if (pNamespace) {
			pNamespace->SetAttribute("value1", r._servername);
			pNamespace->SetAttribute("value2", r._clientname);
			if (m_bHCPP)
				pNamespace->SetAttribute("value3", "hcpp");
			if (r._shortname.length() > 0)
				pNamespace->SetAttribute("shortname", r._shortname);
			pRoot->LinkEndChild(pNamespace);
		}
	}


	for (size_t i = 0; i < m_RefProtocolList.size(); i++) {
		TiXmlElement* pProtocolRef = new TiXmlElement("protocolRef");
		if (pProtocolRef) {
			auto& r = m_RefProtocolList[i];
			if (r._refProtocolName.length() > 0) 
				pProtocolRef->SetAttribute("value1", r._refProtocolName);

			if (r._includeheadnames.length() > 0) 
				pProtocolRef->SetAttribute("value2", r._includeheadnames);

			if (r._includeheadnamec.length() > 0) 
				pProtocolRef->SetAttribute("value3", r._includeheadnamec);

			if (r._usingspacename.length() > 0) 
				pProtocolRef->SetAttribute("value4", r._usingspacename);


			if(r._bNotCreateFile)
				pProtocolRef->SetAttribute("NotCreateFile", "1");

			if (r._strPath.length()>0)
				pProtocolRef->SetAttribute("path", r._strPath);

			pRoot->LinkEndChild(pProtocolRef);
		}
	}
	TiXmlComment* pComment = nullptr;
	pComment = new TiXmlComment(GetUTF8("协议版本信息").c_str());
	if (pComment)
		pRoot->LinkEndChild(pComment);

	
	TiXmlElement* pVManger = new TiXmlElement("VManger");
	if (pVManger) {
		pVManger->SetAttribute("name", "Version");
		pRoot->LinkEndChild(pVManger);

		pComment = new TiXmlComment(GetUTF8("默认的版本号").c_str());
		if (pComment) 
			pVManger->LinkEndChild(pComment);
		
		TiXmlElement *pVMangerVersion = new TiXmlElement("member");
		if (pVMangerVersion) {
			pVMangerVersion->SetAttribute("name","defversion");
			sprintf_s(t_ch64, 64, "%d", m_VersionManger._defaultVersion);
			pVMangerVersion->SetAttribute("value", t_ch64);
			pVManger->LinkEndChild(pVMangerVersion);
		}
		pComment = new TiXmlComment(GetUTF8("参数版本号,每次增加版本号，都需要增加新的，当确认合并时，只需要将桌的都指为同一值，默认值即可").c_str());
		if (pComment)
			pVManger->LinkEndChild(pComment);

		for (size_t i = 0; i < m_VersionManger._parVersionList.size(); i++) {
			auto& r = m_VersionManger._parVersionList[i];
			pVMangerVersion = new TiXmlElement("member");
			if (pVMangerVersion) {
				pVMangerVersion->SetAttribute("name", r._parVerName);
				sprintf_s(t_ch64, 64, "%d", r._parVerValue);
				pVMangerVersion->SetAttribute("value", t_ch64);
				pVManger->LinkEndChild(pVMangerVersion);
			}
		}
	}

	for (size_t i = 0; i < m_PartManageList.size(); i++) {
		WriterXMLComment(pRoot, m_PartManageList[i]._comment);
		TiXmlElement *pPartManager = new TiXmlElement("PartManager");
		if (pPartManager) {
			pPartManager->SetAttribute("name", m_PartManageList[i]._name);
			pRoot->LinkEndChild(pPartManager);
		}

		for (size_t j = 0; j < m_PartManageList[i]._partList.size(); j++) {
			TiXmlElement *pPartManager_Part = new TiXmlElement("member");
			if (pPartManager_Part) {
				pPartManager_Part->SetAttribute("name", m_PartManageList[i]._partList[j]);
				pPartManager->LinkEndChild(pPartManager_Part);
			}
		}
	}

	for (size_t i = 0; i < m_NamespaceVec.size(); i++) {
		WriterToXML_Namespace(pRoot,m_NamespaceVec[i]);
	}


	pDocument->SaveFile(saveFileName);
		

}

void    ProtocolParse::WriterToXML_Namespace(TiXmlElement *pRoot, const stNamespace& rNamespace)
{
	for (size_t i = 0; i < rNamespace._EnumVec.size(); i++) {
		WriterToXML_Enum(pRoot, rNamespace._EnumVec[i]);
	}

	int t_maxIndex = rNamespace.GetMaxIndex_StructAndVector();
	for (int i = 0; i <= t_maxIndex; i++) {
		WriterToXML_StructVectorEnter(pRoot,rNamespace,i);
	}

	////结构体与列表
	//vector<stClass>         _ClassVec;

	if (m_YuanPartList.size() > 0) {
		for (size_t i = 0; i < m_YuanPartList.size(); i++) {
			for (size_t j = 0; j < rNamespace._ClassVec.size(); j++) {
				if (rNamespace._ClassVec[j]._strtype == "request") {
					WriterToXML_ClassPart(pRoot, rNamespace._ClassVec[j], m_YuanPartList[i]);
				}
			}
			for (size_t j = 0; j < rNamespace._ClassVec.size(); j++) {
				if (rNamespace._ClassVec[j]._strtype == "callback") {
					WriterToXML_ClassPart(pRoot, rNamespace._ClassVec[j], m_YuanPartList[i]);
				}
			}
		}
	}
	else {
		for (size_t j = 0; j < rNamespace._ClassVec.size(); j++) {
			if (rNamespace._ClassVec[j]._strtype == "request") {
				WriterToXML_ClassPart(pRoot, rNamespace._ClassVec[j], "");
			}
		}
		for (size_t j = 0; j < rNamespace._ClassVec.size(); j++) {
			if (rNamespace._ClassVec[j]._strtype == "callback") {
				WriterToXML_ClassPart(pRoot, rNamespace._ClassVec[j], "");
			}
		}
	}
}

void    ProtocolParse::WriterToXML_Enum(TiXmlElement* pRoot, const stEnum& rEnum)
{
	WriterXMLComment(pRoot, rEnum._comment);

	TiXmlElement* pEnum = new TiXmlElement("enum");
	if (pEnum) {
		pEnum->SetAttribute("name", rEnum._name);
		int t_maxVersion= rEnum.GetMaxVersion();
		if (t_maxVersion > m_VersionManger._defaultVersion) {
			string t_version = m_VersionManger.GetVersionDesc(t_maxVersion);
			if (t_version.length() > 0)
				pEnum->SetAttribute("version", t_version);
		}

		if(rEnum._bNewVersion)
			pEnum->SetAttribute("New", "1");
		
		pRoot->LinkEndChild(pEnum);

		for (size_t i = 0; i < rEnum._ParNameList.size(); i++) {
			WriterXMLComment(pEnum, rEnum._commentList[i]);
			TiXmlElement* pItem = new TiXmlElement("member");
			if (pItem) {
				pItem->SetAttribute("name", rEnum._ParNameList[i]);
				if (rEnum._versionList[i] > m_VersionManger._defaultVersion) {
					string t_version = m_VersionManger.GetVersionDesc(rEnum._versionList[i]);
					if (t_version.length() > 0) {
						pItem->SetAttribute("version", t_version);
					}
				}


				if (rEnum._keepOldList[i])
					pItem->SetAttribute("KeepOld", "1");

				pEnum->LinkEndChild(pItem);
			}
		}


	}
}
void    ProtocolParse::WriterToXML_StructVectorEnter(TiXmlElement* pRoot, const stNamespace& rNamespace, int index)
{
	for (size_t i = 0; i < rNamespace._StructVec.size(); i++) {
		if (rNamespace._StructVec[i]._index == index) {
			WriterToXML_Struct(pRoot, rNamespace._StructVec[i]);
			return;
		}	
	}

	for (size_t i = 0; i < rNamespace._VectorVec.size(); i++) {
		if (rNamespace._VectorVec[i]._index == index) {
			WriterToXML_Vector(pRoot, rNamespace._VectorVec[i]);
			return;
		}
	}

	for (size_t i = 0; i < rNamespace._WJSVectorVec.size(); i++) {
		if (rNamespace._WJSVectorVec[i]._index == index) {
			WriterToXML_WJSVector(pRoot, rNamespace._WJSVectorVec[i]);
			return;
		}
	}
}
void    ProtocolParse::WriterToXML_Struct(TiXmlElement* pRoot, const stStructStruct& rStruct)
{
	WriterXMLComment(pRoot, rStruct._comment);
	TiXmlElement* pStruct = new TiXmlElement("struct");
	if (pStruct) {
		//int                _index;//序号

		//string             _comment;//注释
		//int                _transNeedValue; //传输需要值
		//bool   _bBiaoZhuUsed;    //标注使用
		pStruct->SetAttribute("name", rStruct._name);
		if (rStruct._version > m_VersionManger._defaultVersion) {
			string t_version = m_VersionManger.GetVersionDesc(rStruct._version);
			if (t_version.length() > 0)
				pStruct->SetAttribute("version",t_version);
		}
		if (rStruct._bWithConstruct) {
			pStruct->SetAttribute("construct", "1");
		}
		
		if (rStruct._startversion_ts > m_VersionManger._defaultVersion) {
			string t_version_ts = m_VersionManger.GetVersionDesc(rStruct._startversion_ts);
			if (t_version_ts.length() > 0)
				pStruct->SetAttribute("startversion_ts", t_version_ts);
		}
		if (rStruct._oldname.length() > 0) {
			pStruct->SetAttribute("oldname", rStruct._oldname);
		}
		pRoot->LinkEndChild(pStruct);

		for (size_t i = 0; i < rStruct._ParVec.size(); i++) {
			auto& r = rStruct._ParVec[i];
			WriterXMLComment(pStruct, r._comment);
			TiXmlElement* pItem = new TiXmlElement("member");
			if (pItem) {
				pItem->SetAttribute("name",r._strParName);
				if (r._bWithDefault && r._strDefaultValue.length()>0) {
					pItem->SetAttribute("default", r._strDefaultValue);
				}
				pItem->SetAttribute("type", r._strParType);
				if (r._transNeedValue != -1) {
					char t_ch[64];
					sprintf_s(t_ch, 64, "%d", r._transNeedValue);
					pItem->SetAttribute("transneedvalue", t_ch);
				}
				if (r._version > m_VersionManger._defaultVersion) {
					string t_version = m_VersionManger.GetVersionDesc(r._version);
					if (t_version.length() > 0)
						pItem->SetAttribute("version", t_version);
				}
				pStruct->LinkEndChild(pItem);
			}

			//bool   _IsConstRef; //是否是constref
			//string _comment;    //注释

		}
	}
}

void   ProtocolParse::WriterToXML_Vector(TiXmlElement* pRoot, const stVector& rVector)
{
	WriterXMLComment(pRoot,rVector._comment);

	TiXmlElement* pVector = new TiXmlElement("sequence");
	if (pVector) {
		pVector->SetAttribute("name",rVector._name);
		pVector->SetAttribute("element", rVector._ParTName);

		if(rVector._oldname.length()>0)
			pVector->SetAttribute("oldname", rVector._oldname);
		pRoot->LinkEndChild(pVector);
	}
}
void   ProtocolParse::WriterToXML_WJSVector(TiXmlElement* pRoot, const stWJSVector& rWJSVector)
{
	TiXmlElement* pVector = new TiXmlElement("sequence2");
	if (pVector) {
		pVector->SetAttribute("name", rWJSVector._name);
		pVector->SetAttribute("element", rWJSVector._ParTName);

		pRoot->LinkEndChild(pVector);
	}
}

void    ProtocolParse::WriterToXML_ClassPart(TiXmlElement* pRoot, const stClass& rClass, const string& partName)
{
	if (partName.length() > 0) {
		vector<string> t_funcNameList;
		for (size_t i = 0; i < rClass._FuncVec.size(); i++) {
			if (rClass._FuncVec[i]._part2 == partName) {
				t_funcNameList.push_back(rClass._FuncVec[i]._name);
			}
		}
		if (t_funcNameList.size() == 0)
			return;
	}

	TiXmlElement* pClass = new TiXmlElement(rClass._strtype);
	if (pClass) {
		pClass->SetAttribute("name",rClass._name);
		pClass->SetAttribute("part", partName);
		pClass->SetAttribute("FuncNew", "1");
		pRoot->LinkEndChild(pClass);

		if (partName.length() > 0) {
			for (size_t i = 0; i < rClass._FuncVec.size(); i++) {
				if (rClass._FuncVec[i]._part2 == partName) {
					WriterToXML_Func(pClass, rClass._FuncVec[i], rClass._bFuncAllUseNew);
				}
			}
		}
		else {
			for (size_t i = 0; i < rClass._FuncVec.size(); i++) {
				WriterToXML_Func(pClass, rClass._FuncVec[i], rClass._bFuncAllUseNew);
			}
		}

	}


	

}

void   ProtocolParse::WriterToXML_Func(TiXmlElement* pClass, const stFunc& rFunc,bool bFuncAllUseNew)
{
	WriterXMLComment(pClass, rFunc._comment);

	TiXmlElement* pFunc = new TiXmlElement("method");
	if (pFunc) {
		char t_ch[64];
		pFunc->SetAttribute("name", rFunc._name);
		if (rFunc._version > m_VersionManger._defaultVersion) {
			string t_version = m_VersionManger.GetVersionDesc(rFunc._version);
			if (t_version.length() > 0) {
				pFunc->SetAttribute("version", t_version);
			}
			if (rFunc._userFuncId != 0) {
				sprintf_s(t_ch, 64, "%d", rFunc._userFuncId);
				pFunc->SetAttribute("FuncId", t_ch);
			}
			if (!bFuncAllUseNew && rFunc._bNew) {
				pFunc->SetAttribute("New", "1");
			}
			if (rFunc._bDiscard) {
				pFunc->SetAttribute("Discard", "1");
			}
			if (rFunc._bSCFlag) {
				pFunc->SetAttribute("SCFlag", "1");
			}
			if (rFunc._bMobaFlag) {
				pFunc->SetAttribute("MobaFlag", "1");
			}
			if (rFunc._Layer != 0) {
				sprintf_s(t_ch, 64, "%d", rFunc._Layer);
				pFunc->SetAttribute("Layer", t_ch);
			}
			if (rFunc._SCSpliteType == 1) {
				pFunc->SetAttribute("SCSplite", "S");
			}
			else if (rFunc._SCSpliteType == -1) {
				pFunc->SetAttribute("SCSplite", "C");
			}

			if (!rFunc._bNew && bFuncAllUseNew) {
				pFunc->SetAttribute("KeepOld", "1");
			}
		}

		//vector<stBasePar>  _InParVec;//输入参数列表
		//string             _comment;//注释
		//string _strSCClassName; //用于强制SC模式
		//string    _part2;          //子部分又叫XX区

		//int    _computedFuncId; //计算过后的函数id
		pClass->LinkEndChild(pFunc);

		if (m_func_IsSkipFuncPars) {
			if (m_func_IsSkipFuncPars())
				return;
		}
		WriterToXML_FuncPars(pFunc,rFunc);
	}
}

void    ProtocolParse::WriterToXML_FuncPars(TiXmlElement* pFunc, const stFunc& rFunc)
{
	for (size_t i = 0; i < rFunc._InParVec.size(); i++) {
		auto& r = rFunc._InParVec[i];

		WriterXMLComment(pFunc, r._comment);
		TiXmlElement* pItem = new TiXmlElement("param");
		if (pItem) {
			pItem->SetAttribute("name",r._strParName);
			pItem->SetAttribute("type", r._strParType);
			if (r._version > m_VersionManger._defaultVersion) {
				string t_version = m_VersionManger.GetVersionDesc(r._version);
				if (t_version.length() > 0) {
					pItem->SetAttribute("version",t_version);
				}
			}

			if (r._bWithDefault && r._strDefaultValue.length()>0) {
				pItem->SetAttribute("default", r._strDefaultValue);
			}

			//bool   _IsConstRef; //是否是constref
			//string _comment;    //注释

			//int    _transNeedValue; //传输需要值
			pFunc->LinkEndChild(pItem);
		}
	}
}

void    ProtocolParse::WriterXMLComment(TiXmlElement* pElement, const string& comment)
{
	if (!pElement)
		return;
	if (comment.length() == 0)
		return;

	vector<string> tt_subList;
	ProtocolUtils::GetAllSubString(comment, "split", tt_subList, false);
	TiXmlComment* pComment = nullptr;
	for (size_t i = 0; i < tt_subList.size(); i++) {
		if(tt_subList[i].length()==0)
			continue;

		//全角“号换成^^
		//反过来
		ProtocolUtils::Replace(tt_subList[i], "^^", "“");
		ProtocolUtils::Replace(tt_subList[i], "^", "\"");

		pComment = new TiXmlComment(tt_subList[i].c_str());
		if (pComment)
			pElement->LinkEndChild(pComment);

	}


}


void     ProtocolParse::CompareProtocol(ProtocolParse* parserB)
{
	vector<string> t_addFuncList;      //新加函数列表
	vector<string> t_addParsFuncList; //新加参数函数列表


}