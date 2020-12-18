#include "../lib.hpp"
using namespace std;

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
		auto d = atan(dy/float(dx))+(kPi/2);
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

	struct hit {
		int dist;
		int x,y; };

	umap<pii, hit, pair_hash> hits{};

	auto probe = [&](int cx, int cy) {
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
					hits[dir] = { dist, x, y }; }
				else {
					if (found->second.dist > dist) {
						hits[dir] = { dist, x, y }; }}}}};

	int p1{0};
	int bestX, bestY;
	for (int cy{0}; cy<h; ++cy) {
		for (int cx{0}; cx<w; ++cx) {
			if (map[cy][cx] == '#') {
				probe(cx, cy);
				if (hits.size() > p1) {
					p1 = hits.size();
					bestX = cx;
					bestY = cy; }}}}

	cout << p1 << nl;

	/*
	for (int y=0; y<10; ++y) {
		for (int x=0; x<10; ++x) {
			int dx = x-5;
			int dy = y-5;
			auto value = vec2deg(dx, dy);
			cout << value << " "; }
		cout << "\n"; }
	exit(1);
	*/

	map[bestY][bestX] = 'B';

	int n{0};
	vector<hit> hits2;
	int p2{-1};
	while (p2==-1) {
		probe(bestX, bestY);
		if (hits.empty()) {
			cerr << "no more hits?\n";
			exit(1); }

		hits2.clear();
		for (const auto& item : hits) {
			hits2.push_back(item.second); }

		sort(ALL(hits2), [=](auto a, auto b) {
			 return vec2deg(a.x-bestX, a.y-bestY) < vec2deg(b.x-bestX, b.y-bestY); });
		for (const auto& hit : hits2) {
			++n;
			map[hit.y][hit.x] = 'o';

/*	for (int y=0; y<h; ++y) {
		for (int x=0; x<w; ++x) {
			cout << map[y][x]; }
		cout << "\n"; }
	cout << "-------------------------------\n";*/

			if (n == 200) {
				p2 = hit.x*100+hit.y;
				break; }}}

	cout << p2 << nl;
	return 0; }
