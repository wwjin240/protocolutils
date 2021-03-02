#pragma once
#include "ShareHead.h"
#include <map>
#include <string>

using namespace ServerCommon;

/**
 * @brief ��������
*/
class ParsManager {
public:
    ParsManager();
    ~ParsManager();
    /**
     * @brief ��ʼ��
     * @param argc 
     * @param argv 
    */
    void Init(int argc, char* argv[]);

    /**
     * @brief ��ʼ����չ��������
    */
    void InitExParCfg();

    /**
     * @brief ��ʼȡ����
     * @param beginParIndex 
    */
    std::string BeginGetPar(int beginParIndex);

    /**
     * @brief �õ���һ������
     * @return 
    */
    std::string GetNextPar();

    /**
     * @brief ��������
     * @param curParIndex 
    */
    void SetParIndex(int curParIndex);

    /**
     * @brief 
    */
    void AddError(const std::string& errorMsg);

public:
    std::map<std::string, PtKVR> _parMaps;

    //��չ�Σ����ǲ�����ֵ�Ǻ�ӵĲ�����
    //����--xxxģʽ
    std::map<std::string, int>    _exParCfgMaps;

    int  _argc;
private:
    int   _curParIndex;

};
