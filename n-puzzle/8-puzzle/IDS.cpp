#include <bits/stdc++.h>
using namespace std;

int DFS(string start, string goal, int d);

bool solvable(string s)
{
    int inv_count = 0;
    for (int i = 0; i < s.size(); i++)
        for (int j = i + 1; j < s.size(); j++)
            if (int(s[i]) - '0' && int(s[j]) - '0' && s[i] > s[j])
                inv_count++;

    inv_count = inv_count % 2;
    return inv_count;
}

int IDS(string start, string goal)
{
    for (int d = 1; d < 32; d++)
    {
        int found = DFS(start, goal, d);
        if (found == 1)
        {
            return 1;
        }
    }
    return -1;
}

int DFS(string start, string goal, int l)
{
    vector<vector<int>> moves = {{1, 3}, {0, 2, 4}, {1, 5}, {0, 4, 6}, {1, 3, 5, 7}, {2, 4, 8}, {3, 7}, {4, 6, 8}, {5, 7}};
    stack<pair<string, int>> s({{start, 0}});
    set<string> record({start});

    while (!s.empty())
    {
        int sz = s.size();
        // 處理完現有的state, 沒找到繼續找goal state
        for (int i = 0; i < sz; i++)
        {
            // 輸出結果
            string cur = s.top().first;
            int d = s.top().second;
            s.pop();

            if (cur == goal)
            {
                cout << "Depth : " << d << endl;
                cout << "Saved state : " << record.size() << endl;
                return 1;
            }

            // 找尋0的位置
            int dir = cur.find("0");
            // 移動0的方向
            if (d <= l)
            {
                for (int k : moves[dir])
                {
                    string next = cur;
                    next[dir] = cur[k];
                    next[k] = '0';
                    // 判斷有無紀錄過
                    if (record.find({next}) == record.end())
                    {
                        s.push({next, d + 1});
                        record.insert({next});
                    }
                }
            }
        }
    }
    return -1;
}

int main()
{
    string start = "123460578", goal = "162403578";
    if (solvable(start) == solvable(goal))
    {
        cout << "Iterative Deeping search" << endl;
        cout << "start : " << start << endl;
        IDS(start, goal);
        cout << "goal : " << goal << endl;
    }
    else
        cout << "Not found" << endl;

    start = "586074231", goal = "580736214";
    if (solvable(start) == solvable(goal))
    {
        cout << "Iterative Deeping search" << endl;
        cout << "start : " << start << endl;
        IDS(start, goal);
        cout << "goal : " << goal << endl;
    }
    else
        cout << "Not found" << endl;
}