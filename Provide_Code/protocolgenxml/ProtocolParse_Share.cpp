#include "ProtocolParse.h"

ProtocolParse_Share::ProtocolParse_Share()
	:m_bUse4SpaceReplaceTable(false)
{
	m_strConflict = "\r\n//";
}

ProtocolParse_Share::~ProtocolParse_Share()
{

}

string ProtocolParse_Share::CreateTableString(int tablenum)
{
	string str;
	if (m_bUse4SpaceReplaceTable) {
		for (int i = 0; i < tablenum; i++)
			str.append("    ");
	}
	else {
		for (int i = 0; i < tablenum; i++)
			str.append("\t");
	}


	return str;

}
int    ProtocolParse_Share::CovertStringToInt(const string& str, bool bNew)
{
	string tempstr = str.substr(0, 64);
	int    ret = 0;

	const char*  p = tempstr.c_str();
	int    len = tempstr.length();

	if (bNew) {
		//          6位 6位 8位
		//6位 6位          

		for (int i = 0; i < len; i++) {
			char tempch = *(p + i);
			int  tempint = (int)tempch;
			tempint = tempint % 64;
			if (tempint != 0) {
				tempint = tempint << 8;
				ret = ret + tempint;
			}
		}

		for (int i = 0; i < len; i++) {
			char tempch = *(p + i);
			int  tempint = (int)tempch;
			tempint = tempint % 31;
			if (tempint != 0) {
				tempint = tempint << 20;
				ret = ret + tempint;
			}
		}

	}
	else {
		for (int i = 0; i < len; i++) {
			char tempch = *(p + i);
			int  tempint = (int)tempch;
			tempint = tempint % 10;
			if (tempint != 0) {
				if (i < 20) {
					tempint = tempint << (i + 8);
				}
				else {
					tempint = tempint << ((i % 20) + 8);
				}

				ret = ret | tempint;
			}
		}

	}


	int sub = AddtoStringIntLogMap(ret);

	int tt_oldret = ret;
	ret = ret | sub;

	m_IntStringMap.Insert(ret, str);
	m_StringIntSearchMap.Insert(str, ret);

	if (sub != 1) {
		tt_oldret = tt_oldret | 1;
		WJS_BBSTreeNode<int, string>* tt_pNode = m_IntStringMap.Search(tt_oldret);
		if (tt_pNode) {
			m_strConflict.append(tt_pNode->m_data.c_str());
			m_strConflict.append("|");
		}
		m_strConflict.append(str.c_str());
		m_strConflict.append(":");
		char tt_ch[64];
		sprintf_s(tt_ch, 64, "%d", ret);
		m_strConflict.append(tt_ch);


		m_strConflict.append(";\r\n//");
	}

	return ret;
}

int     ProtocolParse_Share::CreateSerialVersionUID(const string& str)
{
	string tempstr = str.substr(0, 64);
	int    ret = 0;

	const char*  p = tempstr.c_str();
	int    len = tempstr.length();


	//6位 6位 8位
	//6位 6位          

	for (int i = 0; i < len; i++) {
		char tempch = *(p + i);
		int  tempint = (int)tempch;
		tempint = tempint % 64;
		if (tempint != 0) {
			tempint = tempint << 8;
			ret = ret + tempint;
		}
	}

	for (int i = 0; i < len; i++) {
		char tempch = *(p + i);
		int  tempint = (int)tempch;
		tempint = tempint % 31;
		if (tempint != 0) {
			tempint = tempint << 20;
			ret = ret + tempint;
		}
	}

	ret = ret | (m_ProtocolNo._value % 256);
	return ret;

	//__int64 tt_ret = 0;
	//tt_ret = (((__int64)m_ProtocolNo._value)<<32) | (__int64)ret;
	//return tt_ret;
}

int    ProtocolParse_Share::AddtoStringIntLogMap(int data)
{
	WJS_BBSTreeNode<int, int>* pNode = m_StringIntLogMap.Search(data);

	if (pNode) {
		pNode->Second()++;
		return pNode->Second();
	}
	else {
		m_StringIntLogMap.Insert(data, 1);
		return 1;
	}
}
int     ProtocolParse_Share::GetExistStringToIntValue(const string& str)
{
	WJS_BBSTreeNode<string, int>* pNode = m_StringIntSearchMap.Search(str);
	if (pNode)
		return pNode->Second();

	WJS_BBSTreeNode<string, int>* pNode_2 = m_StringIntSearchMap.begin();
	while (pNode_2) {
		pNode_2 = m_StringIntSearchMap.next();
	}

	return 0;
}



void ProtocolParse_Share::CheckDirectoryEixst(CString strDir)
{
	CString tt_str = strDir;
	CString tt_Path;
	CString tt_Left;

	while (true) {
		int index = tt_str.Find(_T("\\"));
		if (index != -1) {
			tt_Left = tt_str.Left(index);
			if (tt_Path.GetLength() > 0)
				tt_Path += (_T("\\") + tt_Left);
			else
				tt_Path = tt_Left;

			if (!PathFileExists(tt_Path)) {
				SECURITY_ATTRIBUTES tt_attr;
				tt_attr.nLength = sizeof(SECURITY_ATTRIBUTES);
				tt_attr.lpSecurityDescriptor = NULL;
				tt_attr.bInheritHandle = TRUE;

				CreateDirectory(tt_Path, &tt_attr);
			}

			tt_Left += _T("\\");
			tt_str = tt_str.Right(tt_str.GetLength() - tt_Left.GetLength());
		}
		else {
			break;
		}

	}
}

void   ProtocolParse_Share::GetRefProtocolMapPar(WJS_BBSTree<int, int>&  StringIntLogMap, WJS_BBSTree<int, string>&  IntStringMap, WJS_BBSTree<string, int>&  StringIntSearchMap)
{
	WJS_BBSTreeNode<int, int>* pNode = m_StringIntLogMap.begin();

	while (pNode) {
		StringIntLogMap.Insert(pNode->First(), pNode->Second());
		pNode = m_StringIntLogMap.next();
	}


	WJS_BBSTreeNode<int, string>* pNode_1 = m_IntStringMap.begin();

	//这里写错了，需要确认是否能改成正确的
	//while(pNode_1)
	while (pNode) {
		IntStringMap.Insert(pNode_1->First(), pNode_1->Second());
		pNode_1 = m_IntStringMap.next();
	}


	WJS_BBSTreeNode<string, int>* pNode_2 = m_StringIntSearchMap.begin();

	//while(pNode_2)
	while (pNode) {
		StringIntSearchMap.Insert(pNode_2->First(), pNode_2->Second());
		pNode_2 = m_StringIntSearchMap.next();
	}
}