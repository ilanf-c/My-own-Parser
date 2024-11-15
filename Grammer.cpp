#include "Grammer.h"

void GR::init(string filepath)
{
    fstream file;
    file.open(filepath, ios::in);
    if (!file.is_open())
    {
        cerr << "File " << filepath << " open failed" << endl;
        exit(1);
    }
    string line, s, left;
    int falg = false;
    while (getline(file, line))
    {
        istringstream stream(line);
        stream >> left;
        if (Start == "")
            Start = left;
        vector<string> tmp;
        while (!falg)
        {
            tmp.clear();
            while (stream >> s)
            {
                if (s == "|")
                {
                    falg = true;
                    break;
                }
                if (s != "->")
                    tmp.push_back(s);
            }
            if (falg)
                falg = false;
            else
                break;
            grammer[left].push_back(tmp);
        }
        grammer[left].push_back(tmp);
    }
    file.close();
    expand();
}

unordered_map<string, set<string>> GR::getFirst()
{
    unordered_map<string, set<string>> res;
    set<string> emp;
    emp.clear();
    for (auto it = grammer.begin(); it != grammer.end(); it++)
        res[it->first] = emp;
    int haschange = true;
    while (haschange)
    {
        haschange = false;
        for (auto it = grammer.begin(); it != grammer.end(); it++)
        {
            int past = res[it->first].size();
            vector<vector<string>> t = it->second;
            for (int i = 0; i < t.size(); i++)
            {
                int k;
                for (k = 0; k < t[i].size(); k++)
                {
                    set<string> k_first;
                    if (grammer.find(t[i][k]) == grammer.end())
                        k_first = set<string>{t[i][k]};
                    else
                        k_first = res[t[i][k]];
                    int hasEpsilon = (k_first.find("@") != k_first.end());
                    k_first.erase("@");
                    res[it->first].insert(k_first.begin(), k_first.end());
                    if (!hasEpsilon)
                        break;
                }
                if (k == t[i].size())
                    res[it->first].insert("@");
            }

            if (res[it->first].find("@") != res[it->first].end())
            {
                for (auto it2 = grammer.begin(); it2 != grammer.end(); it2++)
                {
                    t = it2->second;
                    for (int i = 0; i < t.size(); i++)
                    {
                        int index = find(t[i].begin(), t[i].end(), it->first) - t[i].begin();
                        if (index >= t[i].size() - 1)
                            continue;
                        res[it->first].insert(res[t[i][index + 1]].begin(), res[t[i][index + 1]].end());
                    }
                }
            }
            int now = res[it->first].size();
            if (now - past)
                haschange = true;
        }
    }

    // cout << "First finished";
    return res;
}

unordered_map<string, set<string>> GR::getFollow()
{
    unordered_map<string, set<string>> res;
    unordered_map<string, set<string>> First = getFirst();
    set<string> emp;
    emp.clear();
    for (auto i = grammer.begin(); i != grammer.end(); i++)
        res[i->first] = emp;
    res[Start].insert("$");
    int haschange = true;
    while (haschange)
    {
        haschange = false;
        for (auto it = grammer.begin(); it != grammer.end(); it++)
        {
            vector<vector<string>> t = it->second;
            for (int i = 0; i < t.size(); i++)
            {
                for (int k = 0; k < t[i].size(); k++)
                {
                    if (grammer.find(t[i][k]) == grammer.end())
                        continue; // 终结符
                    set<string> tmp;
                    if (k != t[i].size() - 1)
                        tmp = First[t[i][k + 1]];
                    else
                        tmp = set<string>{"@"};
                    int hasEpsilon = (tmp.find("@") != tmp.end());
                    tmp.erase("@");
                    int past = res[t[i][k]].size();
                    res[t[i][k]].insert(tmp.begin(), tmp.end());
                    if (hasEpsilon)
                        res[t[i][k]].insert(res[it->first].begin(), res[it->first].end());
                    int now = res[t[i][k]].size();
                    if (now - past)
                        haschange = true;
                }
            }
        }
    }
    // cout << "Follow finished" << endl;
    return res;
}

void GR::expand()
{
    if (grammer[Start].size() > 1)
    {
        vector<string> t{Start};
        Start = Start + "'";
        grammer[Start].push_back(t);
    }
    // cout << "Expand finished";
}
