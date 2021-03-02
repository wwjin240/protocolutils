#include "ProtocolParse.h"

ProtocolParse_Check::ProtocolParse_Check()
	:m_pCheckParUsed(NULL)
{

}

ProtocolParse_Check::~ProtocolParse_Check()
{

}

bool ProtocolParse_Check::Call_CheckParUsed(const string& strPar)
{
	if (m_pCheckParUsed) {
		if (m_pCheckParUsed->Call_CheckParUsed(strPar))
			return true;
	}

	//写命名空间
	for (size_t i = 0; i < m_NamespaceVec.size(); i++) {
		if (Call_CheckParUsed_InNameSapce(strPar, m_NamespaceVec[i]))
			return true;
	}

	return false;

}

bool    ProtocolParse_Check::Call_CheckParUsed_My(const string& strPar)
{
	//if (strPar.compare("PtPlayerDuoBaoList") == 0)
	//{
	//	int a;
	//	a = 10;
	//}
	//如果存在判定则调用判定
	if (m_pCheckParUsed) {
		if (m_pCheckParUsed->Call_CheckParUsed(strPar))
			return true;

		////确认是否在自己的结构其他结构体中
		//for (unsigned int i = 0; i < m_NamespaceVec.size(); i++)
		//{
		//	if (Call_CheckParUsed_InSelfOtherStruct(strPar, m_NamespaceVec[i]))
		//		return true;
		//}

		return false;
	}


	return true;
}

bool    ProtocolParse_Check::Call_CheckParUsed_InNameSapce(const string& strPar, const stNamespace& pNamespace)
{
	//先写枚举
	//for (unsigned int i = 0; i < pNamespace._EnumVec.size(); i++)
	//{
	//	if (pNamespace._EnumVec[i]._name.compare(strPar.c_str()) == 0)
	//		return true;
	//}

	for (unsigned int i = 0; i < pNamespace._StructVec.size(); i++) {
		if (pNamespace._StructVec[i]._name.compare(strPar.c_str()) == 0)
			return true;

		for (unsigned int j = 0; j < pNamespace._StructVec[i]._ParVec.size(); j++) {
			if (pNamespace._StructVec[i]._ParVec[j]._strParType.compare(strPar.c_str()) == 0)
				return true;

			//if (IsExType_vector(pNamespace._StructVec[i]._ParVec[j]._strParType)
			//	&& GetExTypeParT_vector(pNamespace._StructVec[i]._ParVec[j]._strParType).compare(strPar.c_str()) == 0)
			//{
			//	return true;
			//}

			//if (IsExType_WJSVector(pNamespace._StructVec[i]._ParVec[j]._strParType)
			//	&& GetExTypeParT_WJSVector(pNamespace._StructVec[i]._ParVec[j]._strParType).compare(strPar.c_str()) == 0)
			//{
			//	return true;
			//}
		}
	}

	for (unsigned int i = 0; i < pNamespace._VectorVec.size(); i++) {
		if (pNamespace._VectorVec[i]._name.compare(strPar.c_str()) == 0)
			return true;
		if (pNamespace._VectorVec[i]._ParTName.compare(strPar.c_str()) == 0)
			return true;
		//if (IsExType_vector(pNamespace._VectorVec[i]._ParTName)
		//	&& GetExTypeParT_vector(pNamespace._VectorVec[i]._ParTName).compare(strPar.c_str()) == 0)
		//{
		//	return true;
		//}
	}

	for (unsigned int i = 0; i < pNamespace._WJSVectorVec.size(); i++) {
		if (pNamespace._WJSVectorVec[i]._name.compare(strPar.c_str()) == 0)
			return true;
		if (pNamespace._WJSVectorVec[i]._ParTName.compare(strPar.c_str()) == 0)
			return true;
		//if (IsExType_WJSVector(pNamespace._WJSVectorVec[i]._ParTName)
		//	&& GetExTypeParT_WJSVector(pNamespace._WJSVectorVec[i]._ParTName).compare(strPar.c_str()) == 0)
		//{
		//	return true;
		//}
	}

	for (unsigned int i = 0; i < pNamespace._ClassVec.size(); i++) {
		for (unsigned int j = 0; j < pNamespace._ClassVec[i]._FuncVec.size(); j++) {
			if (pNamespace._ClassVec[i]._FuncVec[j]._bDiscard)
				continue;

			for (unsigned int k = 0; k < pNamespace._ClassVec[i]._FuncVec[j]._InParVec.size(); k++) {
				if (pNamespace._ClassVec[i]._FuncVec[j]._InParVec[k]._strParType.compare(strPar.c_str()) == 0)
					return true;

				////如果参数是vector型
				//if (IsExType_vector(pNamespace._ClassVec[i]._FuncVec[j]._InParVec[k]._strParType)
				//	&& GetExTypeParT_vector(pNamespace._ClassVec[i]._FuncVec[j]._InParVec[k]._strParType).compare(strPar.c_str())==0)
				//{
				//	return true;
				//}

				//if (IsExType_WJSVector(pNamespace._ClassVec[i]._FuncVec[j]._InParVec[k]._strParType)
				//	&& GetExTypeParT_WJSVector(pNamespace._ClassVec[i]._FuncVec[j]._InParVec[k]._strParType).compare(strPar.c_str()) == 0)
				//{
				//	return true;
				//}
			}
		}
	}

	return false;
}
//bool    ProtocolParse_1::Call_CheckParUsed_InSelfOtherStruct(const string& strPar, const stNamespace& pNamespace)
//{
//	if (!m_pCheckParUsed)
//		return false;
//	for (unsigned int i = 0; i < pNamespace._StructVec.size(); i++)
//	{
//		for (unsigned int j = 0; j < pNamespace._StructVec[i]._ParVec.size(); j++)
//		{
//			if (pNamespace._StructVec[i]._ParVec[j]._strParType.compare(strPar.c_str()) == 0)
//			{
//				if (m_pCheckParUsed->Call_CheckParUsed(pNamespace._StructVec[i]._name))
//					return true;
//				break;
//			}
//		}
//	}
//	for (unsigned int i = 0; i < pNamespace._VectorVec.size(); i++)
//	{
//		if (pNamespace._VectorVec[i]._ParTName.compare(strPar.c_str()) == 0)
//		{
//			if (m_pCheckParUsed->Call_CheckParUsed(pNamespace._VectorVec[i]._name))
//				return true;
//		}
//	}
//	return false;
//}

void    ProtocolParse_Check::StartBiaoZhuUsed()
{
	Call_BiaoZhuUsed_My(true);
	if (m_pCheckParUsed)
		Call_BiaoZhuUsed_My(false);
}
bool    ProtocolParse_Check::Call_BiaoZhuUsed_My(bool bFirst)
{
	bool tt_bRet = false;
	if (bFirst) {
		//确认是否在自己的结构其他结构体中
		for (unsigned int i = 0; i < m_NamespaceVec.size(); i++) {
			if (Call_BiaoZhuUsed_My_InNameSpace(m_NamespaceVec[i]))
				tt_bRet = true;
		}
	}
	else {
		for (unsigned int i = 0; i < m_NamespaceVec.size(); i++) {
			if (Call_BiaoZhuUsed_My_InSelfOtherStruct_InNameSpace(m_NamespaceVec[i]))
				tt_bRet = true;
		}
		if (tt_bRet) {
			return Call_BiaoZhuUsed_My(false);
		}

	}

	return tt_bRet;
}
bool    ProtocolParse_Check::Call_BiaoZhuUsed_My_InNameSpace(stNamespace& pNamespace)
{
	bool tt_bRet = false;
	for (unsigned int i = 0; i < pNamespace._StructVec.size(); i++) {
		if (pNamespace._StructVec[i]._bBiaoZhuUsed)
			continue;

		if (Call_CheckParUsed_My(pNamespace._StructVec[i]._name)) {
			pNamespace._StructVec[i]._bBiaoZhuUsed = true;
			tt_bRet = true;
		}

	}

	for (unsigned int i = 0; i < pNamespace._VectorVec.size(); i++) {
		if (pNamespace._VectorVec[i]._bBiaoZhuUsed)
			continue;
		if (Call_CheckParUsed_My(pNamespace._VectorVec[i]._name)) {
			pNamespace._VectorVec[i]._bBiaoZhuUsed = true;
			tt_bRet = true;
		}

	}

	for (unsigned int i = 0; i < pNamespace._WJSVectorVec.size(); i++) {
		if (pNamespace._WJSVectorVec[i]._bBiaoZhuUsed)
			continue;
		if (Call_CheckParUsed_My(pNamespace._WJSVectorVec[i]._name)) {
			pNamespace._WJSVectorVec[i]._bBiaoZhuUsed = true;
			tt_bRet = true;
		}
	}

	return tt_bRet;
}
bool    ProtocolParse_Check::Call_BiaoZhuUsed_My_InSelfOtherStruct_InNameSpace(stNamespace& pNamespace)
{
	bool tt_bRet = false;
	for (unsigned int i = 0; i < pNamespace._StructVec.size(); i++) {
		if (pNamespace._StructVec[i]._bBiaoZhuUsed)
			continue;

		if (Call_BiaoZhuUsed_My_InSelfOtherStruct_InNameSpace2(pNamespace._StructVec[i]._name, pNamespace)) {
			pNamespace._StructVec[i]._bBiaoZhuUsed = true;
			tt_bRet = true;
		}

	}

	for (unsigned int i = 0; i < pNamespace._VectorVec.size(); i++) {
		if (pNamespace._VectorVec[i]._bBiaoZhuUsed)
			continue;
		if (Call_BiaoZhuUsed_My_InSelfOtherStruct_InNameSpace2(pNamespace._VectorVec[i]._name, pNamespace)) {
			pNamespace._VectorVec[i]._bBiaoZhuUsed = true;
			tt_bRet = true;

			//直接将T标注
			Call_BiaoZhuUsed_Direct(pNamespace._VectorVec[i]._ParTName, pNamespace);
		}

	}

	for (unsigned int i = 0; i < pNamespace._WJSVectorVec.size(); i++) {
		if (pNamespace._WJSVectorVec[i]._bBiaoZhuUsed)
			continue;
		if (Call_BiaoZhuUsed_My_InSelfOtherStruct_InNameSpace2(pNamespace._WJSVectorVec[i]._name, pNamespace)) {
			pNamespace._WJSVectorVec[i]._bBiaoZhuUsed = true;
			tt_bRet = true;

			//直接将T标注
			Call_BiaoZhuUsed_Direct(pNamespace._WJSVectorVec[i]._ParTName, pNamespace);
		}
	}

	return tt_bRet;
}
bool    ProtocolParse_Check::Call_BiaoZhuUsed_My_InSelfOtherStruct_InNameSpace2(const string& strPar, stNamespace& pNamespace)
{
	for (unsigned int i = 0; i < pNamespace._StructVec.size(); i++) {
		//未标注不做判定
		if (!pNamespace._StructVec[i]._bBiaoZhuUsed)
			continue;
		for (unsigned int j = 0; j < pNamespace._StructVec[i]._ParVec.size(); j++) {
			if (pNamespace._StructVec[i]._ParVec[j]._strParType.compare(strPar.c_str()) == 0)
				return true;
		}
	}
	for (unsigned int i = 0; i < pNamespace._VectorVec.size(); i++) {
		//未标注不做判定
		if (!pNamespace._VectorVec[i]._bBiaoZhuUsed)
			continue;
		if (pNamespace._VectorVec[i]._ParTName.compare(strPar.c_str()) == 0) {
			return true;
		}

	}
	for (unsigned int i = 0; i < pNamespace._WJSVectorVec.size(); i++) {
		//未标注不做判定
		if (!pNamespace._WJSVectorVec[i]._bBiaoZhuUsed)
			continue;
		if (pNamespace._WJSVectorVec[i]._ParTName.compare(strPar.c_str()) == 0)
			return true;
	}

	return false;
}

void    ProtocolParse_Check::Call_BiaoZhuUsed_Direct(const string& strPar, stNamespace& pNamespace)
{
	for (unsigned int i = 0; i < pNamespace._StructVec.size(); i++) {
		if (pNamespace._StructVec[i]._bBiaoZhuUsed)
			continue;

		if (pNamespace._StructVec[i]._name.compare(strPar.c_str()) == 0) {
			pNamespace._StructVec[i]._bBiaoZhuUsed = true;
			return;
		}
	}
	for (unsigned int i = 0; i < pNamespace._VectorVec.size(); i++) {
		if (pNamespace._VectorVec[i]._bBiaoZhuUsed)
			continue;
		if (pNamespace._VectorVec[i]._name.compare(strPar.c_str()) == 0) {
			pNamespace._VectorVec[i]._bBiaoZhuUsed = true;
			Call_BiaoZhuUsed_Direct(pNamespace._VectorVec[i]._ParTName, pNamespace);
			return;
		}

	}
	for (unsigned int i = 0; i < pNamespace._WJSVectorVec.size(); i++) {
		if (pNamespace._WJSVectorVec[i]._bBiaoZhuUsed)
			continue;
		if (pNamespace._WJSVectorVec[i]._name.compare(strPar.c_str()) == 0) {
			pNamespace._WJSVectorVec[i]._bBiaoZhuUsed = true;
			Call_BiaoZhuUsed_Direct(pNamespace._WJSVectorVec[i]._ParTName, pNamespace);
			return;
		}

	}

}