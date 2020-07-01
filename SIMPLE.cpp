// シンプルにダイクストラ法をやるだけのプログラム
// 結果テスト用です

#include <bits/stdc++.h>
#include <random>

using namespace std;

typedef long long ll;
typedef pair<ll, ll> P;


// クエリの数
const int Q = 100;
const int MAX_V = 500000;
const ll INF = (ll)1e18;

vector<P> G[MAX_V];
ll cost[MAX_V];
vector<P> query;

void generate_query(ll V) {
	srand(1);
	for (int i = 0; i < Q; i++) {
		int a = rand() % V, b = rand() % V;
		query.push_back(P(a, b));
	}
	return;
}


int main() {
	// グラフの入力
	ll V, E;
	cin >> V >> E;
	for (int i = 0; i < E; i++) {
		int u, v;
		ll cost;
		cin >> u >> v >> cost;
		G[u].push_back(P(v, cost));
		// G[v].push_back(P(u, cost));
	}
	// クエリの生成
	generate_query(V);

	// クエリの処理
	for (int i = 0; i < Q; i++) {
		int s = query[i].first, t = query[i].second;
		for (int i = 0; i < V; i++) {
			cost[i] = INF;
		}
		cost[s] = 0;
		priority_queue <P, vector<P>, greater<P> > pqps;
		pqps.push(P(0, s));
		while (!pqps.empty()) {
			P p = pqps.top();
			pqps.pop();
			int v = p.second;
			ll nowcost = p.first;
			if (nowcost > cost[v]) continue;
			for (int i = 0; i < G[v].size(); i++) {
				int nv = G[v][i].first;
				ll newcost = cost[v] + G[v][i].second;
				if (cost[nv] > newcost) {
					cost[nv] = newcost;
					pqps.push(P(newcost, nv));
				}
			}
		}

		ll ans = cost[t];
		cout << i << " " << ans << endl;
	}

}