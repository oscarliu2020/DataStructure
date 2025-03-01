#include <algorithm>
#include <bitset>
#include <cassert>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;
using Adj = vector<vector<int>>;
constexpr int N = 200001;
bitset<N> vis;
int po[N];
int pi[N];
int idom[N];
int cnt;
void dfs(int u, const Adj &G) {
  vis[u] = 1;
  for (int v : G[u]) {
    if (!vis[v])
      dfs(v, G);
  }
  po[cnt++] = u;
}
int intersect(int a, int b) {
  while (a != b) {
    if (pi[a] < pi[b])
      a = idom[a];
    else
      b = idom[b];
  }
  return a;
}
int main() {
  int m, n, s;
  cin.tie(0)->sync_with_stdio(0);
  cin >> n >> m >> s;
  Adj G(n), rG(n);
  for (int i = 0; i < m; i++) {
    int u, v;
    cin >> u >> v;
    G[u].push_back(v);
    rG[v].push_back(u);
  }
  queue<int> q;
  q.push(s);
  vis[s] = 1;
  while (q.size()) {
    int t = q.front();
    po[cnt++] = t;
    q.pop();
    for (int v : G[t]) {
      if (vis[v])
        continue;
      vis[v] = 1;
      q.push(v);
    }
  }
  bool changed = true;
  for (int i = 0; i < cnt; i++)
    pi[po[i]] = cnt - i;
  for (int i = 0; i < n; i++)
    idom[i] = -1;
  idom[s] = s;
  while (changed) {
    changed = false;
    for (int i = 1; i < cnt; i++) {
      int new_idom = -1;
      int u = po[i];
      assert(s != u);
      for (int v : rG[u]) {
        if (idom[v] == -1)
          continue;
        if (new_idom == -1)
          new_idom = v;
        else
          new_idom = intersect(new_idom, v), assert(new_idom != -1);
      }
      if (new_idom != idom[u]) {
        changed = true;
        idom[u] = new_idom;
      }
    }
  }
  for (int i = 0; i < n; i++)
    cout << idom[i] << ' ';
}
