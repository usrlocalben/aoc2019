#include "../lib.hpp"
using namespace std;


array<Int2, 4> NSWE = {{
	// north    south        west         east
	Int2{0,-1}, Int2{0,1}, Int2{-1,0}, Int2{1,0} }};

array<string, 4> NSWEname = {{ "N", "S", "W", "E" }};

auto map = vector<char>{};
auto aa = Int2{-1};
auto zz = Int2{-1};
constexpr auto stride = int{128};
auto allKeys = int{0};
auto home = Int2{0};
auto quadsAreOpen = bool{true};
auto warp = umap<Int2, Int2>{};

struct Label {
	string name;
	Int2 coord; };
auto labels = vector<Label>{};


auto CharToKey(char ch) -> int32_t {
	if ('a' <= ch && ch <= 'z') {
		return 1L << (ch-'a'); }
	return 0; }

auto CharToDoor(char ch) -> int32_t {
	if ('A' <= ch && ch <= 'Z') {
		return 1L << (ch-'A'); }
	return 0; }

auto cell(Int2 coord) -> char {
	auto ofs = static_cast<int>(coord.y)*stride + coord.x;
	return map[ofs]; }

auto cell(Int2 coord, char ch) -> char {
	auto ofs = static_cast<int>(coord.y)*stride + coord.x;
	return map[ofs] = ch; }

void OutLinks(Int2 coord, int keys, vector<Int2>& buf) {
	buf.clear();
	for (int i=0; i<4; ++i) {
		auto next = coord + NSWE[i];
		char ch = cell(next);
		if (ch == '#') continue;
		if (auto dd = CharToDoor(ch); dd>0 && (dd&keys)==0) {
			// door, but no key
			continue; }
		buf.push_back(next); }}

auto Inner(Int2 p) -> bool {
	if (p.y == 26) return true;
	if (p.y == 82) return true;
	if (p.x == 26) return true;
	if (p.x == 80) return true;
	return false; }

auto DeadEndProbe(Int2 coord, Int2 prev=Int2{-1}, int depth=0) -> bool {
	// cerr << "d:" << depth << nl;
	array<Int2, 4> links;
	int linkCnt{0};
	for (int i=0; i<4; ++i) {
		const auto next = coord + NSWE[i];
		if (next == prev) continue;
		if (cell(next) != '.') continue;
		links[linkCnt++] = next; }

	/*if (coord == Int2{7,1}) {
		cerr << linkCnt << " ";
		for (int i=0; i<linkCnt; ++i) {
			cerr << links[i]; }
		cerr << nl; }*/

	const auto ch = cell(coord);
	if (linkCnt==0) {
		if (warp.find(coord) != end(warp) || coord==aa || coord==zz) {
			// dead end, but is a warp
			return false; }
		cell(coord, '#');
		return true; }
	else {
		bool allDead{true};
		for (int li=0; li<linkCnt; ++li) {
			allDead = DeadEndProbe(links[li], coord, depth+1) && allDead; }
		if (allDead) {
			if (warp.find(coord) != end(warp) || coord==aa || coord==zz) {
				// all dead ends from here, but this is a key
				return false; }
			cell(coord, '#');
			return true; }
		else {
			return false; }}}

int main() {
	{int y{0};
	string tmp;
	map.clear();
	map.resize(stride*stride, '#');
	while (getline(cin, tmp)) {
		for (int x=0; x<tmp.size(); ++x) {
			const auto ch = tmp[x];
			map[y*stride+x] = ch; }
		++y; }}

	for (int y=0; y<stride-1; ++y) {
		for (int x=0; x<stride-1; ++x) {
			const auto coord = Int2(x, y);
			const auto ch = cell(coord);
			if (!('A' <= ch && ch <= 'Z')) continue;
			const auto chRight = cell(coord + Int2{1,0});
			const auto chDown  = cell(coord + Int2{0,1});
			if ('A' <= chRight && chRight <= 'Z') {
				// horizontal label
				auto rightPt = coord + Int2{2,0};
				auto leftPt = coord + Int2{-1,0};
				Label l;
				l.name.push_back(ch);
				l.name.push_back(chRight);
				l.coord = cell(rightPt)=='.' ? rightPt : leftPt;
				labels.push_back(l); }
			else if ('A' <= chDown && chDown <= 'Z') {
				// vertical label
				auto upPt = coord + Int2{0,-1};
				auto downPt = coord + Int2{0,2};
				Label l;
				l.name.push_back(ch);
				l.name.push_back(chDown);
				l.coord = cell(downPt)=='.' ? downPt : upPt;
				labels.push_back(l); }}}

	for (const auto& l : labels) {
		if (l.name == "AA") {
			aa = l.coord; }
		else if (l.name == "ZZ") {
			zz = l.coord; }
		else {
			for (const auto& ol : labels) {
				if (ol.name == l.name && ol.coord!=l.coord) {
					warp[l.coord] = ol.coord; }}}}


	// cerr << "AA: " << aa << nl;
	// cerr << "ZZ: " << zz << nl;
	// cerr << "warps: " << warp.size() << nl;

	// optimize map
	for (const auto l : labels) {
		DeadEndProbe(l.coord); }

	/*for (int y=0; y<128; ++y) {
		for (int x=0; x<128; ++x) {
			cerr << cell(Int2(x,y)); }
		cerr << "\n"; }*/

	// exit(1);

	optional<int> p1{};
	{
		auto queue = deque<pair<Int2, int>>{};
		auto visited = uset<Int2>{};
		queue.push_back({ aa, 0 });
		while (!queue.empty()) {
			auto [p, d] = queue.front(); queue.pop_front();
			if (visited.find(p) != end(visited)) continue;
			visited.insert(p);
			if (p == zz) {
				p1 = d;
				break; }
			for (int di{0}; di<4; ++di) {
				auto np = p + NSWE[di];
				if (cell(np) != '.') continue;
				if (visited.find(np) != end(visited)) continue;
				queue.push_back({ np, d+1 }); }
			if (auto found = warp.find(p); found!=end(warp)) {
				auto np = found->second;
				if (visited.find(np) == end(visited)) {
					queue.push_back({ np, d+1 }); }}}}
	if (p1) {
		cout << p1.value() << nl; }
	else {
		cerr << "p1: not found!\n"; }

	optional<int> p2{};
	{
		auto queue = deque<pair<pair<Int2, int>, int>>{};
		auto visited = uset<pair<Int2, int>, pair_hash>{};
		queue.push_back({ { aa, 0 }, 0 });
		while (!queue.empty()) {
			auto [tmp, d] = queue.front(); queue.pop_front();
			auto [p, l] = tmp;
			if (visited.find({ p, l }) != end(visited)) continue;
			visited.insert({p, l});
			if (p == zz && l==0) {
				p2 = d;
				break; }
			for (int di{0}; di<4; ++di) {
				auto np = p + NSWE[di];
				if (cell(np) != '.') continue;
				if (visited.find({ np, l }) != end(visited)) continue;
				queue.push_back({ { np, l }, d+1 }); }
			if (auto found = warp.find(p); found!=end(warp)) {
				auto np = found->second;
				int nl = Inner(p) ? l+1 : l-1;
				if (nl >= 0) {
					queue.push_back({ { np, nl }, d+1 }); }}}}

	if (p2) {
		cout << p2.value() << nl; }
	else {
		cerr << "p2 not found!\n"; }
	exit(0);

	return 0; }
