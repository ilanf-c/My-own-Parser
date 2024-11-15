#ifndef _GRAMMER_H_
#define _GRAMMER_H_

#include <iostream>
using namespace std;
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <set>
#include <unordered_map>

class GR
{
private:
    unordered_map<string, vector<vector<string>>> grammer; // 文法规则
    string Start;                                          // 开始符号
public:
    GR()
    {
        Start = "";
        grammer.clear();
    }
    void deleteGrammer()
    {
        Start = "";
        grammer.clear();
    }
    /* 读取文件 */
    void init(string filepath);
    /* 返回所有的Firs集合 */
    unordered_map<string, set<string>> getFirst();
    /* 返回所有的Follow集合 */
    unordered_map<string, set<string>> getFollow();
    /* 扩展文法 */
    void expand();
    string getStart() { return Start; }
    vector<vector<string>> getRight(string left)
    {
        return grammer[left];
    }
    int isEnd(string s)
    {
        return grammer.find(s) == grammer.end();
    }
};

#endif