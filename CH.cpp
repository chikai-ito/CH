// Contraction Hierarchies を実行するプログラムです

#include <bits/stdc++.h>
#include <random>

using namespace std;

typedef long long ll;
typedef pair<ll, ll> P;
typedef pair<P, ll> T;


// クエリの数
const int Q = 100;
const int MAX_V = 24000000;
const ll INF = (ll)1e18;

// 頂点数、辺数
int V, E;
// グラフ
vector<P> G[MAX_V];

ll check[MAX_V];
ll check_cost[MAX_V];

// コスト用の配列
ll cost[2][MAX_V];

// クエリ用のvector
vector<P> query;

// 遅延配列部分
ll lazy[MAX_V];

// 頂点につけられたランク
int Rank[MAX_V];

priority_queue <P, vector<P>, greater<P> > Order;

bool need_shortcut(int s, int t, ll sum, int index, int phase) {
	// ショートカットが必要かどうかを求める
	priority_queue <T, vector<T>, greater<T> > pqps;
	pqps.push(T(P(0, 0), s));

	check[s] = index;
	check_cost[s] = 0;

	while (!pqps.empty()) {
		T p = pqps.top();
		pqps.pop();

		ll cost = p.first.first;
		int step = p.first.second;
		int v = p.second;

		if (step >= 5) continue;

		if (check[v] == index && cost > check_cost[v]) continue;
		for (int i = 0; i < G[v].size(); i++) {
			int nv = G[v][i].first;
			ll newcost = check_cost[v] + G[v][i].second;
			if (Rank[nv] != 0) continue;
			if (newcost > sum) continue;
			if (check[nv] != index || check_cost[nv] > newcost) {
				check[nv] = index;
				check_cost[nv] = newcost;
				pqps.push(T(P(newcost, step + 1), nv));
			}
		}
	}
	return (check[t] == index);
}

void generate_query(ll V) {
	srand(1);
	for (int i = 0; i < Q; i++) {
		int a = rand() % V, b = rand() % V;
		query.push_back(P(a, b));
	}
	return;
}

struct Timer {
	int64_t start;
	const int64_t CYCLES_PER_SEC = 2800000000;
	Timer() { reset(); }
	void reset() { start = getCycle(); }
	inline double get() { return (double)(getCycle() - start) / CYCLES_PER_SEC; }
	inline int64_t getCycle() {
		uint32_t low, high;
		__asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
		return ((int64_t)low) | ((int64_t)high << 32);
	}
};

Timer timer;

void preprocess() {
	// 前計算を行う

	// プライオリティーキューの初期化
	for (int i = 0; i < V; i++) {
		Order.push(P(G[i].size() * 20, i));
	}

	// Node Ordering
	int phase = 0;
	int index = 0;
	while (!Order.empty()) {
		// 順番の小さい順に頂点を取り出していく
		int weight = Order.top().first;
		int u = Order.top().second;
		Order.pop();

		if (lazy[u] > 0) {
			Order.push(P(weight + lazy[u] , u));
			lazy[u] = 0;
			continue;
		}

		phase ++;
		Rank[u] = phase;
		// Node Contraction

		if (phase % 10 == 0) cerr << "Preprocessing ... Phase " << phase << endl;
		if (phase % 1000000 == 0 || phase == 23947347) {
			cout << fixed << setprecision(10) << "Phase: " << phase << " Time: " << timer.get() << endl;
		}

		// ショートカットを生成するかどうかの探索を行う
		for (int i = 0; i < G[u].size(); i++) {
			for (int j = 0; j < i; j++) {
				index ++;

				P edge_v = G[u][i], edge_w = G[u][j];
				// v - w の間にショートカットは必要か？
				int v = edge_v.first, w = edge_w.first;
				ll newcost = edge_v.second + edge_w.second;
				if (v == u || w == u) continue;
				if (Rank[v] != 0 || Rank[w] != 0) continue;

				// v - w を newcost 以下のコストで移動可能か？
				if (!need_shortcut(v, w, newcost, index, phase)) {
					G[v].push_back(P(w, newcost));
					G[w].push_back(P(v, newcost));
					lazy[v] ++;
					lazy[w] +;
				}
			}
		}
	}
	return;
}

bool r(int s, int t, int dir) {
	if (Rank[s] < Rank[t] && dir == 0) return true;
	if (Rank[s] > Rank[t] && dir == 1) return true;
	return false;
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	// グラフの入力
	cerr << "Input ... " << endl;
	cin >> V >> E;
	for (int i = 0; i < E; i++) {
		int u, v;
		ll cost;
		cin >> u >> v >> cost;
		G[u].push_back(P(v, cost));
	}
	cerr << "Input end. " << endl;
	cerr << "Generating Queries ... " << endl;
	// クエリの生成
	generate_query(V);
	cerr << "Generated. " << endl;

	timer.reset();

	cerr << "Start Preprocessing ... " << endl;
	// 前計算パート
	preprocess();

	cerr << "End Preprocessing ... " << endl;

	// クエリの処理
	for (int i = 0; i < Q; i++) {
		int s = query[i].first, t = query[i].second;
		// cout << "Query " << s << " -> " << t << endl;
		ll d = INF;
		for (int i = 0; i < V; i++) {
			cost[0][i] = cost[1][i] = INF;
		}
		cost[0][s] = 0;
		cost[1][t] = 0;

		priority_queue <P, vector<P>, greater<P> > pqps[2];

		pqps[0].push(P(0, s));
		pqps[1].push(P(0, t));

		// 0 : up, 1 : down
		int dir = 0;

		while ((!pqps[0].empty() || !pqps[1].empty()) && (d > min(pqps[0].top().first, pqps[1].top().first))) {
			if (!pqps[1 - dir].empty()) dir = 1 - dir;

			P p = pqps[dir].top(); pqps[dir].pop();
			int u = p.second;

			d = min(d, cost[0][u] + cost[1][u]);

			for (auto edge : G[u]) {
				int v = edge.first;
				if (Rank[u] <= Rank[v] && cost[dir][u] + edge.second < cost[dir][v]) {
					cost[dir][v] = cost[dir][u] + edge.second;
					pqps[dir].push(P(cost[dir][v], v));
				}
			}
		}
		cout << i << " " << d << endl;
	}
	return 0;
}