#include <bits/stdc++.h>
using namespace std;

typedef struct value
{
    int h_value = 0;
    string state;
    int depth = 0;
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

pair<bool, int> RBFS(v start, int flimit);
string goal;
set<string> record;

void Recursive_BFS(v s)
{
    auto [result, d] = RBFS(s, 1e9);
    if (result)
    {
        cout << "Depth : " << d << endl;
        cout << "Saved state : " << record.size() << endl;
    }
    else
        cout << "Not found!" << endl;
}

pair<bool, int> RBFS(v start, int flimit)
{
    vector<vector<int>> moves = {{1, 3}, {0, 2, 4}, {1, 5}, {0, 4, 6}, {1, 3, 5, 7}, {2, 4, 8}, {3, 7}, {4, 6, 8}, {5, 7}};
    priority_queue<v, vector<v>, comp> pq;
    pq.push(start);
    record.insert({start.state});

    // 輸出結果
    string cur = pq.top().state;
    int node_h = pq.top().h_value;
    int node_d = pq.top().depth;
    pq.pop();

    if (cur == goal)
    {
        return {true, node_d};
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
        next.h_value = node_d + 1 + heuristic_function(next.state, goal);

        // 判斷有無紀錄過
        if (record.find({next.state}) == record.end() || node_h >= next.h_value)
        {
            next.depth = node_d + 1;
            next.h_value = max(node_h, next.h_value);
            pq.push({next});
            record.insert({next.state});
        }
    }

    while (!pq.empty())
    {
        v best = pq.top();
        if (best.h_value > flimit)
        {
            return {false, best.h_value};
        }
        pq.pop();

        int alternative = pq.top().h_value;
        pair<bool, int> res = RBFS(best, min(flimit, alternative));

        if (!res.first)
        {
            best.h_value = res.second;
            pq.push(best);
        }
        else
            return res;
    }
    return {false, 1e9};
}

int main()
{
    v start;
    goal = "162403578";
    start.state = "123460578";
    if (solvable(start.state) == solvable(goal))
    {
        cout << "Recursive Best First search" << endl;
        cout << "start : " << start.state << endl;
        Recursive_BFS(start);
        cout << "goal : " << goal << endl;
    }
    else
        cout << "Not found" << endl;

    goal = "580736214";
    start.state = "586074231";
    if (solvable(start.state) == solvable(goal))
    {
        cout << "Recursive Best First search" << endl;
        cout << "start : " << start.state << endl;
        Recursive_BFS(start);
        cout << "goal : " << goal << endl;
    }
    else
        cout << "Not found" << endl;
}