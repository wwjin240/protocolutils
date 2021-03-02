#include "ProtocolParse_Writer_TypeScript.h"
#include "ProtocolParse.h"
#include <time.h>
#include <sys/timeb.h>

extern string g_strBaseType[];

bool    ProtocolParse_Writer_TypeScript::CreateFile_TScript_Model(string fileName, int filetype, bool bRefProtocol)
{
	//bool tt_bNotSaveFile=false;
	//先读取文件，判定MD5是否相同，相同则不用再生成
	//if(TestFileMd5(filename))
	//{
	//if(bRefProtocol)
	//{
	//	tt_bNotSaveFile=true;
	//}
	//	else
	//		return true;
	//}

	//string tt_fullfoldername = foldername;
	//tt_fullfoldername.append("\\");
	////首先创建文件夹
	//CheckDirectoryEixst(tt_fullfoldername.c_str());

	CheckDirectoryEixst(fileName.c_str());

	FILE* fp = fopen(fileName.c_str(), "wb+");
	if (!fp)
		return false;

	//m_ImportPacketNameList_TS_Model

	string tt_strLine;
	char tt_ch[512];

	if (m_pRefProtocolParseList.size() == 0) {
		//写信息头,共用
		WriteInfoHead(fp);

		tt_strLine = "import InStream = yy.InStream;\n";
		fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
		tt_strLine = "import OutStream = yy.OutStream;\n";
		fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
		tt_strLine = "import PArray = yy.PArray;\n";
		fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
		tt_strLine = "import Int64 = yy.Int64;\n\n";
		fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);


	}

	for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
		string tt_strTempName;
		switch (filetype) {
		case 0:
		{
			tt_strTempName = m_pRefProtocolParseList[i]->m_SaveFileName._clientname2;

		}
		break;
		case 1:
		{
			tt_strTempName = m_pRefProtocolParseList[i]->m_SaveFileName._servername2;
		}
		break;
		case 2:
		{
			tt_strTempName = m_pRefProtocolParseList[i]->m_SaveFileName._servername2;

		}
		break;
		default:
			break;
		}

		size_t tt_pos = tt_strTempName.rfind(".");
		if (tt_pos != string::npos)
			tt_strTempName = tt_strTempName.substr(0, tt_pos);
		sprintf(tt_ch, "import * as %s from  \"./%s\";\n", m_pRefProtocolParseList[i]->GetShortName().c_str(), tt_strTempName.c_str());

		tt_strLine = tt_ch;
		fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
	}
	//写命名空间
	for (size_t i = 0; i < m_NamespaceVec.size(); i++) {
		WriteOneNameSpace_TScript_Model(m_NamespaceVec[i], filetype, "", fp);
	}

	if (m_pRefProtocolParseList.size() > 0) {
		unsigned int tt_len = ftell(fp);
		//fflush(fp);
		char* tt_pBuf = new char[tt_len + 1];
		tt_pBuf[tt_len] = 0;
		fseek(fp, 0, SEEK_SET);
		size_t tt = fread(tt_pBuf, tt_len, 1, fp);
		fseek(fp, 0, SEEK_SET);

		//写信息头,共用
		WriteInfoHead(fp);

		tt_strLine = "import InStream = yy.InStream;\n";
		fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
		tt_strLine = "import OutStream = yy.OutStream;\n";
		fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
		tt_strLine = "import PArray = yy.PArray;\n";
		fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
		tt_strLine = "import Int64 = yy.Int64;\n\n";
		fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);



		for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
			string tt_strTempName;
			switch (filetype) {
			case 0:
			{
				tt_strTempName = m_pRefProtocolParseList[i]->m_SaveFileName._clientname2;

			}
			break;
			case 1:
			{
				tt_strTempName = m_pRefProtocolParseList[i]->m_SaveFileName._servername2;
			}
			break;
			case 2:
			{
				tt_strTempName = m_pRefProtocolParseList[i]->m_SaveFileName._servername2;

			}
			break;
			default:
				break;
			}

			size_t tt_pos = tt_strTempName.rfind(".");
			if (tt_pos != string::npos)
				tt_strTempName = tt_strTempName.substr(0, tt_pos);
			sprintf(tt_ch, "import * as %s from  \"./%s\";\n", m_pRefProtocolParseList[i]->GetShortName().c_str(), tt_strTempName.c_str());

			tt_strLine = tt_ch;
			fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
		}
		for (unsigned int i = 0; i < m_ImportPacketNameList_TS_Model.size(); i++) {
			for (unsigned int j = i + 1; j < m_ImportPacketNameList_TS_Model.size();) {
				if (m_ImportPacketNameList_TS_Model[i].compare(m_ImportPacketNameList_TS_Model[j].c_str()) == 0) {
					m_ImportPacketNameList_TS_Model.erase(m_ImportPacketNameList_TS_Model.begin() + j);
				}
				else
					j++;
			}
		}
		for (unsigned int i = 0; i < m_ImportPacketNameList_TS_Model.size(); i++) {
			tt_strLine = m_ImportPacketNameList_TS_Model[i];
			tt_strLine.append("\n");
			fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
		}

		fwrite(tt_pBuf, tt_len, 1, fp);
		delete[] tt_pBuf;
	}


	fclose(fp);
	return true;
}
bool    ProtocolParse_Writer_TypeScript::WriteOneNameSpace_TScript_Model(stNamespace& pNamespace, int filetype, string foldername, FILE* fp)
{
	m_tablenum = 0;

	//char ch[256];

	string tt_strWriteNameSpace = "";
	switch (filetype) {
	case 0:
		tt_strWriteNameSpace = pNamespace._clientname.c_str();
		break;
	case 1:
		tt_strWriteNameSpace = pNamespace._servername.c_str();
		break;
	case 2:
		tt_strWriteNameSpace = pNamespace._servername.c_str();
		break;
	default:
		break;
	}

	////写函数的id,和协议号
	CreateAndWriteFuncIdAndProtocolNo_TScript_Model(pNamespace, foldername, tt_strWriteNameSpace, fp);


	//先写枚举
	for (size_t i = 0; i < pNamespace._EnumVec.size(); i++) {
		WriteEnum_TScript_Model(pNamespace, pNamespace._EnumVec[i], foldername, tt_strWriteNameSpace, fp);
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
				WriteStruct_TScript_Model(pNamespace, pNamespace._StructVec[structindex], foldername, tt_strWriteNameSpace, fp);
				structindex++;
			}
			else if (pNamespace._VectorVec[vectorindex]._index < pNamespace._StructVec[structindex]._index
				&& (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index)) {
				//写vector
				WriteVector_TScript_Model(pNamespace, pNamespace._VectorVec[vectorindex], foldername, tt_strWriteNameSpace, fp);
				vectorindex++;
			}
			else {
				//写类vector
				WriteWJSVector_TScript_Model(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername, tt_strWriteNameSpace, fp);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend && (!bstructend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._VectorVec[vectorindex]._index) {
				//写结构体
				WriteStruct_TScript_Model(pNamespace, pNamespace._StructVec[structindex], foldername, tt_strWriteNameSpace, fp);
				structindex++;
			}
			else {
				//写vector
				WriteVector_TScript_Model(pNamespace, pNamespace._VectorVec[vectorindex], foldername, tt_strWriteNameSpace, fp);
				vectorindex++;
			}
		}
		else if (!bvectorend && (!bwjsvectorend)) {
			if (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写vector
				WriteVector_TScript_Model(pNamespace, pNamespace._VectorVec[vectorindex], foldername, tt_strWriteNameSpace, fp);
				vectorindex++;
			}
			else {
				//写类vector
				WriteWJSVector_TScript_Model(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername, tt_strWriteNameSpace, fp);
				wjsvectorindex++;
			}
		}
		else if ((!bstructend) && (!bwjsvectorend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写结构体
				WriteStruct_TScript_Model(pNamespace, pNamespace._StructVec[structindex], foldername, tt_strWriteNameSpace, fp);
				structindex++;
			}
			else {
				//写类vector
				WriteWJSVector_TScript_Model(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername, tt_strWriteNameSpace, fp);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend) {
			//写vector
			WriteVector_TScript_Model(pNamespace, pNamespace._VectorVec[vectorindex], foldername, tt_strWriteNameSpace, fp);
			vectorindex++;

		}
		else if (!bstructend) {
			//写结构体
			WriteStruct_TScript_Model(pNamespace, pNamespace._StructVec[structindex], foldername, tt_strWriteNameSpace, fp);
			structindex++;
		}
		else if (!bwjsvectorend) {
			//写类vector
			WriteWJSVector_TScript_Model(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername, tt_strWriteNameSpace, fp);
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
		WriteClass_TScript_Model(pNamespace._ClassVec[i], filetype, pNamespace, foldername, tt_strWriteNameSpace, fp);
	}

	return true;
}
void    ProtocolParse_Writer_TypeScript::CreateAndWriteFuncIdAndProtocolNo_TScript_Model(stNamespace& pNamespace, string foldername, const string& strWriteNameSpace, FILE* fp)
{
	if (pNamespace._ClassVec.size() == 0)
		return;

	char t_ch[512];


	string strid;

	//string strTable_0 = CreateTableString(0);
	string strTable_1 = CreateTableString(0);

	//函数id
	for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
		for (size_t j = 0; j < pNamespace._ClassVec[i]._FuncVec.size(); j++) {
			strid.append(strTable_1);
			strid.append("///export const  ");
			sprintf(t_ch, "ID_%s_%s:number", pNamespace._ClassVec[i]._name.c_str(), pNamespace._ClassVec[i]._FuncVec[j]._name.c_str());
			strid.append(t_ch);

			sprintf(t_ch, "ID_%s_%s", pNamespace._ClassVec[i]._name.c_str(), pNamespace._ClassVec[i]._FuncVec[j]._name.c_str());

			int tempId;  //将字符串转成id
			if (pNamespace._ClassVec[i]._FuncVec[j]._userFuncId != 0)
				tempId = pNamespace._ClassVec[i]._FuncVec[j]._userFuncId;
			else
				tempId = CovertStringToInt(t_ch, pNamespace._ClassVec[i]._FuncVec[j]._bNew);
			sprintf(t_ch, "=%d;\n", tempId);
			strid.append(t_ch);
		}
	}

	//协议号
	if (m_ProtocolNo._value != 1) {
		//协议头
		strid.append(strTable_1);
		strid.append("export const ");
		sprintf(t_ch, "%s:number=%d;\n\n", m_ProtocolNo._name.c_str(), m_ProtocolNo._value);
		strid.append(t_ch);
	}



	fwrite(strid.c_str(), strid.length(), 1, fp);

}
void    ProtocolParse_Writer_TypeScript::WriteEnum_TScript_Model(stNamespace& pNamespace, stEnum& pEnum, string foldername, const string& strWriteNameSpace, FILE* fp)
{
	char t_ch[512];

	string strTable_0 = CreateTableString(0);
	string strTable_1 = CreateTableString(1);
	string strTable_2 = CreateTableString(2);
	string strTable_3 = CreateTableString(3);
	string strTable_4 = CreateTableString(4);
	string strTable_5 = CreateTableString(5);
	//string strTable_6=CreateTableString(5);

	string strEnum;


	strEnum.append(WriteComment_TScript(strTable_0, pEnum._comment, "").c_str());

	strEnum.append(strTable_0.c_str());
	sprintf(t_ch, "export class %s{\n", pEnum._name.c_str());
	strEnum.append(t_ch);

	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		strEnum.append(WriteComment_TScript(strTable_1, pEnum._commentList[i], "").c_str());
		strEnum.append(strTable_1.c_str());
		sprintf(t_ch, "static readonly %s = %d;\n", pEnum._ParNameList[i].c_str(), i);
		strEnum.append(t_ch);
	}





	//写读
	strEnum.append(strTable_1.c_str());
	strEnum.append("static read_s(outs:OutStream): number{\n");

	strEnum.append(strTable_2.c_str());
	strEnum.append("let v = outs.readInt32();\n");

	strEnum.append(strTable_2.c_str());
	strEnum.append("switch(v){\n");

	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		if (pEnum._bNewVersion && (!pEnum._keepOldList[i])) {
			sprintf(t_ch, "%scase %d:\n", strTable_3.c_str(), i);
			strEnum.append(t_ch);

			sprintf(t_ch, "%sreturn this.%s;\n", strTable_4.c_str(), pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);
		}
		else {
			sprintf(t_ch, "%scase %d:\n", strTable_3.c_str(), CovertStringToInt(pEnum._ParNameList[i]));
			strEnum.append(t_ch);

			sprintf(t_ch, "%sreturn this.%s;\n", strTable_4.c_str(), pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);
		}

	}


	strEnum.append(strTable_3.c_str());
	strEnum.append("default:\n");

	if (pEnum._ParNameList.size() > 0) {
		sprintf(t_ch, "%sreturn this.%s;\n", strTable_4.c_str(), pEnum._ParNameList[0].c_str());
		strEnum.append(t_ch);
	}
	else {
		strEnum.append(strTable_4.c_str());
		strEnum.append("return v;\n");
	}

	strEnum.append(strTable_2.c_str());
	strEnum.append("}\n");

	strEnum.append(strTable_1.c_str());
	strEnum.append("}\n");

	////////
	//
	///////
	//写写
	strEnum.append(strTable_1.c_str());
	//sprintf(t_ch, "static  write(ins:InStream,v:number){\n", pEnum._name);
	strEnum.append("static  write(ins:InStream,v:number){\n");

	strEnum.append(strTable_2.c_str());
	strEnum.append("switch(v){\n");

	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		if (pEnum._bNewVersion && (!pEnum._keepOldList[i])) {
			sprintf(t_ch, "%scase this.%s:\n", strTable_3.c_str(), pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);

			sprintf(t_ch, "%sins.writeInt32(%d);\n", strTable_4.c_str(), i);
			strEnum.append(t_ch);

			strEnum.append(strTable_4.c_str());
			strEnum.append("return;\n");
		}
		else {
			sprintf(t_ch, "%scase this.%s:\n", strTable_3.c_str(), pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);

			sprintf(t_ch, "%sins.writeInt32(%d);\n", strTable_4.c_str(), CovertStringToInt(pEnum._ParNameList[i]));
			strEnum.append(t_ch);

			strEnum.append(strTable_4.c_str());
			strEnum.append("return;\n");
		}

	}

	strEnum.append(strTable_3.c_str());
	strEnum.append("default:\n");

	strEnum.append(strTable_4.c_str());
	strEnum.append("ins.writeInt32(v);\n");
	strEnum.append(strTable_4.c_str());
	strEnum.append("return;\n");


	strEnum.append(strTable_2.c_str());
	strEnum.append("}\n");

	strEnum.append(strTable_1.c_str());
	strEnum.append("}\n");

	///////类结束
	strEnum.append(strTable_0.c_str());
	strEnum.append("}\n\n");

	fwrite(strEnum.c_str(), strEnum.length(), 1, fp);
}

void    ProtocolParse_Writer_TypeScript::WriteStruct_TScript_Model(stNamespace& pNamespace, stStructStruct& pStruct, string foldername, const string& strWriteNameSpace, FILE* fp)
{
	if (pStruct._inherited.length() > 0)
		return;
	if (!pStruct._bBiaoZhuUsed)
		return;
	char t_ch[512];

	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);
	string strTable_4 = CreateTableString(3);
	string strTable_5 = CreateTableString(4);
	string strTable_6 = CreateTableString(5);

	string strStruct;

	vector<string> tt_PacketNameList_Ref;
	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (!IsTScriptBaseType(pStruct._ParVec[i]._strParType, false, true) || IsExType_enum(pStruct._ParVec[i]._strParType)) {
			GetUsePacketNamesByParType_TScript_reference(pStruct._ParVec[i]._strParType, tt_PacketNameList_Ref, GetSubFolderName(foldername));
		}
	}
	//for (int i = 0; i < tt_PacketNameList_Ref.size(); i++)
	//{
	//	m_ImportPacketNameList_TS_Model.push_back(tt_PacketNameList_Ref[i]);
	//	//strStruct.append(tt_PacketNameList_Ref[i].c_str());
	//	//strStruct.append("\n");
	//}
	//if (tt_PacketNameList_Ref.size() > 0)
	//	strStruct.append("\n");

	vector<string> tt_PacketNameList;
	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (!IsTScriptBaseType(pStruct._ParVec[i]._strParType, false, true)) {
			GetUsePacketNamesByParType_TScript_NSV(pStruct._ParVec[i]._strParType, tt_PacketNameList, GetSubFolderName(foldername));
		}
	}
	for (size_t i = 0; i < tt_PacketNameList.size(); i++) {
		m_ImportPacketNameList_TS_Model.push_back(tt_PacketNameList[i]);
		//strStruct.append(strTable_1);
		//strStruct.append(tt_PacketNameList[i].c_str());
		//strStruct.append("\n");
	}
	//if (tt_PacketNameList.size() > 0)
	//	strStruct.append("\n");



	strStruct.append(WriteComment_TScript(strTable_1, pStruct._comment, "").c_str());
	strStruct.append(strTable_1);
	sprintf(t_ch, "export class %s\n", pStruct._name.c_str());
	strStruct.append(t_ch);

	strStruct.append(strTable_1);
	strStruct.append("{\n");

	if (pStruct._bWithConstruct) {
		strStruct.append(strTable_2);
		strStruct.append("constructor()\n");
		strStruct.append(strTable_2);
		strStruct.append("{\n");

		for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
			if (pStruct._ParVec[i]._bWithDefault) {
				strStruct.append(strTable_3);
				sprintf(t_ch, "this.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), ConvertDefaultValue(pStruct._ParVec[i]._strParType, pStruct._ParVec[i]._strDefaultValue).c_str());
				strStruct.append(t_ch);
			}
		}

		strStruct.append(strTable_2);
		strStruct.append("}\n");
	}


	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		strStruct.append(WriteComment_TScript(strTable_2, pStruct._ParVec[i]._comment, "").c_str());
		strStruct.append(strTable_2);
		if (IsExType_vector(pStruct._ParVec[i]._strParType)) {
			sprintf(t_ch, "%s:%s;\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str());
		}
		else {
			sprintf(t_ch, "%s:%s;\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str());
		}
		strStruct.append(t_ch);
	}

	//写读
	strStruct.append(strTable_2);
	sprintf(t_ch, "static  read_s(outs:OutStream):%s\n", pStruct._name.c_str());
	strStruct.append(t_ch);
	strStruct.append(strTable_2);
	strStruct.append("{\n");

	strStruct.append(strTable_3);
	sprintf(t_ch, "let o:%s = new %s();\n", pStruct._name.c_str(), pStruct._name.c_str());
	strStruct.append(t_ch);

	strStruct.append(strTable_3);
	sprintf(t_ch, "o.read(outs);\n");
	strStruct.append(t_ch);

	strStruct.append(strTable_3);
	strStruct.append("return o;\n");

	strStruct.append(strTable_2);
	strStruct.append("}\n");

	//////////////////
	////
	/////////////////

	//写读
	strStruct.append(strTable_2);
	sprintf(t_ch, "public read(outs:OutStream)\n");
	strStruct.append(t_ch);
	strStruct.append(strTable_2);
	strStruct.append("{\n");

	strStruct.append(strTable_3);
	strStruct.append("let tt_len:number =0;\n");

	strStruct.append(strTable_3);
	strStruct.append("tt_len = outs.readInt16();\n");

	strStruct.append(strTable_3);
	strStruct.append("let tt_version:number=0;\n");

	strStruct.append(strTable_3);
	strStruct.append("tt_version = outs.readInt16();\n");

	strStruct.append(strTable_3);
	strStruct.append("let tt_logsize1:number = 0;\n");

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_3);
		strStruct.append("tt_logsize1 = outs.getPos();\n");
	}

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_3);
		strStruct.append("let tt_transNeedValue:number=0;\n");

		strStruct.append(strTable_3);
		strStruct.append("tt_transNeedValue = outs.readInt32();\n");
	}


	strStruct.append(strTable_3);
	sprintf(t_ch, "if(tt_version >= %d || (tt_version==0)){\n", pStruct._version);
	strStruct.append(t_ch);

	if (!pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_4);
		sprintf(t_ch, "if(tt_version > %d)\n", pStruct._version);
		strStruct.append(t_ch);
		strStruct.append(strTable_5);
		strStruct.append("tt_logsize1 = outs.getPos();\n");
	}



	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1)) {
			strStruct.append(strTable_4);
			sprintf(t_ch, "if((tt_transNeedValue & (1<<%d))!=0)\n", pStruct._ParVec[i]._transNeedValue);
			strStruct.append(t_ch);
			strStruct.append(strTable_4);
			strStruct.append("{\n");

			strStruct.append(strTable_5);
			if (IsTScriptBaseType(pStruct._ParVec[i]._strParType, true)) {
				if (IsExType_enum(pStruct._ParVec[i]._strParType))
					sprintf(t_ch, "this.%s = %s.read_s(outs)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				else
					sprintf(t_ch, "this.%s = outs.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str());
			}
			else {
				sprintf(t_ch, "this.%s = %s.read_s(outs);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());

			}
			strStruct.append(t_ch);

			strStruct.append(strTable_4);
			strStruct.append("}\n");
			strStruct.append(strTable_4);
			strStruct.append("else\n");
			strStruct.append(strTable_4);
			strStruct.append("{\n");
			strStruct.append(strTable_5);
			sprintf(t_ch, "this.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), ConvertDefaultValue(pStruct._ParVec[i]._strParType, pStruct._ParVec[i]._strDefaultValue).c_str());
			strStruct.append(t_ch);
			strStruct.append(strTable_4);
			strStruct.append("}\n");
		}
		else {
			strStruct.append(strTable_4);
			if (IsTScriptBaseType(pStruct._ParVec[i]._strParType, true)) {
				if (IsExType_enum(pStruct._ParVec[i]._strParType))
					sprintf(t_ch, "this.%s = %s.read_s(outs)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				else
					sprintf(t_ch, "this.%s = outs.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str());
			}
			else {
				sprintf(t_ch, "this.%s = %s.read_s(outs);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			strStruct.append(t_ch);
		}


	}

	strStruct.append(strTable_4);
	sprintf(t_ch, "if(tt_version > %d){\n", pStruct._version);
	strStruct.append(t_ch);

	strStruct.append(strTable_5);
	strStruct.append("let tt_logsize2:number = outs.getPos();\n");

	strStruct.append(strTable_5);
	strStruct.append("let tt_offset:number = tt_len - (tt_logsize2 - tt_logsize1);\n");

	strStruct.append(strTable_5);
	strStruct.append("if(tt_offset>0)\n");

	strStruct.append(strTable_5);
	strStruct.append("outs.addOffset(tt_offset);\n");

	strStruct.append(strTable_4);
	strStruct.append("}\n");

	strStruct.append(strTable_3);
	strStruct.append("}\n");

	strStruct.append(strTable_3);
	strStruct.append("else{\n");

	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		string strTable_5_temp = strTable_5;
		string strTable_6_temp = strTable_6;
		bool tt_bNeedVersionCheck = true;
		if (pStruct._ParVec[i]._version <= 1 || pStruct._ParVec[i]._version <= pStruct._startversion_ts) {
			tt_bNeedVersionCheck = false;
			strTable_5_temp = strTable_4;
			strTable_6_temp = strTable_5;
		}



		if (tt_bNeedVersionCheck) {
			strStruct.append(strTable_4);
			sprintf(t_ch, "if(tt_version>=%d)\n", pStruct._ParVec[i]._version);
			strStruct.append(t_ch);
		}

		if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1)) {
			if (tt_bNeedVersionCheck) {
				strStruct.append(strTable_4);
				strStruct.append("{\n");
			}

			strStruct.append(strTable_5_temp);
			sprintf(t_ch, "if((tt_transNeedValue & (1<<%d))!=0)\n", pStruct._ParVec[i]._transNeedValue);
			strStruct.append(t_ch);
			strStruct.append(strTable_5_temp);
			strStruct.append("{\n");

			strStruct.append(strTable_6_temp);
			if (IsTScriptBaseType(pStruct._ParVec[i]._strParType, true)) {
				if (IsExType_enum(pStruct._ParVec[i]._strParType))
					sprintf(t_ch, "this.%s = %s.read_s(outs)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				else
					sprintf(t_ch, "this.%s = outs.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str());

			}
			else {
				sprintf(t_ch, "this.%s = %s.read_s(outs);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			strStruct.append(t_ch);

			strStruct.append(strTable_5_temp);
			strStruct.append("}\n");
			strStruct.append(strTable_5_temp);
			strStruct.append("else\n");
			strStruct.append(strTable_5_temp);
			strStruct.append("{\n");
			strStruct.append(strTable_6_temp);
			sprintf(t_ch, "this.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), ConvertDefaultValue(pStruct._ParVec[i]._strParType, pStruct._ParVec[i]._strDefaultValue).c_str());
			strStruct.append(t_ch);
			strStruct.append(strTable_5_temp);
			strStruct.append("}\n");
			if (tt_bNeedVersionCheck) {
				strStruct.append(strTable_4);
				strStruct.append("}\n");
			}
		}
		else {
			strStruct.append(strTable_5_temp);
			if (IsTScriptBaseType(pStruct._ParVec[i]._strParType, true)) {
				if (IsExType_enum(pStruct._ParVec[i]._strParType))
					sprintf(t_ch, "this.%s = %s.read_s(outs)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				else
					sprintf(t_ch, "this.%s = outs.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str());
			}
			else {
				sprintf(t_ch, "this.%s = %s.read_s(outs);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			strStruct.append(t_ch);
		}

		//if (pStruct._name.compare("FriendsOperation") == 0)
		//{
		//	int a;
		//	a = 10;
		//}

		if (tt_bNeedVersionCheck) {
			string tt_default = GetDefaultValue(pStruct._ParVec[i]._strParType, false, false, false, true);
			if (tt_default.length() > 0) {
				strStruct.append(strTable_4);
				strStruct.append("else\n");

				strStruct.append(strTable_5);
				sprintf(t_ch, "this.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), tt_default.c_str());
				strStruct.append(t_ch);
			}
		}

	}

	strStruct.append(strTable_3);
	strStruct.append("}\n");

	strStruct.append(strTable_2);
	strStruct.append("}\n");

	////////////////////
	////
	////////////////////

	//写写
	strStruct.append(strTable_2);
	strStruct.append("public  write(ins:InStream)\n");
	strStruct.append(strTable_2);
	strStruct.append("{\n");

	//先写入结构体的大小
	strStruct.append(strTable_3);
	strStruct.append("let tt_len:number  = 0;\n");
	strStruct.append(strTable_3);
	strStruct.append("let tt_logsize1:number = ins.getPos();\n");
	strStruct.append(strTable_3);
	strStruct.append("ins.writeInt16(tt_len);\n");


	//再写入版本
	strStruct.append(strTable_3);
	sprintf(t_ch, "let  tt_version:number = %d;\n", pStruct._version);
	strStruct.append(t_ch);
	strStruct.append(strTable_3);
	strStruct.append("ins.writeInt16(tt_version);\n");

	strStruct.append(strTable_3);
	strStruct.append("let tt_logsize2:number = ins.getPos();\n");

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_3);
		strStruct.append("let tt_transNeedValue:number=0;\n");
		strStruct.append(strTable_3);
		strStruct.append("ins.writeInt32(tt_transNeedValue);\n");
	}



	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (pStruct._ParVec[i]._transNeedValue != -1) {
			strStruct.append(strTable_3);
			if (IsInt64_TScript(pStruct._ParVec[i]._strParType))
				sprintf(t_ch, "if(this.%s.isNotEqual(%s))\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
			else
				sprintf(t_ch, "if(this.%s!=%s)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
			strStruct.append(t_ch);
			strStruct.append(strTable_3);
			strStruct.append("{\n");


			strStruct.append(strTable_4);
			if (IsTScriptBaseType(pStruct._ParVec[i]._strParType, true)) {
				if (IsExType_enum(pStruct._ParVec[i]._strParType))
					sprintf(t_ch, "this.%s.write(ins)\n", pStruct._ParVec[i]._strParName.c_str());
				else
					sprintf(t_ch, "ins.%s(this.%s);\n", TypeConvertToWriteTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
			}
			else {
				sprintf(t_ch, "this.%s.write(ins);\n", pStruct._ParVec[i]._strParName.c_str());
			}
			strStruct.append(t_ch);


			strStruct.append(strTable_4);
			sprintf(t_ch, "tt_transNeedValue = tt_transNeedValue | (1<<%d);\n", pStruct._ParVec[i]._transNeedValue);
			strStruct.append(t_ch);

			strStruct.append(strTable_3);
			strStruct.append("}\n");
		}
		else {
			strStruct.append(strTable_3);
			if (IsTScriptBaseType(pStruct._ParVec[i]._strParType, true)) {
				if (IsExType_enum(pStruct._ParVec[i]._strParType))
					sprintf(t_ch, "%s.write(ins,this.%s);\n", pStruct._ParVec[i]._strParType.c_str(), pStruct._ParVec[i]._strParName.c_str());
				else
					sprintf(t_ch, "ins.%s(this.%s);\n", TypeConvertToWriteTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
			}
			else {
				sprintf(t_ch, "this.%s.write(ins);\n", pStruct._ParVec[i]._strParName.c_str());
			}
			strStruct.append(t_ch);
		}


	}
	strStruct.append(strTable_3);
	strStruct.append("let tt_logsize4:number = ins.getPos();\n");
	strStruct.append(strTable_3);
	strStruct.append("let tt_logsize3:number = tt_logsize4 - tt_logsize2;\n");
	strStruct.append(strTable_3);
	strStruct.append("ins.setPos(tt_logsize1);\n");
	strStruct.append(strTable_3);
	strStruct.append("ins.writeInt16(tt_logsize3);\n");

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_3);
		strStruct.append("if(tt_transNeedValue!=0)\n");
		strStruct.append(strTable_3);
		strStruct.append("{\n");
		strStruct.append(strTable_4);
		strStruct.append("ins.setPos(tt_logsize2);\n");
		strStruct.append(strTable_4);
		strStruct.append("ins.writeInt32(tt_transNeedValue);\n");
		strStruct.append(strTable_3);
		strStruct.append("}\n");
	}


	strStruct.append(strTable_3);
	strStruct.append("ins.setPos(tt_logsize4);\n");

	strStruct.append(strTable_2);
	strStruct.append("}\n");



	strStruct.append(strTable_1);
	strStruct.append("}\n\n");

	fwrite(strStruct.c_str(), strStruct.length(), 1, fp);
}

void    ProtocolParse_Writer_TypeScript::WriteVector_TScript_Model(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace, FILE* fp)
{
	if (!pVector._bBiaoZhuUsed)
		return;
	if (pVector._ParTName == "byte" || pVector._ParTName == "BYTE" || pVector._ParTName == "char" || pVector._ParTName == "unsigned char") {
		WriteVector2_TScript_Model(pNamespace, pVector, foldername, strWriteNameSpace, fp);
		return;
	}
	char t_ch[512];

	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);
	string strTable_4 = CreateTableString(3);

	string strVector;


	/////////////有两种版本，一种是非Flash基础类型，一种是
	bool  tt_bTScritpBaseType = IsTScriptBaseType2(pVector._ParTName);

	strVector.append(WriteComment_TScript(strTable_1, pVector._comment, "").c_str());

	strVector.append(strTable_1);
	sprintf(t_ch, "export class %s extends PArray<%s>\n", pVector._name.c_str(), TypeConvertToTScriptType(pVector._ParTName).c_str());
	strVector.append(t_ch);
	strVector.append(strTable_1);
	strVector.append("{\n");

	strVector.append(strTable_2);
	sprintf(t_ch, "constructor()\n");
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	sprintf(t_ch, "super(%s)\n", pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("}\n");


	strVector.append(strTable_2);
	sprintf(t_ch, "static  read_s(outs:OutStream):%s\n", pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	sprintf(t_ch, "var o:%s = new %s();\n", pVector._name.c_str(), pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_3);
	//sprintf(t_ch, "o.read(outs);\n", pVector._name.c_str(), pVector._name.c_str());
	strVector.append("o.read(outs);\n");


	//strVector.append(strTable_3);
	//strVector.append("let count:number = outs.readInt32();\n");


	//strVector.append(strTable_3);
	//strVector.append("for(let i=0;i<count;i++)\n");

	//strVector.append(strTable_3);
	//strVector.append("{\n");

	//if (!tt_bTScritpBaseType)
	//{
	//	if (pVector._ParTName.compare("__int64") == 0)
	//	{
	//		strVector.append(strTable_4);
	//		sprintf(t_ch, "o.push(outs.readInt64());\n");
	//		strVector.append(t_ch);
	//	}
	//	else
	//	{
	//		strVector.append(strTable_4);
	//		sprintf(t_ch, "o.push(%s.read_s(outs));\n", ExTypeNameConvert_TScript(pVector._ParTName).c_str());
	//		strVector.append(t_ch);
	//	}

	//}
	//else
	//{
	//	strVector.append(strTable_4);
	//	sprintf(t_ch, "o.push(outs.%s());\n", TypeConvertToReadTScriptType(pVector._ParTName).c_str());
	//	strVector.append(t_ch);
	//}

	//strVector.append(strTable_3);
	//strVector.append("}\n");

	strVector.append(strTable_3);
	strVector.append("return o;\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	////////////////
	//
	////////////////

	strVector.append(strTable_2);
	sprintf(t_ch, "public  read(outs:OutStream)\n");
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("this.length=0;\n");


	strVector.append(strTable_3);
	strVector.append("let count:number = outs.readInt32();\n");



	strVector.append(strTable_3);
	strVector.append("for(let i=0;i<count;i++)\n");

	strVector.append(strTable_3);
	strVector.append("{\n");

	if (!tt_bTScritpBaseType) {
		if (pVector._ParTName.compare("__int64") == 0) {
			strVector.append(strTable_4);
			sprintf(t_ch, "this.push(outs.readInt64());\n");
			strVector.append(t_ch);
		}
		else {
			strVector.append(strTable_4);
			sprintf(t_ch, "this.push(%s.read_s(outs));\n", ExTypeNameConvert_TScript(pVector._ParTName).c_str());
			strVector.append(t_ch);
		}

	}
	else {
		strVector.append(strTable_4);
		sprintf(t_ch, "this.push(outs.%s());\n", TypeConvertToReadTScriptType(pVector._ParTName).c_str());
		strVector.append(t_ch);
	}

	strVector.append(strTable_3);
	strVector.append("}\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	/////////////
	//
	////////////
	strVector.append(strTable_2);
	strVector.append("public  write(ins:InStream)\n");

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("ins.writeInt32(this.length);\n");

	strVector.append(strTable_3);
	strVector.append("for(let i=0;i<this.length;i++)\n");

	strVector.append(strTable_3);
	strVector.append("{\n");

	if (!tt_bTScritpBaseType) {
		if (pVector._ParTName.compare("__int64") == 0) {
			strVector.append(strTable_4);
			strVector.append("ins.writeInt64(this[i]);\n");
		}
		else {
			strVector.append(strTable_4);
			strVector.append("this[i].write(ins);\n");
		}

	}
	else {
		strVector.append(strTable_4);
		sprintf(t_ch, "ins.%s(this[i]);\n", TypeConvertToWriteTScriptType(pVector._ParTName).c_str());
		strVector.append(t_ch);
	}

	strVector.append(strTable_3);
	strVector.append("}\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_1);
	strVector.append("}\n\n");

	fwrite(strVector.c_str(), strVector.length(), 1, fp);

}
void    ProtocolParse_Writer_TypeScript::WriteVector2_TScript_Model(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace, FILE* fp)
{
	if (!pVector._bBiaoZhuUsed)
		return;
	char t_ch[512];

	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);
	string strTable_4 = CreateTableString(3);

	string strVector;

	strVector.append(strTable_1);
	sprintf(t_ch, "export  class %s\n", pVector._name.c_str());
	strVector.append(t_ch);
	strVector.append(strTable_1);
	strVector.append("{\n");

	strVector.append(strTable_2);
	strVector.append("public buffer:Uint8Array;\n");

	strVector.append(strTable_2);
	strVector.append("constructor()\n");

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_2);
	strVector.append("}\n");


	strVector.append(strTable_2);
	sprintf(t_ch, "static  read_s(outs:OutStream):%s\n", pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	sprintf(t_ch, "let o:%s = new %s();\n", pVector._name.c_str(), pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_3);
	strVector.append("o.read(outs);\n");

	strVector.append(strTable_3);
	strVector.append("return o;\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	////////////////
	//
	////////////////

	strVector.append(strTable_2);
	sprintf(t_ch, "public  read(outs:OutStream)\n");
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("this.buffer=outs.readUint8Array(outs.readInt32());\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	/////////////
	//
	////////////
	strVector.append(strTable_2);
	strVector.append("public  write(ins:InStream)\n");

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("ins.writeUint8Array(this.buffer);\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_1);
	strVector.append("}\n\n");

	fwrite(strVector.c_str(), strVector.length(), 1, fp);

}
void    ProtocolParse_Writer_TypeScript::WriteWJSVector_TScript_Model(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace, FILE* fp)
{
	if (!pWJSVector._bBiaoZhuUsed)
		return;
	if (pWJSVector._ParTName == "byte" || pWJSVector._ParTName == "BYTE" || pWJSVector._ParTName == "char" || pWJSVector._ParTName == "unsigned char") {
		WriteWJSVector2_TScript_Model(pNamespace, pWJSVector, foldername, strWriteNameSpace, fp);
		return;
	}

	char t_ch[512];

	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);
	string strTable_4 = CreateTableString(3);

	string strWJSVector;


	/////////////有两种版本，一种是非Flash基础类型，一种是
	bool  tt_bTScritpBaseType = IsTScriptBaseType2(pWJSVector._ParTName);


	strWJSVector.append(strTable_1);
	sprintf(t_ch, "export  class %s extends Array<%s>\n", pWJSVector._name.c_str(), TypeConvertToTScriptType(pWJSVector._ParTName).c_str());
	strWJSVector.append(t_ch);
	strWJSVector.append(strTable_1);
	strWJSVector.append("{\n");

	strWJSVector.append(strTable_2);
	sprintf(t_ch, "constructor()\n");
	strWJSVector.append(t_ch);

	strWJSVector.append(strTable_2);
	strWJSVector.append("{\n");

	//strVector.append(strTable_3);
	//strVector.append("super();\n");

	strWJSVector.append(strTable_2);
	strWJSVector.append("}\n");


	strWJSVector.append(strTable_2);
	sprintf(t_ch, "export function %s read_s(outs:OutStream)\n", pWJSVector._name.c_str());
	strWJSVector.append(t_ch);

	strWJSVector.append(strTable_2);
	strWJSVector.append("{\n");

	strWJSVector.append(strTable_3);
	sprintf(t_ch, "%s o = new %s();\n", pWJSVector._name.c_str(), pWJSVector._name.c_str());
	strWJSVector.append(t_ch);

	strWJSVector.append(strTable_3);
	strWJSVector.append("o.read(outs);\n");


	//strWJSVector.append(strTable_3);
	//strWJSVector.append("count:number = outs.readInt32();\n");

	//strWJSVector.append(strTable_3);
	//strWJSVector.append("for(let i=0;i<count;i++)\n");

	//strWJSVector.append(strTable_3);
	//strWJSVector.append("{\n");

	//if (!tt_bTScritpBaseType)
	//{
	//	strWJSVector.append(strTable_4);
	//	sprintf(t_ch, "o.push(%s.read_s(outs));\n", ExTypeNameConvert_TScript(pWJSVector._ParTName).c_str());
	//	strWJSVector.append(t_ch);
	//}
	//else
	//{
	//	strWJSVector.append(strTable_4);
	//	sprintf(t_ch, "o.push(outs.%s());\n", TypeConvertToReadTScriptType(pWJSVector._ParTName).c_str());
	//	strWJSVector.append(t_ch);
	//}

	//strWJSVector.append(strTable_3);
	//strWJSVector.append("}\n");

	strWJSVector.append(strTable_3);
	strWJSVector.append("return o;\n");

	strWJSVector.append(strTable_2);
	strWJSVector.append("}\n");

	/////////////////////
	//
	////////////////////
	strWJSVector.append(strTable_2);
	//sprintf(t_ch, "export function read(outs:OutStream)\n", pWJSVector._name.c_str());
	strWJSVector.append("export function read(outs:OutStream)\n");

	strWJSVector.append(strTable_2);
	strWJSVector.append("{\n");

	strWJSVector.append(strTable_3);
	strWJSVector.append("this.length=0;\n");

	strWJSVector.append(strTable_3);
	strWJSVector.append("count:number = outs.readInt32();\n");



	strWJSVector.append(strTable_3);
	strWJSVector.append("for(let i=0;i<count;i++)\n");

	strWJSVector.append(strTable_3);
	strWJSVector.append("{\n");

	if (!tt_bTScritpBaseType) {
		if (pWJSVector._ParTName.compare("__int64") == 0) {
			strWJSVector.append(strTable_4);
			sprintf(t_ch, "this.push(outs.readInt64());\n");
			strWJSVector.append(t_ch);
		}
		else {
			strWJSVector.append(strTable_4);
			sprintf(t_ch, "this.push(%s.read_s(outs));\n", ExTypeNameConvert_TScript(pWJSVector._ParTName).c_str());
			strWJSVector.append(t_ch);
		}

	}
	else {
		strWJSVector.append(strTable_4);
		sprintf(t_ch, "this.push(outStream.%s());\n", TypeConvertToReadTScriptType(pWJSVector._ParTName).c_str());
		strWJSVector.append(t_ch);
	}

	strWJSVector.append(strTable_3);
	strWJSVector.append("}\n");


	strWJSVector.append(strTable_2);
	strWJSVector.append("}\n");

	////////////////
	//
	/////////////////

	strWJSVector.append(strTable_2);
	strWJSVector.append("export function write(ins:InStream)\n");

	strWJSVector.append(strTable_2);
	strWJSVector.append("{\n");

	strWJSVector.append(strTable_3);
	strWJSVector.append("ins.writeInt32(this.length);\n");

	strWJSVector.append(strTable_3);
	strWJSVector.append("for(let i=0;i<Count;i++)\n");

	strWJSVector.append(strTable_3);
	strWJSVector.append("{\n");
	//
	if (!tt_bTScritpBaseType) {
		if (pWJSVector._ParTName.compare("__int64") == 0) {
			strWJSVector.append(strTable_4);
			strWJSVector.append("ins.writeInt64(this[i]);\n");
		}
		else {
			strWJSVector.append(strTable_4);
			strWJSVector.append("this[i].write(ins);\n");
		}

	}
	else {
		strWJSVector.append(strTable_4);
		sprintf(t_ch, "ins.%s(this[i]);\n", TypeConvertToWriteTScriptType(pWJSVector._ParTName).c_str());
		strWJSVector.append(t_ch);
	}

	strWJSVector.append(strTable_3);
	strWJSVector.append("}\n");

	strWJSVector.append(strTable_2);
	strWJSVector.append("}\n");

	strWJSVector.append(strTable_1);
	strWJSVector.append("}\n\n");

	fwrite(strWJSVector.c_str(), strWJSVector.length(), 1, fp);

}
void    ProtocolParse_Writer_TypeScript::WriteWJSVector2_TScript_Model(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace, FILE* fp)
{
	if (!pWJSVector._bBiaoZhuUsed)
		return;
	char t_ch[512];

	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);
	string strTable_4 = CreateTableString(3);

	string strVector;


	strVector.append(strTable_1);
	sprintf(t_ch, "export  class %s\n", pWJSVector._name.c_str());
	strVector.append(t_ch);
	strVector.append(strTable_1);
	strVector.append("{\n");

	strVector.append(strTable_2);
	strVector.append("public buffer:Uint8Array;\n");

	strVector.append(strTable_2);
	//sprintf(t_ch, "constructor()\n", pWJSVector._name.c_str());
	strVector.append("constructor()\n");

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_2);
	strVector.append("}\n");


	strVector.append(strTable_2);
	sprintf(t_ch, "static  read_s(outs:OutStream):%s\n", pWJSVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	sprintf(t_ch, "let o:%s = new %s();\n", pWJSVector._name.c_str(), pWJSVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_3);
	strVector.append("o.read(outs);\n");

	strVector.append(strTable_3);
	strVector.append("return o;\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	////////////////
	//
	////////////////

	strVector.append(strTable_2);
	sprintf(t_ch, "public  read(outs:OutStream)\n");
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("this.buffer=outs.readUint8Array(outs.readInt32());\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	/////////////
	//
	////////////
	strVector.append(strTable_2);
	strVector.append("public  write(ins:InStream)\n");

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("ins.writeUint8Array(this.buffer);\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_1);
	strVector.append("}\n\n");

	fwrite(strVector.c_str(), strVector.length(), 1, fp);

}
void    ProtocolParse_Writer_TypeScript::WriteClass_TScript_Model(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace, FILE* fp)
{
	switch (filetype) {
	case 0:
	{
		////对于客户端而言，request的为发送，callback的为回调
		if (pClass._strtype.compare("callback") == 0) {
			WriteClass_IReceiver_TScript_Model(pClass, filetype, pNamespace, foldername, false, strWriteNameSpace, fp);
			WriteClass_Receiver_TScript_Model(pClass, filetype, pNamespace, foldername, strWriteNameSpace, fp);
		}
		else {
			WriteClass_Send_TScript_Model(pClass, filetype, pNamespace, foldername, strWriteNameSpace, fp);
		}
	}
	break;
	case 1:
	{
		////服务器而言，请求的是回调，callback是发送
		if (pClass._strtype.compare("request") == 0) //C2S
		{
			WriteClass_IReceiver_TScript_Model(pClass, filetype, pNamespace, foldername, false, strWriteNameSpace, fp);
			WriteClass_Receiver_TScript_Model(pClass, filetype, pNamespace, foldername, strWriteNameSpace, fp);
		}
		else //S2C
		{
			WriteClass_Send_TScript_Model(pClass, filetype, pNamespace, foldername, strWriteNameSpace, fp);
		}
	}
	break;
	case 2:
	{

		//事实上两个里面是一样的
		if (pClass._strtype.compare("request") == 0) //C2S
		{
			WriteClass_IReceiver_TScript_Model(pClass, filetype, pNamespace, foldername, true, strWriteNameSpace, fp);
			WriteClass_Receiver_TScript_Model(pClass, filetype, pNamespace, foldername, strWriteNameSpace, fp);
			WriteClass_Send_TScript_Model(pClass, filetype, pNamespace, foldername, strWriteNameSpace, fp);
		}

	}
	break;
	default:
		break;
	}
}
void    ProtocolParse_Writer_TypeScript::WriteClass_IReceiver_TScript_Model(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement, const string& strWriteNameSpace, FILE* fp)
{
	char t_ch[1024];

	string strclass;

	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);


	vector<string> tt_PacketNameList;
	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
			continue;
		for (size_t j = 0; j < pClass._FuncVec[i]._InParVec.size(); j++) {
			if (!IsTScriptBaseType(pClass._FuncVec[i]._InParVec[j]._strParType, false, true)) {
				GetUsePacketNamesByParType_TScript_NSV(pClass._FuncVec[i]._InParVec[j]._strParType, tt_PacketNameList, GetSubFolderName(foldername));
			}
		}
	}

	for (size_t i = 0; i < tt_PacketNameList.size(); i++) {
		m_ImportPacketNameList_TS_Model.push_back(tt_PacketNameList[i]);
		//strclass.append(strTable_1);
		//strclass.append(tt_PacketNameList[i].c_str());
		//strclass.append("\n");
	}

	//if (tt_PacketNameList.size() > 0)
	//	strclass.append("\n");


	strclass.append(strTable_1);
	sprintf(t_ch, "export interface IReceiver_%s\n", pNamespace._protocolname.c_str());
	strclass.append(t_ch);

	strclass.append(strTable_1);
	strclass.append("{\n");
	fwrite(strclass.c_str(), strclass.length(), 1, fp);


	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
			continue;
		//写函数
		WriteFunc_IReceiver_TScript_Model(pClass._FuncVec[i], filetype, 1, pClass, pNamespace, foldername, fp, bemptyimplement);
	}

	strclass = "";
	strclass.append(strTable_1);
	strclass.append("}\n\n");

	fwrite(strclass.c_str(), strclass.length(), 1, fp);
}
void    ProtocolParse_Writer_TypeScript::WriteFunc_IReceiver_TScript_Model(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement)
{
	char t_ch[1024];

	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);


	string strFunc;
	strFunc.append(strTable_0);
	strFunc.append("/**\n");

	strFunc.append(WriteComment_TScript(strTable_0, pFunc._comment, "", false).c_str());
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strFunc.append(WriteComment_TScript(strTable_0, pFunc._InParVec[i]._comment, pFunc._InParVec[i]._strParName, false).c_str());
	}

	strFunc.append(strTable_0);
	strFunc.append("*/\n");


	strFunc.append(strTable_0);
	sprintf(t_ch, "IReceiver_%s(", pFunc._name.c_str());
	strFunc.append(t_ch);

	if (filetype == 0) {
		if (pFunc._InParVec.size() > 0) {
			for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
				if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
					sprintf(t_ch, "%s:%s,", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToTScriptType(pFunc._InParVec[i]._strParType).c_str());
					strFunc.append(t_ch);
				}
				else {
					sprintf(t_ch, "%s:%s,", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToTScriptType(pFunc._InParVec[i]._strParType).c_str());
					strFunc.append(t_ch);
				}

			}
		}
		strFunc.append("pExData);\n");
	}
	else {
		if (pFunc._InParVec.size() > 0) {
			for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
				if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
					sprintf(t_ch, "%s:%s,", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToTScriptType(pFunc._InParVec[i]._strParType).c_str());
					strFunc.append(t_ch);
				}
				else {
					sprintf(t_ch, "%s:%s,", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToTScriptType(pFunc._InParVec[i]._strParType).c_str());
					strFunc.append(t_ch);
				}

			}
		}
		strFunc.append("sessionId:number,pExData);\n");
	}


	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}
void    ProtocolParse_Writer_TypeScript::WriteClass_Receiver_TScript_Model(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace, FILE* fp)
{
	char t_ch[1024];

	string strclass;

	string strTable_0 = CreateTableString(0);
	string strTable_1 = CreateTableString(1);
	string strTable_2 = CreateTableString(2);
	string strTable_3 = CreateTableString(3);

	vector<string> tt_PacketNameList_ref;
	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
			continue;

		for (size_t j = 0; j < pClass._FuncVec[i]._InParVec.size(); j++) {
			if (!IsTScriptBaseType(pClass._FuncVec[i]._InParVec[j]._strParType, false, true)) {
				GetUsePacketNamesByParType_TScript_reference(pClass._FuncVec[i]._InParVec[j]._strParType, tt_PacketNameList_ref, GetSubFolderName(foldername));
			}
		}
	}

	//for (int i = 0; i < tt_PacketNameList_ref.size(); i++)
	//{
	//	m_ImportPacketNameList_TS_Model.push_back(tt_PacketNameList_ref[i]);
	//	//strclass.append(tt_PacketNameList_ref[i].c_str());
	//	//strclass.append("\n");
	//}

	//if (tt_PacketNameList_ref.size() > 0)
	//	strclass.append("\n");


	vector<string> tt_PacketNameList;
	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
			continue;

		for (size_t j = 0; j < pClass._FuncVec[i]._InParVec.size(); j++) {
			if (!IsTScriptBaseType(pClass._FuncVec[i]._InParVec[j]._strParType, false, true)) {
				GetUsePacketNamesByParType_TScript_NSV(pClass._FuncVec[i]._InParVec[j]._strParType, tt_PacketNameList, GetSubFolderName(foldername));
			}
		}
	}

	for (size_t i = 0; i < tt_PacketNameList.size(); i++) {
		m_ImportPacketNameList_TS_Model.push_back(tt_PacketNameList[i]);
		//strclass.append(strTable_0);
		//strclass.append(tt_PacketNameList[i].c_str());
		//strclass.append("\n");
	}

	//if (tt_PacketNameList.size() > 0)
	//	strclass.append("\n");

	strclass.append(strTable_0);
	sprintf(t_ch, "export class %s_Callback\n", pNamespace._protocolname.c_str());
	strclass.append(t_ch);

	strclass.append(strTable_0);
	strclass.append("{\n");
	fwrite(strclass.c_str(), strclass.length(), 1, fp);

	WriteFunc_ReceiverParser_TScript_Model(pClass, filetype, 1, pNamespace, foldername, fp);


	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
			continue;
		WriteFunc_Receiver_TScript_Model(pClass._FuncVec[i], filetype, 1, pClass, pNamespace, foldername, fp);
	}


	strclass = "";
	strclass.append(strTable_0);
	strclass.append("}\n\n");

	fwrite(strclass.c_str(), strclass.length(), 1, fp);
}

void    ProtocolParse_Writer_TypeScript::WriteFunc_ReceiverParser_TScript_Model(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp)
{
	char t_ch[1024];

	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);
	string strTable_2 = CreateTableString(tablenum + 2);
	string strTable_3 = CreateTableString(tablenum + 3);
	string strTable_4 = CreateTableString(tablenum + 4);


	string strFunc;
	strFunc.append(strTable_0);

	if (filetype == 0) {
		sprintf(t_ch, "Parser(cls:IReceiver_%s,outs:OutStream,pExData):boolean\n", pNamespace._protocolname.c_str());
		strFunc.append(t_ch);
	}
	else {
		sprintf(t_ch, "Parser(cls:IReceiver_%s,outs:OutStream,sessionId:number,pExData):boolean\n", pNamespace._protocolname.c_str());
		strFunc.append(t_ch);
	}

	strFunc.append(strTable_0);
	strFunc.append("{\n");

	strFunc.append(strTable_1);
	strFunc.append("let no:number = outs.readInt32();\n");

	strFunc.append(strTable_1);
	sprintf(t_ch, "if(no!=%s)\n", m_ProtocolNo._name.c_str());
	strFunc.append(t_ch);

	strFunc.append(strTable_1);
	strFunc.append("{\n");

	strFunc.append(strTable_2);
	strFunc.append("outs.addOffset(- 4);\n");

	strFunc.append(strTable_2);
	strFunc.append("return false;\n");

	strFunc.append(strTable_1);
	strFunc.append("}\n");

	strFunc.append(strTable_1);
	strFunc.append("let proversionno:number = outs.readInt16();\n");


	strFunc.append(strTable_1);
	strFunc.append("let methodid:number =outs.readInt32();\n");

	strFunc.append(strTable_1);
	strFunc.append("switch(methodid)\n");

	strFunc.append(strTable_1);
	strFunc.append("{\n");

	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
			continue;
		sprintf(t_ch, "ID_%s_%s", pClass._name.c_str(), pClass._FuncVec[i]._name.c_str());
		string tt_strFuncID = t_ch;
		strFunc.append(strTable_2);
		if (pClass._FuncVec[i]._userFuncId != 0)
			sprintf(t_ch, "case %d:\n", pClass._FuncVec[i]._userFuncId);
		else
			sprintf(t_ch, "case %d:\n", GetExistStringToIntValue(tt_strFuncID));
		strFunc.append(t_ch);

		strFunc.append(strTable_3);
		if (filetype == 0) {
			sprintf(t_ch, "this._parser_%s(cls,outs,pExData,proversionno);\n", pClass._FuncVec[i]._name.c_str());
			strFunc.append(t_ch);
		}
		else {
			sprintf(t_ch, "this._parser_%s(cls,outs,sessionId,pExData,proversionno);\n", pClass._FuncVec[i]._name.c_str());
			strFunc.append(t_ch);
		}


		strFunc.append(strTable_3);
		strFunc.append("return true;\n");
	}


	strFunc.append(strTable_2);
	strFunc.append("default:\n");

	strFunc.append(strTable_3);
	strFunc.append("return false;\n");

	strFunc.append(strTable_1);
	strFunc.append("}\n");

	strFunc.append(strTable_0);
	strFunc.append("}\n");


	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}
void    ProtocolParse_Writer_TypeScript::WriteClass_Send_TScript_Model(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace, FILE* fp)
{
	//char t_ch[512];

	string strclass;


	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);

	vector<string> tt_PacketNameList;
	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == 1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == -1)))
			continue;
		for (size_t j = 0; j < pClass._FuncVec[i]._InParVec.size(); j++) {
			if (!IsTScriptBaseType(pClass._FuncVec[i]._InParVec[j]._strParType, false)) {
				GetUsePacketNamesByParType_TScript_NSV(pClass._FuncVec[i]._InParVec[j]._strParType, tt_PacketNameList, GetSubFolderName(foldername));
			}
		}
	}
	for (size_t i = 0; i < tt_PacketNameList.size(); i++) {
		m_ImportPacketNameList_TS_Model.push_back(tt_PacketNameList[i]);
		//strclass.append(strTable_1);
		//strclass.append(tt_PacketNameList[i].c_str());
		//strclass.append("\n");
	}
	//if (tt_PacketNameList.size() > 0)
	//	strclass.append("\n");


	//strclass.append(strTable_1);
	//sprintf(t_ch, "export class %s_Send\n", pNamespace._protocolname.c_str());
	//strclass.append(t_ch);

	//strclass.append(strTable_1);
	//strclass.append("{\n");


	fwrite(strclass.c_str(), strclass.length(), 1, fp);
	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == 1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == -1)))
			continue;
		//写函数
		WriteFunc_SendByteSeq_TScript_Model(pClass._FuncVec[i], 0, pClass, pNamespace, foldername, fp);
	}


	//strclass = "";
	////strclass.append(strTable_1);
	////strclass.append("}\n");
	//strclass.append("}\n");

	//fwrite(strclass.c_str(), strclass.length(), 1, fp);


}

void    ProtocolParse_Writer_TypeScript::WriteFunc_SendByteSeq_TScript_Model(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp)
{
	char t_ch[1024];


	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);
	string strTable_2 = CreateTableString(tablenum + 2);

	string strfunc;

	strfunc.append(strTable_0);
	strfunc.append("/**\n");

	strfunc.append(WriteComment_TScript(strTable_0, pFunc._comment, "", false).c_str());
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strfunc.append(WriteComment_TScript(strTable_0, pFunc._InParVec[i]._comment, pFunc._InParVec[i]._strParName, false).c_str());
	}

	strfunc.append(strTable_0);
	strfunc.append("*/\n");

	strfunc.append(strTable_0);
	sprintf(t_ch, "export function  Send_%s(", pFunc._name.c_str());
	strfunc.append(t_ch);

	//有参
	if (pFunc._InParVec.size() > 0) {
		for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
			//if (i == pFunc._InParVec.size() - 1)
			//{
			//	sprintf(t_ch, "%s:%s", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToTScriptType(pFunc._InParVec[i]._strParType).c_str());
			//	strfunc.append(t_ch);
			//}
			//else
			//{
			sprintf(t_ch, "%s:%s,", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToTScriptType(pFunc._InParVec[i]._strParType).c_str());
			strfunc.append(t_ch);
			//}

		}

	}
	strfunc.append("ins:InStream");

	strfunc.append(")\n");
	strfunc.append(strTable_0);
	strfunc.append("{\n");

	strfunc.append(strTable_1);
	strfunc.append("ins.reset();\n");

	//先写协议号
	strfunc.append(strTable_1);
	sprintf(t_ch, "ins.writeInt32(%s);\n", m_ProtocolNo._name.c_str());
	strfunc.append(t_ch);

	//写版本号

	strfunc.append(strTable_1);
	sprintf(t_ch, "let tt_version:number = %d;\n", pFunc._version);
	strfunc.append(t_ch);
	strfunc.append(strTable_1);
	strfunc.append("ins.writeInt16(tt_version);\n");

	//再写函数号
	sprintf(t_ch, "ID_%s_%s", pClass._name.c_str(), pFunc._name.c_str());
	string tt_strFuncID = t_ch;

	strfunc.append(strTable_1);
	if (pFunc._userFuncId != 0)
		sprintf(t_ch, "ins.writeInt32(%d);\n", pFunc._userFuncId);
	else
		sprintf(t_ch, "ins.writeInt32(%d);\n", GetExistStringToIntValue(tt_strFuncID));
	strfunc.append(t_ch);

	//
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strfunc.append(strTable_1);
		if (IsTScriptBaseType(pFunc._InParVec[i]._strParType, true)) {
			//if (IsExType_enum(pFunc._InParVec[i]._strParType))
			//{
			//	sprintf(t_ch, "%s.write(inStream,%s);\n", pFunc._InParVec[i]._strParType.c_str(), pFunc._InParVec[i]._strParName.c_str());
			//}
			//else
			sprintf(t_ch, "ins.%s(%s);\n", TypeConvertToWriteTScriptType(pFunc._InParVec[i]._strParType.c_str()).c_str(), pFunc._InParVec[i]._strParName.c_str());
		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s.write(ins,%s);\n", pFunc._InParVec[i]._strParType.c_str(), pFunc._InParVec[i]._strParName.c_str());

		}
		else {
			sprintf(t_ch, "%s.write(ins);\n", pFunc._InParVec[i]._strParName.c_str());

		}
		strfunc.append(t_ch);
	}

	strfunc.append(strTable_0);
	strfunc.append("}\n");

	fwrite(strfunc.c_str(), strfunc.length(), 1, fp);
}
void    ProtocolParse_Writer_TypeScript::WriteFunc_Receiver_TScript_Model(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp)
{
	char t_ch[1024];

	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);
	string strTable_2 = CreateTableString(tablenum + 2);
	string strTable_3 = CreateTableString(tablenum + 3);


	string strFunc;

	if (filetype == 0) {
		strFunc.append(strTable_0);
		sprintf(t_ch, "private _parser_%s(cls:IReceiver_%s,outs:OutStream,pExData,proversionno:number)\n", pFunc._name.c_str(), pNamespace._protocolname.c_str());
		strFunc.append(t_ch);
	}
	else {
		strFunc.append(strTable_0);
		sprintf(t_ch, "private _parser_%s(cls:IReceiver_%s,outs:OutStream,sessionId:number,pExData,proversionno:number)\n", pFunc._name.c_str(), pNamespace._protocolname.c_str());
		strFunc.append(t_ch);
	}

	strFunc.append(strTable_0);
	strFunc.append("{\n");

	//if (m_bWithLog)
	//{
	//	strFunc.append(strTable_1);
	//	sprintf(t_ch, "Debug.Log(\"[[[%s.%s]]]\");\n", pClass._name.c_str(), pFunc._name.c_str());
	//	strFunc.append(t_ch);
	//}


	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strFunc.append(strTable_1);
		if (IsTScriptBaseType(pFunc._InParVec[i]._strParType, false) || IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "let %s:%s;\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToTScriptType(pFunc._InParVec[i]._strParType)).c_str());

		}
		else if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "let %s:%s;\n", ExTypeNameConvert_TScript(pFunc._InParVec[i]._strParName).c_str(), (TypeConvertToTScriptType(pFunc._InParVec[i]._strParType)).c_str());

		}
		else {
			sprintf(t_ch, "let %s:%s;\n", ExTypeNameConvert_TScript(pFunc._InParVec[i]._strParName).c_str(), (TypeConvertToTScriptType(pFunc._InParVec[i]._strParType)).c_str());
		}
		strFunc.append(t_ch);
	}

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		if (pFunc._InParVec[i]._version > 1)
			break;
		strFunc.append(strTable_1);
		if (IsTScriptBaseType(pFunc._InParVec[i]._strParType, false))
			sprintf(t_ch, "%s = outs.%s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadTScriptType(pFunc._InParVec[i]._strParType)).c_str());
		else if (IsExType_enum(pFunc._InParVec[i]._strParType))
			sprintf(t_ch, "%s = %s.read_s(outs);\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		else
			sprintf(t_ch, "%s = %s.read_s(outs);\n", ExTypeNameConvert_TScript(pFunc._InParVec[i]._strParName).c_str(), (TypeConvertToTScriptType(pFunc._InParVec[i]._strParType)).c_str());
		strFunc.append(t_ch);
	}

	string ttt_strFunc;
	bool   tt_bHasReadData = false;
	//作出判定相等
	ttt_strFunc.append(strTable_1);
	sprintf(t_ch, "if(proversionno==%d || (proversionno==0))\n", pFunc._version);
	ttt_strFunc.append(t_ch);

	ttt_strFunc.append(strTable_1);
	ttt_strFunc.append("{\n");

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		if (pFunc._InParVec[i]._version <= 1)
			continue;
		tt_bHasReadData = true;
		ttt_strFunc.append(strTable_2);
		if (IsTScriptBaseType(pFunc._InParVec[i]._strParType, false))
			sprintf(t_ch, "%s = outs.%s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadTScriptType(pFunc._InParVec[i]._strParType)).c_str());
		else if (IsExType_enum(pFunc._InParVec[i]._strParType))
			sprintf(t_ch, "%s = %s.read_s(outs);\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		else
			sprintf(t_ch, "%s = %s.read_s(outs);\n", ExTypeNameConvert_TScript(pFunc._InParVec[i]._strParName).c_str(), (TypeConvertToTScriptType(pFunc._InParVec[i]._strParType)).c_str());
		ttt_strFunc.append(t_ch);
	}


	ttt_strFunc.append(strTable_1);
	ttt_strFunc.append("}\n");


	//////////////低解高
	ttt_strFunc.append(strTable_1);
	sprintf(t_ch, "else if(proversionno>%d)\n", pFunc._version);
	ttt_strFunc.append(t_ch);

	ttt_strFunc.append(strTable_1);
	ttt_strFunc.append("{\n");

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		if (pFunc._InParVec[i]._version <= 1)
			continue;
		tt_bHasReadData = true;
		ttt_strFunc.append(strTable_2);
		if (IsTScriptBaseType(pFunc._InParVec[i]._strParType, false))
			sprintf(t_ch, "%s = outs.%s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadTScriptType(pFunc._InParVec[i]._strParType)).c_str());
		else if (IsExType_enum(pFunc._InParVec[i]._strParType))
			sprintf(t_ch, "%s = %s.read_s(outs);\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		else
			sprintf(t_ch, "%s = %s.read_s(outs);\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToTScriptType(pFunc._InParVec[i]._strParType)).c_str());
		ttt_strFunc.append(t_ch);
	}

	ttt_strFunc.append(strTable_1);
	ttt_strFunc.append("}\n");

	///////////////高解低
	ttt_strFunc.append(strTable_1);
	ttt_strFunc.append("else\n");

	ttt_strFunc.append(strTable_1);
	ttt_strFunc.append("{\n");

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		if (pFunc._InParVec[i]._version <= 1)
			continue;
		tt_bHasReadData = true;
		ttt_strFunc.append(strTable_2);
		sprintf(t_ch, "if(proversionno>=%d)\n", pFunc._InParVec[i]._version);
		ttt_strFunc.append(t_ch);

		ttt_strFunc.append(strTable_3);
		if (IsTScriptBaseType(pFunc._InParVec[i]._strParType, false))
			sprintf(t_ch, "%s = outs.%s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadTScriptType(pFunc._InParVec[i]._strParType)).c_str());
		else if (IsExType_enum(pFunc._InParVec[i]._strParType))
			sprintf(t_ch, "%s = %s.read_s(outs);\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		else
			sprintf(t_ch, "%s = %s.read_s(outs);\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToTScriptType(pFunc._InParVec[i]._strParType)).c_str());
		ttt_strFunc.append(t_ch);

		string tt_default = GetDefaultValue(pFunc._InParVec[i]._strParType, false, false, false, true);
		if (tt_default.length() > 0) {
			ttt_strFunc.append(strTable_2);
			ttt_strFunc.append("else\n");

			ttt_strFunc.append(strTable_3);
			sprintf(t_ch, "%s = %s;\n", pFunc._InParVec[i]._strParName.c_str(), tt_default.c_str());
			ttt_strFunc.append(t_ch);
		}
		else if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
			ttt_strFunc.append(strTable_2);
			ttt_strFunc.append("else\n");

			ttt_strFunc.append(strTable_3);
			sprintf(t_ch, "%s = new %s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToTScriptType(pFunc._InParVec[i]._strParType)).c_str());
			ttt_strFunc.append(t_ch);
		}
		else if (IsExType_WJSVector(pFunc._InParVec[i]._strParType)) {
			ttt_strFunc.append(strTable_2);
			ttt_strFunc.append("else\n");

			ttt_strFunc.append(strTable_3);
			sprintf(t_ch, "%s = new %s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToTScriptType(pFunc._InParVec[i]._strParType)).c_str());
			ttt_strFunc.append(t_ch);
		}
		else if (IsExType_struct(pFunc._InParVec[i]._strParType)) {
			ttt_strFunc.append(strTable_2);
			ttt_strFunc.append("else\n");

			ttt_strFunc.append(strTable_3);
			sprintf(t_ch, "%s = new %s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToTScriptType(pFunc._InParVec[i]._strParType)).c_str());
			ttt_strFunc.append(t_ch);
		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			ttt_strFunc.append(strTable_2);
			ttt_strFunc.append("else\n");

			ttt_strFunc.append(strTable_3);
			sprintf(t_ch, "%s = 0;\n", pFunc._InParVec[i]._strParName.c_str());
			ttt_strFunc.append(t_ch);
		}
	}



	ttt_strFunc.append(strTable_1);
	ttt_strFunc.append("}\n");

	if (tt_bHasReadData)
		strFunc.append(ttt_strFunc.c_str());


	strFunc.append(strTable_1);
	sprintf(t_ch, "cls.IReceiver_%s(", pFunc._name.c_str());
	strFunc.append(t_ch);


	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		sprintf(t_ch, "%s,", pFunc._InParVec[i]._strParName.c_str());
		strFunc.append(t_ch);
	}

	if (filetype == 0)
		strFunc.append("pExData);\n");
	else
		strFunc.append("sessionId,pExData);\n");
	/////////////////
	strFunc.append(strTable_0);
	strFunc.append("}\n");


	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);

}