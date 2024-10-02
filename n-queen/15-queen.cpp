#include <bits/stdc++.h>
using namespace std;
const int N = 15;
int max_saved = 0;

class obj
{
public:
    int cost = 0;
    int board[N][N] = {};
    int hvalue = 0;
    void print()
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (board[i][j] == 1)
                    cout << "Q ";
                else
                    cout << ". ";
            }
            cout << endl;
        }
    }
};

struct u_comp
{
    bool operator()(obj a, obj b)
    {
        return a.cost < b.cost;
    }
};

struct h_comp
{
    bool operator()(obj a, obj b)
    {
        return a.hvalue > b.hvalue;
    }
};

struct a_comp
{
    bool operator()(obj a, obj b)
    {
        return a.cost + a.hvalue < b.cost + b.hvalue;
    }
};

struct r_comp
{
    bool operator()(obj a, obj b)
    {
        return a.cost + a.hvalue > b.cost + b.hvalue;
    }
};

vector<obj> expand(obj node) // enpand 1 row
{
    vector<obj> child_nodes;
    int c = node.cost;
    int a[N] = {};
    for (int i = 0; i < N; i++) // 判斷哪個row上面已經有queen
    {
        for (int j = 0; j < N; j++)
        {
            if (node.board[i][j] == 1)
            {
                a[i] = 1;
                break;
            }
        }
    }
    obj child;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            child.board[i][j] = node.board[i][j]; // 複製parent盤面
        }
    }
    for (int i = 0; i < N; i++)
    {
        if (a[i]) // i row 上有queen跳過,直到第i row上沒有queen,expand那行
            continue;
        else
        {
            for (int y = 0; y < N; y++)
            {
                child.board[i][y] = 1;
                child.cost = c + 1;
                child_nodes.push_back(child);
                child.board[i][y] = 0;
                max_saved++;
            }
        }
        break;
    }
    return child_nodes;
}

bool is_valid(obj node)
{
    for (int i = 0; i < N; i++)
    {
        int f = 0;
        for (int j = 0; j < N; j++)
        {
            if (node.board[i][j] == 1)
            {
                f = j;
                break;
            }
            else
                f = -1;
        }
        if (f != -1)
        {
            for (int x = 0; x < N; x++) // 判斷row有沒有queen
            {
                if (node.board[i][x] == 1 && x != f)
                {
                    // cout << "a" << endl;
                    return false;
                }
            }
            for (int y = 0; y < N; y++) // 判斷column有沒有queen
            {
                if (node.board[y][f] == 1 && y != i)
                {
                    // cout << "b" << endl;
                    return false;
                }
            }
            int x = i;
            int y = f;
            while (x != -1 && y != -1) // 判斷左上對角有沒有queen
            {
                x--;
                y--;
                if (node.board[x][y] == 1 && x != -1 && y != -1)
                {
                    // cout << "c" << endl;
                    return false;
                }
            }
            x = i;
            y = f;
            while (x != -1 && y != N) // 判斷左下對角有沒有queen
            {
                x--;
                y++;
                if (node.board[x][y] == 1 && x != -1 && y != N)
                {
                    // cout << "d" << endl;
                    return false;
                }
            }
            x = i;
            y = f;
            while (x != N && y != -1) // 判斷右上對角有沒有queen
            {
                x++;
                y--;
                if (node.board[x][y] == 1 && x != N && y != -1)
                {
                    // cout << "e" << endl;
                    return false;
                }
            }
            x = i;
            y = f;
            while (x != N && y != N) // 判斷右下對角有沒有queen
            {
                x++;
                y++;
                if (node.board[x][y] == 1 && x != N && y != N)
                {
                    // cout << "f" << endl;
                    return false;
                }
            }
        }
    }
    return true;
}

bool is_goal(obj node)
{
    int count = 0;
    if (!is_valid(node))
        return false;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (node.board[i][j] == 1)
                count++;
        }
    }
    if (count == N)
        return true;
    return false;
}

int heuristic(obj node)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (node.board[i][j] == 1)
                node.hvalue++;
        }
    }
    return -1;
}

int ucs(obj node)
{
    priority_queue<obj, vector<obj>, u_comp> pq;
    pq.push(node);
    max_saved++;
    while (!pq.empty())
    {
        int sz = pq.size();
        for (int i = 0; i < sz; i++)
        {
            obj cur = pq.top();
            pq.pop();
            if (is_goal(cur) && is_valid(cur))
            {
                cur.print();
                cout << "Step : " << cur.cost << endl;
                cout << "Maximum saved state : " << max_saved << endl;
                cout << endl;
                max_saved = 0;
                return 1;
            }

            vector<obj> child = expand(cur);
            for (auto i : child)
            {
                if (is_valid(i))
                    pq.push(i);
            }
        }
    }
    cout << "No solution" << endl;
    return -1;
}

int dls(obj node, int limit)
{
    stack<obj> s({node});
    max_saved++;
    while (!s.empty())
    {
        int sz = s.size();
        for (int i = 0; i < sz; i++)
        {
            obj cur = s.top();
            s.pop();
            if (is_goal(cur) && is_valid(cur))
            {
                cur.print();
                cout << "Step : " << cur.cost << endl;
                cout << "Maximum saved state : " << max_saved << endl;
                max_saved = 0;
                return 1;
            }

            if (cur.cost <= limit)
            {
                vector<obj> child = expand(cur);
                for (auto i : child)
                {
                    if (is_valid(i))
                        s.push(i);
                }
            }
        }
    }
    return -1;
}

int ids(obj node)
{
    int res;
    for (int l = N; l < 30; l++)
    {
        res = dls(node, l);
        cout << "The limit is " << l << endl;
        if (res == 1)
        {
            cout << endl;
            break;
        }
        if (l == 29)
            cout << "No solution" << endl;
    }
    return res;
}

int greedy(obj node)
{
    priority_queue<obj, vector<obj>, h_comp> pq;
    pq.push(node);
    max_saved++;
    while (!pq.empty())
    {
        int sz = pq.size();
        for (int i = 0; i < sz; i++)
        {
            obj cur = pq.top();
            pq.pop();
            if (is_goal(cur) && is_valid(cur))
            {
                cur.print();
                cout << "Step : " << cur.cost << endl;
                cout << "Maximum saved state : " << max_saved << endl;
                cout << endl;
                max_saved = 0;
                return 1;
            }

            vector<obj> child = expand(cur);
            for (auto i : child)
            {
                if (is_valid(i))
                {
                    heuristic(i);
                    pq.push(i);
                }
            }
        }
    }
    cout << "No solution" << endl;
    return -1;
}

int Astar(obj node)
{
    priority_queue<obj, vector<obj>, a_comp> pq;
    pq.push(node);
    max_saved++;
    while (!pq.empty())
    {
        int sz = pq.size();
        for (int i = 0; i < sz; i++)
        {
            obj cur = pq.top();
            pq.pop();
            if (is_goal(cur) && is_valid(cur))
            {
                cur.print();
                cout << "Step : " << cur.cost << endl;
                cout << "Maximum saved state : " << max_saved << endl;
                cout << endl;
                max_saved = 0;
                return 1;
            }

            vector<obj> child = expand(cur);
            for (auto i : child)
            {
                if (is_valid(i))
                {
                    heuristic(i);
                    pq.push(i);
                }
            }
        }
    }
    cout << "No solution" << endl;
    return -1;
}

int rbfs(obj node, int f_limit)
{
    if (is_goal(node) && is_valid(node))
    {
        node.print();
        cout << "Step : " << node.cost << endl;
        cout << "Maximum saved state : " << max_saved << endl;
        cout << endl;
        max_saved = 0;
        return 1;
    }
    priority_queue<obj, vector<obj>, r_comp> pq;
    vector<obj> child = expand(node);
    for (auto i : child)
    {
        if (is_valid(i))
        {
            heuristic(i);
            pq.push(i);
        }
    }
    if (pq.empty())
        return 1e9;
    while (!pq.empty())
    {
        obj best = pq.top();
        pq.pop();
        if (best.hvalue > f_limit)
            return best.hvalue;
        if (best.hvalue == 1e9)
            return 1e9;
        obj alternative = pq.top();
        int res = rbfs(best, min(f_limit, alternative.hvalue));
        if (res == 1)
            return 1;
        best.hvalue = res;
        pq.push(best);
    }
    return 1e9;
}

int RBFS(obj node)
{
    max_saved++;
    if (rbfs(node, 1e9) == 1e9)
        cout << "No solution" << endl;
    return -1;
}

int main()
{
    int n;
    cout << "Enter the number of queen:" << endl;
    cin >> n;
    obj start;
    cout << "Enter the location of each queen:" << endl;
    for (int i = 0; i < n; i++)
    {
        int x, y;
        cin >> x >> y;
        start.board[x][y] = 1;
    }
    start.print();
    cout << endl;
    if (!is_valid(start))
        cout << "No solution!" << endl;
    else
    {
        cout << "UCS:" << endl;
        ucs(start);
        cout << "IDS:" << endl;
        ids(start);
        cout << "Greedy search:" << endl;
        greedy(start);
        cout << "Astar search:" << endl;
        Astar(start);
        cout << "RBFS:" << endl;
        RBFS(start);
    }
    return 0;
}
