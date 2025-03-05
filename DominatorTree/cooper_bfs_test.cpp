// https://cses.fi/problemset/task/1703/
#include <algorithm>
#include <bitset>
#include <cassert>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;
using Adj = vector<vector<int>>;
bitset<100001> vis;
int intersect(int a, int b, const vector<int> &idom, const vector<int> &pi) {
  while (a != b) {
    if (pi[a] < pi[b])
      a = idom[a];
    else
      b = idom[b];
  }
  return a;
}
int main() {
  int n, m;
  cin >> n >> m;
  Adj G(n + 1), rG(n + 1);
  for (int i = 0; i < m; i++) {
    int u, v;
    cin >> u >> v;
    G[u].push_back(v);
    rG[v].push_back(u);
  }
  vector<int> bo;
  queue<int> q;
  q.push(1);
  vis[1] = 1;
  while (q.size()) {
    int u = q.front();
    bo.push_back(u);
    q.pop();
    for (int v : G[u]) {
      if (!vis[v]) {
        q.push(v);
        vis[v] = 1;
      }
    }
  }
  vector<int> pi(n + 1);
  for (int i = 0; i < bo.size(); i++)
    pi[bo[i]] = bo.size() - i - 1;
  bool changed = true;
  vector<int> idom(n + 1, -1);
  idom[1] = 1;
  while (changed) {
    changed = false;
    for (int i = 1; i < bo.size(); i++) {
      int u = bo[i];
      int j;
      for (j = 0; j < rG[u].size(); j++)
        if (idom[rG[u][j]] != -1)
          break;
      int new_dom = rG[u][j];
      for (; j < rG[u].size(); j++) {
        int v = rG[u][j];
        if (idom[v] == -1)
          continue;
        new_dom = intersect(new_dom, v, idom, pi);
      }
      if (new_dom != idom[u]) {
        changed = true;
        idom[u] = new_dom;
      }
    }
  }
  vector<int> res;
  int t = n;
  do {
    res.push_back(t);
    t = idom[t];
  } while (t > 1);
  res.push_back(1);
  sort(res.begin(), res.end());
  cout << res.size() << endl;
  for (int i = 0; i < res.size(); i++)
    cout << res[i] << " \n"[i == res.size() - 1];
}
