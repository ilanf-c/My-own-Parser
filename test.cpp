#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
using namespace std;

int main()
{
    // multimap<int, int> tmp;
    // tmp.insert({1, 2});
    // tmp.insert({1, 3});
    // tmp.insert({1, 4});
    // tmp.insert({2, 3});
    // auto range = tmp.equal_range(1);
    // for (auto i = range.first; i != range.second; i++)
    //     if (i->second == 3)
    //     {
    //         tmp.erase(i);
    //         break;
    //     }
    set<int> a;
    a.insert(1);
    a.insert(2);
    auto p = a.insert(3);
    if (p.second)
        cout << 'y';
    p = a.insert(3);
    if (!p.second)
        cout << "n";
    for (auto x : a)
        cout << x;
    return 0;
}