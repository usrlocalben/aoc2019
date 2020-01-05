#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#define umap unordered_map
#define uset unordered_set
using vi = std::vector<int>;
using namespace std;
const std::string nl{"\n"};
constexpr int INF{0x3f3f3f3f};

auto Split(const std::string& str, char ch) -> std::vector<std::string>& {
	static std::vector<std::string> items;
	items.clear();
	std::string src(str);
	auto nextmatch = src.find(ch);
	while (true) {
		auto item = src.substr(0, nextmatch);
		items.push_back(item);
		if (nextmatch == std::string::npos) { break; }
		src = src.substr(nextmatch + 1);
		nextmatch = src.find(ch); }
	return items; }

auto SplitNums(const std::string& str, char ch=',') -> std::vector<int>& {
	static std::vector<int> items;
	items.clear();
	std::string src(str);
	auto nextmatch = src.find(ch);
	while (true) {
		auto item = src.substr(0, nextmatch);
		items.push_back(stoi(item));
		if (nextmatch == std::string::npos) { break; }
		src = src.substr(nextmatch + 1);
		nextmatch = src.find(ch); }
	return items; }



int main() {
	string tmp;

	int seq{0};
	umap<string, int> entities;

	uset<int> orbiteds, orbiters;

	vector<pair<int, int>> links;

	while (getline(cin, tmp)) {
		auto& segs = Split(tmp, ')');

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

		
	/*
	vector<string> none;
	copy_if(begin(orbiteds), end(orbiteds), back_inserter(none),
			[&](auto item) { return orbiters.find(item) == end(orbiters); });
	assert(none.size() == 1);
	assert(none[0] == "COM");*/

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
	cout << "p1: " << p1 << nl;


	int p2{INF};
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

	cout << "p2: " << p2 << nl;

	return 0; }
