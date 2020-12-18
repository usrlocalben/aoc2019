#include "../lib.hpp"
using namespace std;
#include "../intcode.hpp"

array<Int2, 4> NSWE = {{
	// north    south        west         east
	Int2{0,1}, Int2{0,-1}, Int2{-1,0}, Int2{1,0} }};

array<string, 4> NSWEname = {{ "N", "S", "W", "E" }};


int main() {
	string tmp;
	getline(cin, tmp);

	auto mem = SplitNums(tmp);

	Int2 bMin{ 0, 0};
	Int2 bMax{ 0, 0};
	umap<Int2, char> map;
	Int2 pos{ 0, 0 };
	map[pos] = 1;
	optional<Int2> oxygenPos{};

	constexpr char T_UNKNOWN = 0;
	constexpr char T_NORMAL = 1;
	constexpr char T_WALL = 2;

	auto DumpMap = [&]() {
		for (int yy=bMax.y; yy>=bMin.y; --yy) {
			for (int xx=bMin.x; xx<=bMax.x; ++xx) {
				auto ppp = Int2{xx,yy};
				if (pos == ppp) {
					cerr << "D"; }
				else if (oxygenPos.has_value() && oxygenPos.value()==ppp) {
					cerr << "O"; }
				else {
					auto t = map[ppp];
					if (t == T_UNKNOWN) {
						cerr << "?"; }
					else if (t == T_NORMAL) {
						cerr << " "; }
					else {
						cerr << "#"; }}}
			cerr << "\n"; }};

	auto vm = IntCodeMachine(mem);

	uset<Int2> visited;
	umap<Int2, pair<Int2, char>> steps;
	deque<pair<Int2, int>> queue;
	vector<char> path;
	while (1) {
		// DumpMap();
		// cerr << "----------------------------\n";
				

		visited.clear();
		steps.clear();
		queue.clear();
		queue.push_back({ pos, 0 });
		optional<Int2> target{};
		while (!queue.empty()) {
			auto [p, dist] = queue.front(); queue.pop_front();
			if (visited.find(p) != end(visited)) continue;
			visited.insert(p);
			if (map[p] == 0) {
				target = p;
				break; }
			for (int di{0}; di<4; ++di) {
				auto pp = p + NSWE[di];
				if (map[pp] == T_WALL) continue;
				if (visited.find(pp) != end(visited)) continue;
				steps[pp] = { p, di };
				queue.push_back({ pp, dist+1 }); }}

		if (!target.has_value()) {
			// cerr << "search complete\n";
			break; }

		Int2 t = *target;
		// cerr << "target is " << t << nl;
		path.clear();
		while (t != pos) {
			// cerr << steps[t].first << ", " << NSWEname[steps[t].second] << "\n";
			path.push_back(steps[t].second);
			t = steps[t].first; }

		for (int pi=path.size()-1; pi>=0; --pi) {
			auto nextPos = pos + NSWE[path[pi]];
			bMin = vmin(bMin, nextPos);
			bMax = vmax(bMax, nextPos);
			vm.Recv(path[pi] + 1);  // NSWE 1234
			vm.Run();
			int status = vm.Out();
			switch (status) {
				case 0 :
					assert(pi==0);
					map[nextPos] = T_WALL;
					break;
				case 2 :
					oxygenPos = nextPos;
				case 1 :
					map[nextPos] = T_NORMAL;
					pos = nextPos;
					break; }}}

	if (0) {// && oxygenPos) {
		cerr << "oxygen at " << oxygenPos.value() << nl; }

	// DumpMap();

	visited.clear();
	queue.clear();
	queue.push_back({ Int2{0,0}, 0 });
	while (!queue.empty()) {
		auto [p, dist] = queue.front(); queue.pop_front();
		if (visited.find(p) != end(visited)) continue;
		visited.insert(p);
		if (p == oxygenPos.value()) {
			cout << dist << "\n";
			break; }
		for (int di{0}; di<4; ++di) {
			auto pp = p + NSWE[di];
			if (map[pp] == T_WALL) continue;
			if (visited.find(pp) != end(visited)) continue;
			steps[pp] = { p, di };
			queue.push_back({ pp, dist+1 }); }}

	visited.clear();
	queue.clear();
	queue.push_back({ oxygenPos.value(), 0 });
	int maxDist{-oo};
	while (!queue.empty()) {
		auto [p, dist] = queue.front(); queue.pop_front();
		if (visited.find(p) != end(visited)) continue;
		visited.insert(p);
		maxDist = max(maxDist, dist);
		for (int di{0}; di<4; ++di) {
			auto pp = p + NSWE[di];
			if (map[pp] == T_WALL) continue;
			if (visited.find(pp) != end(visited)) continue;
			steps[pp] = { p, di };
			queue.push_back({ pp, dist+1 }); }}

	cout << maxDist << nl;
	return 0; }
