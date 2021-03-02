#pragma once
#include "ShareHead.h"
#include <map>
#include <string>

using namespace ServerCommon;

/**
 * @brief 参数管理
*/
class ParsManager {
public:
    ParsManager();
    ~ParsManager();
    /**
     * @brief 初始化
     * @param argc 
     * @param argv 
    */
    void Init(int argc, char* argv[]);

    /**
     * @brief 初始化扩展参数配置
    */
    void InitExParCfg();

    /**
     * @brief 开始取参数
     * @param beginParIndex 
    */
    std::string BeginGetPar(int beginParIndex);

    /**
     * @brief 得到下一个参数
     * @return 
    */
    std::string GetNextPar();

    /**
     * @brief 设参数序号
     * @param curParIndex 
    */
    void SetParIndex(int curParIndex);

    /**
     * @brief 
    */
    void AddError(const std::string& errorMsg);

public:
    std::map<std::string, PtKVR> _parMaps;

    //扩展参，键是参数，值是后接的参数数
    //键用--xxx模式
    std::map<std::string, int>    _exParCfgMaps;

    int  _argc;
private:
    int   _curParIndex;

};
