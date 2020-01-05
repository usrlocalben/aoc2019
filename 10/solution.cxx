#include <algorithm>
#include <cmath>
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
using pii = std::pair<int, int>;
constexpr auto kPi = 3.14169265359F;

struct pair_hash {
	template <class T1, class T2>
	size_t operator()(const pair<T1, T2>& p) const {
		return std::hash<T1>()(p.first) ^ std::hash<T2>()(p.second); } };

auto signsep(int x) -> pii {
	if (x < 0) {
		return { -x, -1 }; }
	return { x, 1 }; }

auto vec2deg(int dx, int dy) -> float {
	if (dx == 0) {
		if (dy < 0) {
			return 0; }
		else {
			return kPi; }}
	else if (dy == 0) {
		if (dx > 0) {
			return kPi/2; }
		else {
			return kPi+kPi/2; }}
	else {
		auto d = atan(dy/dx)+(kPi/2);
		if (dx < 0) {
			d += kPi; }
		return d; }}

int main() {
	string tmp;
	vector<string> map;
	while (getline(cin, tmp)) {
		map.emplace_back(tmp); }

	const int h = map.size();
	const int w = map[0].size();

	int p1{0};
	int bestX, bestY;
	umap<pii, int, pair_hash> hits{};
	for (int cy{0}; cy<h; ++cy) {
		for (int cx{0}; cx<w; ++cx) {
			if (map[cy][cx] != '#') continue;

			hits.clear();
			for (int y{0}; y<h; ++y) {
				for (int x{0}; x<w; ++x) {
					if (cx==x && cy==y) continue;
					if (map[y][x] != '#') continue;

					const auto [vx, vxs] = signsep(x-cx);
					const auto [vy, vys] = signsep(y-cy);
					const int dist = vx*vx + vy*vy;

					const int g = gcd(vx, vy);

					const int nvx = vx / g * vxs;
					const int nvy = vy / g * vys;

					const auto dir = pii{ nvx, nvy };
					if (auto found = hits.find(dir); found==end(hits)) {
						hits[dir] = dist; }
					else {
						hits[dir] = std::min(hits[dir], dist); }}}
			if (hits.size() > p1) {
				p1 = hits.size();
				bestX = cx, bestY = cy; }}}

	cout << "p1: " << p1 << nl;

	const int cx = bestX;
	const int cy = bestY;

	umap<pii,  , pair_hash> hits{};
	int n = 0;
	while (1) {
		hits.clear();
		for (int y{0}; y<h; ++y) {
			for (int x{0}; x<w; ++x) {
				if (cx==x && cy==y) continue;
				if (map[y][x] != '#') continue;

				const auto [vx, vxs] = signsep(x-cx);
				const auto [vy, vys] = signsep(y-cy);
				const int dist = vx*vx + vy*vy;

				const int g = gcd(vx, vy);

				const int nvx = vx / g * vxs;
				const int nvy = vy / g * vys;

				const auto dir = pii{ nvx, nvy };
				if (auto found = hits.find(dir); found==end(hits)) {
					hits[dir] = dist; }
				else {
					hits[dir] = std::min(hits[dir], dist); }}}
		if (hits.empty()) {
			cout << "no more hits?\n";
			std::exit(1); }

		std::vector<std::tuple<float, int, int>> q;
		for (const auto& item : hits) {
			const auto& dir = item.first;
			auto deg = vec2deg(dir.first, dir.second);
			q.emplace_back(deg, dir.first, dir.second); }
		sort(begin(q), end(q));
		for (const auto& item : q) {
			++n;
			if (n != 200) continue;
			const auto [dir, dx, dy] = item;
			std::cout << dx << "," << dy;


	}



	
	return 0; }
