#include "ProtocolParse_Writer_TSMiddle.h"
#include "ProtocolParse.h"
#include <time.h>
#include <sys/timeb.h>

extern string g_strBaseType[];

ProtocolParse_Writer_TSMiddle::ProtocolParse_Writer_TSMiddle(const string& strPathName)
	:ProtocolParse_Writer(strPathName, eProtocolWriterType::TSMiddle)
{

}
ProtocolParse_Writer_TSMiddle::~ProtocolParse_Writer_TSMiddle()
{

}

bool    ProtocolParse_Writer_TSMiddle::CreateFile_Path(string path, int filetype, bool brefprotocol)
{
	return CreateFile_Path_TScript_Middle(path,filetype,brefprotocol);
}
ProtocolParse_Writer* ProtocolParse_Writer_TSMiddle::Create_RefProtocolParse_Writer(const string& strPathName)
{
	return new ProtocolParse_Writer_TSMiddle(strPathName);
}

bool    ProtocolParse_Writer_TSMiddle::CreateFile_Path_TScript_Middle(string path, int filetype, bool brefprotocol, bool bSaveToFile)
{
	//按协议名来建文建夹
	if (m_NamespaceVec.size() > 0) {

		char tt_ch[256];
		m_SaveFileName._servername = m_NamespaceVec[0]._servername;
		sprintf(tt_ch, "%s.json", m_NamespaceVec[0]._servername.c_str());
		m_SaveFileName._servername2 = tt_ch;

		m_SaveFileName._clientname = m_NamespaceVec[0]._servername;
		sprintf(tt_ch, "%s.json", m_NamespaceVec[0]._servername.c_str());
		m_SaveFileName._clientname2 = tt_ch;


	}

	//StartBiaoZhuUsed();
	//if (!m_bTScriptUseNameSpaceVersion)
	//{
	//	for (int i = 0; i < m_pRefProtocolParseList.size(); i++)
	//		m_pRefProtocolParseList[i]->StartBiaoZhuUsed();
	//}

	for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
		//主要为了使得id不出现重复
		if (m_pRefProtocolParseList[i]) {
			m_pRefProtocolParseList[i]->m_pCheckParUsed = this;
			m_pRefProtocolParseList[i]->CreateFile_Path(path, filetype, true);
			m_pRefProtocolParseList[i]->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
		}
	}

	string str2 = path;
	str2.append(m_SaveFileName._servername2);
	return CreateFile_TScript_Middle(str2, filetype, brefprotocol);
}

void    ProtocolParse_Writer_TSMiddle::PreComputeAllFuncId(stNamespace& pNamespace)
{
	if (pNamespace._ClassVec.size() == 0)
		return;

	char t_ch[512];

	//函数id
	for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
		for (size_t j = 0; j < pNamespace._ClassVec[i]._FuncVec.size(); j++) {
			sprintf(t_ch, "ID_%s_%s", pNamespace._ClassVec[i]._name.c_str(), pNamespace._ClassVec[i]._FuncVec[j]._name.c_str());
			int tempId;  //将字符串转成id
			if (pNamespace._ClassVec[i]._FuncVec[j]._userFuncId != 0)
				tempId = pNamespace._ClassVec[i]._FuncVec[j]._userFuncId;
			else
				tempId = CovertStringToInt(t_ch, pNamespace._ClassVec[i]._FuncVec[j]._bNew);
			pNamespace._ClassVec[i]._FuncVec[j]._computedFuncId = tempId;
		}
	}
}
void    ProtocolParse_Writer_TSMiddle::PreComputeAllEnumId(stNamespace& pNamespace)
{
	for (size_t i = 0; i < pNamespace._EnumVec.size(); i++) {
		PreComputeAllEnumId_(pNamespace._EnumVec[i]);
	}
}
void    ProtocolParse_Writer_TSMiddle::PreComputeAllEnumId_(stEnum& pEnum)
{
	for (int i = 0; i < (int)pEnum._ParNameList.size(); i++) {
		//如果已经算过
		if (pEnum._computedIdList[i] != -1)
			continue;
		if (pEnum._bNewVersion && (!pEnum._keepOldList[i])) {
			pEnum._computedIdList[i] = i;
		}
		else {
			pEnum._computedIdList[i] = CovertStringToInt(pEnum._ParNameList[i]);
		}
	}
}
void    ProtocolParse_Writer_TSMiddle::ConvertComment(const string& strComment, int tablenum, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	if (strComment.length() == 0) {
		writer.Key("remarks");
		writer.StartArray();
		writer.EndArray();
		return;
	}

	//char tt_ch[2048];
	string strLine;

	vector<string> tt_subList;
	ProtocolUtils::GetAllSubString(strComment, "split", tt_subList, false);

	writer.Key("remarks");
	writer.StartArray();

	for (unsigned int i = 0; i < tt_subList.size(); i++) {
		writer.String(tt_subList[i].c_str());
	}

	writer.EndArray();
}


bool    ProtocolParse_Writer_TSMiddle::CreateFile_TScript_Middle(string fileName, int filetype, bool bRefProtocol)
{

	CheckDirectoryEixst(fileName.c_str());

	FILE* fp = fopen(fileName.c_str(), "wb+");
	if (!fp)
		return false;

	rapidjson::StringBuffer strJson;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strJson);
	writer.StartObject();

	//写命名空间
	for (size_t i = 0; i < m_NamespaceVec.size(); i++) {
		PreComputeAllFuncId(m_NamespaceVec[i]);
		PreComputeAllEnumId(m_NamespaceVec[i]);
		WriteOneNameSpace_TScript_Middle(m_NamespaceVec[i], "", filetype, writer);
	}

	writer.EndObject();
	writer.Flush();
	strJson.GetString();

	fwrite(strJson.GetString(), strJson.GetLength(), 1, fp);
	fclose(fp);
	return true;
}

bool    ProtocolParse_Writer_TSMiddle::WriteOneNameSpace_TScript_Middle(stNamespace& pNamespace, string foldername, int filetype, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	m_tablenum = 0;

	//char ch[256];


	////写函数的id,和协议号
	CreateAndWriteFuncIdAndProtocolNo_TScript_Middle(pNamespace, foldername, writer);

	WriteTypes_TScript_Middle(pNamespace, writer);

	//SC,协议文件确是S/C
	if (filetype == 2 && pNamespace._ClassVec.size() > 1) {
		vector<stClass>  tt_bakClassList = pNamespace._ClassVec;
		for (size_t i = 1; i < pNamespace._ClassVec.size(); i++) {
			for (size_t j = 0; j < pNamespace._ClassVec[i]._FuncVec.size(); j++) {
				pNamespace._ClassVec[i]._FuncVec[j]._strSCClassName = pNamespace._ClassVec[i]._name;
				pNamespace._ClassVec[0]._FuncVec.push_back(pNamespace._ClassVec[i]._FuncVec[j]);
			}
		}
		while (pNamespace._ClassVec.size() > 1) {
			pNamespace._ClassVec.pop_back();
		}


		for (size_t i = 0; i < pNamespace._ClassVec[0]._FuncVec.size(); i++) {
			if (!pNamespace._ClassVec[0]._FuncVec[i]._bSCFlag) {
				pNamespace._ClassVec[0]._FuncVec[i]._bDiscard = true;
			}

		}

		//写类
		for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
			WriteClass_TScript_Middle(pNamespace._ClassVec[i], pNamespace, filetype, writer);
		}

		pNamespace._ClassVec.clear();
		pNamespace._ClassVec = tt_bakClassList;
	}
	else if (filetype == 1 && pNamespace._ClassVec.size() > 1) {
		vector<stClass>  tt_bakClassList = pNamespace._ClassVec;
		int tt_fucsize = pNamespace._ClassVec[0]._FuncVec.size();
		for (size_t i = 1; i < pNamespace._ClassVec.size(); i++) {
			for (size_t j = 0; j < pNamespace._ClassVec[i]._FuncVec.size(); j++) {
				if (pNamespace._ClassVec[i]._FuncVec[j]._bSCFlag) {
					pNamespace._ClassVec[0]._FuncVec.push_back(pNamespace._ClassVec[i]._FuncVec[j]);
					pNamespace._ClassVec[0]._FuncVec[pNamespace._ClassVec[0]._FuncVec.size() - 1]._strSCClassName = pNamespace._ClassVec[i]._name;
				}
			}
		}

		for (int m = 0; m < tt_fucsize; m++) {
			if (!pNamespace._ClassVec[0]._FuncVec[m]._bSCFlag)
				continue;
			for (size_t i = 1; i < pNamespace._ClassVec.size(); i++) {
				pNamespace._ClassVec[i]._FuncVec.push_back(pNamespace._ClassVec[0]._FuncVec[m]);
				pNamespace._ClassVec[i]._FuncVec[pNamespace._ClassVec[i]._FuncVec.size() - 1]._strSCClassName = pNamespace._ClassVec[0]._name;
			}
		}

		for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
			WriteClass_TScript_Middle(pNamespace._ClassVec[i], pNamespace, filetype, writer);
		}

		pNamespace._ClassVec.clear();
		pNamespace._ClassVec = tt_bakClassList;
	}
	else {
		//写类
		for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
			WriteClass_TScript_Middle(pNamespace._ClassVec[i], pNamespace, filetype, writer);
		}
	}



	return true;
}

void    ProtocolParse_Writer_TSMiddle::CreateAndWriteFuncIdAndProtocolNo_TScript_Middle(stNamespace& pNamespace, string foldername, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	//协议号
	if (m_ProtocolNo._value != 1) {
		writer.Key("protoID");
		writer.Int(m_ProtocolNo._value);
		//string strTable_1 = CreateTableString(1);

		//string strid;
		//strid.append(strTable_1);

		//char tt_ch[256];	
		//sprintf_s(tt_ch,256, R"("protoID":%d,)", m_ProtocolNo._value);
		//strid.append(tt_ch);
		//strid.append("\n");

		//fwrite(strid.c_str(), strid.size(), 1, fp);
	}
}

void    ProtocolParse_Writer_TSMiddle::WriteTypes_TScript_Middle(stNamespace& pNamespace, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("types");
	writer.StartObject();

	//先写枚举
	for (size_t i = 0; i < pNamespace._EnumVec.size(); i++) {
		WriteTypes_Enum_TScript_Middle(pNamespace, pNamespace._EnumVec[i], 2, writer);
	}


	//////////需要考虑顺序,按顺序插着写
	////结构体和vector型大小
	int totalsize = pNamespace._StructVec.size() + pNamespace._VectorVec.size() + pNamespace._WJSVectorVec.size();
	int structindex = 0;
	int vectorindex = 0;
	int wjsvectorindex = 0;
	bool bstructend = false;
	bool bvectorend = false;
	bool bwjsvectorend = false;

	if (pNamespace._StructVec.size() == 0) {
		bstructend = true;
	}
	if (pNamespace._VectorVec.size() == 0) {
		bvectorend = true;
	}
	if (pNamespace._WJSVectorVec.size() == 0) {
		bwjsvectorend = true;
	}

	for (int i = 0; i < totalsize; i++) {
		if ((!bvectorend) && (!bstructend) && (!bwjsvectorend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._VectorVec[vectorindex]._index
				&& (pNamespace._StructVec[structindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index)) {
				//写结构体
				WriteTypes_Struct_TScript_Middle(pNamespace, pNamespace._StructVec[structindex], 2, writer);
				structindex++;
			}
			else if (pNamespace._VectorVec[vectorindex]._index < pNamespace._StructVec[structindex]._index
				&& (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index)) {
				//写vector
				WriteTypes_Vector_TScript_Middle(pNamespace, pNamespace._VectorVec[vectorindex], 2, writer);
				vectorindex++;
			}
			else {
				//写类vector
				WriteTypes_WJSVector_TScript_Middle(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], 2, writer);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend && (!bstructend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._VectorVec[vectorindex]._index) {
				//写结构体
				WriteTypes_Struct_TScript_Middle(pNamespace, pNamespace._StructVec[structindex], 2, writer);
				structindex++;
			}
			else {
				//写vector
				WriteTypes_Vector_TScript_Middle(pNamespace, pNamespace._VectorVec[vectorindex], 2, writer);
				vectorindex++;
			}
		}
		else if (!bvectorend && (!bwjsvectorend)) {
			if (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写vector
				WriteTypes_Vector_TScript_Middle(pNamespace, pNamespace._VectorVec[vectorindex], 2, writer);
				vectorindex++;
			}
			else {
				//写类vector
				WriteTypes_WJSVector_TScript_Middle(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], 2, writer);
				wjsvectorindex++;
			}
		}
		else if ((!bstructend) && (!bwjsvectorend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写结构体
				WriteTypes_Struct_TScript_Middle(pNamespace, pNamespace._StructVec[structindex], 2, writer);
				structindex++;
			}
			else {
				//写类vector
				WriteTypes_WJSVector_TScript_Middle(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], 2, writer);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend) {
			//写vector
			WriteTypes_Vector_TScript_Middle(pNamespace, pNamespace._VectorVec[vectorindex], 2, writer);
			vectorindex++;

		}
		else if (!bstructend) {
			//写结构体
			WriteTypes_Struct_TScript_Middle(pNamespace, pNamespace._StructVec[structindex], 2, writer);
			structindex++;
		}
		else if (!bwjsvectorend) {
			//写类vector
			WriteTypes_WJSVector_TScript_Middle(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], 2, writer);
			wjsvectorindex++;
		}

		if (vectorindex >= (int)pNamespace._VectorVec.size()) {
			bvectorend = true;
		}
		if (structindex >= (int)pNamespace._StructVec.size()) {
			bstructend = true;
		}
		if (wjsvectorindex >= (int)pNamespace._WJSVectorVec.size()) {
			bwjsvectorend = true;
		}

	}


	writer.EndObject(); //end types;




}
void    ProtocolParse_Writer_TSMiddle::WriteTypes_Enum_TScript_Middle(stNamespace& pNamespace, stEnum& pEnum, int tablenum, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key(pEnum._name.c_str());
	writer.StartObject();
	writer.Key("type");
	writer.String("enum");
	writer.Key("attributes");
	writer.StartArray();


	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		writer.StartObject();

		writer.Key("name");
		writer.String(pEnum._ParNameList[i].c_str());
		writer.Key("value");
		writer.Int(pEnum._computedIdList[i]);

		if (pEnum._commentList[i].length() > 0) {
			ConvertComment(pEnum._commentList[i], tablenum + 3, writer);
		}

		writer.EndObject();
	}

	writer.EndArray();


	writer.EndObject();



}
void    ProtocolParse_Writer_TSMiddle::WriteTypes_Struct_TScript_Middle(stNamespace& pNamespace, stStructStruct& pStruct, int tablenum, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	if (pStruct._inherited.length() > 0)
		return;
	//if (!pStruct._bBiaoZhuUsed)
	//	return;

	//if (pStruct._name == "PtChangedEvent")
	//{
	//	int a;
	//	a = 10;
	//}

	writer.Key(pStruct._name.c_str());
	writer.StartObject();

	writer.Key("type");
	writer.String("struct");

	if (pStruct._version > 1) {
		writer.Key("version");
		writer.Int(pStruct._version);
	}
	if (pStruct._startversion_ts > 1) {
		writer.Key("BaseVersion");
		writer.Int(pStruct._startversion_ts);
	}

	writer.Key("attributes");
	writer.StartArray();

	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		writer.StartObject();

		writer.Key("name");
		writer.String(pStruct._ParVec[i]._strParName.c_str());

		writer.Key("type");
		writer.String(TypeConvertToMiddleType(pStruct._ParVec[i]._strParType, true).c_str());

		if (pStruct._ParVec[i]._transNeedValue != -1) {
			writer.Key("TransValue");
			writer.Int(pStruct._ParVec[i]._transNeedValue);
		}

		if (pStruct._ParVec[i]._version > 1) {
			writer.Key("version");
			writer.Int(pStruct._ParVec[i]._version);
		}

		if (pStruct._ParVec[i]._bWithDefault && pStruct._ParVec[i]._strDefaultValue.length() > 0) {
			writer.Key("Default");
			writer.String(pStruct._ParVec[i]._strDefaultValue.c_str());
		}

		if (pStruct._ParVec[i]._comment.length() > 0) {
			ConvertComment(pStruct._ParVec[i]._comment, tablenum + 3, writer);
		}

		writer.EndObject();
	}

	writer.EndArray();
	if (pStruct._comment.length() > 0) {
		ConvertComment(pStruct._comment, tablenum + 3, writer);
	}


	writer.EndObject();

}

void    ProtocolParse_Writer_TSMiddle::WriteTypes_Vector_TScript_Middle(stNamespace& pNamespace, stVector& pVector, int tablenum, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	//if (!pVector._bBiaoZhuUsed)
	//	return;

	writer.Key(pVector._name.c_str());
	writer.StartObject();

	writer.Key("type");
	writer.String("array");

	writer.Key("elementType");
	writer.String(TypeConvertToMiddleType(pVector._ParTName, true).c_str());

	if (pVector._comment.length() > 0) {
		ConvertComment(pVector._comment, tablenum + 1, writer);
	}


	writer.EndObject();

	//char t_ch[512];
	//string strTable_0 = CreateTableString(tablenum + 0);
	//string strTable_1 = CreateTableString(tablenum + 1);
	//string strTable_2 = CreateTableString(tablenum + 2);
	//string strTable_3 = CreateTableString(tablenum + 3);
	//string strTable_4 = CreateTableString(tablenum + 4);

	//string strVector;

	//bool  tt_bTScritpBaseType = IsTScriptBaseType2(pVector._ParTName);

	//strVector.append(strTable_0.c_str());
	//sprintf(t_ch, R"("%s":{)", pVector._name.c_str());
	//strVector.append(t_ch);
	//strVector.append("\n");
	//strVector.append(strTable_1.c_str());
	//strVector.append(R"("type":"array",)");
	//strVector.append("\n");

	//strVector.append(strTable_1.c_str());
	//sprintf(t_ch, R"("elementType":"%s")", TypeConvertToMiddleType(pVector._ParTName, true).c_str());
	//strVector.append(t_ch);
	//


	//if (pVector._comment.length() > 0)
	//{
	//	strVector.append(",\n");
	//	strVector.append(strTable_1.c_str());
	//	strVector.append(ConvertComment(pVector._comment, tablenum + 1).c_str());
	//	strVector.append("\n");
	//}
	//else
	//{
	//	strVector.append("\n");
	//}
	//strVector.append(strTable_0.c_str());
	//strVector.append("},\n");

	//fwrite(strVector.c_str(), strVector.size(), 1, fp);
}

void    ProtocolParse_Writer_TSMiddle::WriteTypes_WJSVector_TScript_Middle(stNamespace& pNamespace, stWJSVector& pWJSVector, int tablenum, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	//if (!pWJSVector._bBiaoZhuUsed)
	//	return;

	writer.Key(pWJSVector._name.c_str());
	writer.StartObject();

	writer.Key("type");
	writer.String("array");

	writer.Key("elementType");
	writer.String(TypeConvertToMiddleType(pWJSVector._ParTName, true).c_str());


	writer.EndObject();

	//char t_ch[512];
	//string strTable_0 = CreateTableString(tablenum + 0);
	//string strTable_1 = CreateTableString(tablenum + 1);
	//string strTable_2 = CreateTableString(tablenum + 2);
	//string strTable_3 = CreateTableString(tablenum + 3);
	//string strTable_4 = CreateTableString(tablenum + 4);

	//string strWJSVector;


	///////////////有两种版本，一种是非Flash基础类型，一种是
	////bool  tt_bTScritpBaseType = IsTScriptBaseType2(pWJSVector._ParTName);

	//strWJSVector.append(strTable_0.c_str());
	//sprintf(t_ch, R"("%s":{)", pWJSVector._name.c_str());
	//strWJSVector.append(t_ch);
	//strWJSVector.append("\n");
	//strWJSVector.append(strTable_1.c_str());
	//strWJSVector.append(R"("type":"array",)");
	//strWJSVector.append("\n");

	//strWJSVector.append(strTable_1.c_str());
	//sprintf(t_ch, R"("elementType":"%s")", TypeConvertToMiddleType(pWJSVector._ParTName, true).c_str());
	//strWJSVector.append(t_ch);
	//strWJSVector.append("\n");

	//strWJSVector.append(strTable_0.c_str());
	//strWJSVector.append("},\n");

	//fwrite(strWJSVector.c_str(), strWJSVector.size(), 1, fp);


}
void    ProtocolParse_Writer_TSMiddle::WriteClass_TScript_Middle(stClass& pClass, stNamespace& pNamespace, int filetype, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	if (filetype == 0) {
		if (pClass._strtype.compare("request") == 0) {
			WriteSends_TScript_Middle(pClass, pNamespace, writer);
		}
		else if (pClass._strtype.compare("callback") == 0) {
			WriteRecvs_TScript_Middle(pClass, pNamespace, writer);
		}
	}
	else if (filetype == 1) {
		if (pClass._strtype.compare("callback") == 0) {
			WriteSends_TScript_Middle(pClass, pNamespace, writer);
		}
		else if (pClass._strtype.compare("request") == 0) {
			WriteRecvs_TScript_Middle(pClass, pNamespace, writer);
		}
	}
	else if (filetype == 2) {
		if (pClass._strtype.compare("request") == 0) {
			if (m_bSCSpliteS) {
				WriteSendRecvs_TScript_Middle(pClass, pNamespace, writer, 1);
			}
			else if (m_bSCSpliteC) {
				WriteSendRecvs_TScript_Middle(pClass, pNamespace, writer, -1);
			}
			else {
				WriteSendRecvs_TScript_Middle(pClass, pNamespace, writer, 0);
			}

		}
	}
}

void    ProtocolParse_Writer_TSMiddle::WriteSends_TScript_Middle(stClass& pClass, stNamespace& pNamespace, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("sends");
	writer.StartArray();
	for (unsigned int i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		Write_Func_TScript_Middle(pClass._FuncVec[i], pNamespace, 2, writer);

	}

	writer.EndArray();
}

void    ProtocolParse_Writer_TSMiddle::Write_Func_TScript_Middle(stFunc& pFunc, stNamespace& pNamespace, int tablenum, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();
	writer.Key("name");
	writer.String(pFunc._name.c_str());

	if (pFunc._version > 1) {
		writer.Key("version");
		writer.Int(pFunc._version);
	}

	writer.Key("methodid");
	writer.Int(pFunc._computedFuncId);

	if (pFunc._comment.length() > 0) {
		ConvertComment(pFunc._comment, tablenum + 1, writer);
	}

	writer.Key("args");
	writer.StartArray();

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		writer.StartObject();
		writer.Key("name");
		writer.String(pFunc._InParVec[i]._strParName.c_str());
		writer.Key("type");
		writer.String(TypeConvertToMiddleType(pFunc._InParVec[i]._strParType, true).c_str());
		if (pFunc._InParVec[i]._comment.length() > 0) {
			ConvertComment(pFunc._InParVec[i]._comment, tablenum + 3, writer);
		}

		if (pFunc._InParVec[i]._version > 1) {
			writer.Key("version");
			writer.Int(pFunc._InParVec[i]._version);
		}
		writer.EndObject();

	}

	writer.EndArray();


	writer.EndObject();
}
void    ProtocolParse_Writer_TSMiddle::WriteRecvs_TScript_Middle(stClass& pClass, stNamespace& pNamespace, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("recvs");
	writer.StartArray();

	for (unsigned int i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		Write_Func_TScript_Middle(pClass._FuncVec[i], pNamespace, 2, writer);

	}

	writer.EndArray();
}
void    ProtocolParse_Writer_TSMiddle::WriteSendRecvs_TScript_Middle(stClass& pClass, stNamespace& pNamespace, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, int SplitSCType)
{
	if (SplitSCType == 0) {
		writer.Key("sendrecvs");
		writer.StartArray();

		for (unsigned int i = 0; i < pClass._FuncVec.size(); i++) {
			if (pClass._FuncVec[i]._bDiscard)
				continue;
			Write_Func_TScript_Middle(pClass._FuncVec[i], pNamespace, 2, writer);
		}

		writer.EndArray();
	}
	else if (SplitSCType == 1 || SplitSCType == -1) {
		writer.Key("sendrecvs");
		writer.StartArray();

		for (unsigned int i = 0; i < pClass._FuncVec.size(); i++) {
			if (pClass._FuncVec[i]._bDiscard)
				continue;
			if (pClass._FuncVec[i]._SCSpliteType != 0)
				continue;
			Write_Func_TScript_Middle(pClass._FuncVec[i], pNamespace, 2, writer);
		}

		writer.EndArray();



		if (SplitSCType == 1) {
			writer.Key("sends");
			writer.StartArray();

			for (unsigned int i = 0; i < pClass._FuncVec.size(); i++) {
				if (pClass._FuncVec[i]._bDiscard)
					continue;
				if (pClass._FuncVec[i]._SCSpliteType != -1)
					continue;
				Write_Func_TScript_Middle(pClass._FuncVec[i], pNamespace, 2, writer);
			}

			writer.EndArray();

			writer.Key("recvs");
			writer.StartArray();

			for (unsigned int i = 0; i < pClass._FuncVec.size(); i++) {
				if (pClass._FuncVec[i]._bDiscard)
					continue;
				if (pClass._FuncVec[i]._SCSpliteType != 1)
					continue;
				Write_Func_TScript_Middle(pClass._FuncVec[i], pNamespace, 2, writer);
			}

			writer.EndArray();
		}
		else {
			writer.Key("sends");
			writer.StartArray();

			for (unsigned int i = 0; i < pClass._FuncVec.size(); i++) {
				if (pClass._FuncVec[i]._bDiscard)
					continue;
				if (pClass._FuncVec[i]._SCSpliteType != 1)
					continue;
				Write_Func_TScript_Middle(pClass._FuncVec[i], pNamespace, 2, writer);
			}

			writer.EndArray();

			writer.Key("recvs");
			writer.StartArray();

			for (unsigned int i = 0; i < pClass._FuncVec.size(); i++) {
				if (pClass._FuncVec[i]._bDiscard)
					continue;
				if (pClass._FuncVec[i]._SCSpliteType != -1)
					continue;
				Write_Func_TScript_Middle(pClass._FuncVec[i], pNamespace, 2, writer);
			}

			writer.EndArray();
		}



	}


}

string ProtocolParse_Writer_TSMiddle::TypeConvertToMiddleType(const string& strType, bool bFirstEnter)
{
	if (bFirstEnter) {
		if (IsBaseType(strType))
			return strType;
		//if (IsTScriptBaseType(strType, false, true))
		//	return strType;
	}

	const vector<stEnum>& pEnum = m_NamespaceVec[m_NamespaceVec.size() - 1]._EnumVec;
	for (unsigned int i = 0; i < pEnum.size(); i++) {
		if (pEnum[i]._name == strType) {
			if (bFirstEnter)
				return strType;
			else {
				string tt_ret;
				tt_ret.append(m_NamespaceVec[0]._servername.c_str());
				tt_ret.append(".");
				tt_ret.append(strType.c_str());
				return tt_ret;
			}
		}
	}

	const vector<stStructStruct>&  pStructVec = m_NamespaceVec[m_NamespaceVec.size() - 1]._StructVec;
	for (unsigned int i = 0; i < pStructVec.size(); i++) {
		if (pStructVec[i]._name == strType) {
			if (bFirstEnter)
				return strType;
			else {
				string tt_ret;
				tt_ret.append(m_NamespaceVec[0]._servername.c_str());
				tt_ret.append(".");
				tt_ret.append(strType.c_str());
				return tt_ret;
			}
		}

	}


	const vector<stVector>&  pVectorVec = m_NamespaceVec[m_NamespaceVec.size() - 1]._VectorVec;
	for (unsigned int i = 0; i < pVectorVec.size(); i++) {
		if (pVectorVec[i]._name == strType) {
			if (bFirstEnter)
				return strType;
			else {
				string tt_ret;
				tt_ret.append(m_NamespaceVec[0]._servername.c_str());
				tt_ret.append(".");
				tt_ret.append(strType.c_str());
				return tt_ret;
			}
		}
	}

	const vector<stWJSVector>&  pWJSVectorVec = m_NamespaceVec[m_NamespaceVec.size() - 1]._WJSVectorVec;
	for (unsigned int i = 0; i < pWJSVectorVec.size(); i++) {
		if (pWJSVectorVec[i]._name == strType) {
			if (bFirstEnter)
				return strType;
			else {
				string tt_ret;
				tt_ret.append(m_NamespaceVec[0]._servername.c_str());
				tt_ret.append(".");
				tt_ret.append(strType.c_str());
				return tt_ret;
			}
		}
	}

	for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
		if (m_pRefProtocolParseList[i]) {
			string tt_ret = ((ProtocolParse_Writer_TSMiddle*)m_pRefProtocolParseList[i])->TypeConvertToMiddleType(strType, false);
			if (tt_ret.length() > 0)
				return tt_ret;
		}
	}

	if (bFirstEnter)
		return strType;

	return "";
}