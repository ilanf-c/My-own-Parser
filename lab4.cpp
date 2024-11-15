#include "Grammer.h"
#include <queue>
#include <stack>
#include <map>

struct NFAnode_LR0 // 核心项
{
    string left;
    vector<string> right;
    int pos;
    bool operator==(const NFAnode_LR0 &a)
    {
        if (pos != a.pos || left != a.left || right.size() != a.right.size())
            return false;
        return equal(right.begin(), right.end(), a.right.begin(), a.right.end());
    }
};
struct NFAnode_LR1
{
    NFAnode_LR0 core;
    set<string> pre;
    NFAnode_LR1(set<string> p, string l, vector<string> r, int pos)
    {
        pre.insert(p.begin(), p.end());
        core.left = l;
        for (int i = 0; i < r.size(); i++)
            core.right.push_back(r[i]);
        core.pos = pos;
    }
    bool operator==(const NFAnode_LR1 &a)
    {
        return core == a.core && pre == a.pre;
    }
};

class NFA
{
private:
    vector<NFAnode_LR1> NFAStates;                       // NFA状态集
    vector<pair<int, int>> epTran;                       // epsilon转移集
    unordered_map<int, unordered_map<string, int>> Tran; // 转移集
    vector<string> edges;
    int StateNum; // 状态数

public:
    void initNFA()
    {
        StateNum = -1;
    }
    void deleteNFA()
    {
        NFAStates.clear();
        Tran.clear();
        epTran.clear();
        edges.clear();
    }
    int addState(NFAnode_LR1 lre) // 返回状态编号
    {
        int it = find(NFAStates.begin(), NFAStates.end(), lre) - NFAStates.begin();
        if (it < NFAStates.size())
            return it;
        NFAStates.push_back(lre);
        return ++StateNum;
    }
    void addTrans(int strt, int end, string sta)
    {
        if (sta == "@")
            epTran.push_back({strt, end});
        else
        {
            if (find(edges.begin(), edges.end(), sta) == edges.end())
                edges.push_back(sta);
            Tran[strt].insert({sta, end});
        }
    }
    int getStateNum() // 返回状态数
    {
        return StateNum;
    }
    vector<string> getEdge() // 返回除epsilon外的边
    {
        return edges;
    }
    NFAnode_LR1 getState(int i) // 输入状态编号返回状态
    {
        return NFAStates[i];
    }
    vector<int> getNextOfStateByEpsilon(int x) // 返回顶点编号n通过epsilon到达下一状态集合
    {
        vector<int> res;
        for (int i = 0; i < epTran.size(); i++)
            if (epTran[i].first == x)
                res.push_back(epTran[i].second);
        return res;
    }
    vector<int> getNextState(int strt, string sta)
    {
        vector<int> res;
        if (sta == "@")
        {
            for (int i = 0; i < epTran.size(); i++)
                if (epTran[i].first == strt)
                    res.push_back(epTran[i].second);
        }
        else
        {
            if (Tran[strt].find(sta) != Tran[strt].end())
                res.push_back(Tran[strt][sta]);
        }
        return res;
    }
    void addPreOfState(int i, string x) // 给第i个状态添加先行符
    {
        NFAStates[i].pre.insert(x);
    }
};

class LRE
{
private:
    NFA nfa;
    GR grammers; // 文法规则
    // DFA状态集(内层第二字段是NFA状态编号第一字段标记有没有被删除)
    vector<pair<int, vector<int>>> DFAStates;
    map<int, map<string, set<int>>> DFATran; // DFA边关系

private:
    set<string> First(vector<string> fomu, int pos) // 返回文法规则fomu中pos位置的符号的First集合
    {
        set<string> res;
        int i;
        for (i = pos; i < fomu.size(); i++)
        {
            set<string> t;
            if (grammers.isEnd(fomu[i]))
                t.insert(fomu[i]);
            else
            {
                vector<vector<string>> right = grammers.getRight(fomu[i]);
                for (int j = 0; j < right.size(); j++)
                {
                    set<string> tmp = First(right[j], 0);
                    t.insert(tmp.begin(), tmp.end());
                }
            }
            int hasEpsilon = (t.find("@") != t.end());
            t.erase("@");
            res.insert(t.begin(), t.end());
            if (!hasEpsilon)
                break;
        }
        if (i == fomu.size())
            res.insert("@");
        return res;
    }
    void generNFA()
    {
        string Start = grammers.getStart();
        NFAnode_LR1 t({"$"}, Start, grammers.getRight(Start)[0], 0);
        int num = nfa.addState(t);
        queue<int> Q;
        Q.push(num);
        unordered_map<string, set<string>> first = grammers.getFirst();
        while (!Q.empty())
        {
            int s = Q.front();
            Q.pop();
            t = nfa.getState(s);
            if (t.core.pos == t.core.right.size())
                continue;
            string x = t.core.right[t.core.pos];
            if (!grammers.isEnd(x)) // 非终结符
            {
                vector<vector<string>> right = grammers.getRight(x);
                for (int i = 0; i < right.size(); i++)
                {
                    // set<string> p = First(t.core.right, t.core.pos + 1);
                    set<string> p;
                    if (t.core.pos < t.core.right.size() - 1)
                        p = first[t.core.right[t.core.pos + 1]];
                    else
                        p = set<string>{"@"};
                    if (p.find("@") != p.end())
                        p.insert(t.pre.begin(), t.pre.end());
                    p.erase("@");
                    NFAnode_LR1 tmp(p, x, right[i], 0);
                    int e = nfa.addState(tmp);
                    nfa.addTrans(s, e, "@");
                    if (e == nfa.getStateNum())
                        Q.push(e); // 新创建的结点才入队
                }
            }
            t.core.pos++;
            int e = nfa.addState(t);
            nfa.addTrans(s, e, x);
            if (e == nfa.getStateNum())
                Q.push(e); // 新建的结点才入队
        }
    }
    vector<int> epClosure(int n) // epsilon闭包(输入状态编号，返回状态编号集合)
    {
        vector<int> res;
        int *vis = new int[nfa.getStateNum() + 1];
        for (int i = 0; i <= nfa.getStateNum(); i++)
            vis[i] = 0;
        stack<int> s;
        s.push(n);
        while (!s.empty())
        {
            int curr = s.top();
            s.pop();
            if (!vis[curr])
            {
                vis[curr] = 1;
                res.push_back(curr);
            }
            vector<int> next = nfa.getNextOfStateByEpsilon(curr);
            for (int i = 0; i < next.size(); i++)
                if (!vis[next[i]])
                    s.push(next[i]);
        }
        sort(res.begin(), res.end());
        return res;
    }
    int BinaryFind(const vector<int> &a, int b) // 二分查找
    {
        int l = 0;
        int r = a.size() - 1;
        while (l <= r)
        {
            int mid = l + (r - l) / 2;
            if (a[mid] == b)
                return mid;
            else if (a[mid] > b)
                r = mid - 1;
            else if (a[mid] < b)
                l = mid + 1;
        }
        return -1;
    }
    void setUnion(vector<int> &a, const vector<int> &b) // 将集合b并入集合a
    {
        // 输入a有序
        for (int i = 0; i < b.size(); i++)
            if (BinaryFind(a, b[i]) == -1)
                a.push_back(b[i]);
    }
    int isInit(const vector<pair<int, vector<int>>> &a, const vector<int> &b) // 集合a有元素b则返回b的编号
    {
        // 输入b有序且元素唯一
        for (int it = 0; it < a.size(); it++)
            if (equal(b.begin(), b.end(), a[it].second.begin(), a[it].second.end()))
                return it;
        return -1;
    }
    void ConstructDFA()
    {
        vector<int> epc = epClosure(0);
        DFAStates.push_back(make_pair(0, epc));
        vector<string> edges = nfa.getEdge();
        vector<int> tmp;
        int x = 0;
        while (x < DFAStates.size())
        {
            for (int i = 0; i < edges.size(); i++)
            {
                tmp.clear();
                for (int j = 0; j < DFAStates[x].second.size(); j++)
                {
                    vector<int> t = nfa.getNextState(DFAStates[x].second[j], edges[i]);
                    for (int k = 0; k < t.size(); k++)
                        setUnion(tmp, epClosure(t[k]));
                }
                int it = isInit(DFAStates, tmp);
                set<int> temp;
                if (!tmp.size()) // 集合为空不做存储
                {
                    DFATran[x][edges[i]] = temp;
                }
                else if (it != -1) // 已存在集合
                {
                    temp.insert(it);
                    DFATran[x][edges[i]] = temp;
                }
                else if (tmp.size()) // 新集合且不空
                {
                    DFAStates.push_back(make_pair(0, tmp));
                    temp.insert(DFAStates.size() - 1);
                    DFATran[x][edges[i]] = temp;
                }
            }
            x++;
        }
    }
    void UnionEdge(int x, int y)
    {
        vector<string> edges = nfa.getEdge();
        for (int i = 0; i < DFAStates.size(); i++)                                  // 索引DFA状态
            for (int j = 0; j < edges.size(); j++)                                  // 索引边
                for (int k = 0; k < DFATran[i][edges[j]].size(); k++)               // 索引到达的状态
                    if (DFATran[i][edges[j]].find(y) != DFATran[i][edges[j]].end()) // 如果是y则改为x
                    {
                        DFATran[i][edges[j]].erase(y);
                        DFATran[i][edges[j]].insert(x);
                    }
        for (auto it = DFATran[y].begin(); it != DFATran[y].end(); it++)
        {
            // it指边和到达的状态集
            for (auto it2 : it->second) // 将y到达的状态添加到x
                DFATran[x][it->first].insert(it2);
            DFATran[y][it->first].clear(); // 清空y到达的状态
        }
    }
    int isSame(vector<NFAnode_LR0> a, vector<NFAnode_LR0> b) // 无视顺序比较内容
    {
        // 假定一样大
        for (int i = 0; i < a.size(); i++)
            if (find(b.begin(), b.end(), a[i]) == b.end())
                return false;
        return true;
    }
    void toLALRE()
    {
        for (auto pi = DFAStates.begin(); pi != DFAStates.end(); pi++)
        {
            for (auto pj = pi + 1; pj != DFAStates.end(); pj++)
            {
                if (pi->second.size() != pj->second.size())
                    continue;
                vector<NFAnode_LR0> coresOfj, coresOfi;
                for (int k = 0; k < pi->second.size(); k++)
                {
                    coresOfj.push_back(nfa.getState(pj->second[k]).core);
                    coresOfi.push_back(nfa.getState(pi->second[k]).core);
                }
                if (isSame(coresOfi, coresOfj))
                {
                    // 给项目i添加先行符
                    for (int k = 0; k < coresOfi.size(); k++)
                    {
                        int index = find(coresOfj.begin(), coresOfj.end(), coresOfi[k]) - coresOfj.begin();
                        for (auto p : nfa.getState(pj->second[index]).pre)
                            nfa.addPreOfState(pi->second[k], p);
                    }
                    // 修改边
                    UnionEdge(pi - DFAStates.begin(), pj - DFAStates.begin());
                    // 删除状态
                    DFAStates[pj - DFAStates.begin()].first = -1;
                }
            }
        }
    }

public:
    void init(string filepath)
    {
        nfa.initNFA();
        grammers.init(filepath);
        generFirstFollow();
        generNFA();
        ConstructDFA();
        toLALRE();
        generDFApng();
    }
    void deleteLRE()
    {
        nfa.deleteNFA();
        grammers.deleteGrammer();
        DFAStates.clear();
        DFATran.clear();
    }
    void generDFApng()
    {
        ofstream dotfile("C:\\Users\\xu\\Desktop\\experiment\\code\\byyllab4\\dfa.dot");
        if (!dotfile.is_open())
            cerr << "ERROR: UNABLE CREATE DOT FILE." << endl;
        vector<string> edges = nfa.getEdge();
        dotfile << "digraph DFA {" << endl;
        dotfile << "    rankdir=LR;" << endl;
        dotfile << "    node[shape=Mrecord];" << endl;
        unordered_map<int, string> State;
        string tmp = "";
        int id = 0;
        for (int i = 0; i < DFAStates.size(); i++)
        {
            tmp = "    \"" + to_string(id) + "\\l";
            if (DFAStates[i].first == -1)
                continue;
            for (int j = 0; j < DFAStates[i].second.size(); j++)
            {
                NFAnode_LR1 t = nfa.getState(DFAStates[i].second[j]);
                tmp += "    [" + t.core.left + " ——> ";
                for (int i = 0; i < t.core.right.size(); i++)
                {
                    if (i == t.core.pos)
                        tmp += ".";
                    tmp += t.core.right[i];
                }
                if (t.core.pos == t.core.right.size())
                    tmp += ".";
                tmp += ",";
                for (auto x : t.pre)
                    tmp += x + "/";
                tmp.pop_back();
                tmp += "]\\l\n";
            }
            tmp.pop_back();
            tmp += "\"\n";
            State[i] = tmp;
            id++;
        }
        for (auto it = State.begin(); it != State.end(); it++)
        {
            for (int j = 0; j < edges.size(); j++)
            {
                set<int> nextStates;
                nextStates.insert(DFATran[it->first][edges[j]].begin(), DFATran[it->first][edges[j]].end());
                for (auto nextState : nextStates)
                    dotfile << "    " + State[it->first] + "\n\t->\n\n" + State[nextState] + "\t[label=\"" + edges[j] + "\"];\n\n";
            }
        }
        dotfile << "}" << endl;
        dotfile.close();
        system("dot -Tpng C:\\Users\\xu\\Desktop\\experiment\\code\\byyllab4\\dfa.dot -o C:\\Users\\xu\\Desktop\\experiment\\code\\byyllab4\\dfa.png");
    }
    void generFirstFollow()
    {
        unordered_map<string, set<string>> first = grammers.getFirst();
        unordered_map<string, set<string>> follow = grammers.getFollow();
        ofstream dotfile("C:\\Users\\xu\\Desktop\\experiment\\code\\byyllab4\\firstandfollow.dot");
        if (!dotfile.is_open())
            cerr << "ERROR: UNABLE CREATE DOT FILE." << endl;
        dotfile << "digraph G{" << endl;
        dotfile << "    node[shape=plaintext];" << endl;
        dotfile << "    a [label=<" << endl;
        dotfile << "<table border=\"1\" cellborder=\"1\" cellspacing=\"0\">" << endl;
        for (auto it = first.begin(); it != first.end(); it++)
        {
            dotfile << "<tr><td>First(" << it->first << ")</td>" << endl;
            dotfile << "<td>{";
            for (auto x : it->second)
                dotfile << x << ",";
            dotfile << "}</td></tr>";
        }
        for (auto it = follow.begin(); it != follow.end(); it++)
        {
            dotfile << "<tr><td>Follow(" << it->first << ")</td>" << endl;
            dotfile << "<td>{";
            for (auto x : it->second)
                dotfile << x << ",";
            dotfile << "}</td></tr>";
        }

        dotfile << "</table>" << ">];}" << endl;
        system("dot -Tpng C:\\Users\\xu\\Desktop\\experiment\\code\\byyllab4\\firstandfollow.dot -o C:\\Users\\xu\\Desktop\\experiment\\code\\byyllab4\\firstandfollow.png");
    }
};
// int main()
// {
//     string filepath = "C:\\Users\\xu\\Desktop\\experiment\\code\\byyllab4\\test.txt";
//     // printf("请输入文件地址: ");
//     // scanf("%s", &filename);
//     LRE lre;
//     lre.init(filepath);
//     // system("dot -Tpng C:\\Users\\xu\\Desktop\\experiment\\code\\byyllab4\\dfa.dot -o C:\\Users\\xu\\Desktop\\experiment\\code\\byyllab4\\dfa.png");
//     return 0;
// }