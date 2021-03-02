//
#include "ProtocolParse.h"
#include <windows.h>
#include "CCreateComment.h"
#include <process.h>
#include "CCreateEventFunc.h"
#include "WJS_JsonStream.h"
#include "stdio.h"


#include <regex>
#include "ProtocolUtils.h"
#include "ProtocolParse_Writer_CPlus.h"
#include "ProtocolParse_Writer_IPhone.h"
#include "ProtocolParse_Writer_Flash.h"
#include "ProtocolParse_Writer_Java.h"
#include "ProtocolParse_Writer_CSharp.h"
#include "ProtocolParse_Writer_Lua.h"
#include "ProtocolParse_Writer_JavaScript.h"
#include "ProtocolParse_Writer_TypeScript.h"
#include "ProtocolParse_Writer_Python.h"
#include "ProtocolParse_Writer_TSMiddle.h"

#include "ShareHead.h"
#include "ParsManager.h"
#include "PickParUtil.h"
#include "ProtocolUtils.h"
#include <set>
#include <fstream>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace ServerCommon;

//-i���Ҫ������Э���ļ�
//-ops���������������������·����/
//-opc���������ͻ��������·����/
//-opsc������-scģʽ�½���������·����/

//-s����Ҫ����C++������
//-c����Ҫ����C++�ͻ���
//-sc����C++�ͻ��˺ͷ�����������һ���ļ���
//-ac����Ҫ������C++android�ͻ���
//-asc����Ҫ������C++android����˺Ϳͻ���


//-is�������Iphoneƽ̨�ķ�������
//-ic�������Iphoneƽ̨�Ŀͻ���
//-isc�������Iphoneƽ̨�ķ������˺Ϳͻ���

//-ws�������Flashƽ̨�ķ�������
//-wc�������Flashƽ̨�Ŀͻ�����
//-wsc�������Flashƽ̨�ķ������Ϳͻ���

//-js�������Javaƽ̨�ķ����
//-jc�������Javaƽ̨�Ŀͻ���
//-jsc�������Javaƽ̨�ķ���˺Ϳͻ���

//-jp��������java����

//-cps�������C#ƽ̨�ķ����
//-cpc�������C#ƽ̨�Ŀͻ���
//-cpsc�������C#ƽ̨�ķ���˺Ϳͻ���

//-luas �������Luaƽ̨�ķ����
//-luac �������Luaƽ̨�Ŀͻ���
//-luasc �������Luaƽ̨�ķ���˺Ϳͻ���

//-tss �������tsƽ̨�ķ����
//-tsc �������tsƽ̨�Ŀͻ���
//-tssc �������tsƽ̨�ķ���˺Ϳͻ���  

//-tsm  ��������һ���м��,���·��

//-tsms �������Ϊ����˰���м��
//-tsmc �������Ϊ�ͻ��˰���м��
//-tsmsc �������Ϊ�������ͻ���ͨ�õ��м��

//-pys �������pythonƽ̨�ķ����
//-pyc �������pythonƽ̨�Ŀͻ���
//-pysc �������pythonƽ̨�ķ���˺Ϳͻ���  

//�ݲ�֧��javascript
//-jsts�������JavaScriptƽ̨�ķ����
//-jstc�������JavaScriptƽ̨�Ŀͻ���
//-jstsc�������JavaScriptƽ̨�ķ���˺Ϳͻ���


//-stdafx      c++�����ļ�����Ԥ����ͷ
//-outjson     c++֧�����json����

//-scsplits   scģʽ�зֳ�s
//-scsplitc   scģʽ�зֳ�c

//-purevirtual �Ƿ��飬����SCģʽ
//-excpetmoba �Ƿ��ų�moba
//-fastmodel  ֧�ֿ���ģ�͵��µĴ�������ǿյ�
//-funcidsort ���һ��ע�Ͱ���Ź���ĺ���id,���ڱȶ�
//-module      �Ƿ���ts��ʹ��ģ�飬Ĭ���������ռ�

//-withlog    �Ƿ��log,��CSharp��ʹ�ã���ʹ����UnityEngine

//-relayout  ���²���,Э���ļ����¸�ʽ��

//-upart      ʹ�����ķ���
//-cjson     ת����json,���ת�����ļ�
//-comment   ����ע��,ʹ��-i������ļ�,��Ҫ�ǲ鿴ע�͵��м�ת���ļ�
//-eventfunc �����¼�����,ʹ��-i������ļ�,�����CSharpר�ã���C++��������ʽ����

//-copy      ����ʲô������
//-copy2      

//Ĭ����c11
//-c98       ����ʹ��c98
//-c11       ����ʹ��c++11
//-c14       
//-c17
//-c20

//-windowsbyte  ǿ��ʹ��windows��byte



/////////////
//
//////////////
//<request name = "XiaoZhuShouRequest" FuncNew="1">  
//FuncNew��ʾ���еغ���id���㷽������New="1"����

//<method name = "OnUpdateServerList" Discard="1">
//Discard�����˺����������������뷴���к�����ֻ�����亯��id

//<method name="ReqPlayerLobbyNoticeList" FuncId="1001">
//FuncId����ָ������id,���65535,�ĳɿ���ָ�����id

//<method name="ReqVIPValue" New="1">
//New�������·������㺯��id

//�����ļ������ʾ���Ǻ���id��������ͻ���б�

//MD5=dfe0896b5247d718510a448c6522c139BDM
//Ϊ���ж��Ƿ����ı䣬��Ҫ���������ļ�

//<method name = "GWPlayerChangeNick" version="parversion2">
//version����Э�����

//C++��
//�ṹ����ӹ��� <struct name = "PtBeyondGoodsInfo2" version="parversion3" construct="1">
//�ṹ���Ա���Ĭ��ֵ<member name = "Version"  type="short" version="parversion3" default="0"/>

//�����ֵ�Ŵ���,���ö�Ӧ�����ر�ʶ�е�λ��[0,30],Ϊ�������31λ,һ���ṹ����������������ռ��ͬһλ
//���ܶԴ����ʹ��޵�����Э����ݣ����뱾����д����Ͳ��ܼ���
//<member name="BigInt1" default="0" type="__int64" transneedvalue="1"/>



//����SCFlag,Ҳ���ǽ�S/C ��Э�飬ͨ��SC�ķ�ʽ���ɣ�ֻȡ��SCFlag��
//<method name = "NotifyDuoBaoResult" New="1" SCFlag="1">


//����MobaFlag,ͨ�������������ų��˱�ʶ
//<method name="ReqPlayerMobaStatus" New="1" MobaFlag="1">

//����Layer,�����ɶ༶��
//<method name = "PlayerLoginRoom" vertion="parversion6" Layer="1">

//hcpp�����������ɵ���.h��.cpp�汾
//fujianN�����Լ��½Ӹ���,ֻ�ܰ�˳������
//yuanjian�����Լ���˭�ĸ���
//<namespacename value1 = "ServerCommon" value2 = "ServerCommon" value3 = "hcpp" fujian1 = "ServerCommon_Moba" / >
//<namespacename value1 = "ServerCommon" value2 = "ServerCommon" value3 = "hcpp" yuanjian = "ServerCommon" / >
//������ԭ����Ч

//startversion_ts ��ʼ�汾 ts����ʹ��
//<struct name = "PtPlayerBaseInfo" construct="1" version = "parversion12" startversion_ts="parversion12">

//��������ʹ�����㷨���㺯��idʱ�������������Ϊ�ɵ�
//<method name = "PlayerLoginRoom" KeepOld="1">

//֧��partд��
//<request name = "GateWay2ClientRequest" part="1" FuncNew="1">
//<request name = "GateWay2ClientRequest" part="2" FuncNew="1">

//enum����֧��New="1"���°�֮�⣬������KeepOld="1"


string g_ProgressId = "";
vector<string> g_AllTempFilesList;

//string g_CommandPar[] = { "-i","-ops","-opc","-s","-c","-opsc","-sc","-is","-ic","-isc",
//"-ws","-wc","-wsc","-ac","-asc","-js","-jc","-jsc","-jp","-cps",
//"-cpc","-cpsc","-comment","-eventfunc","-copy","-copy2","-luas","-luac","-luasc","-exceptmoba",
//"-withlog","-purevirtual","-hcpp","-stdafx","-tss","-tsc","-tssc","-module","-pys","-pyc",
//"-pysc" ,"-outjson","-scsplits","-scsplitc","-funcidsort","-upart","-tsm","-tsms","-tsmc","-tsmsc",
//"-cjson","-fastmodel","-jsts","-jstc","-jstsc","-relayout" };
////#define COMMANDPARNUM 55
//
//enum eCurParState {
//	eCPS_None = 0,
//	eCPS_ProtocolName,
//	eCPS_OutPathServer,
//	eCPS_OutPathClient,
//	eCPS_CreateServerFile,
//	eCPS_CreateClientFile,
//	eCPS_OutPatchSC,
//	eCPS_CreateSCFile,
//	eCPS_CreateIphoneServerFile,
//	eCPS_CreateIphoneClientFile,
//	eCPS_CreateIphoneSCFile,
//	eCPS_CreateFlashServerFile,
//	eCPS_CreateFlashClientFile,
//	eCPS_CreateFlashSCFile,
//	eCPS_CreateAndroidClientFile,
//	eCPS_CreateAndroidSCFile,
//	eCPS_CreateJavaServerFile,
//	eCPS_CreateJavaClientFile,
//	eCPS_CreateJavaSCFile,
//	eCPS_JavaPackName,
//	eCPS_CreateCSharpServerFile,
//	eCPS_CreateCSharpClientFile,
//	eCPS_CreateCSharpSCFile,
//	eCPS_CreateComment,
//	eCPS_CreateEventFunc,
//	eCPS_CreateCopy,
//	eCPS_CreateCopy2,
//	eCPS_CreateLuaServerFile,
//	eCPS_CreateLuaClientFile,
//	eCPS_CreateLuaSCFile,
//	eCPS_ExceptMoba,
//	eCPS_WithLog,
//	eCPS_PureVirtual,
//	eCPS_HCPP,
//	eCPS_Stdafx,
//	eCPS_CreateTScriptServerFile,
//	eCPS_CreateTScriptClientFile,
//	eCPS_CreateTscriptSCFile,
//	eCPS_TscriptUseModule,
//	eCPS_CreatePythonServerFile,
//	eCPS_CreatePythonClientFile,
//	eCPS_CreatePythonSCFile,
//	eCPS_OutJson,
//	eCPS_SCSPLITES, //ʹ��SCģʽ�������S
//	eCPS_SCSPLITEC, //ʹ��SCģʽ�������C
//	eCPS_FuncIdSort,//���һ��ע�Ͱ���Ź���ĺ���id
//	eCPS_UsePart,   //ʹ��part����
//	eCPS_TSJsonMiddle, //����ts���м��
//	eCPS_TSMiddleServer,
//	eCPS_TSMiddleClient,
//	eCPS_TSMiddleSC,
//	eCPS_CJson,   //ת��json
//	eCPS_FastModel,
//	eCPS_CreateJavaScriptServerFile,
//	eCPS_CreateJavaScriptClientFile,
//	eCPS_CreateJavaScriptSCFile,
//	eCPS_Relayout,
//	eCPS_Max
//
//};
//
//int ParCommandIndex(const char* pCh)
//{
//	for (int i = 0; i < sizeof(g_CommandPar) / sizeof(string); i++) {
//		if (g_CommandPar[i].compare(pCh) == 0)
//			return i;
//	}
//	return -1;
//}

bool g_bCreateCopy = false;
bool g_bCreateCopy2 = false;


void CallProtocolWriter_Server(ProtocolParse_Writer* pProtocolWriter, bool bCreateServerFile, string& strOutPathServer, const string& strFullPath)
{
	if (bCreateServerFile) {
		if (strOutPathServer.length() == 0) {
			strOutPathServer = ProtocolUtils::PickPath(strFullPath, 1);
			strOutPathServer.append("\\");
		}
		pProtocolWriter->FreeWriteLog();
		pProtocolWriter->CreateFile_Path(strOutPathServer, 1, false);
	}
}
void CallProtocolWriter_Client(ProtocolParse_Writer* pProtocolWriter, bool bCreateClientFile, string& strOutPathClient, const string& strFullPath)
{
	if (bCreateClientFile) {
		if (strOutPathClient.length() == 0) {
			strOutPathClient = ProtocolUtils::PickPath(strFullPath, 1);
			strOutPathClient.append("\\");
		}
		pProtocolWriter->FreeWriteLog();
		pProtocolWriter->CreateFile_Path(strOutPathClient, 0, false);
	}
}

void CallProtocolWriter_SC(ProtocolParse_Writer* pProtocolWriter, bool bCreateSCFile, string& strOutPathSC, const string& strFullPath)
{
	if (bCreateSCFile) {
		if (strOutPathSC.length() == 0) {
			strOutPathSC = ProtocolUtils::PickPath(strFullPath, 1);
			strOutPathSC.append("\\");
		}
		pProtocolWriter->FreeWriteLog();
		pProtocolWriter->CreateFile_Path(strOutPathSC, 2, false);
	}
}


string g_singleCommands[] = { "-s","-c","-sc","-ac","-asc","-is","-ic","-isc","-ws","-wc","-wsc","-js","-jc","-jsc","-cps","-cpc","-cpsc","-luas","-luac","-luasc",
"-tss","-tsc","-tssc","-tsms","-tsmc","-tsmsc","-pys","-pyc","-pysc","-jsts","-jstc","-jstsc","-stdafx","-outjson","-scsplits","-scsplitc","-comment",
"-eventfunc","-purevirtual","-excpetmoba","-fastmodel","-funcidsort","-module","-withlog","-relayout","-c98","-c11","-c14","-c17","-c20","-windowsbyte" };


/**
 * @brief ȡ��Э����������
 * @param parMaps 
 * @return 
*/
eProtocolWriterType GetProtocolWriterType(const std::map<std::string, ServerCommon::PtKVR>& parMaps)
{
	PickParUtil t_util;
	std::vector<string> t_cplus{"-s","-c","-sc","-ac","-asc"};
	for (auto& r : t_cplus) {
		if (t_util.GetInt_KVR(parMaps, r)!=0)
			return eProtocolWriterType::CPlus;
	}

	std::vector<string> t_iphone{ "-is","-ic","-isc" };
	for (auto& r : t_iphone) {
		if (t_util.GetInt_KVR(parMaps, r) != 0)
			return eProtocolWriterType::IPhone;
	}

	std::vector<string> t_flash{ "-ws","-wc","-wsc" };
	for (auto& r : t_flash) {
		if (t_util.GetInt_KVR(parMaps, r) != 0)
			return eProtocolWriterType::Flash;
	}

	std::vector<string> t_java{ "-js","-jc","-jsc" };
	for (auto& r : t_java) {
		if (t_util.GetInt_KVR(parMaps, r) != 0)
			return eProtocolWriterType::Java;
	}

	std::vector<string> t_csharp{ "-cps","-cpc","-cpsc" };
	for (auto& r : t_csharp) {
		if (t_util.GetInt_KVR(parMaps, r) != 0)
			return eProtocolWriterType::CSharp;
	}

	std::vector<string> t_lua{ "-luas","-luac","-luasc" };
	for (auto& r : t_lua) {
		if (t_util.GetInt_KVR(parMaps, r) != 0)
			return eProtocolWriterType::Lua;
	}

	std::vector<string> t_typescript{ "-tss","-tsc","-tssc" };
	for (auto& r : t_typescript) {
		if (t_util.GetInt_KVR(parMaps, r) != 0)
			return eProtocolWriterType::TypeScript;
	}

	std::vector<string> t_typescriptmiddle{ "-tsm","-tsms","-tsmc","-tsmsc" };
	for (auto& r : t_typescriptmiddle) {
		if (t_util.GetInt_KVR(parMaps, r) != 0)
			return eProtocolWriterType::TSMiddle;
	}

	std::vector<string> t_python{ "-pys","-pyc","-pysc" };
	for (auto& r : t_python) {
		if (t_util.GetInt_KVR(parMaps, r) != 0)
			return eProtocolWriterType::Python;
	}

	std::vector<string> t_javascript{ "-jsts","-jstc","-jstsc" };
	for (auto& r : t_javascript) {
		if (t_util.GetInt_KVR(parMaps, r) != 0)
			return eProtocolWriterType::JavaScript;
	}

	return eProtocolWriterType::eNone;
}

int main(int argc, char* argv[])
{
	//ProtocolParse t_Parse(R"(H:\proj_protocols_Tx\Provide_Proj2015\bin32\protocolCMP\Y\GateWay2Client.xml)");
	//if (t_Parse.Parser()) {
	//	//t_Parse.m_func_IsSkipFuncPars = []()->bool {
	//	//	return true;
	//	//};
	//	t_Parse.WriterToXML("test.xml");
	//}
	//
	//return 0;

	PickParUtil t_util;

	std::map<std::string, ServerCommon::PtKVR> t_parMaps;

	ParsManager t_ParsManager;
	t_ParsManager.InitExParCfg();
	t_ParsManager.Init(argc, argv);

	g_ProgressId = t_util.GetString_KVR(t_ParsManager._parMaps,"progressid");

	//�ý�����Ĳ�������һ���ְ�������������ȡ
	argc = t_ParsManager._argc;

	if (argc <= 1) {
		printf("��ͨ��-help�鿴�������\n");
		return 0;
	}
	//printf(argv[0]);
	//printf("\n");

	//��ȡ��ȫ·��
	string strFullPath = t_util.GetString_KVR(t_ParsManager._parMaps,"fullpath");
	//ȡ���ײ�
	string firstPar = t_ParsManager.BeginGetPar(1);

	if (firstPar == "-help") {

		if (argc == 2) {
			printf("-help [option]\n");
			printf("option֧�� enum,struct,vector,protocol,part,method,membertype,generator,example\n");
			return 0;
		}
		else{
			string t_optionPar = t_ParsManager.GetNextPar();
			if (t_optionPar == "enum") {
				string t_tip = 
R"(
<enum name = "ePhoneLoginFailReason" New="1" version="parversion2">
<member name = "ePLFR_0" KeepOld="1"/>
<member name = "ePLFR_LimitReg" version="parversion2"/>
</enum>)";  
				printf("%s\n",t_tip.c_str());

				printf("New��ʹ���µ��㷨����ö��ֵ\n");
				printf("KeepOld����ʹ�þɵ��㷨���㵱ǰö��ֵ\n");
				return 0;

			}
			else if (t_optionPar == "struct") {
				string t_tip =
R"( <struct name="PtDoubleColorCfg" construct="1" version="parversion2" startversion_ts = "parversion2">
	<member name="gameId" type="int"/>
	<member name="everdayFreeCount" type="int" default="0" transneedvalue="0"/>
	<member name="videoAd" type="int" default="0" transneedvalue="1"/>	
	<member name="everdayLimitCount" type="int" default="0" transneedvalue="2"/>
	<member name="initPoolValue" type="__int64"/>	
	<member name="poolMoneyType" type="int"/>	
	<member name="rewards" type="string"/>	
	<member name = "kaiJiangPlan" type="string" version="parversion2"/>
  </struct>

  <struct name="PtDoubleColorCfgServer" construct="1" inherited="PtDoubleColorCfg">
	<member name="initPoolValue" type="__int64"/>	
  </struct>
)";
				printf("%s\n", t_tip.c_str());
				printf("construct��ʾ���ɹ���\n");
				printf("default��ʾĬ��ֵ\n");
				printf("����ʱ����ʹ��ָ����Ĭ��ֵ������ʹ�ö�Ӧ���͵�Ĭ��ֵ\n");
				printf("inherited��ʾ�̳�֮�ĸ�������;�����޸ļ̳й�ϵ\n");
				printf("transneedvalue:�����ֵ�Ŵ���,���ö�Ӧ�����ر�ʶ�е�λ��[0,30],Ϊ�������31λ,һ���ṹ����������������ռ��ͬһλ\n");
				printf("transneedvalue:���ܶԴ����ʹ��޵�����Э����ݣ����뱾����д����Ͳ��ܼ���\n");
				printf("startversion_ts:��ʾ��Ӧts����ʼ�汾\n");

				return 0;

			}
			else if (t_optionPar == "vector") {
				string t_tip = 
R"(<sequence name = "PtDoubleColorCfgList" element = "PtDoubleColorCfg"/>)";
				printf("%s\n", t_tip.c_str());
				printf("��Ӧ��c++��std::vector<PtDoubleColorCfg>\n");
				return 0;
			}
			else if (t_optionPar == "protocol") {
				string t_tip = 
R"(<namespacename value1="ServerCommon" value2="ServerCommon" value3="hcpp" shortname="bbb"/>)";
				printf("%s\n", t_tip.c_str());

				printf("value1�����������ռ�,value2��ͻ��������ռ�\n");
				printf("value3���ָ��Ϊhcpp��ʾͬʱ����.h��.cpp\n");
				printf("shortname��ʾ����\n");

				t_tip = 
R"(<protocolRef   value1 = "ServerCommon.xml" value2 = "ServerCommonProtocol.h" value3 = "ServerCommonProtocol.h" value4 = "ServerCommon" path="..\NewProtocolXml"/ >)";
				printf("%s\n", t_tip.c_str());

				printf("value1�����õ�Э���ļ�,value2,value3�����õ�ͷ�ļ���\n");
				printf("value4��ʹ�õ������ռ���\n");
				printf("NotCreateFile������ʵ�ʵ������ļ�\n");
				printf("path���������ļ��뵱ǰ�ļ���·����ϵ\n");

				t_tip = 
					R"(<protocolNo name = "GateWay2Client_No" value = "1000" / >)";
				printf("%s\n", t_tip.c_str());

				printf("protocolNo����ָ��Э��ű�����ֵ\n");

				t_tip = R"(<protocol name = "GateWay2Client"></protocol>)";
					
				printf("%s\n", t_tip.c_str());
				printf("protocol ��ʾ����Э��Σ���ָ������\n");

				t_tip = 
R"(  <VManger name="Version">
  <member name = "defversion" value = "1"/>
  <member name = "parversion1" value = "1"/>
  <member name = "parversion2" value = "2"/>
  <member name = "parversion3" value = "3"/>
  <member name = "parversion4" value = "4"/>
  </VManger>)";
				printf("%s\n", t_tip.c_str());
				printf("�����еİ汾ֵ���ж���\n");

				t_tip=R"(
<request name = "GateWay2ClientRequest"  FuncNew="1"> 
</request>
<callback name = "GateWay2ClientCallback"  FuncNew="1">
</callback>)";
				printf("%s\n", t_tip.c_str());
				printf("��Ӧ��C/S����request,Ҳ��callback\n");
				printf("��Ӧ��SCֻ��request\n");
				printf("FuncNew���������ڵ�method��ʹ���·�������ID\n");

				
				return 0;
			}
			else if (t_optionPar == "part") {
				string t_tip =
R"(<PartManager name="LoginServer">  
	<member name = "��������"/>
	<member name = "΢�Ű���"/> 	
 </PartManager>)";
				printf("%s\n", t_tip.c_str());
				printf("����ÿ��part��������֣��Լ�������Щpart\n");

				t_tip=R"(
<request name = "GateWay2ClientRequest" part="��¼��" FuncNew="1"> 
</request>
<callback name = "GateWay2ClientCallback" part="��¼��" FuncNew="1">
</callback>
)";
				printf("%s\n", t_tip.c_str());
				printf("����ÿ��part����Э��,�Լ�part����\n");


				return 0;
			}
			else if (t_optionPar == "method") {
			    string t_tip = R"(
	<method name="ReqGameXiangGuangData" New="1" version="parversion2" SCFlag="1" Discard="1" FuncId="1001" KeepOld="1">
	  <param name="gameId" type="int"/>
	  <param name="dataType" type="int"/>
	  <param name="otherPlayerId" type="int" version="parversion2"/>
	</method>)";
				printf("%s\n", t_tip.c_str());

				printf("�������壬New��ʹ���µķ������㷽��ID\n");
				printf("SCFlag����C/SЭ���У������շ����ַ���\n");
				printf("���ָ��Discard��ʾ�˺���ֻ����ID���㣬�����ɺ���\n");
				printf("FuncId��ʾǿ��ָ��������ID,������ͨ������ó�\n");
				printf("KeepOld��ʾ�԰��ɷ������㺯��ID����������ָ�����·���FuncNewʱ�����ݾɰ�\n");
				printf("MobaFlag�����ʶ������ʹ��\n");
				printf("Layer��㣬ָ��ֵ����ݲ�ͬ�㣬�����������ϵ,���Ҳ��Ҫʹ����\n");
				printf("SCSplite=\"S\"����SC���Э�飬ǿ�зֳ������\n");
				printf("SCSplite=\"C\"����SC���Э�飬ǿ�зֳ��ͻ���\n");

				return 0;
			}
			else if (t_optionPar == "membertype") {
			string t_tip = 
R"({"void","int","unsigned int","short","unsigned short","char","byte","BYTE","bool","BOOL", 
"DWORD","float","long","unsigned long","string","__int64",
"wjs_char32","wjs_char64","wjs_char128","wjs_char256","wjs_char512","wjs_char1024",
"long long","unsigned char","wjs_utf8","u16string","wstring" };)";
			printf("%s\n", t_tip.c_str());

			printf("����Ϊ֧�ֵĻ������ͣ���ò�Ҫ��byte,��Ϊvs2019���г�ͻ������unsigned char\n");
			printf("©����double���ͣ��Ժ�Ჹ\n");
			return 0;

			}
			else if (t_optionPar == "generator") {

			string t_tip = 
R"(-i���Ҫ������Э���ļ�
-ops���������������������·����/
-opc���������ͻ��������·����/
-opsc������-scģʽ�½���������·����/

-s����Ҫ����C++������
-c����Ҫ����C++�ͻ���
-sc����C++�ͻ��˺ͷ�����������һ���ļ���
-ac����Ҫ������C++android�ͻ���
-asc����Ҫ������C++android����˺Ϳͻ���


-is�������Iphoneƽ̨�ķ�������
-ic�������Iphoneƽ̨�Ŀͻ���
-isc�������Iphoneƽ̨�ķ������˺Ϳͻ���

-ws�������Flashƽ̨�ķ�������
-wc�������Flashƽ̨�Ŀͻ�����
-wsc�������Flashƽ̨�ķ������Ϳͻ���

-js�������Javaƽ̨�ķ����
-jc�������Javaƽ̨�Ŀͻ���
-jsc�������Javaƽ̨�ķ���˺Ϳͻ���

-jp��������java����

-cps�������C#ƽ̨�ķ����
-cpc�������C#ƽ̨�Ŀͻ���
-cpsc�������C#ƽ̨�ķ���˺Ϳͻ���

-luas �������Luaƽ̨�ķ����
-luac �������Luaƽ̨�Ŀͻ���
-luasc �������Luaƽ̨�ķ���˺Ϳͻ���

-tss �������tsƽ̨�ķ����
-tsc �������tsƽ̨�Ŀͻ���
-tssc �������tsƽ̨�ķ���˺Ϳͻ���  

-tsm  ��������һ���м��,���·��

-tsms �������Ϊ����˰���м��
-tsmc �������Ϊ�ͻ��˰���м��
-tsmsc �������Ϊ�������ͻ���ͨ�õ��м��

-pys �������pythonƽ̨�ķ����
-pyc �������pythonƽ̨�Ŀͻ���
-pysc �������pythonƽ̨�ķ���˺Ϳͻ���  

�ݲ�֧��javascript
-jsts�������JavaScriptƽ̨�ķ����
-jstc�������JavaScriptƽ̨�Ŀͻ���
-jstsc�������JavaScriptƽ̨�ķ���˺Ϳͻ���


-stdafx      c++�����ļ�����Ԥ����ͷ
-outjson     c++֧�����json����

-scsplits   scģʽ�зֳ�s
-scsplitc   scģʽ�зֳ�c

-purevirtual �Ƿ��飬����SCģʽ
-excpetmoba �Ƿ��ų�moba
-fastmodel  ֧�ֿ���ģ�͵��µĴ�������ǿյ�
-funcidsort ���һ��ע�Ͱ���Ź���ĺ���id,���ڱȶ�
-module      �Ƿ���ts��ʹ��ģ�飬Ĭ���������ռ�

-withlog    �Ƿ��log,��CSharp��ʹ�ã���ʹ����UnityEngine

-relayout  ���²���,Э���ļ����¸�ʽ��

-upart      ʹ�����ķ���
-cjson     ת����json,���ת�����ļ�
-comment   ����ע��,ʹ��-i������ļ�,��Ҫ�ǲ鿴ע�͵��м�ת���ļ�
-eventfunc �����¼�����,ʹ��-i������ļ�,�����CSharpר�ã���C++��������ʽ����

-copy      ����ʲô������
-copy2      

Ĭ����c11
-c98       ����ʹ��c98
-c11       ����ʹ��c++11
-c14       
-c17
-c20

-windowsbyte  ǿ��ʹ��windows��byte
)";
			

			printf("%s\n", t_tip.c_str());
			return 0;
			}
			else if (t_optionPar == "example") {
				string t_example = t_ParsManager.GetNextPar();
				if (t_example == "") {
					string t_tip = R"(��Ҫ�ٽ�һ�����²�:python,cplus,csharp,java,lua,typescript,typescript-module,monojson,flash,eventfunc,comment)";
					printf("%s\n", t_tip.c_str());
					return 0;
				}
				else if (t_example == "python") {
					string t_tip =
R"(protocolgenxml.exe -i ../NewProtocolXml/GateWay2Client.xml -opc Python/C/GW/  -pyc
protocolgenxml.exe -i ../NewProtocolXml/GateWay2Client.xml -ops Python/S/GW/  -pys
protocolgenxml.exe -i  ../NewProtocolXml/Client2Room2.xml -opsc Python/SC/Room/ -pysc
protocolgenxml.exe -i  ../NewProtocolXml/Client2Room2.xml -opsc Python/SC_C/ -pysc  -scsplitc)";
					printf("%s\n", t_tip.c_str());
					return 0;
				}
				else if (t_example == "cplus") {
					string t_tip =
R"(protocolgenxml.exe -i ../NewProtocolXml/GateWay2Client.xml  -ops Cplus/GW/S/OverSeaServer/ -s -stdafx -outjson -funcidsort -upart OverSeaServer
protocolgenxml.exe -i ../NewProtocolXml/GateWay2Client.xml  -opc  Cplus/GW/C/OverSea/ -c -stdafx -outjson -funcidsort -upart OverSea
protocolgenxml.exe -i ../NewProtocolXml/Client2Room2.xml -opsc   Cplus/Room/SC/ -sc -scsplits)";
					printf("%s\n", t_tip.c_str());
					return 0;
				}
				else if (t_example == "csharp") {
					string t_tip =
R"(protocolgenxml.exe -i ../NewProtocolXml/Client2Room2.xml -opsc Csharp/Room/SC/ -cpsc
protocolgenxml.exe -i ../NewProtocolXml/GateWay2Client.xml  -ops Csharp/GW/S/OverSeaServer/ -cps  -upart OverSeaServer)";
					printf("%s\n", t_tip.c_str());
					return 0;
				}
				else if (t_example == "java") {
					string t_tip =
						R"(protocolgenxml.exe -i  ../NewProtocolXml/Client2Room2.xml -opsc Java/Room/C/ -jsc -scsplitc
protocolgenxml.exe -i  ../NewProtocolXml/Client2Room2.xml -opsc Java/Room/SC/ -jsc)";
					printf("%s\n", t_tip.c_str());
					return 0;
				}
				else if (t_example == "lua") {
					string t_tip =
						R"(protocolgenxml.exe -i ../NewProtocolXml/Client2Room2.xml -opsc Lua/Room/C/ -luasc -scsplitc
protocolgenxml.exe -i  ../NewProtocolXml/Client2Room2.xml -opsc Lua/Room/SC/ -luasc)";
					printf("%s\n", t_tip.c_str());
					return 0;
				}
				else if (t_example == "typescript") {
					string t_tip =
						R"(protocolgenxml.exe -i ../NewProtocolXml/Client2Room2.xml -opsc TScript/Room/C/ -tssc  -scsplitc
protocolgenxml.exe -i ../NewProtocolXml/Client2Room2.xml -opsc TScript/Room/SC/ -tssc)";
					printf("%s\n", t_tip.c_str());
					return 0;
				}
				else if (t_example == "typescript-module") {
					string t_tip =
						R"(protocolgenxml.exe -i ../NewProtocolXml/Client2Room2.xml -opsc TScript_M/Room/C/ -tssc -module -scsplitc
protocolgenxml.exe -i  ../NewProtocolXml/Client2Room2.xml -opsc TScript_M/Room/SC/ -tssc  -module)";
					printf("%s\n", t_tip.c_str());
					return 0;
				}
				else if (t_example == "monojson") {
					string t_tip =
						R"(protocolgenxml.exe -cjson login_2019_3_12.json)";
					printf("%s\n", t_tip.c_str());
					printf("ת���ɿ��Դ洢��monogodb��json���б�\n");
					return 0;
				}
				else if (t_example == "flash") {
					string t_tip =
						R"(protocolgenxml.exe -i ../NewProtocolXml/Client2Room2.xml -opsc Flash/Room/SC/ -wsc
protocolgenxml.exe -i ../NewProtocolXml/GateWay2Client.xml  -ops Flash/GW/S/OverSeaServer/ -ws  -upart OverSeaServer)";
					printf("%s\n", t_tip.c_str());
					return 0;
				}
				else if (t_example == "eventfunc") {
					string t_tip =
						R"(protocolgenxml.exe -i Cplus/GW/S/OverSeaServer/GateWay2ClientProtocol_server.h  -eventfunc)";
					printf("%s\n", t_tip.c_str());
					printf("���ɵ��ļ�ΪGateWay2ClientProtocol_server.h.copy\n");
					return 0;
				}
				else if (t_example == "comment") {
					string t_tip =
						R"(protocolgenxml.exe -i  ../NewProtocolXml/GateWay2Client.xml -comment)";
					printf("%s\n", t_tip.c_str());
					printf("��Ҫ��Э�����ɵ��м��ļ�����ע��ת��xml��ʽ�ڲ���GateWay2Client.xml.copy25284��25284Ϊ����ID\n");
					return 0;
				}
				
			}
		}


		return 0;
	}

	//�����
	std::set<string> t_singleCommandSet;
	for (int i = 0; i < sizeof(g_singleCommands) / sizeof(string); i++) {
		t_singleCommandSet.insert(g_singleCommands[i]);
	}

	while (firstPar != "") {
		if (firstPar == "-i") {
			//����һ������Э���ļ���
			string t_protocolName = t_ParsManager.GetNextPar();
			if (t_protocolName.length() > 0) {
				t_protocolName = ProtocolUtils::ComputerPath(strFullPath, t_protocolName);
				
				t_parMaps.insert(make_pair("protocolname", PtKVR("protocolname", t_protocolName, "")));
			}
			else {
				printf("-i���Ҫ������Э���ļ�\n");
				return 0;
			}
		}
		else if (firstPar == "-ops") {
			//���������������������·����/
			string t_serverOutPath = t_ParsManager.GetNextPar();
			if (t_serverOutPath.length() > 0) {
				t_serverOutPath = ProtocolUtils::ComputerPath(strFullPath, t_serverOutPath);
				t_parMaps.insert(make_pair("severoutpath", PtKVR("severoutpath", t_serverOutPath, "")));
			}
			else {
				printf("-ops ���������������������·����/\n");
				return 0;
			}
		}
		else if (firstPar == "-opc") {
			//-opc���������ͻ��������·����/
			string t_clientOutPath = t_ParsManager.GetNextPar();
			if (t_clientOutPath.length() > 0) {
				t_clientOutPath = ProtocolUtils::ComputerPath(strFullPath, t_clientOutPath);
				t_parMaps.insert(make_pair("clientoutpath", PtKVR("clientoutpath", t_clientOutPath, "")));
			}
			else {
				printf("-opc���������ͻ��������·����/\n");
				return 0;
			}
		}
		else if (firstPar == "-opsc") {
			//-opsc������-scģʽ�½���������·����/
			string t_scOutPath = t_ParsManager.GetNextPar();
			if (t_scOutPath.length() > 0) {
				t_scOutPath = ProtocolUtils::ComputerPath(strFullPath, t_scOutPath);
				t_parMaps.insert(make_pair("scoutpath", PtKVR("scoutpath", t_scOutPath, "")));
			}
			else {
				printf("-opsc������-scģʽ�½���������·����/\n");
				return 0;
			}
		}
		else if (firstPar == "-tsm") {
			//-tsm  ��������һ���м��,���·��
			string t_tsMiddlePath = t_ParsManager.GetNextPar();
			if (t_tsMiddlePath.length() > 0) {
				t_tsMiddlePath = ProtocolUtils::ComputerPath(strFullPath, t_tsMiddlePath);
				t_parMaps.insert(make_pair("tsmiddlepath", PtKVR("tsmiddlepath", t_tsMiddlePath, "")));
			}
			else {
				printf("-tsm  ��������һ���м��,���·��\n");
				return 0;
			}
		}
		else if (firstPar == "-upart") {
			//-upart ʹ�����ķ���
			string t_partName = t_ParsManager.GetNextPar();
			if (t_partName.length() > 0) {
				t_parMaps.insert(make_pair("partname", PtKVR("partname", t_partName, "")));
			}
			else {
				printf("-upart ���ʹ�����ķ���\n");
				return 0;
			}
		}
		else if (firstPar == "-cjson") {
			//-cjson     ת����json,���ת�����ļ�
			string t_convertFile = t_ParsManager.GetNextPar();
			if (t_convertFile.length() > 0) {
				t_convertFile = ProtocolUtils::ComputerPath(strFullPath, t_convertFile);
				t_parMaps.insert(make_pair("converttojson", PtKVR("converttojson", t_convertFile, "")));
			}
			else {
				printf("-cjson ת����json,���ת�����ļ�\n");
				return 0;
			}

		}
		else if (firstPar == "-jp") {
			//-jp��������java����
			string t_javaPackName = t_ParsManager.GetNextPar();
			if (t_javaPackName.length() > 0) {
				t_parMaps.insert(make_pair("javapackname", PtKVR("javapackname", t_javaPackName, "")));
			}
			else {
				printf("-jp��������java����\n");
				return 0;
			}
		}
		else if (firstPar == "-copy") {
			g_bCreateCopy = true;
		}
		else if (firstPar == "-copy2") {
			g_bCreateCopy = true;
			g_bCreateCopy2 = true;
		}
		else {
			//ʶ���Ƿ��ǵ�����
			auto iter = t_singleCommandSet.find(firstPar);
			if (iter != t_singleCommandSet.end()) {
				t_parMaps.insert(make_pair(firstPar, PtKVR(firstPar, "1", "")));
			}
			else {
				printf("%s һ������ʶ�������\n", firstPar.c_str());
				return 0;
			}
		}


		firstPar = t_ParsManager.GetNextPar();
	}


	string strProtocolName = t_util.GetString_KVR(t_parMaps, "protocolname");

	if (t_util.GetInt_KVR(t_parMaps, "-cjson") != 0) {
		ProtocolUtils::ConvertToMongoJson(t_util.GetString_KVR(t_parMaps, "converttojson"));
		return 0;
	}

	if (t_util.GetInt_KVR(t_parMaps, "-comment") != 0) {
		if (strProtocolName.length() == 0) {
			printf("δָ��Э����\n");
			return 0;
		}
		CCreateComment tt_CreateComment(strProtocolName);
		tt_CreateComment.Parser();
		return 0;
	}

	if (t_util.GetInt_KVR(t_parMaps, "-eventfunc") != 0) {
		if (strProtocolName.length() == 0) {
			printf("δָ��Э����\n");
			return 0;
		}

		CCreateEventFunc tt_CreateEventFunc(strProtocolName);
		tt_CreateEventFunc.Parser();
		return 0;
	}



	eProtocolWriterType   t_writerType = GetProtocolWriterType(t_parMaps);
	if (t_writerType == eProtocolWriterType::eNone) {

		printf("��֧�ֵ�����Э������\n");
		return 0;
	}

	ProtocolParse_Writer* pProtocolWriter = nullptr;


	if (strProtocolName.length() == 0) {
		printf("δָ��Э����\n");
		return 0;
	}

	if (t_writerType == eProtocolWriterType::CPlus) {
		eCPlusStandardType t_cplusStandardType = eCPlusStandardType::CPlus11;
		if(t_util.GetBool_KVR(t_parMaps, "-c20"))
			t_cplusStandardType = eCPlusStandardType::CPlus20;
		else if (t_util.GetBool_KVR(t_parMaps, "-c17"))
			t_cplusStandardType = eCPlusStandardType::CPlus17;
		else if (t_util.GetBool_KVR(t_parMaps, "-c14"))
			t_cplusStandardType = eCPlusStandardType::CPlus14;
		else if (t_util.GetBool_KVR(t_parMaps, "-c11"))
			t_cplusStandardType = eCPlusStandardType::CPlus11;
		else if (t_util.GetBool_KVR(t_parMaps, "-c98"))
			t_cplusStandardType = eCPlusStandardType::CPlus98;

		bool t_bUseStdByte = false;
		if (t_cplusStandardType >= eCPlusStandardType::CPlus17) {
			t_bUseStdByte = true;
			if (t_util.GetBool_KVR(t_parMaps, "-windowsbyte")) {
				t_bUseStdByte = false;
			}
		}



		pProtocolWriter = new ProtocolParse_Writer_CPlus(strProtocolName, t_cplusStandardType, t_bUseStdByte);
	}
	else if (t_writerType == eProtocolWriterType::IPhone) {
		pProtocolWriter = new ProtocolParse_Writer_IPhone(strProtocolName);
	}
	else if (t_writerType == eProtocolWriterType::Flash) {
		pProtocolWriter = new ProtocolParse_Writer_Flash(strProtocolName);
	}
	else if (t_writerType == eProtocolWriterType::Java) {
		pProtocolWriter = new ProtocolParse_Writer_Java(strProtocolName);
	}
	else if (t_writerType == eProtocolWriterType::CSharp) {
		pProtocolWriter = new ProtocolParse_Writer_CSharp(strProtocolName);
	}
	else if (t_writerType == eProtocolWriterType::Lua) {
		pProtocolWriter = new ProtocolParse_Writer_Lua(strProtocolName);
	}
	else if (t_writerType == eProtocolWriterType::TypeScript) {
		pProtocolWriter = new ProtocolParse_Writer_TypeScript(strProtocolName);
	}
	else if (t_writerType == eProtocolWriterType::TSMiddle) {
		pProtocolWriter = new ProtocolParse_Writer_TSMiddle(strProtocolName);
	}
	else if (t_writerType == eProtocolWriterType::Python) {
		pProtocolWriter = new ProtocolParse_Writer_Python(strProtocolName);
	}
	else if (t_writerType == eProtocolWriterType::JavaScript) {
		pProtocolWriter = new ProtocolParse_Writer_JavaScript(strProtocolName);
	}

	//��һ��ͨ������
	if (pProtocolWriter) {
		if (t_util.GetBool_KVR(t_parMaps, "-ac") || t_util.GetBool_KVR(t_parMaps, "-asc"))
			pProtocolWriter->m_bAndroidPeer = true;
		pProtocolWriter->m_bExceptMoba = t_util.GetBool_KVR(t_parMaps, "-excpetmoba");
		pProtocolWriter->m_bPureVirtual = t_util.GetBool_KVR(t_parMaps, "-purevirtual");
		pProtocolWriter->m_bTScriptUseNameSpaceVersion = !t_util.GetBool_KVR(t_parMaps, "-module");
		pProtocolWriter->m_bOutJson = t_util.GetBool_KVR(t_parMaps,"-outjson");
		if (t_util.GetBool_KVR(t_parMaps, "-scsplits"))
			pProtocolWriter->m_bSCSpliteS = true;
		else if (t_util.GetBool_KVR(t_parMaps, "-scsplitc"))
			pProtocolWriter->m_bSCSpliteC = true;
		pProtocolWriter->m_bWriteSortFuncId = t_util.GetBool_KVR(t_parMaps, "-funcidsort");
		pProtocolWriter->m_UsePartManager = t_util.GetString_KVR(t_parMaps,"partname");
		pProtocolWriter->m_bFastModel = t_util.GetBool_KVR(t_parMaps, "-fastmodel");
		pProtocolWriter->m_bStdafx = t_util.GetBool_KVR(t_parMaps,"-stdafx");
	}

	string t_outPaths[] = { "severoutpath" ,"clientoutpath" ,"scoutpath" };
	string strOutPathServer = t_util.GetString_KVR(t_parMaps, t_outPaths[0]);
	string strOutPathClient = t_util.GetString_KVR(t_parMaps, t_outPaths[1]);
	string strOutPathSC = t_util.GetString_KVR(t_parMaps, t_outPaths[2]);

	if (pProtocolWriter && pProtocolWriter->Parser()) {
		pProtocolWriter->m_bAndroidPeer = false;

		if (t_writerType == eProtocolWriterType::CPlus) {
			string t_createFiles[] = { "-s","-c","-sc" };
			
			CallProtocolWriter_Server(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[0]), strOutPathServer, strFullPath);
			CallProtocolWriter_Client(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[1]), strOutPathClient, strFullPath);
			CallProtocolWriter_SC(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[2]), strOutPathSC, strFullPath);

			if (t_util.GetBool_KVR(t_parMaps, "-ac")) {
				if (strOutPathClient.length() == 0) {
					strOutPathClient = ProtocolUtils::PickPath(strFullPath, 1);
					strOutPathClient.append("\\");
				}
				pProtocolWriter->m_bAndroidPeer = true;
				pProtocolWriter->FreeWriteLog();
				pProtocolWriter->CreateFile_Path(strOutPathClient, 0, false);
				pProtocolWriter->m_bAndroidPeer = false;
			}

			if (t_util.GetBool_KVR(t_parMaps, "-asc")) {
				if (strOutPathSC.length() == 0) {
					strOutPathSC = ProtocolUtils::PickPath(strFullPath, 1);
					strOutPathSC.append("\\");
				}
				pProtocolWriter->m_bAndroidPeer = true;
				pProtocolWriter->FreeWriteLog();
				pProtocolWriter->CreateFile_Path(strOutPathSC, 2, false);
				pProtocolWriter->m_bAndroidPeer = false;

			}
		}
		else if (t_writerType == eProtocolWriterType::IPhone) {
			string t_createFiles[] = { "-is","-ic","-isc" };

			CallProtocolWriter_Server(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[0]), strOutPathServer, strFullPath);
			CallProtocolWriter_Client(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[1]), strOutPathClient, strFullPath);
			CallProtocolWriter_SC(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[2]), strOutPathSC, strFullPath);
		}
		else if (t_writerType == eProtocolWriterType::Flash) {
			string t_createFiles[] = { "-ws","-wc","-wsc" };

			CallProtocolWriter_Server(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[0]), strOutPathServer, strFullPath);
			CallProtocolWriter_Client(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[1]), strOutPathClient, strFullPath);
			CallProtocolWriter_SC(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[2]), strOutPathSC, strFullPath);
		}
		else if (t_writerType == eProtocolWriterType::Java) {
			string t_createFiles[] = { "-js","-jc","-jsc" };
			((ProtocolParse_Writer_Java*)pProtocolWriter)->SetJavaPackName(t_util.GetString_KVR(t_parMaps, "javapackname"));
			CallProtocolWriter_Server(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[0]), strOutPathServer, strFullPath);
			CallProtocolWriter_Client(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[1]), strOutPathClient, strFullPath);
			CallProtocolWriter_SC(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[2]), strOutPathSC, strFullPath);
		}
		else if (t_writerType == eProtocolWriterType::CSharp) {
			string t_createFiles[] = { "-cps","-cpc","-cpsc" };

			((ProtocolParse_Writer_CSharp*)pProtocolWriter)->m_bWithLog = t_util.GetBool_KVR(t_parMaps, "-withlog");
			CallProtocolWriter_Server(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[0]), strOutPathServer, strFullPath);
			CallProtocolWriter_Client(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[1]), strOutPathClient, strFullPath);
			CallProtocolWriter_SC(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[2]), strOutPathSC, strFullPath);
		}
		else if (t_writerType == eProtocolWriterType::Lua) {
			string t_createFiles[] = { "-luas","-luac","-luasc" };

			CallProtocolWriter_Server(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[0]), strOutPathServer, strFullPath);
			CallProtocolWriter_Client(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[1]), strOutPathClient, strFullPath);
			CallProtocolWriter_SC(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[2]), strOutPathSC, strFullPath);
		}
		else if (t_writerType == eProtocolWriterType::TypeScript) {
			string t_createFiles[] = { "-tss","-tsc","-tssc" };

			CallProtocolWriter_Server(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[0]), strOutPathServer, strFullPath);
			CallProtocolWriter_Client(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[1]), strOutPathClient, strFullPath);
			CallProtocolWriter_SC(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[2]), strOutPathSC, strFullPath);
		}
		else if (t_writerType == eProtocolWriterType::TSMiddle) {
			ProtocolParse_Writer_TSMiddle* pWriter = (ProtocolParse_Writer_TSMiddle*)pProtocolWriter;

			string strOutPathM = t_util.GetString_KVR(t_parMaps, "tsmiddlepath");
			if (strOutPathM.length() == 0) {
				strOutPathM = ProtocolUtils::PickPath(strFullPath, 1);
				strOutPathM.append("\\");
			}

			string t_createFiles[] = { "-tsms","-tsmc","-tsmsc" };

			if (t_util.GetBool_KVR(t_parMaps, t_createFiles[0])) {
				pWriter->FreeWriteLog();
				pWriter->CreateFile_Path_TScript_Middle(strOutPathM, 1, false);
			}

			if (t_util.GetBool_KVR(t_parMaps, t_createFiles[1])) {
				pWriter->FreeWriteLog();
				pWriter->CreateFile_Path_TScript_Middle(strOutPathM, 0, false);
			}

			if (t_util.GetBool_KVR(t_parMaps, t_createFiles[2])) {
				pWriter->FreeWriteLog();
				pWriter->CreateFile_Path_TScript_Middle(strOutPathM, 2, false);
			}

			if ((!t_util.GetBool_KVR(t_parMaps, t_createFiles[0])) && (!t_util.GetBool_KVR(t_parMaps, t_createFiles[1])) && (!t_util.GetBool_KVR(t_parMaps, t_createFiles[2]))) {
				pWriter->FreeWriteLog();
				pWriter->CreateFile_Path_TScript_Middle(strOutPathM, -1, false);
			}
		}
		else if (t_writerType == eProtocolWriterType::Python) {
			string t_createFiles[] = { "-pys","-pyc","-pysc" };

			CallProtocolWriter_Server(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[0]), strOutPathServer, strFullPath);
			CallProtocolWriter_Client(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[1]), strOutPathClient, strFullPath);
			CallProtocolWriter_SC(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[2]), strOutPathSC, strFullPath);
		}
		else if (t_writerType == eProtocolWriterType::JavaScript) {
			string t_createFiles[] = { "-jsts","-jstc","-jstsc" };

			CallProtocolWriter_Server(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[0]), strOutPathServer, strFullPath);
			CallProtocolWriter_Client(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[1]), strOutPathClient, strFullPath);
			CallProtocolWriter_SC(pProtocolWriter, t_util.GetBool_KVR(t_parMaps, t_createFiles[2]), strOutPathSC, strFullPath);
		}

		char ch[512];
		sprintf_s(ch,512, "parser %s good\n", strProtocolName.c_str());
		string strerror = ch;
		if (strOutPathServer.size() > 0) {
			sprintf_s(ch,512, "strOutPathServer = %s\n", strOutPathServer.c_str());
			strerror.append(ch);
		}
		if (strOutPathClient.size() > 0) {
			sprintf_s(ch,512, "strOutPathClient = %s\n", strOutPathClient.c_str());
			strerror.append(ch);
		}
		//if(strFullPath.size()>0){
		//	sprintf(ch,"strFullPath = %s\n",strFullPath.c_str());
		//	strerror.append(ch);
		//}

		FILE* fp;
		fp = fopen("geninfo.txt", "w");
		if (!fp)
			return 0;

		fseek(fp, 0, SEEK_SET);

		fwrite(strerror.c_str(), strerror.length(), 1, fp);
		fwrite("\n", 1, 1, fp);
		for (auto& r : g_AllTempFilesList) {
			fwrite(r.c_str(), r.length(), 1, fp);
			fwrite("\n", 1, 1, fp);
		}
		fclose(fp);

		printf("%s", strerror.c_str());


		//ɾ����ʱ�ļ�
		for (auto& r : g_AllTempFilesList) {
			remove(r.c_str());
		}
	}
	else {
		char ch[512];
		sprintf_s(ch,512, "parser %s failed\n", strProtocolName.c_str());
		string strerror = ch;
		if (strOutPathServer.size() > 0) {
			sprintf_s(ch,512, "strOutPathServer = %s\n", strOutPathServer.c_str());
			strerror.append(ch);
		}
		if (strOutPathClient.size() > 0) {
			sprintf_s(ch,512, "strOutPathClient = %s\n", strOutPathClient.c_str());
			strerror.append(ch);
		}
		//if(strFullPath.size()>0){
		//	sprintf(ch,"strFullPath = %s\n",strFullPath.c_str());
		//	strerror.append(ch);
		//}

		for (int i = 0; i < argc; i++) {
			strerror.append(argv[i]);
			strerror.append("\n");
		}

		strerror.append(t_util.GetString_KVR(t_parMaps, "fullpath").c_str());


		FILE* fp;

		//string tt_strTempFileName = "geninfo";
		//tt_strTempFileName.append(g_ProgressId.c_str());
		//fp = fopen(tt_strTempFileName.c_str(), "w");
		fp = fopen("geninfo.txt", "w");
		if (!fp)
			return 0;

		fseek(fp, 0, SEEK_SET);

		fwrite(strerror.c_str(), strerror.length(), 1, fp);
		fwrite("\n", 1, 1, fp);

		for (auto& r : g_AllTempFilesList) {
			fwrite(r.c_str(), r.length(), 1, fp);
			fwrite("\n", 1, 1, fp);
		}
		fclose(fp);

		printf("%s", strerror.c_str());

		//ɾ����ʱ�ļ�
		for (auto& r : g_AllTempFilesList) {
			remove(r.c_str());
		}
	}
	return 0;
}

//-i Client2Room2.xml -opsc flash/ -wsc
//-i PhoneClient2Login.xml -opc flash/ -wc
//-i RoomShortOp.xml -opsc flash/ -wsc
//-i GateWayShortOp.xml -opsc flash/ -wsc
//-i GateWayPhone.xml -opc flash/ -wc

/*

*/

