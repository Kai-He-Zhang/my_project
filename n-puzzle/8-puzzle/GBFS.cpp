#include <bits/stdc++.h>
using namespace std;

typedef struct value
{
    int depth = 0;
    int h_value = 0;
    string state;
} v;

struct comp
{
    bool operator()(v l, v r)
    {
        return (l.h_value > r.h_value);
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

int heuristic_function(string s, string g)
{
    int distance = 0;
    unordered_map<char, pair<int, int>> goal_puzzle;
    for (int row = 0; row < s.size() / 3; row++)
    {
        for (int col = 0; col < s.size() / 3; col++)
        {
            goal_puzzle[g[row * (s.size() / 3) + col]] = make_pair(row, col);
        }
    }

    for (int i = 0; i < s.size(); i++)
    {
        if (s[i] == '0')
        {
            continue;
        }
        int gr = goal_puzzle[s[i]].first;
        int gc = goal_puzzle[s[i]].second;
        int r = i / 3;
        int c = i % 3;
        distance += abs(gr - r) + abs(gc - c);
    }
    return distance;
}

int GBFS(v start, string goal)
{
    vector<vector<int>> moves = {{1, 3}, {0, 2, 4}, {1, 5}, {0, 4, 6}, {1, 3, 5, 7}, {2, 4, 8}, {3, 7}, {4, 6, 8}, {5, 7}};
    priority_queue<v, vector<v>, comp> pq;
    pq.push(start);
    set<string> record({start.state});

    while (!pq.empty())
    {
        int sz = pq.size();
        // 處理完現有的state, 沒找到繼續找goal state
        for (int i = 0; i < sz; i++)
        {
            // 輸出結果
            string cur = pq.top().state;
            int d = pq.top().depth;
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
                v next;
                next.state = cur;
                next.state[dir] = cur[k];
                next.state[k] = '0';

                // h function
                next.h_value = heuristic_function(next.state, goal);

                // 判斷有無紀錄過
                if (record.find({next.state}) == record.end())
                {
                    next.depth = d + 1;
                    pq.push({next});
                    record.insert({next.state});
                }
            }
        }
    }
    return -1;
}

int main()
{
    v start;
    string goal = "162403578";
    start.state = "123460578";
    if (solvable(start.state) == solvable(goal))
    {
        cout << "Greedy Best First search" << endl;
        cout << "start : " << start.state << endl;
        GBFS(start, goal);
        cout << "goal : " << goal << endl;
    }
    else
        cout << "No solution" << endl;

    goal = "580736214";
    start.state = "586074231";
    if (solvable(start.state) == solvable(goal))
    {
        cout << "Greedy Best First search" << endl;
        cout << "start : " << start.state << endl;
        GBFS(start, goal);
        cout << "goal : " << goal << endl;
    }
    else
        cout << "No solution" << endl;
}