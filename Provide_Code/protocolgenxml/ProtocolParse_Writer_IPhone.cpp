#include "ProtocolParse_Writer_IPhone.h"
#include "ProtocolParse.h"
#include <time.h>
#include <sys/timeb.h>

extern string g_strBaseType[];

/////////////
//            Iphone
////////////////
ProtocolParse_Writer_IPhone::ProtocolParse_Writer_IPhone(const string& strPathName)
	:ProtocolParse_Writer(strPathName, eProtocolWriterType::IPhone)
{

}

ProtocolParse_Writer_IPhone::~ProtocolParse_Writer_IPhone()
{

}

bool    ProtocolParse_Writer_IPhone::CreateFile_Path(string path, int filetype, bool brefprotocol)
{
	return CreateFile_Path_Iphone(path,filetype,brefprotocol);
}
ProtocolParse_Writer* ProtocolParse_Writer_IPhone::Create_RefProtocolParse_Writer(const string& strPathName)
{
	return new ProtocolParse_Writer_IPhone(strPathName);
}
bool    ProtocolParse_Writer_IPhone::CreateFile_Path_Iphone(string path, int filetype, bool brefprotocol)
{
	if (filetype < 0 || filetype>2)
		return false;

	if (m_NamespaceVec.size() > 0) {
		if (brefprotocol) {
			char tt_ch[256];
			sprintf(tt_ch, "%sProtocol.h", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._servername = tt_ch;
			sprintf(tt_ch, "%sProtocol.m", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._servername2 = tt_ch;
			sprintf(tt_ch, "%sProtocol.h", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._clientname = tt_ch;
			sprintf(tt_ch, "%sProtocol.m", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._clientname2 = tt_ch;
		}
		else {
			char tt_ch[256];
			sprintf(tt_ch, "%sProtocol_server.h", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._servername = tt_ch;
			sprintf(tt_ch, "%sProtocol_server.m", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._servername2 = tt_ch;
			sprintf(tt_ch, "%sProtocol_client.h", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._clientname = tt_ch;
			sprintf(tt_ch, "%sProtocol_client.m", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._clientname2 = tt_ch;
		}

	}


	switch (filetype) {
	case 1://服务器
	{
		for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
			//主要为了使得id不出现重复
			if (m_pRefProtocolParseList[i]) {
				m_pRefProtocolParseList[i]->CreateFile_Path(path, filetype, true);
				m_pRefProtocolParseList[i]->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
			}
		}


		string str1 = path;
		str1.append(m_SaveFileName._servername);
		string str2 = path;
		str2.append(m_SaveFileName._servername2);
		return CreateFile_Iphone(str1, str2, filetype);
	}
	break;
	case 0://客户端
	{
		for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
			if (m_pRefProtocolParseList[i]) {
				m_pRefProtocolParseList[i]->CreateFile_Path(path, filetype, true);
				m_pRefProtocolParseList[i]->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
			}
		}


		string str1 = path;
		str1.append(m_SaveFileName._clientname);
		string str2 = path;
		str2.append(m_SaveFileName._clientname2);
		return CreateFile_Iphone(str1, str2, filetype);
	}
	break;
	case 2: //合成
	{
		for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
			//主要为了使得id不出现重复
			if (m_pRefProtocolParseList[i]) {
				m_pRefProtocolParseList[i]->CreateFile_Path(path, filetype, true);
				m_pRefProtocolParseList[i]->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
			}
		}


		//先按服务器名存
		string str1 = path;
		str1.append(m_SaveFileName._servername);
		string str2 = path;
		str2.append(m_SaveFileName._servername2);
		return CreateFile_Iphone(str1, str2, filetype);
	}
	break;
	default:
		return false;
		break;
	}

}

bool    ProtocolParse_Writer_IPhone::CreateFile_Iphone(string filename1, string filename2, int filetype)
{
	FILE *fp1;
	FILE *fp2;

	fp1 = fopen(filename1.c_str(), "w");
	if (!fp1)
		return false;

	fp2 = fopen(filename2.c_str(), "w");
	if (!fp2)
		return false;


	fseek(fp1, 0, SEEK_SET);
	fseek(fp2, 0, SEEK_SET);

	//先写头
	//写信息头,共用
	WriteInfoHead(fp1);
	WriteInfoHead(fp2);

	//写文件头
	WriteFileHead_Iphone(fp1, filetype);
	//写m文件头
	WriteFileHead_Iphone_M(fp2, filetype);

	//写命名空间
	for (size_t i = 0; i < m_NamespaceVec.size(); i++) {
		WriteOneNameSpace_Iphone(m_NamespaceVec[i], fp1, fp2, filetype);
	}

	//WriteFileEnd_Iphone_M(fp2,filetype);
	fclose(fp1);
	fclose(fp2);
	return true;
}

void    ProtocolParse_Writer_IPhone::WriteFileHead_Iphone(FILE* fp, int filetype)
{
	string str;
	if (m_pRefProtocolParseList.size() > 0) {
		char ch[256];
		sprintf(ch, "#import <Foundation/Foundation.h>\n");
		str.append(ch);

		for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
			if (!m_pRefProtocolParseList[i])
				continue;
			switch (filetype) {
			case 0:
			{
				sprintf(ch, "#import \"%s\"\n", m_RefProtocolList[i]._includeheadnamec.c_str());

			}
			break;
			case 1:
			{
				sprintf(ch, "#import \"%s\"\n", m_RefProtocolList[i]._includeheadnames.c_str());
			}
			break;
			case 2:
			{
				sprintf(ch, "#import \"%s\"\n", m_RefProtocolList[i]._includeheadnames.c_str());
			}
			break;
			default:
				break;
			}

			str.append(ch);
		}

	}
	else {
		str = "#import <Foundation/Foundation.h>\n";
	}

	fwrite(str.c_str(), str.length(), 1, fp);
}

void    ProtocolParse_Writer_IPhone::WriteFileHead_Iphone_M(FILE* fp, int filetype)
{
	string str;
	char ch[512];
	switch (filetype) {
	case 0:
	{
		sprintf(ch, "#import \"%s\"\n", m_SaveFileName._clientname.c_str());
		str.append(ch);
	}
	break;
	case 1:
	{
		sprintf(ch, "#import \"%s\"\n", m_SaveFileName._servername.c_str());
		str.append(ch);
	}
	break;
	case 2:
	{
		sprintf(ch, "#import \"%s\"\n", m_SaveFileName._servername.c_str());
		str.append(ch);
	}
	break;
	default:
		break;
	}

	str.append("#import \"NSMutableData-Extensions.h\"\n\n");

	//sprintf(ch,"@implementation  %s\n\n",m_SaveFileName._servername.c_str());
	fwrite(str.c_str(), str.length(), 1, fp);


}
void    ProtocolParse_Writer_IPhone::WriteFileEnd_Iphone_M(FILE* fp, int filetype)
{
	//string str = "@end\n";
	//fwrite(str.c_str(),str.length(),1,fp);
}
bool    ProtocolParse_Writer_IPhone::WriteOneNameSpace_Iphone(stNamespace& pNamespace, FILE* fp1, FILE* fp2, int filetype)
{
	m_tablenum = 0;

	//char ch[256];

	//写函数的id写到.m
	CreateAndWriteFuncId_Iphone(pNamespace, fp2, m_tablenum + 1);

	//写协议号,写到.m
	WriteProtocolNo_Iphone(fp2, m_tablenum + 1);

	//先写枚举,写到.h
	for (size_t i = 0; i < pNamespace._EnumVec.size(); i++) {
		WriteEnum_Iphone(pNamespace._EnumVec[i], fp1, m_tablenum + 1);
	}

	////////需要考虑顺序,按顺序插着写
	//结构体和vector型大小
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
				WriteStruct_Iphone(pNamespace._StructVec[structindex], fp1, m_tablenum + 1);
				WriteStruct_Iphone_M(pNamespace._StructVec[structindex], fp2, m_tablenum + 1);
				structindex++;
			}
			else if (pNamespace._VectorVec[vectorindex]._index < pNamespace._StructVec[structindex]._index
				&& (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index)) {
				//写vector
				WriteVector_Iphone(pNamespace._VectorVec[vectorindex], fp1, fp2, m_tablenum + 1);
				vectorindex++;
			}
			else {
				//写类vector
				WriteWJSVector_Iphone(pNamespace._WJSVectorVec[wjsvectorindex], fp1, fp2, m_tablenum + 1);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend && (!bstructend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._VectorVec[vectorindex]._index) {
				//写结构体
				WriteStruct_Iphone(pNamespace._StructVec[structindex], fp1, m_tablenum + 1);
				WriteStruct_Iphone_M(pNamespace._StructVec[structindex], fp2, m_tablenum + 1);
				structindex++;
			}
			else {
				//写vector
				WriteVector_Iphone(pNamespace._VectorVec[vectorindex], fp1, fp2, m_tablenum + 1);
				vectorindex++;
			}
		}
		else if (!bvectorend && (!bwjsvectorend)) {
			if (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写vector
				WriteVector_Iphone(pNamespace._VectorVec[vectorindex], fp1, fp2, m_tablenum + 1);
				vectorindex++;
			}
			else {
				//写类vector
				WriteWJSVector_Iphone(pNamespace._WJSVectorVec[wjsvectorindex], fp1, fp2, m_tablenum + 1);
				wjsvectorindex++;
			}
		}
		else if ((!bstructend) && (!bwjsvectorend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写结构体
				WriteStruct_Iphone(pNamespace._StructVec[structindex], fp1, m_tablenum + 1);
				WriteStruct_Iphone_M(pNamespace._StructVec[structindex], fp2, m_tablenum + 1);
				structindex++;
			}
			else {
				//写类vector
				WriteWJSVector_Iphone(pNamespace._WJSVectorVec[wjsvectorindex], fp1, fp2, m_tablenum + 1);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend) {
			//写vector
			WriteVector_Iphone(pNamespace._VectorVec[vectorindex], fp1, fp2, m_tablenum + 1);
			vectorindex++;

		}
		else if (!bstructend) {
			//写结构体
			WriteStruct_Iphone(pNamespace._StructVec[structindex], fp1, m_tablenum + 1);
			WriteStruct_Iphone_M(pNamespace._StructVec[structindex], fp2, m_tablenum + 1);
			structindex++;
		}
		else if (!bwjsvectorend) {
			//写类vector
			WriteWJSVector_Iphone(pNamespace._WJSVectorVec[wjsvectorindex], fp1, fp2, m_tablenum + 1);
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


	//写类
	for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
		WriteClass_Iphone(pNamespace._ClassVec[i], fp1, fp2, m_tablenum + 1, filetype, pNamespace._protocolname, pNamespace);
	}

	//写结束的中括号}
	//WriteEndZhongKuoKao(fp);
	return true;
}

void    ProtocolParse_Writer_IPhone::CreateAndWriteFuncId_Iphone(stNamespace& pNamespace, FILE* fp, int tablenum)
{
	//没有类是不用写方法的
	if (pNamespace._ClassVec.size() == 0)
		return;

	char t_ch[512];
	string strTable = CreateTableString(tablenum);  //写表格符
	string strTable_2 = CreateTableString(tablenum + 1);

	string strFuncId;

	for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
		for (size_t j = 0; j < pNamespace._ClassVec[i]._FuncVec.size(); j++) {
			strFuncId.append(strTable_2);
			strFuncId.append("static const int ");
			sprintf(t_ch, "ID_%s_%s", pNamespace._ClassVec[i]._name.c_str(), pNamespace._ClassVec[i]._FuncVec[j]._name.c_str());
			strFuncId.append(t_ch);
			int tempId;  //将字符串转成id
			if (pNamespace._ClassVec[i]._FuncVec[j]._userFuncId != 0)
				tempId = pNamespace._ClassVec[i]._FuncVec[j]._userFuncId;
			else
				tempId = CovertStringToInt(t_ch, pNamespace._ClassVec[i]._FuncVec[j]._bNew);

			sprintf(t_ch, "=%d;\n", tempId);
			strFuncId.append(t_ch);
		}
	}

	fwrite(strFuncId.c_str(), strFuncId.length(), 1, fp);
}
void    ProtocolParse_Writer_IPhone::WriteProtocolNo_Iphone(FILE* fp, int tablenum)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum + 1);

	string strProtocolNo;
	strProtocolNo.append(strTable);

	if (m_ProtocolNo._value != 1) {
		sprintf(t_ch, "static const int %s=%d;\n", m_ProtocolNo._name.c_str(), m_ProtocolNo._value);
		strProtocolNo.append(t_ch);
	}
	else {
		strProtocolNo.append("\n");

	}


	fwrite(strProtocolNo.c_str(), strProtocolNo.length(), 1, fp);


}
void    ProtocolParse_Writer_IPhone::WriteEnum_Iphone(stEnum& pEnum, FILE* fp, int tablenum)//tablenum代数制表符数
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);
	string strTable_3 = CreateTableString(tablenum + 3);
	string strEnum;
	strEnum.append(strTable);
	sprintf(t_ch, "enum %s\n", pEnum._name.c_str());
	strEnum.append(t_ch);

	strEnum.append(strTable);
	strEnum.append("{\n");

	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		strEnum.append(strTable_2);
		if (i == 0) {
			sprintf(t_ch, "%s = %d,\n", pEnum._ParNameList[i].c_str(), CovertStringToInt(pEnum._ParNameList[i]));
		}
		else if (i != pEnum._ParNameList.size() - 1) {
			sprintf(t_ch, "%s=%d,\n", pEnum._ParNameList[i].c_str(), CovertStringToInt(pEnum._ParNameList[i]));
		}
		else {
			sprintf(t_ch, "%s=%d\n", pEnum._ParNameList[i].c_str(), CovertStringToInt(pEnum._ParNameList[i]));
		}
		strEnum.append(t_ch);
	}
	strEnum.append(strTable);
	strEnum.append("};\n");

	fwrite(strEnum.c_str(), strEnum.length(), 1, fp);


}

void    ProtocolParse_Writer_IPhone::WriteStruct_Iphone(stStructStruct& pStruct, FILE* fp, int tablenum)
{
	if (pStruct._inherited.length() > 0)
		return;
	char t_ch[1024];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);

	string strStruct;

	strStruct.append(strTable);
	sprintf(t_ch, "@interface %s:NSObject{\n", pStruct._name.c_str());
	strStruct.append(t_ch);

	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		strStruct.append(strTable_2);
		if (IsIphoneBaseType(pStruct._ParVec[i]._strParType)) {
			sprintf(t_ch, "%s %s;\n", TypeConvertToIphoneType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());

		}
		else {
			sprintf(t_ch, "%s* %s;\n", TypeConvertToIphoneType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());

		}
		strStruct.append(t_ch);
	}



	strStruct.append(strTable);
	strStruct.append("};\n");

	fwrite(strStruct.c_str(), strStruct.length(), 1, fp);

	strStruct = "";

	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		strStruct.append(strTable);
		if (IsIphoneBaseType(pStruct._ParVec[i]._strParType)) {
			if (IsBaseTypeNeedAddRetain(pStruct._ParVec[i]._strParType)) {
				sprintf(t_ch, "@property(nonatomic,retain) %s %s;\n", TypeConvertToIphoneType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());

			}
			else {
				sprintf(t_ch, "@property(nonatomic) %s %s;\n", TypeConvertToIphoneType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());

			}
		}
		else {
			sprintf(t_ch, "@property(nonatomic,retain) %s* %s;\n", TypeConvertToIphoneType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
		}

		strStruct.append(t_ch);
	}
	//读函数
	strStruct.append(strTable);
	sprintf(t_ch, "+(%s*)Read:(NSMutableData*)data;\n", pStruct._name.c_str());
	strStruct.append(t_ch);
	//strStruct.append("-(void)Read:(NSMutableData*)data;\n");

	//写函数
	strStruct.append(strTable);
	strStruct.append("-(NSMutableData*)Write:(NSMutableData*)data;\n");
	strStruct.append(strTable);
	strStruct.append("@end\n\n");
	fwrite(strStruct.c_str(), strStruct.length(), 1, fp);

}

void    ProtocolParse_Writer_IPhone::WriteStruct_Iphone_M(stStructStruct& pStruct, FILE* fp, int tablenum)
{
	char t_ch[2048];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);
	string strTable_3 = CreateTableString(tablenum + 2);

	string strStruct;

	strStruct.append(strTable);
	sprintf(t_ch, "@implementation %s\n", pStruct._name.c_str());
	strStruct.append(t_ch);
	strStruct.append(strTable);
	strStruct.append("@synthesize ");

	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (i == pStruct._ParVec.size() - 1) {
			sprintf(t_ch, "%s;\n", pStruct._ParVec[i]._strParName.c_str());
			strStruct.append(t_ch);
		}
		else {
			sprintf(t_ch, "%s, ", pStruct._ParVec[i]._strParName.c_str());
			strStruct.append(t_ch);
		}
	}

	strStruct.append(strTable);
	strStruct.append("-(id)init\n");

	fwrite(strStruct.c_str(), strStruct.length(), 1, fp);

	strStruct = "";
	strStruct.append(strTable);
	strStruct.append("{\n");

	strStruct.append(strTable_2);
	strStruct.append("if(self == [super init])\n");

	strStruct.append(strTable_2);
	strStruct.append("{\n");

	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (!IsIphoneBaseType(pStruct._ParVec[i]._strParType)) {
			strStruct.append(strTable_3);
			sprintf(t_ch, "%s = [[[NSString alloc]init] autorelease]; \n", pStruct._ParVec[i]._strParName.c_str());
			strStruct.append(t_ch);
		}
	}

	strStruct.append(strTable_2);
	strStruct.append("}\n");

	strStruct.append(strTable_2);
	strStruct.append("return self;\n");

	strStruct.append(strTable);
	strStruct.append("}\n");

	//读函数
	strStruct.append(strTable);
	//strStruct.append("-(void)Read:(NSMutableData*)data\n");
	sprintf(t_ch, "+(%s*)Read:(NSMutableData*)data\n", pStruct._name.c_str());
	strStruct.append(t_ch);
	strStruct.append(strTable);
	strStruct.append("{\n");

	strStruct.append(strTable_2);
	sprintf(t_ch, "%s* SelfCopy = [[[%s alloc]init]autorelease];\n", pStruct._name.c_str(), pStruct._name.c_str());
	strStruct.append(t_ch);



	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		strStruct.append(strTable_2);
		string ttt_strRead = GetIphoneTypeRead("data", pStruct._ParVec[i]._strParType);
		if (ttt_strRead.length() > 0) {
			sprintf(t_ch, "SelfCopy.%s =  %s;\n", pStruct._ParVec[i]._strParName.c_str(), ttt_strRead.c_str());
			strStruct.append(t_ch);
		}
		else {
			//非基本类型提供读写方法
			//先定义一个成员
			sprintf(t_ch, "SelfCopy.%s = [%s Read:data];\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToIphoneType(pStruct._ParVec[i]._strParType).c_str());
			strStruct.append(t_ch);
			//sprintf(t_ch,"[SelfCopy.%s Read:data];\n",pStruct._ParVec[i]._strParName.c_str());
			//strStruct.append(t_ch);
		}
	}

	strStruct.append(strTable_2);
	strStruct.append("return SelfCopy;\n");

	strStruct.append(strTable);
	strStruct.append("}\n");

	//写函数
	strStruct.append(strTable);
	strStruct.append("-(NSMutableData*)Write:(NSMutableData*)data\n");
	strStruct.append(strTable);
	strStruct.append("{\n");

	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		strStruct.append(strTable_2);
		string strWrite = GetIphoneTypeWrite("data", pStruct._ParVec[i]._strParType);
		if (strWrite.length() > 0) {
			sprintf(t_ch, "[%s:%s];\n", strWrite.c_str(), pStruct._ParVec[i]._strParName.c_str());
			strStruct.append(t_ch);
		}
		else {
			sprintf(t_ch, "data = [%s Write:data];\n", pStruct._ParVec[i]._strParName.c_str());
			strStruct.append(t_ch);
		}
	}

	strStruct.append(strTable_2);
	strStruct.append("return data;\n");
	strStruct.append(strTable);
	strStruct.append("}\n");
	strStruct.append(strTable);
	strStruct.append("@end\n\n");
	fwrite(strStruct.c_str(), strStruct.length(), 1, fp);
}

void    ProtocolParse_Writer_IPhone::WriteVector_Iphone(stVector& pVector, FILE* fp1, FILE* fp2, int tablenum)
{
	char t_ch[1024];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);
	string strTable_3 = CreateTableString(tablenum + 2);

	//写头
	string strVector;
	strVector.append(strTable);
	sprintf(t_ch, "@interface %s : NSMutableArray{}\n", pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable);
	sprintf(t_ch, "+(%s*)Read:(NSMutableData*)data;\n", pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable);
	sprintf(t_ch, "-(NSMutableData*)Write:(NSMutableData*)data;\n");
	strVector.append(t_ch);

	strVector.append(strTable);
	strVector.append("@end\n\n");
	fwrite(strVector.c_str(), strVector.length(), 1, fp1);
	strVector = "";


	//写.m部分
	strVector.append(strTable);
	sprintf(t_ch, "@implementation %s\n", pVector._name.c_str());
	strVector.append(t_ch);

	//strVector.append(strTable);
	//strVector.append("-(id)init\n");

	//fwrite(strVector.c_str(),strVector.length(),1,fp2);

	//strVector="";
	//strVector.append(strTable);
	//strVector.append("{\n");

	//strVector.append(strTable_2);
	//strVector.append("if(self == [super init])\n");

	//strVector.append(strTable_2);
	//strVector.append("{\n");



	//strVector.append(strTable_2);
	//strVector.append("}\n");

	//strVector.append(strTable_2);
	//strVector.append("return self;\n");

	//strVector.append(strTable);
	//strVector.append("}\n");
	fwrite(strVector.c_str(), strVector.length(), 1, fp2);
	strVector = "";



	//读函数
	strVector.append(strTable);
	sprintf(t_ch, "+(%s*)Read:(NSMutableData*)data\n", pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable);
	strVector.append("{\n");

	strVector.append(strTable_2);
	sprintf(t_ch, "%s* SelfCopy = [[[%s alloc]init]autorelease];\n", pVector._name.c_str(), pVector._name.c_str());
	strVector.append(t_ch);



	//先读长度
	strVector.append(strTable_2);
	strVector.append("int num = [data readInt];\n");

	strVector.append(strTable_2);
	strVector.append("for(int i=0;i<num;i++)\n");

	strVector.append(strTable_2);
	strVector.append("{\n");

	//_ParTName
	strVector.append(strTable_3);
	string ttt_strRead = GetIphoneTypeRead("data", pVector._ParTName);
	if (ttt_strRead.length() > 0) {
		if (IsIphoneBaseType(pVector._ParTName)) {
			//sprintf(t_ch,"%s tt =  [[%s alloc]init];\n",TypeConvertToIphoneType(pVector._ParTName).c_str(),TypeConvertToIphoneType(pVector._ParTName).c_str());

			sprintf(t_ch, "[SelfCopy addObject:%s];\n", GetIphoneBaseTypeRead_Vector("data", pVector._ParTName).c_str());
			strVector.append(t_ch);
			//sprintf(t_ch,"%s tt =  %s;\n",TypeConvertToIphoneType(pVector._ParTName).c_str(),ttt_strRead.c_str());
			//strVector.append(t_ch);
		}
		else {
			sprintf(t_ch, "[SelfCopy addObject:%s];\n", GetIphoneBaseTypeRead_Vector("data", pVector._ParTName).c_str());
			strVector.append(t_ch);
			//sprintf(t_ch,"%s* tt =  %s;\n",pVector._ParTName.c_str(),ttt_strRead.c_str());
			//strVector.append(t_ch);
		}
	}
	else {
		sprintf(t_ch, "[SelfCopy addObject:[%s Read:data]];\n", pVector._ParTName.c_str());
		strVector.append(t_ch);
		//sprintf(t_ch,"%s* tt =  [%s Read:data];\n",pVector._ParTName.c_str(),pVector._ParTName.c_str());
		//strVector.append(t_ch);
	}

	//strVector.append(strTable_3);
	//sprintf(t_ch,"[SelfCopy addObject:tt];\n",pVector._ParTName.c_str(),ttt_strRead.c_str());
	//strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_2);
	strVector.append("return SelfCopy;\n");

	strVector.append(strTable);
	strVector.append("}\n");
	fwrite(strVector.c_str(), strVector.length(), 1, fp2);
	strVector = "";


	//写函数
	strVector.append(strTable);
	sprintf(t_ch, "-(NSMutableData*)Write:(NSMutableData*)data\n");
	strVector.append(t_ch);

	strVector.append(strTable);
	strVector.append("{\n");

	strVector.append(strTable_2);
	strVector.append("int num = [self count];\n");

	strVector.append(strTable_2);
	strVector.append("[data appendInt:num];\n"); //将数量写入

	strVector.append(strTable_2);
	strVector.append("for(int i=0;i<num;i++)\n");

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	if (IsIphoneBaseType(pVector._ParTName)) {
		sprintf(t_ch, "[data %s:[((NSNumber*)[self objectAtIndex:i]) %s]];\n", GetIphoneTypeWrite_Vector_Pre(pVector._ParTName).c_str(), GetIphoneTypeWrite_Vector_Next(pVector._ParTName).c_str());
		strVector.append(t_ch);

	}
	else {
		sprintf(t_ch, "%s* sub =(%s*)[self objectAtIndex:i];\n", TypeConvertToIphoneType(pVector._ParTName).c_str(), TypeConvertToIphoneType(pVector._ParTName).c_str());
		strVector.append(t_ch);

		strVector.append(strTable_3);
		strVector.append("[sub Write:data];\n");
	}

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_2);
	strVector.append("return data;\n");

	strVector.append(strTable);
	strVector.append("}\n");
	strVector.append(strTable);
	strVector.append("@end\n\n");

	fwrite(strVector.c_str(), strVector.length(), 1, fp2);
}
void    ProtocolParse_Writer_IPhone::WriteWJSVector_Iphone(stWJSVector& pWJSVector, FILE* fp1, FILE* fp2, int tablenum)
{
	char t_ch[2048];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);
	string strTable_3 = CreateTableString(tablenum + 2);

	//写头
	string strVector;
	strVector.append(strTable);
	sprintf(t_ch, "@interface %s : NSMutableArray{}\n", pWJSVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable);
	sprintf(t_ch, "+(%s*)Read:(NSMutableData*)data;\n", pWJSVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable);
	sprintf(t_ch, "-(NSMutableData*)Write:(NSMutableData*)data;\n");
	strVector.append(t_ch);

	strVector.append(strTable);
	strVector.append("@end\n\n");
	fwrite(strVector.c_str(), strVector.length(), 1, fp1);
	strVector = "";


	//写.m部分
	strVector.append(strTable);
	sprintf(t_ch, "@implementation %s\n", pWJSVector._name.c_str());
	strVector.append(t_ch);

	//strVector.append(strTable);
	//strVector.append("-(id)init\n");

	//fwrite(strVector.c_str(),strVector.length(),1,fp2);

	//strVector="";
	//strVector.append(strTable);
	//strVector.append("{\n");

	//strVector.append(strTable_2);
	//strVector.append("if(self == [super init])\n");

	//strVector.append(strTable_2);
	//strVector.append("{\n");



	//strVector.append(strTable_2);
	//strVector.append("}\n");

	//strVector.append(strTable_2);
	//strVector.append("return self;\n");

	//strVector.append(strTable);
	//strVector.append("}\n");
	fwrite(strVector.c_str(), strVector.length(), 1, fp2);
	strVector = "";



	//读函数
	strVector.append(strTable);
	sprintf(t_ch, "+(%s*)Read:(NSMutableData*)data\n", pWJSVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable);
	strVector.append("{\n");

	strVector.append(strTable_2);
	sprintf(t_ch, "%s* SelfCopy = [[[%s alloc]init]autorelease];\n", pWJSVector._name.c_str(), pWJSVector._name.c_str());
	strVector.append(t_ch);



	//先读长度
	strVector.append(strTable_2);
	strVector.append("int num = [data readInt];\n");

	strVector.append(strTable_2);
	strVector.append("for(int i=0;i<num;i++)\n");

	strVector.append(strTable_2);
	strVector.append("{\n");

	//_ParTName
	strVector.append(strTable_3);
	string ttt_strRead = GetIphoneTypeRead("data", pWJSVector._ParTName);
	if (ttt_strRead.length() > 0) {
		if (IsIphoneBaseType(pWJSVector._ParTName)) {
			//sprintf(t_ch,"%s tt =  [[%s alloc]init];\n",TypeConvertToIphoneType(pVector._ParTName).c_str(),TypeConvertToIphoneType(pVector._ParTName).c_str());

			sprintf(t_ch, "[SelfCopy addObject:%s];\n", ttt_strRead.c_str());
			strVector.append(t_ch);
			//sprintf(t_ch,"%s tt =  %s;\n",TypeConvertToIphoneType(pVector._ParTName).c_str(),ttt_strRead.c_str());
			//strVector.append(t_ch);
		}
		else {
			sprintf(t_ch, "[SelfCopy addObject:%s];\n", ttt_strRead.c_str());
			strVector.append(t_ch);
			//sprintf(t_ch,"%s* tt =  %s;\n",pVector._ParTName.c_str(),ttt_strRead.c_str());
			//strVector.append(t_ch);
		}
	}
	else {
		sprintf(t_ch, "[SelfCopy addObject:[%s Read:data]];\n", pWJSVector._ParTName.c_str());
		strVector.append(t_ch);
		//sprintf(t_ch,"%s* tt =  [%s Read:data];\n",pVector._ParTName.c_str(),pVector._ParTName.c_str());
		//strVector.append(t_ch);
	}

	//strVector.append(strTable_3);
	//sprintf(t_ch,"[SelfCopy addObject:tt];\n",pVector._ParTName.c_str(),ttt_strRead.c_str());
	//strVector.append(t_ch);




	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_2);
	strVector.append("return SelfCopy;\n");

	strVector.append(strTable);
	strVector.append("}\n");
	fwrite(strVector.c_str(), strVector.length(), 1, fp2);
	strVector = "";


	//写函数
	strVector.append(strTable);
	sprintf(t_ch, "-(NSMutableData*)Write:(NSMutableData*)data\n");
	strVector.append(t_ch);

	strVector.append(strTable);
	strVector.append("{\n");

	strVector.append(strTable_2);
	strVector.append("int num = [self count];\n");

	strVector.append(strTable_2);
	strVector.append("[data appendInt:num];\n"); //将数量写入

	strVector.append(strTable_2);
	strVector.append("for(int i=0;i<num;i++)\n");

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	if (IsIphoneBaseType(pWJSVector._ParTName)) {
		sprintf(t_ch, "%s sub =(%s)[self objectAtIndex:i];\n", TypeConvertToIphoneType(pWJSVector._ParTName).c_str(), TypeConvertToIphoneType(pWJSVector._ParTName).c_str());
		strVector.append(t_ch);
		strVector.append(strTable_3);
		sprintf(t_ch, "[%s:sub];\n", GetIphoneTypeWrite("data", pWJSVector._ParTName).c_str());
		strVector.append(t_ch);

	}
	else {
		sprintf(t_ch, "%s* sub =(%s*)[self objectAtIndex:i];\n", TypeConvertToIphoneType(pWJSVector._ParTName).c_str(), TypeConvertToIphoneType(pWJSVector._ParTName).c_str());
		strVector.append(t_ch);

		strVector.append(strTable_3);
		strVector.append("[sub Write:data];\n");
	}

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_2);
	strVector.append("return data;\n");

	strVector.append(strTable);
	strVector.append("}\n");
	///////

	strVector.append(strTable);
	strVector.append("@end\n\n");

	fwrite(strVector.c_str(), strVector.length(), 1, fp2);

}

string  ProtocolParse_Writer_IPhone::TypeConvertToIphoneType(const string& str)
{
	for (int i = 0; i < GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(str.c_str()) == 0) {
			switch (i) {
			case 0://void
			case 1://int
			case 2://unsigned int
			case 3://short
			case 4://unsigned short
			case 5://char


			case 11://float
			case 12://long
			case 13://unsigned long
				return str;


			case 16://wjs_char32
			case 17://wjs_char64
			case 18://wjs_char128
			case 19://wjs_char256
			case 20://wjs_char512
			case 21://wjs_char1024
				return "NSString*";
				//break;
			case 6://byte
			case 7://BYTE
				return "unsigned char";
			case 9://
				return "int";
			case 8://bool
				return "unsigned char";
			case 10://DWORD
				return "unsigned long";
			case 14://string
				return "NSString*";
			case 15://__int64
				return "long long int";
			}
			break;

		}
	}

	if (IsExType_enum(str)) {
		string tt_temp;
		tt_temp = "enum ";
		tt_temp.append(str);
		return tt_temp;
	}

	return str;

}

string  ProtocolParse_Writer_IPhone::TypeConvertToIphoneType_Write(const string& str)
{
	for (int i = 0; i < GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(str.c_str()) == 0) {
			switch (i) {
			case 0://void
			case 1://int
			case 2://unsigned int
			case 3://short
			case 4://unsigned short
			case 5://char


			case 11://float
			case 12://long
			case 13://unsigned long
				return str;


			case 16://wjs_char32
			case 17://wjs_char64
			case 18://wjs_char128
			case 19://wjs_char256
			case 20://wjs_char512
			case 21://wjs_char1024
				return "NSString";
				//break;
			case 6://byte
			case 7://BYTE
				return "unsigned char";
			case 9://
				return "int";
			case 8://bool
				return "unsigned char";
			case 10://DWORD
				return "unsigned long";
			case 14://string
				return "NSString";
			case 15://__int64
				return "long long int";
			}
			break;
		}
	}

	return str;
}

bool    ProtocolParse_Writer_IPhone::IsIphoneBaseType(const string& str)
{
	if (IsBaseType_WriterType(str, eProtocolWriterType::IPhone))
		return true;

	//枚举类型作为基本类型
	if (IsExType_enum(str))
		return true;

	return false;
}

bool    ProtocolParse_Writer_IPhone::IsBaseTypeNeedAddRetain(const string& str)
{
	for (int i = 0; i <  GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(str.c_str()) == 0) {
			switch (i) {
			case 0://void
			case 1://int
			case 2://unsigned int
			case 3://short
			case 4://unsigned short
			case 5://char

			case 9://BOOL	
			case 11://float
			case 12://long
			case 13://unsigned long
			case 6://byte
			case 7://BYTE
			case 8://bool
			case 10://DWORD
			case 15://__int64
				return false;


			case 16://wjs_char32
			case 17://wjs_char64
			case 18://wjs_char128
			case 19://wjs_char256
			case 20://wjs_char512
			case 21://wjs_char1024
			case 14://string
				return true;
			}
			break;

		}
	}

	//枚举类型作为基本类型
	if (IsExType_enum(str))
		return false;

	return true;
}

string  ProtocolParse_Writer_IPhone::GetIphoneTypeRead(const string& datastr, const string& str)
{
	string tt_temp;
	char tt_ch[256];
	for (int i = 0; i <  GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(str.c_str()) == 0) {
			switch (i) {
			case 0://void
				break;
			case 1://int
			{
				sprintf(tt_ch, "[%s readInt]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 2://unsigned int
			{
				sprintf(tt_ch, "[%s readuInt]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 3://short
			{
				sprintf(tt_ch, "[%s readShort]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 4://unsigned short
			{
				sprintf(tt_ch, "[%s readuShort]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 5://char
			{
				sprintf(tt_ch, "[%s readChar]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;

			case 9://BOOL	
			{
				sprintf(tt_ch, "[%s readInt]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 11://float
			{
				sprintf(tt_ch, "[%s readFloat]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 12://long
			{
				sprintf(tt_ch, "[%s readLong]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 13://unsigned long
			{
				sprintf(tt_ch, "[%s readuLong]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;


			case 16://wjs_char32
			{
				sprintf(tt_ch, "[%s readString]", datastr.c_str());
				tt_temp = tt_ch;

			}
			break;
			case 17://wjs_char64
			{
				sprintf(tt_ch, "[%s readString]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 18://wjs_char128
			{
				sprintf(tt_ch, "[%s readString]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 19://wjs_char256
			{
				sprintf(tt_ch, "[%s readString]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 20://wjs_char512
			{
				sprintf(tt_ch, "[%s readString]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 21://wjs_char1024
			{
				sprintf(tt_ch, "[%s readString]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			//break;
			case 6://byte
			{
				sprintf(tt_ch, "[%s readuChar]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 7://BYTE
			{
				sprintf(tt_ch, "[%s readuChar]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 8://bool
			{
				sprintf(tt_ch, "[%s readBOOL]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 10://DWORD
			{
				sprintf(tt_ch, "[%s readuLong]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 14://string
			{
				sprintf(tt_ch, "[%s readString]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 15://__int64
			{
				sprintf(tt_ch, "[%s readInt64]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			}
			break;

		}
	}

	if (IsExType_enum(str)) {
		sprintf(tt_ch, "[%s readInt]", datastr.c_str());
		tt_temp = tt_ch;
	}

	return tt_temp;
}
string  ProtocolParse_Writer_IPhone::GetIphoneBaseTypeRead_Vector(const string& datastr, const string& str)
{
	string tt_temp;
	char tt_ch[256];
	for (int i = 0; i <  GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(str.c_str()) == 0) {
			switch (i) {
			case 0://void
				break;
			case 1://int
			{
				sprintf(tt_ch, "[[NSNumber numberWithInt:[%s readInt]]autorelease]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 2://unsigned int
			{
				sprintf(tt_ch, "[[NSNumber numberWithUnsignedInt:[%s readuInt]]autorelease]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 3://short
			{
				sprintf(tt_ch, "[[NSNumber numberWithShort:[%s readShort]]autorelease]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 4://unsigned short
			{
				sprintf(tt_ch, "[[NSNumber numberWithUnsignedShort:[%s readuShort]]autorelease]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 5://char
			{
				sprintf(tt_ch, "[[NSNumber numberWithChar:[%s readChar]]autorelease]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;

			case 9://BOOL	
			{
				sprintf(tt_ch, "[[NSNumber numberWithInt:[%s readInt]]autorelease]", datastr.c_str());
				tt_temp = tt_ch;

			}
			break;
			case 11://float
			{
				sprintf(tt_ch, "[[NSNumber numberWithFloat:[%s readFloat]]autorelease]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 12://long
			{
				sprintf(tt_ch, "[[NSNumber numberWithLong:[%s readLong]]autorelease]", datastr.c_str());
				tt_temp = tt_ch;

			}
			break;
			case 13://unsigned long
			{
				sprintf(tt_ch, "[[NSNumber numberWithUnsignedLong:[%s readuLong]]autorelease]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;


			case 16://wjs_char32
			{
				sprintf(tt_ch, "[%s readString]", datastr.c_str());
				tt_temp = tt_ch;

			}
			break;
			case 17://wjs_char64
			{
				sprintf(tt_ch, "[%s readString]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 18://wjs_char128
			{
				sprintf(tt_ch, "[%s readString]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 19://wjs_char256
			{
				sprintf(tt_ch, "[%s readString]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 20://wjs_char512
			{
				sprintf(tt_ch, "[%s readString]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 21://wjs_char1024
			{
				sprintf(tt_ch, "[%s readString]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			//break;
			case 6://byte
			{
				sprintf(tt_ch, "[[NSNumber numberWithuChar:[%s readuChar]]autorelease]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 7://BYTE
			{
				sprintf(tt_ch, "[[NSNumber numberWithuChar:[%s readuChar]]autorelease]", datastr.c_str());
				tt_temp = tt_ch;

			}
			break;
			case 8://bool
			{
				sprintf(tt_ch, "[[NSNumber numberWithuBool:[%s readBOOL]]autorelease]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 10://DWORD
			{
				sprintf(tt_ch, "[[NSNumber numberWithUnsignedLong:[%s readuLong]]autorelease]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 14://string
			{
				sprintf(tt_ch, "[%s readString]", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 15://__int64
			{
				sprintf(tt_ch, "[[NSNumber numberWithLongLong:[%s readInt64]]autorelease]", datastr.c_str());
				tt_temp = tt_ch;

			}
			break;
			}
			break;

		}
	}

	if (IsExType_enum(str)) {
		sprintf(tt_ch, "[[NSNumber numberWithInt:[%s readInt]]autorelease]", datastr.c_str());
		tt_temp = tt_ch;
	}

	return tt_temp;
}
string  ProtocolParse_Writer_IPhone::GetIphoneTypeWrite(const string& datastr, const string& str)
{
	string tt_temp;
	char tt_ch[256];
	for (int i = 0; i <  GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(str.c_str()) == 0) {
			switch (i) {
			case 0://void
				break;
			case 1://int
			{
				sprintf(tt_ch, "%s appendInt", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 2://unsigned int
			{
				sprintf(tt_ch, "%s appenduInt", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 3://short
			{
				sprintf(tt_ch, "%s appendShort", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 4://unsigned short
			{
				sprintf(tt_ch, "%s appenduShort", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 5://char
			{
				sprintf(tt_ch, "%s appendChar", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;

			case 9://BOOL	
			{
				sprintf(tt_ch, "%s appendInt", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 11://float
			{
				sprintf(tt_ch, "%s appendFloat", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 12://long
			{
				sprintf(tt_ch, "%s appendLong", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 13://unsigned long
			{
				sprintf(tt_ch, "%s appenduLong", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;


			case 16://wjs_char32
			{
				sprintf(tt_ch, "%s appendString", datastr.c_str());
				tt_temp = tt_ch;

			}
			break;
			case 17://wjs_char64
			{
				sprintf(tt_ch, "%s appendString", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 18://wjs_char128
			{
				sprintf(tt_ch, "%s appendString", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 19://wjs_char256
			{
				sprintf(tt_ch, "%s appendString", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 20://wjs_char512
			{
				sprintf(tt_ch, "%s appendString", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 21://wjs_char1024
			{
				sprintf(tt_ch, "%s appendString", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			//break;
			case 6://byte
			{
				sprintf(tt_ch, "%s appenduChar", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 7://BYTE
			{
				sprintf(tt_ch, "%s appenduChar", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 8://bool
			{
				sprintf(tt_ch, "%s appendBool", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 10://DWORD
			{
				sprintf(tt_ch, "%s appenduLong", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 14://string
			{
				sprintf(tt_ch, "%s appendString", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			case 15://__int64
			{
				sprintf(tt_ch, "%s appendInt64", datastr.c_str());
				tt_temp = tt_ch;
			}
			break;
			}
			break;

		}
	}

	if (IsExType_enum(str)) {
		sprintf(tt_ch, "%s appendInt", datastr.c_str());
		tt_temp = tt_ch;
	}

	return tt_temp;
}

string  ProtocolParse_Writer_IPhone::GetIphoneTypeWrite_Vector_Pre(const string& str)//前部分
{
	string tt_temp;
	//char tt_ch[256];
	for (int i = 0; i <  GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(str.c_str()) == 0) {
			switch (i) {
			case 0://void
				break;
			case 1://int
			{
				tt_temp.append("appendInt");
			}
			break;
			case 2://unsigned int
			{
				tt_temp.append("appenduInt");
			}
			break;
			case 3://short
			{
				tt_temp.append("appendShort");
			}
			break;
			case 4://unsigned short
			{
				tt_temp.append("appenduShort");
			}
			break;
			case 5://char
			{
				tt_temp.append("appendChar");

			}
			break;

			case 9://BOOL	
			{
				tt_temp.append("appendInt");
			}
			break;
			case 11://float
			{
				tt_temp.append("appendFloat");
			}
			break;
			case 12://long
			{
				tt_temp.append("appendLong");

			}
			break;
			case 13://unsigned long
			{
				tt_temp.append("appenduLong");
			}
			break;


			case 16://wjs_char32
			{
				tt_temp.append("appendString");


			}
			break;
			case 17://wjs_char64
			{
				tt_temp.append("appendString");
			}
			break;
			case 18://wjs_char128
			{
				tt_temp.append("appendString");
			}
			break;
			case 19://wjs_char256
			{
				tt_temp.append("appendString");

			}
			break;
			case 20://wjs_char512
			{
				tt_temp.append("appendString");
			}
			break;
			case 21://wjs_char1024
			{
				tt_temp.append("appendString");
			}
			break;
			//break;
			case 6://byte
			{
				tt_temp.append("appenduChar");
			}
			break;
			case 7://BYTE
			{
				tt_temp.append("appenduChar");
			}
			break;
			case 8://bool
			{
				tt_temp.append("appendBool");
			}
			break;
			case 10://DWORD
			{
				tt_temp.append("appenduLong");
			}
			break;
			case 14://string
			{
				tt_temp.append("appendString");
			}
			break;
			case 15://__int64
			{
				tt_temp.append("appendInt64");
			}
			break;
			}
			break;

		}
	}

	if (IsExType_enum(str)) {
		tt_temp.append("appendInt");
	}

	return tt_temp;
}
string  ProtocolParse_Writer_IPhone::GetIphoneTypeWrite_Vector_Next(const string& str)//后部分
{
	string tt_temp;
//	char tt_ch[256];
	for (int i = 0; i <  GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(str.c_str()) == 0) {
			switch (i) {
			case 0://void
				break;
			case 1://int
			{
				tt_temp.append("intValue");
			}
			break;
			case 2://unsigned int
			{
				tt_temp.append("unsignedIntValue");
			}
			break;
			case 3://short
			{
				tt_temp.append("shortValue");
			}
			break;
			case 4://unsigned short
			{
				tt_temp.append("unsignedShortValue");
			}
			break;
			case 5://char
			{
				tt_temp.append("charValue");

			}
			break;

			case 9://BOOL	
			{
				tt_temp.append("intValue");
			}
			break;
			case 11://float
			{
				tt_temp.append("floatValue");
			}
			break;
			case 12://long
			{
				tt_temp.append("longValue");

			}
			break;
			case 13://unsigned long
			{
				tt_temp.append("unsignedLongValue");
			}
			break;


			case 16://wjs_char32
			{
				tt_temp.append("appendString");

			}
			break;
			case 17://wjs_char64
			{
				tt_temp.append("appendString");
			}
			break;
			case 18://wjs_char128
			{
				tt_temp.append("appendString");
			}
			break;
			case 19://wjs_char256
			{
				tt_temp.append("appendString");

			}
			break;
			case 20://wjs_char512
			{
				tt_temp.append("appendString");
			}
			break;
			case 21://wjs_char1024
			{
				tt_temp.append("appendString");
			}
			break;
			//break;
			case 6://byte
			{
				tt_temp.append("unsignedCharValue");
			}
			break;
			case 7://BYTE
			{
				tt_temp.append("unsignedCharValue");
			}
			break;
			case 8://bool
			{
				tt_temp.append("boolValue");
			}
			break;
			case 10://DWORD
			{
				tt_temp.append("unsignedLongValue");
			}
			break;
			case 14://string
			{
				tt_temp.append("appendString");
			}
			break;
			case 15://__int64
			{
				tt_temp.append("longLongValue");
			}
			break;
			}
			break;

		}
	}

	if (IsExType_enum(str)) {
		tt_temp.append("intValue");
	}

	return tt_temp;
}

void    ProtocolParse_Writer_IPhone::WriteClass_Iphone(stClass& pClass, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename, stNamespace& pNamespace)
{
	switch (filetype) {
	case 0:
	{
		////对于客户端而言，request的为发送，callback的为回调
		if (pClass._strtype.compare("callback") == 0) {
			WriteClass_IReceiver_Iphone(pClass, fp1, fp2, tablenum, filetype, spacename);
			WriteClass_Receiver_Iphone(pClass, fp1, fp2, tablenum, filetype, spacename, "");
		}
		else {
			WriteClass_Send_Iphone(pClass, fp1, fp2, tablenum, filetype, spacename);
		}
	}
	break;
	case 1:
	{
		//服务器而言，请求的是回调，callback是发送
		if (pClass._strtype.compare("request") == 0) //C2S
		{
			WriteClass_IReceiver_Iphone(pClass, fp1, fp2, tablenum, filetype, spacename);
			WriteClass_Receiver_Iphone(pClass, fp1, fp2, tablenum, filetype, spacename, "");
		}
		else //S2C
		{
			WriteClass_Send_Iphone(pClass, fp1, fp2, tablenum, filetype, spacename);
		}
	}
	break;
	case 2:
	{
		//取一下callback的类名

		string strcallbackclassname;
		for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
			if (pNamespace._ClassVec[i]._strtype.compare("callback") == 0) {
				strcallbackclassname = pNamespace._ClassVec[i]._name;
				break;
			}
		}

		//事实上两个里面是一样的
		if (pClass._strtype.compare("request") == 0) //C2S
		{
			WriteClass_IReceiver_Iphone(pClass, fp1, fp2, tablenum, filetype, spacename, true);
			WriteClass_Receiver_Iphone(pClass, fp1, fp2, tablenum, filetype, spacename, strcallbackclassname);

			WriteClass_Send_Iphone(pClass, fp1, fp2, tablenum, filetype, spacename);
		}
		else //S2C
		{

		}

	}
	break;
	default:
		break;
	}
}

void    ProtocolParse_Writer_IPhone::WriteClass_IReceiver_Iphone(stClass& pClass, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename, bool bemptyimplement)//写接口类
{
	////////////没有接口
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);

	string strClass;

	strClass.append(strTable);
	sprintf(t_ch, "@protocol  IReceiver_%s\n", spacename.c_str());
	strClass.append(t_ch);


	if (bemptyimplement) {
		strClass.append(strTable);
		strClass.append("@optional\n");
	}
	else {
		strClass.append(strTable);
		strClass.append("@required\n");

	}
	fwrite(strClass.c_str(), strClass.length(), 1, fp1);

	strClass = "";

	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		WriteFunc_IReceiver_Iphone(pClass._FuncVec[i], fp1, fp2, tablenum + 1, filetype, bemptyimplement);
	}

	strClass.append(strTable);
	strClass.append("@end\n\n");

	fwrite(strClass.c_str(), strClass.length(), 1, fp1);
}

void    ProtocolParse_Writer_IPhone::WriteClass_Receiver_Iphone(stClass& pClass, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename, const string& strcallbackclassname)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);

	string strClass;

	//////////写到头
	strClass.append("\n\n");
	strClass.append(strTable);
	sprintf(t_ch, "@interface ReceiveCallback_%s : NSObject{}\n", spacename.c_str());
	strClass.append(t_ch);

	fwrite(strClass.c_str(), strClass.length(), 1, fp1);
	strClass = "";

	//写到.m的
	strClass.append("\n\n");
	strClass.append(strTable);
	sprintf(t_ch, "@implementation ReceiveCallback_%s\n", spacename.c_str());
	strClass.append(t_ch);

	fwrite(strClass.c_str(), strClass.length(), 1, fp2);
	strClass = "";

	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		WriteFunc_Receiver_Iphone(pClass._FuncVec[i], fp1, fp2, tablenum + 1, filetype, spacename);
	}


	//函数
	WriteFunc_ReceiverParser_Iphone(pClass, fp1, fp2, tablenum + 1, filetype, spacename, strcallbackclassname);

	strClass = "";
	strClass.append("\n");
	strClass.append(strTable);
	strClass.append("@end\n\n");
	fwrite(strClass.c_str(), strClass.length(), 1, fp1);

	strClass = "";
	strClass.append("\n");
	strClass.append(strTable);
	strClass.append("@end\n\n");
	fwrite(strClass.c_str(), strClass.length(), 1, fp2);
}

void    ProtocolParse_Writer_IPhone::WriteClass_Send_Iphone(stClass& pClass, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename)//发送
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);

	//////////先写头
	string strClass;
	strClass.append(strTable);
	strClass.append("@interface ");
	switch (filetype) {
	case 0:
	{
		sprintf(t_ch, "SendData_%s", spacename.c_str());
	}
	break;
	case 1:
	{
		sprintf(t_ch, "SendData_%s", spacename.c_str());
	}
	break;
	case 2:
	{
		sprintf(t_ch, "SendData_%s", spacename.c_str());
	}
	break;
	default:
		break;
	}
	strClass.append(t_ch);
	strClass.append(": NSObject{}\n");
	fwrite(strClass.c_str(), strClass.length(), 1, fp1);
	strClass = "";

	//写.m
	strClass.append(strTable);
	strClass.append("@implementation  ");
	switch (filetype) {
	case 0:
	{
		sprintf(t_ch, "SendData_%s\n", spacename.c_str());
	}
	break;
	case 1:
	{
		sprintf(t_ch, "SendData_%s\n", spacename.c_str());
	}
	break;
	case 2:
	{
		sprintf(t_ch, "SendData_%s\n", spacename.c_str());
	}
	break;
	default:
		break;
	}
	strClass.append(t_ch);
	fwrite(strClass.c_str(), strClass.length(), 1, fp2);
	strClass = "";

	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		//写函数
		WriteFunc_SendByteSeq_Iphone(pClass._FuncVec[i], fp1, fp2, tablenum + 1, pClass._name);
	}


	strClass = "";
	strClass.append(strTable);
	strClass.append("@end\n\n");
	fwrite(strClass.c_str(), strClass.length(), 1, fp1);

	strClass = "";
	strClass.append(strTable);
	strClass.append("@end\n\n");
	fwrite(strClass.c_str(), strClass.length(), 1, fp2);
}

void    ProtocolParse_Writer_IPhone::WriteFunc_IReceiver_Iphone(stFunc& pFunc, FILE* fp1, FILE* fp2, int tablenum, int filetype, bool bemptyimplement)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	//	string strTable_2=CreateTableString(tablenum+1);

	string strFunc;
	////做为接收，函数名前+callback
	strFunc.append(strTable);

	//返回类型都改为void型
	sprintf(t_ch, "-(void) IReceiver_%s:", pFunc._name.c_str());
	strFunc.append(t_ch);

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		//if(pFunc._InParVec[i]._strParType.compare("PlayerState")==0)
		//{
		//	int a;
		//	a=10;

		//}
		if (i == 0) {
			string strpar = CovertParToString_Iphone(pFunc._InParVec[i], true);
			strFunc.append(strpar.c_str());
		}
		else {
			string strpar = CovertParToString_Iphone(pFunc._InParVec[i], false);
			strFunc.append(strpar.c_str());
		}
	}


	if (pFunc._InParVec.size() == 0) {
		switch (filetype) {
		case 0:
		{
			//客户端传入一个void* 
			strFunc.append("(NSData*)pExData");
		}
		break;
		case 1:
		{
			//服务器端增另会话号
			strFunc.append("(unsigned int)sessionId pExData:(NSMutableData*)pExData");

		}
		break;
		case 2:
		{
			//服务器端增另会话号
			strFunc.append("(unsigned int)sessionId pExData:(NSMutableData*)pExData");
		}
		break;
		default:
			break;
		}

	}
	else {
		switch (filetype) {
		case 0:
		{
			//客户端传入一个void* 
			strFunc.append("pExData:(NSData*)pExData");
		}
		break;
		case 1:
		{
			//服务器端增另会话号
			strFunc.append("sessionId:(unsigned int)sessionId pExData:(NSMutableData*)pExData");

		}
		break;
		case 2:
		{
			//服务器端增另会话号
			strFunc.append("sessionId:(unsigned int)sessionId pExData:(NSMutableData*)pExData");
		}
		break;
		default:
			break;
		}

	}

	strFunc.append(";\n");

	fwrite(strFunc.c_str(), strFunc.length(), 1, fp1);
}

string ProtocolParse_Writer_IPhone::CovertParToString_Iphone(stBasePar& pPar, bool bFirst)
{
	char ch[256];
	ch[0] = '\0';

	if (IsIphoneBaseType(pPar._strParType)) {
		if (bFirst) {
			sprintf(ch, "(%s)%s ", TypeConvertToIphoneType(pPar._strParType).c_str(), pPar._strParName.c_str());
		}
		else {
			sprintf(ch, "%s:(%s)%s ", pPar._strParName.c_str(), TypeConvertToIphoneType(pPar._strParType).c_str(), pPar._strParName.c_str());
		}
	}
	else {
		if (bFirst) {
			sprintf(ch, "(%s*)%s ", TypeConvertToIphoneType(pPar._strParType).c_str(), pPar._strParName.c_str());
		}
		else {
			sprintf(ch, "%s:(%s*)%s ", pPar._strParName.c_str(), TypeConvertToIphoneType(pPar._strParType).c_str(), pPar._strParName.c_str());
		}
	}


	return ch;
}

void    ProtocolParse_Writer_IPhone::WriteFunc_Receiver_Iphone(stFunc& pFunc, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);

	//先写头文件部分
	string strFunc;
	strFunc.append(strTable);

	//返回类型void
	sprintf(t_ch, "+(void) _parser_%s:(id<IReceiver_%s>)target data:(NSMutableData*)data ", pFunc._name.c_str(), spacename.c_str());
	strFunc.append(t_ch);

	switch (filetype) {
	case 0:
	{
		//sessionId:(unsigned int)sessionId
		sprintf(t_ch, "  pExData:(NSMutableData*)pExData;\n"/*,spacename.c_str()*/);
	}
	break;
	case 1:
	{
		sprintf(t_ch, "sessionId:(unsigned int)sessionId  pExData:(NSMutableData*)pExData;\n"/*,spacename.c_str()*/);
	}
	break;
	case 2:
	{
		sprintf(t_ch, "sessionId:(unsigned int)sessionId  pExData:(NSMutableData*)pExData;\n"/*,spacename.c_str()*/);
	}
	break;
	default:
		break;
	}
	strFunc.append(t_ch);
	//strFunc.append(";");

	fwrite(strFunc.c_str(), strFunc.length(), 1, fp1);
	strFunc = "";

	//写.m
	//+(void)_ParseLoginFail:(id<Client2LoginCallBack>)target Data:(NSMutableData*)data
	strFunc.append(strTable);
	sprintf(t_ch, "+(void) _parser_%s:(id<IReceiver_%s>)target data:(NSMutableData*)data ", pFunc._name.c_str(), spacename.c_str());
	strFunc.append(t_ch);

	switch (filetype) {
	case 0:
	{
		//sessionId:(unsigned int)sessionId
		sprintf(t_ch, "  pExData:(NSMutableData*)pExData\n"/*,spacename.c_str()*/);
	}
	break;
	case 1:
	{
		sprintf(t_ch, "sessionId:(unsigned int)sessionId  pExData:(NSMutableData*)pExData\n"/*,spacename.c_str()*/);
	}
	break;
	case 2:
	{
		sprintf(t_ch, "sessionId:(unsigned int)sessionId  pExData:(NSMutableData*)pExData\n"/*,spacename.c_str()*/);
	}
	break;
	default:
		break;
	}
	strFunc.append(t_ch);
	strFunc.append(strTable);
	strFunc.append("{\n");

	fwrite(strFunc.c_str(), strFunc.length(), 1, fp2);
	strFunc = "";

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strFunc.append(strTable_2);
		string ttt_strRead = GetIphoneTypeRead("data", pFunc._InParVec[i]._strParType);
		if (ttt_strRead.length() > 0) {
			//使用自己提供的读写方法
			if (IsIphoneBaseType(pFunc._InParVec[i]._strParType)) {
				sprintf(t_ch, "%s %s = %s;\n", TypeConvertToIphoneType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str(),
					ttt_strRead.c_str());
			}
			else {
				sprintf(t_ch, "%s* %s = %s;\n", TypeConvertToIphoneType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str(),
					ttt_strRead.c_str());
			}
			strFunc.append(t_ch);

		}
		else {
			//非基本类型提供读写方法
			//先定义一个成员
			//sprintf(t_ch,"%s* %s = [[[%s alloc]init]autorelease];\n",TypeConvertToIphoneType( pFunc._InParVec[i]._strParType).c_str(),pFunc._InParVec[i]._strParName.c_str(),
			//	TypeConvertToIphoneType( pFunc._InParVec[i]._strParType).c_str());
			//strFunc.append(t_ch);
			//strFunc.append(strTable_2);	
			//sprintf(t_ch,"[%s Read:data];\n", pFunc._InParVec[i]._strParName.c_str());
			//strFunc.append(t_ch);

			sprintf(t_ch, "%s* %s = [%s Read:data];\n", TypeConvertToIphoneType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str(),
				TypeConvertToIphoneType(pFunc._InParVec[i]._strParType).c_str());
			strFunc.append(t_ch);
		}
	}

	strFunc.append(strTable_2);
	sprintf(t_ch, "[target IReceiver_%s:", pFunc._name.c_str());
	strFunc.append(t_ch);

	bool tt_first = true;
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		if (tt_first) {
			sprintf(t_ch, "%s ", pFunc._InParVec[i]._strParName.c_str());
			strFunc.append(t_ch);
		}
		else {
			sprintf(t_ch, "%s:%s ", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParName.c_str());
			strFunc.append(t_ch);
		}
		tt_first = false;

	}

	switch (filetype) {
	case 0:
	{
		if (tt_first) {
			//sessionId  pExData
			strFunc.append(":pExData");
		}
		else {
			//sessionId:sessionId
			strFunc.append("  pExData:pExData");
		}

	}
	break;
	case 1:
	{
		if (tt_first) {
			strFunc.append("sessionId  pExData:pExData");
		}
		else {
			strFunc.append("sessionId:sessionId  pExData:pExData");
		}
	}
	break;
	case 2:
	{
		if (tt_first) {
			strFunc.append("sessionId  pExData:pExData");
		}
		else {
			strFunc.append("sessionId:sessionId  pExData:pExData");
		}
	}
	break;
	default:
		break;
	}

	strFunc.append("];\n");
	fwrite(strFunc.c_str(), strFunc.length(), 1, fp2);

	strFunc = "";
	strFunc.append(strTable);
	strFunc.append("}\n");
	fwrite(strFunc.c_str(), strFunc.length(), 1, fp2);
	strFunc = "";

}

void    ProtocolParse_Writer_IPhone::WriteFunc_ReceiverParser_Iphone(stClass& pClass, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename, const string& strcallbackclassname)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);
	string strTable_3 = CreateTableString(tablenum + 2);

	string strFunc;
	strFunc.append(strTable);
	//先写头部分
	sprintf(t_ch, "+(BOOL) Parser:(id<IReceiver_%s>)target data:(NSMutableData*)data ", spacename.c_str());
	strFunc.append(t_ch);

	switch (filetype) {
	case 0:
	{
		//sessionId:(unsigned int)sessionId  
		sprintf(t_ch, "pExData:(NSMutableData*)pExData;\n"/*,spacename.c_str()*/);
		strFunc.append(t_ch);
	}
	break;
	case 1:
	{
		sprintf(t_ch, "sessionId:(unsigned int)sessionId  pExData:(NSMutableData*)pExData;\n"/*,spacename.c_str()*/);
		strFunc.append(t_ch);
	}
	break;
	case 2:
	{
		sprintf(t_ch, "sessionId:(unsigned int)sessionId  pExData:(NSMutableData*)pExData;\n"/*,spacename.c_str()*/);
		strFunc.append(t_ch);
	}
	break;
	default:
		break;
	}

	//strFunc.append(t_ch);
	//strFunc.append(";");

	fwrite(strFunc.c_str(), strFunc.length(), 1, fp1);
	strFunc = "";

	//写.m
	strFunc.append(strTable);
	sprintf(t_ch, "+(BOOL) Parser:(id<IReceiver_%s>)target data:(NSMutableData*)data ", spacename.c_str());
	strFunc.append(t_ch);

	switch (filetype) {
	case 0:
	{
		//sessionId:(unsigned int)sessionId
		sprintf(t_ch, "  pExData:(NSMutableData*)pExData\n"/*,spacename.c_str()*/);
		strFunc.append(t_ch);
	}
	break;
	case 1:
	{
		sprintf(t_ch, "sessionId:(unsigned int)sessionId  pExData:(NSMutableData*)pExData\n"/*,spacename.c_str()*/);
		strFunc.append(t_ch);
	}
	break;
	case 2:
	{
		sprintf(t_ch, "sessionId:(unsigned int)sessionId  pExData:(NSMutableData*)pExData\n"/*,spacename.c_str()*/);
		strFunc.append(t_ch);
	}
	break;
	default:
		break;
	}

	fwrite(strFunc.c_str(), strFunc.length(), 1, fp2);
	strFunc = "";

	//写左中括号
	strFunc.append(strTable);
	strFunc.append("{\n");
	fwrite(strFunc.c_str(), strFunc.length(), 1, fp2);
	strFunc = "";

	//先读协议号
	strFunc.append(strTable_2);
	strFunc.append("int prono=[data readInt];\n"); //先读出协议号
	strFunc.append(strTable_2);
	sprintf(t_ch, "if(prono!=%s)\n", m_ProtocolNo._name.c_str());
	strFunc.append(t_ch);
	strFunc.append(strTable_3);
	strFunc.append("return NO;\n");

	//再读方法号
	strFunc.append(strTable_2);
	strFunc.append("int methodid=[data readInt];\n");
	fwrite(strFunc.c_str(), strFunc.length(), 1, fp2);
	strFunc = "";
	bool tt_bFirst = true;

	//写另外一部分
	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		strFunc.append(strTable_2);
		if (tt_bFirst) {
			sprintf(t_ch, "if(methodid == ID_%s_%s)\n", pClass._name.c_str(), pClass._FuncVec[i]._name.c_str());
			strFunc.append(t_ch);
		}
		else {
			sprintf(t_ch, "else if(methodid == ID_%s_%s)\n", pClass._name.c_str(), pClass._FuncVec[i]._name.c_str());
			strFunc.append(t_ch);
		}

		strFunc.append(strTable_2);
		strFunc.append("{\n");


		strFunc.append(strTable_3);
		switch (filetype) {
		case 0:
		{
			//sessionId:sessionId
			sprintf(t_ch, "[self _parser_%s:target data:data  pExData:pExData];\n", pClass._FuncVec[i]._name.c_str());
		}
		break;
		case 1:
		{
			sprintf(t_ch, "[self _parser_%s:target data:data sessionId:sessionId pExData:pExData];\n", pClass._FuncVec[i]._name.c_str());
		}
		break;
		case 2:
		{
			sprintf(t_ch, "[self _parser_%s:target data:data sessionId:sessionId pExData:pExData];\n", pClass._FuncVec[i]._name.c_str());
		}
		break;
		default:
			break;
		}
		strFunc.append(t_ch);
		strFunc.append(strTable_3);
		strFunc.append("return YES;\n");
		strFunc.append(strTable_2);
		strFunc.append("}\n");
		tt_bFirst = false;

	}


	strFunc.append(strTable_2);
	strFunc.append("return NO;\n");


	fwrite(strFunc.c_str(), strFunc.length(), 1, fp2);


	//写右中括号
	strFunc = "";
	strFunc.append(strTable);
	strFunc.append("}\n");
	fwrite(strFunc.c_str(), strFunc.length(), 1, fp2);
	strFunc = "";

}

void    ProtocolParse_Writer_IPhone::WriteFunc_SendByteSeq_Iphone(stFunc& pFunc, FILE* fp1, FILE* fp2, int tablenum, string classname)
{
	char t_ch[2048];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);

	string strFunc;
	strFunc.append(strTable);
	strFunc.append("+(NSMutableData*) ");

	if (pFunc._InParVec.size() > 0)
		sprintf(t_ch, "Send_%s:", pFunc._name.c_str());
	else
		sprintf(t_ch, "Send_%s", pFunc._name.c_str());

	strFunc.append(t_ch);

	bool tt_bFirst = true;

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		if (tt_bFirst) {
			if (IsIphoneBaseType(pFunc._InParVec[i]._strParType)) {
				sprintf(t_ch, "(%s)%s ", TypeConvertToIphoneType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
				strFunc.append(t_ch);
			}
			else {
				sprintf(t_ch, "(%s*)%s ", TypeConvertToIphoneType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
				strFunc.append(t_ch);
			}
		}
		else {
			if (IsIphoneBaseType(pFunc._InParVec[i]._strParType)) {
				sprintf(t_ch, "%s:(%s)%s ", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToIphoneType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
				strFunc.append(t_ch);
			}
			else {
				sprintf(t_ch, "%s:(%s*)%s ", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToIphoneType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
				strFunc.append(t_ch);
			}
		}



		tt_bFirst = false;
	}

	strFunc.append(";\n");

	fwrite(strFunc.c_str(), strFunc.length(), 1, fp1);
	strFunc = "";

	////////////////再写.m
	strFunc.append(strTable);
	strFunc.append("+(NSMutableData*) ");

	if (pFunc._InParVec.size() > 0)
		sprintf(t_ch, "Send_%s:", pFunc._name.c_str());
	else
		sprintf(t_ch, "Send_%s", pFunc._name.c_str());

	strFunc.append(t_ch);

	tt_bFirst = true;

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		if (tt_bFirst) {
			if (IsIphoneBaseType(pFunc._InParVec[i]._strParType)) {
				sprintf(t_ch, "(%s)%s ", TypeConvertToIphoneType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
				strFunc.append(t_ch);
			}
			else {
				sprintf(t_ch, "(%s*)%s ", TypeConvertToIphoneType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
				strFunc.append(t_ch);
			}
		}
		else {
			if (IsIphoneBaseType(pFunc._InParVec[i]._strParType)) {
				sprintf(t_ch, "%s:(%s)%s ", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToIphoneType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
				strFunc.append(t_ch);
			}
			else {
				sprintf(t_ch, "%s:(%s*)%s ", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToIphoneType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
				strFunc.append(t_ch);
			}
		}

		tt_bFirst = false;
	}
	strFunc.append("\n");
	fwrite(strFunc.c_str(), strFunc.length(), 1, fp2);
	strFunc = "";

	strFunc.append(strTable);
	strFunc.append("{\n");

	strFunc.append(strTable_2);
	strFunc.append("NSMutableData *data = [[[NSMutableData alloc]init] autorelease];\n");

	strFunc.append(strTable_2);
	sprintf(t_ch, "[data appenduInt:%s];\n", m_ProtocolNo._name.c_str());
	strFunc.append(t_ch);

	strFunc.append(strTable_2);
	sprintf(t_ch, "[data appenduInt:ID_%s_%s];\n", classname.c_str(), pFunc._name.c_str());
	strFunc.append(t_ch);

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strFunc.append(strTable_2);

		string strWrite = GetIphoneTypeWrite("data", pFunc._InParVec[i]._strParType);
		if (strWrite.length() > 0) {
			sprintf(t_ch, "[%s:%s];\n", strWrite.c_str(), pFunc._InParVec[i]._strParName.c_str());
			strFunc.append(t_ch);
		}
		else {
			//sprintf(t_ch,"[%s Write:data %s:%s];\n",TypeConvertToIphoneType_Write(pFunc._InParVec[i]._strParType).c_str(),pFunc._InParVec[i]._strParName.c_str(),pFunc._InParVec[i]._strParName.c_str());
			//strFunc.append(t_ch);
			sprintf(t_ch, "data = [%s Write:data];\n", pFunc._InParVec[i]._strParName.c_str());
			strFunc.append(t_ch);
		}
	}


	strFunc.append(strTable_2);
	strFunc.append("return data;\n");
	fwrite(strFunc.c_str(), strFunc.length(), 1, fp2);

	strFunc = "";
	strFunc.append(strTable);
	strFunc.append("}\n");
	fwrite(strFunc.c_str(), strFunc.length(), 1, fp2);
}