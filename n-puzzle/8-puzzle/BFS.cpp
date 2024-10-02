#include <bits/stdc++.h>
using namespace std;

bool solvable(string s)
{
    int inv_count = 0;
    for (int i = 0; i < s.size() - 1; i++)
        for (int j = i + 1; j < s.size(); j++)
            if (s[j] && s[i] && s[i] > s[j])
                inv_count++;

    inv_count = inv_count % 2;
    return inv_count;
}

int BFS(string start)
{
    vector<vector<int>> moves = {{1, 3}, {0, 2, 4}, {1, 5}, {0, 4, 6}, {1, 3, 5, 7}, {2, 4, 8}, {3, 7}, {4, 6, 8}, {5, 7}};
    string goal = "586074231";

    queue<string> q({start});
    unordered_set<string> record({start});

    int depth = 0;

    while (!q.empty())
    {
        int sz = q.size();
        // 處理完現有的state, 沒找到繼續找goal state
        for (int i = 0; i < sz; i++)
        {
            // 輸出結果
            string cur = q.front();
            cout << cur << endl;
            q.pop();

            if (cur == goal)
            {
                cout << depth << endl;
                cout << record.size() << endl;
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
                if (record.find(next) == record.end())
                {
                    q.push(next);
                    record.insert(next);
                }
            }
        }
        depth++;
    }
    return -1;
}

int main()
{
    string start = "812043765";
    if (!solvable(start))
        BFS(start);
    else
        cout << "No solution" << endl;
}