#include <bits/stdc++.h>
using namespace std;
/* 返回A的First集合 */
set<string> getFirst(unordered_map<string, vector<vector<string>>> grammer, string A)
{
    set<string> res;
    if (grammer.find(A) == grammer.end())
    {
        res.insert(A);
        return res;
    }
    vector<vector<string>> t = grammer[A];
    for (int i = 0; i < t.size(); i++)
    {
        int k;
        for (k = 0; k < t[i].size(); k++)
        {
            set<string> tmp;
            if (grammer.find(t[i][k]) == grammer.end() || t[i][k] == "@") // 终结符
                tmp.insert(t[i][k]);
            else // 非终结符
                tmp = getFirst(grammer, t[i][k]);
            int falg = (tmp.find("@") == tmp.end());
            tmp.erase("@");
            res.insert(tmp.begin(), tmp.end());
            if (falg) // first(x_k)没有epsilon
                break;
        }
        if (k == t[i].size())
            res.insert("@");
    }
    if (res.find("@") != res.end())
    {
        for (auto it = grammer.begin(); it != grammer.end(); it++)
        {
            t = grammer[it->first];
            for (int i = 0; i < t.size(); i++)
            {
                int index = find(t[i].begin(), t[i].end(), A) - t[i].begin();
                if (index >= t[i].size() - 1)
                    continue;
                set<string> tmp = getFirst(grammer, t[i][index + 1]);
                res.insert(tmp.begin(), tmp.end());
            }
        }
    }
    cout << "f;";
    return res;
}

/* 判断两个状态集合是否相等 */
int isSame(const vector<int> &a, const vector<int> &b)
{
    // 输入状态集合有序且元素唯一
    if (a.size() != b.size()) // 长度不等
        return 0;
    for (int i = 0; i < a.size(); i++)
        if (a[i] != b[i]) // 元素不同
            return 0;
    return 1;
}

// namespace std // 定义vector<core>哈希函数
// {
//     template <>
//     struct hash<vector<string>>
//     {
//         size_t operator()(const vector<string> &ves) const
//         {
//             size_t hash_val = 0;
//             for (const auto &str : ves)
//                 hash_val ^= hash<string>()(str);
//             return hash_val;
//         }
//     };
//     template <>
//     struct hash<core>
//     {
//         size_t operator()(const core &c) const
//         {
//             size_t h1 = hash<string>()(c.rule.first);
//             size_t h2 = hash<vector<string>>()(c.rule.second);
//             size_t h3 = hash<int>()(c.pos);
//             return h1 ^ (h2 << 1) ^ (h3 << 2);
//         }
//     };
//     template <>
//     struct hash<vector<core>>
//     {
//         size_t operator()(const vector<core> &vc) const
//         {
//             size_t hash_value = 0;
//             for (const auto &c : vc)
//             {
//                 hash_value ^= hash<core>()(c);
//             }
//             return hash_value;
//         }
//     };
// }
