#include <bits/stdc++.h>
using namespace std;

struct comp
{
    bool operator()(pair<string, int> l, pair<string, int> r)
    {
        return (l.second > r.second);
    }
};

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

int UCS(string start, string goal)
{
    vector<vector<int>> moves = {
        {1, 4},
        {0, 2, 5},
        {1, 3, 6},
        {2, 7},
        {0, 5, 8},
        {1, 4, 6, 9},
        {2, 5, 7, 10},
        {3, 6, 11},
        {4, 9, 12},
        {5, 8, 10, 13},
        {6, 9, 11, 14},
        {7, 10, 15},
        {8, 13},
        {9, 12, 14},
        {10, 13, 15},
        {11, 14},
    };
    priority_queue<pair<string, int>, vector<pair<string, int>>, comp> pq;
    pq.push({start, 0});
    set<string> record({start});

    while (!pq.empty())
    {
        int sz = pq.size();
        // 處理完現有的state, 沒找到繼續找goal state
        for (int i = 0; i < sz; i++)
        {
            // 輸出結果
            string cur = pq.top().first;
            int d = pq.top().second;
            pq.pop();

            if (cur == goal)
            {
                cout << "Depth : " << d << endl;
                cout << "Saved state : " << record.size() << endl;
                return 1;
            }

            // 找尋0的位置
            int dir = cur.find("0");
            // 移動0的方向
            for (int k : moves[dir])
            {
                string next = cur;
                next[dir] = cur[k];
                next[k] = '0';
                // 判斷有無紀錄過
                if (record.find({next}) == record.end())
                {
                    pq.push({next, d + 1});
                    record.insert({next});
                }
            }
        }
    }
    return -1;
}

int main()
{
    string start = "1234567890bcadef";
    string goal = "123456789becadf0";
    cout << "Uniform cost search" << endl;
    cout << "start : " << start << endl;
    UCS(start, goal);
    cout << "goal : " << goal << endl;

    start = "7eb58cda204f6391";
    goal = "7eb58caf24d16390";
    cout << "Uniform cost search" << endl;
    cout << "start : " << start << endl;
    UCS(start, goal);
    cout << "goal : " << goal << endl;
}