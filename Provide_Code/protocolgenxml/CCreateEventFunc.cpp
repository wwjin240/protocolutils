#include "CCreateEventFunc.h"
#include "ProtocolUtils.h"
#include "stdio.h"

CCreateEventFunc::CCreateEventFunc(string& strPathName)
{
	m_strFileName = strPathName;

}

CCreateEventFunc::~CCreateEventFunc()
{

}

void CCreateEventFunc::SetFileName(const string& strPathName)//设文件名
{
	m_strFileName = strPathName;
}

bool CCreateEventFunc::Parser()
{
	FILE* fp1 = fopen(m_strFileName.c_str(), "rb");
	if (!fp1)
		return false;


	//得到文件大小
	fseek(fp1, 0, SEEK_END);
	long fileLen = ftell(fp1);
	fseek(fp1, 0, SEEK_SET);

	//读出所有数据
	char* pBuf = new char[fileLen + 1];
	fread(pBuf, fileLen, 1, fp1);
	pBuf[fileLen] = '\0';

	fclose(fp1);

	char* pOld = pBuf;


	string tt_strFileName2 = m_strFileName;
	tt_strFileName2.append(".copy");
	FILE* fp2 = fopen(tt_strFileName2.c_str(), "wb");
	if (!fp2) {
		delete[] pOld;
		return false;
	}

	string tt_strLine2;
	vector<string> tt_commentLines;

	bool bEnd = false;
	while (!bEnd) {
		string strLine;
		int readNum = 0;
		bool tt_bNewLine = false;
		strLine = ProtocolUtils::GetLine_4(pBuf, readNum, bEnd, tt_bNewLine);
		pBuf += readNum;

		if (strLine.length() > 0) {
			if (ProtocolUtils::IsContextLine(strLine)) {
				stVoidFunc tt_VoidFunc;
				if (strLine.find("<method") != string::npos) {
					int a;
					a = 10;
				}
				if (ParLine(strLine, tt_VoidFunc)) {
					for (unsigned int j = 0; j < tt_VoidFunc._commentList.size(); j++) {
						tt_commentLines.push_back(tt_VoidFunc._commentList[j]);

					}
					tt_VoidFunc._commentList = tt_commentLines;
					WriteVoidFuncToFile(fp2, tt_VoidFunc);
					tt_commentLines.clear();
				}
				else {
					string tt_strComment = ProtocolUtils::FoundCommont_AfterLine(strLine, 0);
					if (tt_strComment.length() > 0) {
						tt_commentLines.push_back(tt_strComment);
					}
					else {
						tt_commentLines.clear();
					}
				}
			}
		}
	}




	delete[] pOld;
	fclose(fp2);
	return true;
}



bool   CCreateEventFunc::ParLine(const string& strLine, stVoidFunc& VoidFunc)
{
	int tt_startIndex = 0;
	int tt_endIndex = -1;
	string tt_strVoid = ProtocolUtils::FoundStringEndSpace(strLine, tt_startIndex, tt_endIndex);
	if (tt_strVoid.compare("void") != 0)
		return false;

	tt_startIndex = tt_endIndex;
	string tt_strFuncName = ProtocolUtils::FoundStringEndSpecifyFuHao(strLine, tt_startIndex, tt_endIndex, '(', false);
	if (tt_strFuncName.length() == 0)
		return false;

	tt_startIndex = tt_endIndex;
	string tt_strParName = ProtocolUtils::FoundStringEndSpecifyFuHao(strLine, tt_startIndex, tt_endIndex, ')', true);
	if (tt_strParName.length() == 0) {
		VoidFunc._name = tt_strFuncName;
		tt_startIndex = tt_endIndex;
		string strComment = ProtocolUtils::FoundCommont_AfterLine(strLine, tt_startIndex);
		if (strComment.length() > 0)
			VoidFunc._commentList.push_back(strComment);

		return true;
	}

	string strComment = ProtocolUtils::FoundCommont_AfterLine(strLine, tt_startIndex);
	if (strComment.length() > 0)
		VoidFunc._commentList.push_back(strComment);

	//进一步解析参数，以',' ' ' '&'结束
	vector<string> tt_pars;
	ProtocolUtils::SplitStringByFuncPar(tt_strParName, tt_pars);

	VoidFunc._name = tt_strFuncName;
	stVoidFuncPar tt_VoidFuncPar;
	for (unsigned int i = 0; i < tt_pars.size(); i++) {
		if (tt_pars[i].compare(",") == 0) {
			VoidFunc._InParVec.push_back(tt_VoidFuncPar);
			tt_VoidFuncPar.Reset();
		}
		else if (tt_pars[i].compare("const") == 0) {
			tt_VoidFuncPar._isConst = true;
		}
		else if (tt_pars[i].compare("&") == 0) {
			tt_VoidFuncPar._isRef = true;
		}
		else {
			if (tt_VoidFuncPar._strParType.length() == 0) {
				tt_VoidFuncPar._strParType = tt_pars[i];
			}
			else if (tt_VoidFuncPar._strParName.length() == 0) {
				tt_VoidFuncPar._strParName = tt_pars[i];
			}
			else {
				tt_VoidFuncPar._strParType.append(" ");
				tt_VoidFuncPar._strParType.append(tt_VoidFuncPar._strParName.c_str());
				tt_VoidFuncPar._strParName = tt_pars[i];
			}
		}
	}

	if (tt_VoidFuncPar._strParType.length() > 0 && tt_VoidFuncPar._strParName.length() > 0) {
		//移除默认值
		tt_VoidFuncPar._strParName = ProtocolUtils::FoundStringEndSpecifyFuHao(tt_VoidFuncPar._strParName, 0, tt_endIndex, '=', true);
		VoidFunc._InParVec.push_back(tt_VoidFuncPar);
	}

	return true;

}


void   CCreateEventFunc::WriteVoidFuncToFile(FILE* fp, const stVoidFunc& VoidFunc)
{
	string tt_strComment;
	string tt_strLine;
	char   tt_ch[2048];

	//先写函数注释
	for (unsigned int j = 0; j < VoidFunc._commentList.size(); j++) {
		sprintf(tt_ch, "////[func:%s]////\n", VoidFunc._commentList[j].c_str());
		tt_strComment.append(tt_ch);
	}

	if (VoidFunc._InParVec.size() == 0) {
		tt_strLine.append("OUTCALL_FUNC_VOID");
		tt_strLine.append("(NS,N,");
		tt_strLine.append(VoidFunc._name.c_str());
		tt_strLine.append(");");
	}
	else {

		sprintf(tt_ch, "OUTCALL_FUNC_VOID_%d", VoidFunc._InParVec.size());
		tt_strLine.append(tt_ch);
		tt_strLine.append("(NS,N,");
		tt_strLine.append(VoidFunc._name.c_str());

		for (unsigned int i = 0; i < VoidFunc._InParVec.size(); i++) {
			string tt_strType = GetParTypeMacro(VoidFunc._InParVec[i]);
			//需要添加注释
			if (tt_strType.compare("VRC") == 0 || tt_strType.compare("VR") == 0 || tt_strType.compare("VC") == 0 || tt_strType.compare("V") == 0) {
				sprintf(tt_ch, "////[%s:%s]////\n", VoidFunc._InParVec[i]._strParName.c_str(), VoidFunc._InParVec[i]._strParType.c_str());
				tt_strComment.append(tt_ch);
			}
			sprintf(tt_ch, ",%s,%s", tt_strType.c_str(), VoidFunc._InParVec[i]._strParName.c_str());
			tt_strLine.append(tt_ch);
		}
		tt_strLine.append(");");
	}

	tt_strLine.append("\n");
	if (tt_strComment.length() > 0) {
		fwrite(tt_strComment.c_str(), tt_strComment.size(), 1, fp);

	}
	fwrite(tt_strLine.c_str(), tt_strLine.size(), 1, fp);
}
string CCreateEventFunc::GetParTypeMacro(const stVoidFuncPar& VoidFuncPar)
{
	if (VoidFuncPar._strParType.compare("int") == 0) {
		if (VoidFuncPar._isConst && VoidFuncPar._isRef) {
			return "IRC";
		}
		else if (VoidFuncPar._isRef) {
			return "IR";
		}
		else if (VoidFuncPar._isConst) {
			return "IC";
		}
		else
			return "I";
	}
	else if (VoidFuncPar._strParType.compare("unsigned int") == 0) {
		if (VoidFuncPar._isConst && VoidFuncPar._isRef) {
			return "UIRC";
		}
		else if (VoidFuncPar._isRef) {
			return "UIR";
		}
		else if (VoidFuncPar._isConst) {
			return "UIC";
		}
		else
			return "UI";
	}
	else if (VoidFuncPar._strParType.compare("short") == 0) {
		if (VoidFuncPar._isConst && VoidFuncPar._isRef) {
			return "SRC";
		}
		else if (VoidFuncPar._isRef) {
			return "SR";
		}
		else if (VoidFuncPar._isConst) {
			return "SC";
		}
		else
			return "S";
	}
	else if (VoidFuncPar._strParType.compare("unsigned short") == 0) {
		if (VoidFuncPar._isConst && VoidFuncPar._isRef) {
			return "USRC";
		}
		else if (VoidFuncPar._isRef) {
			return "USR";
		}
		else if (VoidFuncPar._isConst) {
			return "USC";
		}
		else
			return "US";
	}
	else if (VoidFuncPar._strParType.compare("float") == 0) {
		if (VoidFuncPar._isConst && VoidFuncPar._isRef) {
			return "FRC";
		}
		else if (VoidFuncPar._isRef) {
			return "FR";
		}
		else if (VoidFuncPar._isConst) {
			return "FC";
		}
		else
			return "F";
	}
	else if (VoidFuncPar._strParType.compare("double") == 0) {
		if (VoidFuncPar._isConst && VoidFuncPar._isRef) {
			return "DFRC";
		}
		else if (VoidFuncPar._isRef) {
			return "DFR";
		}
		else if (VoidFuncPar._isConst) {
			return "DFC";
		}
		else
			return "DF";
	}
	else if (VoidFuncPar._strParType.compare("bool") == 0 || VoidFuncPar._strParType.compare("BOOL") == 0) {
		if (VoidFuncPar._isConst && VoidFuncPar._isRef) {
			return "BRC";
		}
		else if (VoidFuncPar._isRef) {
			return "BR";
		}
		else if (VoidFuncPar._isConst) {
			return "BC";
		}
		else
			return "B";
	}
	else if (VoidFuncPar._strParType.compare("long long") == 0) {
		if (VoidFuncPar._isConst && VoidFuncPar._isRef) {
			return "LLRC";
		}
		else if (VoidFuncPar._isRef) {
			return "LLR";
		}
		else if (VoidFuncPar._isConst) {
			return "LLC";
		}
		else
			return "LL";
	}
	else if (VoidFuncPar._strParType.compare("string") == 0 || VoidFuncPar._strParType.compare("std::string") == 0) {
		if (VoidFuncPar._isConst && VoidFuncPar._isRef) {
			return "SSRC";
		}
		else if (VoidFuncPar._isRef) {
			return "SSR";
		}
		else if (VoidFuncPar._isConst) {
			return "SSC";
		}
		else
			return "SS";
	}
	else if (VoidFuncPar._strParType.compare("u16string") == 0 || VoidFuncPar._strParType.compare("std::u16string") == 0) {
		if (VoidFuncPar._isConst && VoidFuncPar._isRef) {
			return "U16SRC";
		}
		else if (VoidFuncPar._isRef) {
			return "U16SR";
		}
		else if (VoidFuncPar._isConst) {
			return "U16SC";
		}
		else
			return "U16S";
	}
	else if (VoidFuncPar._strParType.compare("wstring") == 0 || VoidFuncPar._strParType.compare("std::wstring") == 0) {
		if (VoidFuncPar._isConst && VoidFuncPar._isRef) {
			return "WSRC";
		}
		else if (VoidFuncPar._isRef) {
			return "WSR";
		}
		else if (VoidFuncPar._isConst) {
			return "WSC";
		}
		else
			return "WS";
	}
	else if (VoidFuncPar._strParType.compare("void*") == 0) {
		//if(VoidFuncPar._isConst && VoidFuncPar._isRef)
		//{
		//	return "VRC";
		//}
		//else if(VoidFuncPar._isRef)
		//{
		//	return "VR";
		//}
		if (VoidFuncPar._isConst) {
			return "VC";
		}
		else
			return "V";
	}
	else {
		//if(VoidFuncPar._isConst && VoidFuncPar._isRef)
		//{
		//	return "VRC";
		//}
		//else if(VoidFuncPar._isRef)
		//{
		//	return "VR";
		//}
		if (VoidFuncPar._isConst) {
			return "VC";
		}
		else
			return "V";
	}

}



