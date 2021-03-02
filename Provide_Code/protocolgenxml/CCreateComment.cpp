#include "CCreateComment.h"
#include "ProtocolUtils.h"
#include "stdio.h"

extern string g_ProgressId;
extern vector<string> g_AllTempFilesList;
CCreateComment::CCreateComment(string& strPathName)
{
	m_strFileName = strPathName;

}

CCreateComment::~CCreateComment()
{

}

void CCreateComment::SetFileName(const string& strPathName)//设文件名
{
	m_strFileName = strPathName;
}

bool CCreateComment::Parser()
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
	tt_strFileName2.append(g_ProgressId.c_str());
	g_AllTempFilesList.push_back(tt_strFileName2);
	FILE* fp2 = fopen(tt_strFileName2.c_str(), "wb");
	if (!fp2) {
		delete[] pOld;
		return false;
	}

	//char tt_chline[4096];
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
				string tt_newLine;
				string tt_comment;

				tt_newLine = ProtocolUtils::RemoveLineNote(strLine, tt_comment);
				if (tt_comment.length() > 0) {
					tt_commentLines.push_back(tt_comment);
				}

				if (ProtocolUtils::IsContextLine(tt_newLine)) {
					tt_newLine = ProtocolUtils::InsertComment(tt_newLine, tt_commentLines);
					tt_commentLines.clear();

					tt_strLine2 = tt_newLine;
					tt_strLine2.append("\r\n");
					fwrite(tt_strLine2.c_str(), tt_strLine2.size(), 1, fp2);
				}
			}
			else {
				tt_strLine2 = strLine;
				tt_strLine2.append("\r\n");
				fwrite(tt_strLine2.c_str(), tt_strLine2.size(), 1, fp2);
			}
		}
		else {
			if (tt_bNewLine) {
				tt_strLine2 = "\r\n";
				fwrite(tt_strLine2.c_str(), tt_strLine2.size(), 1, fp2);
			}

		}
	}

	delete[] pOld;
	fclose(fp2);
	return true;
}







