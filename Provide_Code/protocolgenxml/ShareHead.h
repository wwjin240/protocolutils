#pragma once
#include <string>
#include <vector>

//using namespace std;


namespace ServerCommon {
	struct PtKVR {
		std::string k;
		std::string v;
		std::string r;
		PtKVR()
		{

		}
		PtKVR(const std::string& k_,const std::string& v_,const std::string& r_)
		{
			k = k_;
			v = v_;
			r = r_;
		}
	};
	typedef std::vector<PtKVR> PtKVRList;

	struct PtKVSS {
		std::string k;
		std::string v;
		PtKVSS()
		{
		}
	};

	typedef std::vector<PtKVSS> PtKVSSList;

	struct PtKVSI {
		std::string k;
		int v;
		PtKVSI()
		{
			v = 0;
		}
	};
	typedef std::vector<PtKVSI> PtKVSIList;

	struct PtKVSI64 {
		std::string k;
		__int64 v;
		PtKVSI64()
		{
			v = 0;
		}
	};
	typedef std::vector<PtKVSI64> PtKVSI64List;

	struct PtKVII {
		int k;
		int v;
		PtKVII()
		{
			k = 0;
			v = 0;
		}
	};
	typedef std::vector<PtKVII> PtKVIIList;

	struct PtKeyValueData {
		std::string key;
		std::string value;
	};
	typedef std::vector<PtKeyValueData> PtKeyValueDataList;
}