#include "../lib.hpp"
using namespace std;

int main() {
	string tmp;

	int seq{0};
	umap<string, int> entities;

	uset<int> orbiteds, orbiters;

	vector<pair<int, int>> links;
	vs segs;

	while (getline(cin, tmp)) {
		Split(tmp, ')', segs);

		int idx0, idx1;
		if (auto found = entities.find(segs[0]); found != end(entities)) {
			idx0 = found->second; }
		else {
			entities[segs[0]] = idx0 = seq++; }
		if (auto found = entities.find(segs[1]); found != end(entities)) {
			idx1 = found->second; }
		else {
			entities[segs[1]] = idx1 = seq++; }

		orbiteds.insert(idx0);
		orbiters.insert(idx1);
		links.emplace_back(idx0, idx1); }

	auto lm = vector<bool>( seq*seq, 0 );
	for (const auto& link : links) {
		lm[link.first * seq + link.second] = true; }


	int p1{0};
	function<void(int, int)> R;
	R = [&](int depth, int p) -> void {
		p1 += depth;

		// outlinks in row
		for (int n{0}; n<seq; ++n) {
			if (lm[p*seq+n]) {
				R(depth+1, n); }}
	};
	R(0, entities["COM"]);
	cout << p1 << nl;


	int p2{oo};
	vector<bool> visited(seq, false);
	deque<pair<int, int>> queue;
	queue.push_front({0, entities["YOU"]});
	while (!queue.empty()) {
		auto [dist, here] = queue.back(); queue.pop_back();
		if (visited[here]) continue;
		visited[here] = true;
		if (here == entities["SAN"]) {
			p2 = dist;
			break; }

		// outlinks
		for (int n{0}; n<seq; ++n) {
			if (n == here) continue;
			if (lm[here*seq+n] && !visited[n]) {
				queue.push_front({ dist+1, n }); }
			if (lm[n*seq+here] && !visited[n]) {
				queue.push_front({ dist+1, n }); }}}
	p2 -= 2;

	cout << p2 << nl;
	return 0; }
