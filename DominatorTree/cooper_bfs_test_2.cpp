#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cstring>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;
using Adj = vector<vector<int>>;
constexpr int N = 200001;
struct Edge {
  int v, next;
};
struct Graph {
  Edge edges[N];
  int head[N];
  int size = 0;
  Graph(int n) { memset(head, -1, n * sizeof(int)); }
  void add_edge(int u, int v) {
    int e = size++;
    edges[e] = {v, head[u]};
    head[u] = e;
  }
};
class Cooper {
  int cnt;
  array<int, N> po, pi, idom;
  Graph G, rG;
  bitset<N> vis;
  int n;
  int intersect(int u, int v) {
    while (u != v) {
      if (pi[u] < pi[v])
        u = idom[u];
      else
        v = idom[v];
    }
    return u;
  }

public:
  Cooper(int n) : n(n), rG(n), G(n), vis(0) {
    fill(idom.begin(), idom.begin() + n, -1);
  }
  void add_edge(int u, int v) {
    G.add_edge(u, v);
    rG.add_edge(v, u);
  }
  array<int, N> solve(int s) {
    queue<int> q;
    q.push(s);
    vis[s] = 1;
    while (q.size()) {
      int t = q.front();
      q.pop();
      pi[t] = n - cnt - 1;
      po[cnt++] = t;
      for (int x = G.head[t]; ~x; x = G.edges[x].next) {
        int v = G.edges[x].v;
        if (vis[v])
          continue;
        vis[v] = 1;
        q.push(v);
      }
    }
    idom[s] = s;
    bool changed = true;
    while (changed) {
      changed = false;
      for (int i = 1; i < cnt; i++) {
        int temp = -1;
        int u = po[i];
        for (int x = rG.head[u]; ~x; x = rG.edges[x].next) {
          int v = rG.edges[x].v;
          if (idom[v] == -1)
            continue;
          if (temp == -1)
            temp = v;
          else
            temp = intersect(temp, v);
        }
        if (idom[u] != temp)
          idom[u] = temp, changed = true;
      }
    }
    return idom;
  }
};
int main() {
  int m, n, s;
  cin.tie(0)->sync_with_stdio(0);
  cin >> n >> m >> s;
  Cooper sol(n);
  for (int i = 0; i < m; i++) {
    int u, v;
    cin >> u >> v;
    sol.add_edge(u, v);
  }
  auto idom = sol.solve(s);
  for (int i = 0; i < n; i++)
    cout << idom[i] << ' ';
}
