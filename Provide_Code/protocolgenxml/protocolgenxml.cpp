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

//-i后带要解析的协议文件
//-ops后带解析后服务器端输出的路径带/
//-opc后带解析后客户端输出的路径带/
//-opsc代表在-sc模式下解析后的输出路径带/

//-s代表要解析C++服务器
//-c代表要解析C++客户端
//-sc代表C++客户端和服务器生成在一个文件上
//-ac代表要解析成C++android客户端
//-asc代表要解析成C++android服务端和客户端


//-is代表解析Iphone平台的服务器端
//-ic代表解析Iphone平台的客户端
//-isc代表解析Iphone平台的服务器端和客户端

//-ws代表解析Flash平台的服务器端
//-wc代表解析Flash平台的客户器端
//-wsc代表解析Flash平台的服务器和客户端

//-js代表解析Java平台的服务端
//-jc代表解析Java平台的客户端
//-jsc代表解析Java平台的服务端和客户端

//-jp代表后面接java包名

//-cps代表解析C#平台的服务端
//-cpc代表解析C#平台的客户端
//-cpsc代表解析C#平台的服务端和客户端

//-luas 代表解析Lua平台的服务端
//-luac 代表解析Lua平台的客户端
//-luasc 代表解析Lua平台的服务端和客户端

//-tss 代表解析ts平台的服务端
//-tsc 代表解析ts平台的客户端
//-tssc 代表解析ts平台的服务端和客户端  

//-tsm  代表生成一个中间件,后接路径

//-tsms 代表解析为服务端版的中间件
//-tsmc 代表解析为客户端版的中间件
//-tsmsc 代表解析为服务端与客户端通用的中间件

//-pys 代表解析python平台的服务端
//-pyc 代表解析python平台的客户端
//-pysc 代表解析python平台的服务端和客户端  

//暂不支持javascript
//-jsts代表解析JavaScript平台的服务端
//-jstc代表解析JavaScript平台的客户端
//-jstsc代表解析JavaScript平台的服务端和客户端


//-stdafx      c++生成文件包含预编译头
//-outjson     c++支持输出json内容

//-scsplits   sc模式中分出s
//-scsplitc   sc模式中分出c

//-purevirtual 是否纯虚，用于SC模式
//-excpetmoba 是否排除moba
//-fastmodel  支持快速模型的下的打包函数是空的
//-funcidsort 输出一个注释版的排过序的函数id,用于比对
//-module      是否在ts中使用模块，默认是命名空间

//-withlog    是否带log,在CSharp中使用，并使用了UnityEngine

//-relayout  重新布局,协议文件重新格式化

//-upart      使用区的方案
//-cjson     转换成json,后接转换的文件
//-comment   生成注释,使用-i输入的文件,主要是查看注释的中间转换文件
//-eventfunc 生成事件函数,使用-i输入的文件,这个是CSharp专用，与C++函数的流式调用

//-copy      忘记什么功能了
//-copy2      

//默认是c11
//-c98       表明使用c98
//-c11       表明使用c++11
//-c14       
//-c17
//-c20

//-windowsbyte  强制使用windows的byte



/////////////
//
//////////////
//<request name = "XiaoZhuShouRequest" FuncNew="1">  
//FuncNew表示所有地函数id计算方法都按New="1"计算

//<method name = "OnUpdateServerList" Discard="1">
//Discard表明此函数不再生成序列与反序列函数，只计算其函数id

//<method name="ReqPlayerLobbyNoticeList" FuncId="1001">
//FuncId表明指明函数id,最大65535,改成可以指到大的id

//<method name="ReqVIPValue" New="1">
//New表明按新方法计算函数id

//生成文件后面表示的是函数id出现主冲突的列表

//MD5=dfe0896b5247d718510a448c6522c139BDM
//为了判定是否发生改变，需要生新生成文件

//<method name = "GWPlayerChangeNick" version="parversion2">
//version用于协议兼容

//C++版
//结构体添加构造 <struct name = "PtBeyondGoodsInfo2" version="parversion3" construct="1">
//结构体成员添加默认值<member name = "Version"  type="short" version="parversion3" default="0"/>

//添加有值才传输,配置对应的隐藏标识中的位数[0,30],为兼容最多31位,一个结构体中两个变量不能占用同一位
//不能对此类型从无到有做协议兼容，必须本身就有此类型才能兼容
//<member name="BigInt1" default="0" type="__int64" transneedvalue="1"/>



//增加SCFlag,也就是将S/C 的协议，通过SC的方式生成，只取有SCFlag的
//<method name = "NotifyDuoBaoResult" New="1" SCFlag="1">


//增加MobaFlag,通过生成配置来排除此标识
//<method name="ReqPlayerMobaStatus" New="1" MobaFlag="1">

//增加Layer,来生成多级类
//<method name = "PlayerLoginRoom" vertion="parversion6" Layer="1">

//hcpp表明的是生成的是.h与.cpp版本
//fujianN表明自己下接附件,只能按顺序自增
//yuanjian表明自己是谁的附件
//<namespacename value1 = "ServerCommon" value2 = "ServerCommon" value3 = "hcpp" fujian1 = "ServerCommon_Moba" / >
//<namespacename value1 = "ServerCommon" value2 = "ServerCommon" value3 = "hcpp" yuanjian = "ServerCommon" / >
//附件与原件无效

//startversion_ts 起始版本 ts生成使用
//<struct name = "PtPlayerBaseInfo" construct="1" version = "parversion12" startversion_ts="parversion12">

//当整个类使用新算法计算函数id时，这个函数保持为旧的
//<method name = "PlayerLoginRoom" KeepOld="1">

//支持part写法
//<request name = "GateWay2ClientRequest" part="1" FuncNew="1">
//<request name = "GateWay2ClientRequest" part="2" FuncNew="1">

//enum除了支持New="1"的新版之外，增加了KeepOld="1"


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
//	eCPS_SCSPLITES, //使用SC模式，分离出S
//	eCPS_SCSPLITEC, //使用SC模式，分离出C
//	eCPS_FuncIdSort,//输出一个注释版的排过序的函数id
//	eCPS_UsePart,   //使用part方案
//	eCPS_TSJsonMiddle, //生成ts的中间件
//	eCPS_TSMiddleServer,
//	eCPS_TSMiddleClient,
//	eCPS_TSMiddleSC,
//	eCPS_CJson,   //转换json
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
 * @brief 取到协议生成类型
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

	//用解析后的参数数，一部分按公共方法的提取
	argc = t_ParsManager._argc;

	if (argc <= 1) {
		printf("请通过-help查看相关命令\n");
		return 0;
	}
	//printf(argv[0]);
	//printf("\n");

	//先取到全路径
	string strFullPath = t_util.GetString_KVR(t_ParsManager._parMaps,"fullpath");
	//取到首参
	string firstPar = t_ParsManager.BeginGetPar(1);

	if (firstPar == "-help") {

		if (argc == 2) {
			printf("-help [option]\n");
			printf("option支持 enum,struct,vector,protocol,part,method,membertype,generator,example\n");
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

				printf("New表使用新的算法计算枚举值\n");
				printf("KeepOld表仍使用旧的算法计算当前枚举值\n");
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
				printf("construct表示生成构造\n");
				printf("default表示默认值\n");
				printf("构造时优先使用指定的默认值，否则使用对应类型的默认值\n");
				printf("inherited表示继承之哪个对象，中途不能修改继承关系\n");
				printf("transneedvalue:添加有值才传输,配置对应的隐藏标识中的位数[0,30],为兼容最多31位,一个结构体中两个变量不能占用同一位\n");
				printf("transneedvalue:不能对此类型从无到有做协议兼容，必须本身就有此类型才能兼容\n");
				printf("startversion_ts:表示对应ts的起始版本\n");

				return 0;

			}
			else if (t_optionPar == "vector") {
				string t_tip = 
R"(<sequence name = "PtDoubleColorCfgList" element = "PtDoubleColorCfg"/>)";
				printf("%s\n", t_tip.c_str());
				printf("对应于c++的std::vector<PtDoubleColorCfg>\n");
				return 0;
			}
			else if (t_optionPar == "protocol") {
				string t_tip = 
R"(<namespacename value1="ServerCommon" value2="ServerCommon" value3="hcpp" shortname="bbb"/>)";
				printf("%s\n", t_tip.c_str());

				printf("value1表服务端命名空间,value2表客户端命名空间\n");
				printf("value3如果指定为hcpp表示同时生成.h与.cpp\n");
				printf("shortname表示短名\n");

				t_tip = 
R"(<protocolRef   value1 = "ServerCommon.xml" value2 = "ServerCommonProtocol.h" value3 = "ServerCommonProtocol.h" value4 = "ServerCommon" path="..\NewProtocolXml"/ >)";
				printf("%s\n", t_tip.c_str());

				printf("value1表引用的协议文件,value2,value3表引用的头文件名\n");
				printf("value4表使用的命名空间名\n");
				printf("NotCreateFile表不生成实际的引用文件\n");
				printf("path表明引用文件与当前文件的路径关系\n");

				t_tip = 
					R"(<protocolNo name = "GateWay2Client_No" value = "1000" / >)";
				printf("%s\n", t_tip.c_str());

				printf("protocolNo用来指定协议号别名与值\n");

				t_tip = R"(<protocol name = "GateWay2Client"></protocol>)";
					
				printf("%s\n", t_tip.c_str());
				printf("protocol 表示整个协议段，并指定名字\n");

				t_tip = 
R"(  <VManger name="Version">
  <member name = "defversion" value = "1"/>
  <member name = "parversion1" value = "1"/>
  <member name = "parversion2" value = "2"/>
  <member name = "parversion3" value = "3"/>
  <member name = "parversion4" value = "4"/>
  </VManger>)";
				printf("%s\n", t_tip.c_str());
				printf("对所有的版本值进行定义\n");

				t_tip=R"(
<request name = "GateWay2ClientRequest"  FuncNew="1"> 
</request>
<callback name = "GateWay2ClientCallback"  FuncNew="1">
</callback>)";
				printf("%s\n", t_tip.c_str());
				printf("对应于C/S既有request,也有callback\n");
				printf("对应于SC只有request\n");
				printf("FuncNew表明区域内的method都使用新方法计算ID\n");

				
				return 0;
			}
			else if (t_optionPar == "part") {
				string t_tip =
R"(<PartManager name="LoginServer">  
	<member name = "机器绑定区"/>
	<member name = "微信绑定区"/> 	
 </PartManager>)";
				printf("%s\n", t_tip.c_str());
				printf("定义每个part管理的名字，以及包含哪些part\n");

				t_tip=R"(
<request name = "GateWay2ClientRequest" part="登录区" FuncNew="1"> 
</request>
<callback name = "GateWay2ClientCallback" part="登录区" FuncNew="1">
</callback>
)";
				printf("%s\n", t_tip.c_str());
				printf("定义每个part区的协议,以及part区名\n");


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

				printf("方法定义，New表使用新的方法计算方法ID\n");
				printf("SCFlag用于C/S协议中，生成收发两种方法\n");
				printf("如果指定Discard表示此函数只保留ID计算，不生成函数\n");
				printf("FuncId表示强行指定函数的ID,而无需通过计算得出\n");
				printf("KeepOld表示仍按旧方法计算函数ID，用于整体指定用新方法FuncNew时，兼容旧版\n");
				printf("MobaFlag这个标识将不再使用\n");
				printf("Layer表层，指定值会根据不同层，生成派生类关系,最好也不要使用了\n");
				printf("SCSplite=\"S\"用于SC版的协议，强行分出服务端\n");
				printf("SCSplite=\"C\"用于SC版的协议，强行分出客户端\n");

				return 0;
			}
			else if (t_optionPar == "membertype") {
			string t_tip = 
R"({"void","int","unsigned int","short","unsigned short","char","byte","BYTE","bool","BOOL", 
"DWORD","float","long","unsigned long","string","__int64",
"wjs_char32","wjs_char64","wjs_char128","wjs_char256","wjs_char512","wjs_char1024",
"long long","unsigned char","wjs_utf8","u16string","wstring" };)";
			printf("%s\n", t_tip.c_str());

			printf("以上为支持的基本类型，最好不要用byte,因为vs2019会有冲突，改用unsigned char\n");
			printf("漏掉了double类型，以后会补\n");
			return 0;

			}
			else if (t_optionPar == "generator") {

			string t_tip = 
R"(-i后带要解析的协议文件
-ops后带解析后服务器端输出的路径带/
-opc后带解析后客户端输出的路径带/
-opsc代表在-sc模式下解析后的输出路径带/

-s代表要解析C++服务器
-c代表要解析C++客户端
-sc代表C++客户端和服务器生成在一个文件上
-ac代表要解析成C++android客户端
-asc代表要解析成C++android服务端和客户端


-is代表解析Iphone平台的服务器端
-ic代表解析Iphone平台的客户端
-isc代表解析Iphone平台的服务器端和客户端

-ws代表解析Flash平台的服务器端
-wc代表解析Flash平台的客户器端
-wsc代表解析Flash平台的服务器和客户端

-js代表解析Java平台的服务端
-jc代表解析Java平台的客户端
-jsc代表解析Java平台的服务端和客户端

-jp代表后面接java包名

-cps代表解析C#平台的服务端
-cpc代表解析C#平台的客户端
-cpsc代表解析C#平台的服务端和客户端

-luas 代表解析Lua平台的服务端
-luac 代表解析Lua平台的客户端
-luasc 代表解析Lua平台的服务端和客户端

-tss 代表解析ts平台的服务端
-tsc 代表解析ts平台的客户端
-tssc 代表解析ts平台的服务端和客户端  

-tsm  代表生成一个中间件,后接路径

-tsms 代表解析为服务端版的中间件
-tsmc 代表解析为客户端版的中间件
-tsmsc 代表解析为服务端与客户端通用的中间件

-pys 代表解析python平台的服务端
-pyc 代表解析python平台的客户端
-pysc 代表解析python平台的服务端和客户端  

暂不支持javascript
-jsts代表解析JavaScript平台的服务端
-jstc代表解析JavaScript平台的客户端
-jstsc代表解析JavaScript平台的服务端和客户端


-stdafx      c++生成文件包含预编译头
-outjson     c++支持输出json内容

-scsplits   sc模式中分出s
-scsplitc   sc模式中分出c

-purevirtual 是否纯虚，用于SC模式
-excpetmoba 是否排除moba
-fastmodel  支持快速模型的下的打包函数是空的
-funcidsort 输出一个注释版的排过序的函数id,用于比对
-module      是否在ts中使用模块，默认是命名空间

-withlog    是否带log,在CSharp中使用，并使用了UnityEngine

-relayout  重新布局,协议文件重新格式化

-upart      使用区的方案
-cjson     转换成json,后接转换的文件
-comment   生成注释,使用-i输入的文件,主要是查看注释的中间转换文件
-eventfunc 生成事件函数,使用-i输入的文件,这个是CSharp专用，与C++函数的流式调用

-copy      忘记什么功能了
-copy2      

默认是c11
-c98       表明使用c98
-c11       表明使用c++11
-c14       
-c17
-c20

-windowsbyte  强制使用windows的byte
)";
			

			printf("%s\n", t_tip.c_str());
			return 0;
			}
			else if (t_optionPar == "example") {
				string t_example = t_ParsManager.GetNextPar();
				if (t_example == "") {
					string t_tip = R"(需要再接一个如下参:python,cplus,csharp,java,lua,typescript,typescript-module,monojson,flash,eventfunc,comment)";
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
					printf("转换成可以存储到monogodb的json行列表\n");
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
					printf("生成的文件为GateWay2ClientProtocol_server.h.copy\n");
					return 0;
				}
				else if (t_example == "comment") {
					string t_tip =
						R"(protocolgenxml.exe -i  ../NewProtocolXml/GateWay2Client.xml -comment)";
					printf("%s\n", t_tip.c_str());
					printf("主要是协议生成的中间文件，将注释转到xml格式内部，GateWay2Client.xml.copy25284，25284为进程ID\n");
					return 0;
				}
				
			}
		}


		return 0;
	}

	//单命令集
	std::set<string> t_singleCommandSet;
	for (int i = 0; i < sizeof(g_singleCommands) / sizeof(string); i++) {
		t_singleCommandSet.insert(g_singleCommands[i]);
	}

	while (firstPar != "") {
		if (firstPar == "-i") {
			//后面一个参是协议文件名
			string t_protocolName = t_ParsManager.GetNextPar();
			if (t_protocolName.length() > 0) {
				t_protocolName = ProtocolUtils::ComputerPath(strFullPath, t_protocolName);
				
				t_parMaps.insert(make_pair("protocolname", PtKVR("protocolname", t_protocolName, "")));
			}
			else {
				printf("-i后带要解析的协议文件\n");
				return 0;
			}
		}
		else if (firstPar == "-ops") {
			//后带解析后服务器端输出的路径带/
			string t_serverOutPath = t_ParsManager.GetNextPar();
			if (t_serverOutPath.length() > 0) {
				t_serverOutPath = ProtocolUtils::ComputerPath(strFullPath, t_serverOutPath);
				t_parMaps.insert(make_pair("severoutpath", PtKVR("severoutpath", t_serverOutPath, "")));
			}
			else {
				printf("-ops 后带解析后服务器端输出的路径带/\n");
				return 0;
			}
		}
		else if (firstPar == "-opc") {
			//-opc后带解析后客户端输出的路径带/
			string t_clientOutPath = t_ParsManager.GetNextPar();
			if (t_clientOutPath.length() > 0) {
				t_clientOutPath = ProtocolUtils::ComputerPath(strFullPath, t_clientOutPath);
				t_parMaps.insert(make_pair("clientoutpath", PtKVR("clientoutpath", t_clientOutPath, "")));
			}
			else {
				printf("-opc后带解析后客户端输出的路径带/\n");
				return 0;
			}
		}
		else if (firstPar == "-opsc") {
			//-opsc代表在-sc模式下解析后的输出路径带/
			string t_scOutPath = t_ParsManager.GetNextPar();
			if (t_scOutPath.length() > 0) {
				t_scOutPath = ProtocolUtils::ComputerPath(strFullPath, t_scOutPath);
				t_parMaps.insert(make_pair("scoutpath", PtKVR("scoutpath", t_scOutPath, "")));
			}
			else {
				printf("-opsc代表在-sc模式下解析后的输出路径带/\n");
				return 0;
			}
		}
		else if (firstPar == "-tsm") {
			//-tsm  代表生成一个中间件,后接路径
			string t_tsMiddlePath = t_ParsManager.GetNextPar();
			if (t_tsMiddlePath.length() > 0) {
				t_tsMiddlePath = ProtocolUtils::ComputerPath(strFullPath, t_tsMiddlePath);
				t_parMaps.insert(make_pair("tsmiddlepath", PtKVR("tsmiddlepath", t_tsMiddlePath, "")));
			}
			else {
				printf("-tsm  代表生成一个中间件,后接路径\n");
				return 0;
			}
		}
		else if (firstPar == "-upart") {
			//-upart 使用区的方案
			string t_partName = t_ParsManager.GetNextPar();
			if (t_partName.length() > 0) {
				t_parMaps.insert(make_pair("partname", PtKVR("partname", t_partName, "")));
			}
			else {
				printf("-upart 后接使用区的方案\n");
				return 0;
			}
		}
		else if (firstPar == "-cjson") {
			//-cjson     转换成json,后接转换的文件
			string t_convertFile = t_ParsManager.GetNextPar();
			if (t_convertFile.length() > 0) {
				t_convertFile = ProtocolUtils::ComputerPath(strFullPath, t_convertFile);
				t_parMaps.insert(make_pair("converttojson", PtKVR("converttojson", t_convertFile, "")));
			}
			else {
				printf("-cjson 转换成json,后接转换的文件\n");
				return 0;
			}

		}
		else if (firstPar == "-jp") {
			//-jp代表后面接java包名
			string t_javaPackName = t_ParsManager.GetNextPar();
			if (t_javaPackName.length() > 0) {
				t_parMaps.insert(make_pair("javapackname", PtKVR("javapackname", t_javaPackName, "")));
			}
			else {
				printf("-jp代表后面接java包名\n");
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
			//识别是否是单命令
			auto iter = t_singleCommandSet.find(firstPar);
			if (iter != t_singleCommandSet.end()) {
				t_parMaps.insert(make_pair(firstPar, PtKVR(firstPar, "1", "")));
			}
			else {
				printf("%s 一个不能识别的命令\n", firstPar.c_str());
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
			printf("未指定协议名\n");
			return 0;
		}
		CCreateComment tt_CreateComment(strProtocolName);
		tt_CreateComment.Parser();
		return 0;
	}

	if (t_util.GetInt_KVR(t_parMaps, "-eventfunc") != 0) {
		if (strProtocolName.length() == 0) {
			printf("未指定协议名\n");
			return 0;
		}

		CCreateEventFunc tt_CreateEventFunc(strProtocolName);
		tt_CreateEventFunc.Parser();
		return 0;
	}



	eProtocolWriterType   t_writerType = GetProtocolWriterType(t_parMaps);
	if (t_writerType == eProtocolWriterType::eNone) {

		printf("不支持的生成协议类型\n");
		return 0;
	}

	ProtocolParse_Writer* pProtocolWriter = nullptr;


	if (strProtocolName.length() == 0) {
		printf("未指定协议名\n");
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

	//设一下通用属性
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


		//删除临时文件
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

		//删除临时文件
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

