#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
using namespace std;

multimap<string, vector<string>> rules; // 全部文法规则
map<string, set<string>> First;
map<string, set<string>> Follow;
void generFirst()
{
    int haschange = true;
    while (haschange)
    {
        haschange = false;
        for (auto rule : rules)
        {
            int past = First[rule.first].size();
            set<string> tmp;
            for (int pos = 0; pos < rule.second.size(); pos++)
            {
                auto range = rules.equal_range(rule.second[pos]);
                if (range.first == range.second) // 终结符
                    tmp.insert(rule.second[pos]);
                else
                    tmp.insert(First[rule.first].begin(), First[rule.first].end());
                auto hasepsilon = tmp.find("@");
                tmp.erase("@");
                First[rule.first].insert(tmp.begin(), tmp.end());
                if (hasepsilon == tmp.end())
                    break;
            }
            haschange |= First[rule.first].size() > past;
        }
    }
}
void generFollow()
{
    int haschange = true;
    while (haschange)
    {
        haschange = false;
        for (auto rule : rules)
        {
            for (int pos = 0; pos < rule.second.size(); pos++)
            {
                set<string> tmp;
                auto range = rules.equal_range(rule.second[pos]);
                if (range.first == range.second)
                    continue; // 终结符
                if (pos == rule.second.size() - 1)
                    tmp.insert("@");
                else
                    tmp.insert(First[rule.second[pos]].begin(), First[rule.second[pos]].end());
                auto hasepsilon = tmp.find("@");
                tmp.erase("@");
                Follow.insert({rule.first, tmp});
                if (hasepsilon != tmp.end())
                    Follow[rule.first].insert(Follow[rule.second[pos]].begin(), Follow[rule.second[pos]].end());
            }
        }
    }
}
/*******************************************************************************
 * LR(0)
 */
struct core // 也叫item
{
    pair<string, vector<string>> rule;
    int pos;
    core(){};
    core(int p, string left, vector<string> right)
    {
        pos = p;
        rule.first = left;
        rule.second = right;
    }
    bool operator==(const core &other) const { return rule == other.rule && pos == other.pos; }
};
string Start;
vector<vector<core>> LR0DFAs;               // LR(0)DFA数字化
multimap<int, pair<string, int>> trans_LR0; // LR(0)转移关系
void addRulesOfA(vector<core> &items, string A)
{
    auto range = rules.equal_range(A);
    for (auto right = range.first; right != range.second; right++)
        items.push_back(core(-1, A, right->second));
}
void generLR0DFA()
{
    // 添加新开始符号
    rules.insert({Start + "'", {Start}});
    Start = Start + "'";

    // 第一个DFA结点生成
    queue<int> Q;
    string A; // A -> x1 x2 x3...
    vector<core> items;
    core c(-1, Start, rules.find(Start)->second);
    if (c.pos >= (int)c.rule.second.size())
        return;
    A = c.rule.second.at(c.pos + 1);
    items.push_back(c);
    addRulesOfA(items, A);
    LR0DFAs.push_back(items);
    Q.push(LR0DFAs.size() - 1);
    while (!Q.empty())
    {
        // 取出队首元素
        int t = Q.front();
        Q.pop();
        // 生成新DFA
        for (auto item : LR0DFAs[t])
        {
            vector<core> newItems;
            if (item.pos >= (int)item.rule.second.size() - 1)
                continue;
            item.pos++;
            newItems.push_back(item);
            if (item.pos < (int)item.rule.second.size() - 1) // 不是规约项
                addRulesOfA(newItems, item.rule.second.at(item.pos + 1));
            if (find(LR0DFAs.begin(), LR0DFAs.end(), newItems) == LR0DFAs.end())
            {
                LR0DFAs.push_back(newItems);
                Q.push(LR0DFAs.size() - 1);
            }
            trans_LR0.insert({t, {item.rule.second.at(item.pos), find(LR0DFAs.begin(), LR0DFAs.end(), newItems) - LR0DFAs.begin()}});
        }
    }
}

/*******************************************************************************
 * LR(1)
 */
struct Item
{ // 一个LR(1)DFA叫Items，由一个或多个Item组成
    int id;
    set<string> pre;
    core c;
    Item(int i, int pos, string left, vector<string> right, set<string> p)
    {
        id = i;
        c = core(pos, left, right);
        pre = p;
    }
    bool CompareCore(const Item &other) const { return c == other.c; }
    bool operator==(const Item &other) const { return c == other.c && pre == other.pre; }
    bool operator<(const Item &other) const { return id < other.id; }
};
struct Tran
{
    int start, end;
    string edge;
    Tran(int s, string edg, int e) : start(s), edge(edg), end(e) {}
    bool operator==(const Tran &other) const { return start == other.start && end == other.end && edge == other.edge; }
};

vector<vector<Item>> LR1DFAs;          // LR(1)DFA数字化
vector<int> hasDrop;                   // 记录DFA有没有被删
vector<vector<vector<Item>>> LALRDFAs; // LALR(1)DFA
vector<Tran> trans_LR1;                // LR(1)转移关系
set<string> FirstOfA(vector<string> right)
{
    auto range = rules.equal_range(right[0]);
    if (range.first == range.second)
        return {right[0]}; // 终结符直接返回
    set<string> res;
    for (auto rule = range.first; rule != range.second; rule++)
    { // 不是终结符，合并它所有的规则对应的First
        set<string> f = FirstOfA(rule->second);
        res.insert(f.begin(), f.end());
    }
    return res;
}
set<string> FollowOfA(core c, set<string> pre)
{ // 确保要进入该函数的A是非终结符
    set<string> res;
    bool hasepsilon = true;
    int i = c.pos + 1; // A所在位置
    while (hasepsilon)
    {
        hasepsilon = false;
        i++; // 从A后面的第一个符号开始
        if (i > (int)c.rule.second.size() - 1)
            break; // 最终的Follow集为空或最终的Follow集仍有epsilon
        auto range = rules.equal_range(c.rule.second.at(i));
        set<string> FirstOfi;
        if (range.first == range.second) // 终结符
            FirstOfi.insert(c.rule.second.at(i));
        for (auto it = range.first; it != range.second; it++)
        { // 非终结符求得First集
            set<string> f = FirstOfA(it->second);
            FirstOfi.insert(f.begin(), f.end());
        }
        hasepsilon = find(FirstOfi.begin(), FirstOfi.end(), "@") != FirstOfi.end();
        FirstOfi.erase("@");
        res.insert(FirstOfi.begin(), FirstOfi.end());
    }
    if (i > c.rule.second.size() - 1)
        res.insert("$");
    return res;
}
void addRulesOfA(vector<Item> &items, string A)
{
    auto range = rules.equal_range(A);
    int id = items.begin()->id + 1;
    for (auto right = range.first; right != range.second; right++)
        items.push_back(Item(id++, -1, A, right->second, FollowOfA(items.begin()->c, items.begin()->pre)));
}
void generLR1DFA()
{
    // 添加新开始符号
    rules.insert({Start + "'", {Start}});
    Start = Start + "'";

    // 第一个DFA结点生成
    queue<int> Q;
    string A; // A -> x1 x2 x3...
    vector<Item> items;
    Item item(0, -1, Start, rules.find(Start)->second, {"$"});
    if (item.c.pos >= (int)item.c.rule.second.size())
        return;
    A = item.c.rule.second.at(item.c.pos + 1);
    items.push_back(item);
    addRulesOfA(items, A);
    LR1DFAs.push_back(items);
    hasDrop.push_back(0);
    Q.push(LR1DFAs.size() - 1);
    while (!Q.empty())
    {
        // 取出队首元素
        int t = Q.front();
        Q.pop();
        // 生成新DFA
        for (auto i = LR1DFAs[t].begin(); i != LR1DFAs[t].end(); i++)
        {
            auto item = *i;
            vector<Item> newItems;
            if (item.c.pos >= (int)item.c.rule.second.size() - 1)
                continue;
            item.c.pos++;
            newItems.push_back(item);
            if (item.c.pos < (int)item.c.rule.second.size() - 1) // 不是规约项
                addRulesOfA(newItems, item.c.rule.second.at(item.c.pos + 1));
            if (find(LR1DFAs.begin(), LR1DFAs.end(), newItems) == LR1DFAs.end())
            {
                LR1DFAs.push_back(newItems);
                hasDrop.push_back(0);
                Q.push(LR1DFAs.size() - 1);
            }
            trans_LR1.push_back(Tran(t, item.c.rule.second.at(item.c.pos), find(LR1DFAs.begin(), LR1DFAs.end(), newItems) - LR1DFAs.begin()));
        }
    }
}
bool hasSameCores(vector<Item> a, vector<Item> b)
{
    if (a.size() != b.size())
        return false;
    sort(a.begin(), a.end());
    sort(b.begin(), b.end());
    for (int i = 0; i < a.size(); i++)
        if (!a[i].CompareCore(b[i]))
            return false;
    return true;
}
void generLALR1DFA()
{
    for (int i = 0; i < LR1DFAs.size(); i++)
    { // 遍历每个DFA(items)进行合并
        for (int j = i + 1; j < LR1DFAs.size(); j++)
        {
            if (hasSameCores(LR1DFAs[i], LR1DFAs[j]))
            {
                for (int k = 0; k < LR1DFAs[i].size(); k++) // 合并到i结点
                    LR1DFAs[i][k].pre.insert(LR1DFAs[j][k].pre.begin(), LR1DFAs[j][k].pre.end());
                hasDrop[j] = 1; // 删除j结点要维持数组元素的位置不变，所以只能假删
                for (int k = 0; k < trans_LR1.size(); k++)
                    if (trans_LR1[k].end == j) // 更新入边
                        trans_LR1[k].end = i;
                    else if (trans_LR1[k].start == j) // 更新出边
                        trans_LR1[k].end = i;
                // 维持转移关系不重复
                for (int m = 0; m < trans_LR1.size(); m++)
                    for (int n = m + 1; n < trans_LR1.size(); n++)
                        if (trans_LR1[m] == trans_LR1[n])
                            trans_LR1.erase(remove(trans_LR1.begin(), trans_LR1.end(), trans_LR1[n]), trans_LR1.end());
            }
        }
    }
}

int main()
{
    // 初始化
    Start = "A";
    rules.insert({"A", {"(", "A", ")"}});
    rules.insert({"A", {"a"}});

    // generLR0DFA();
    generLR1DFA();
    generLALR1DFA();

    cout << "F";
    return 0;
}