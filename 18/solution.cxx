#include "../lib.hpp"
using namespace std;

array<Int2, 4> NSWE = {{
	// north    south        west         east
	Int2{0,-1}, Int2{0,1}, Int2{-1,0}, Int2{1,0} }};

array<string, 4> NSWEname = {{ "N", "S", "W", "E" }};

vector<char> map;
int allKeys{0};
Int2 home;
bool quadsAreOpen{true};

auto CharToKey(char ch) -> int32_t {
	if ('a' <= ch && ch <= 'z') {
		return 1L << (ch-'a'); }
	return 0; }

auto CharToDoor(char ch) -> int32_t {
	if ('A' <= ch && ch <= 'Z') {
		return 1L << (ch-'A'); }
	return 0; }

auto cell(Int2 coord) -> char {
	auto ofs = static_cast<int>(coord.y)*256 + coord.x;
	return map[ofs]; }

auto cell(Int2 coord, char ch) -> char {
	auto ofs = static_cast<int>(coord.y)*256 + coord.x;
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

auto DeadEndProbe(Int2 coord, Int2 prev=Int2{-1}, int depth=0) -> bool {
	// cerr << "d:" << depth << nl;
	array<Int2, 4> links;
	int linkCnt{0};
	for (int i=0; i<4; ++i) {
		const auto next = coord + NSWE[i];
		if (next == prev) continue;
		if (cell(next) == '#') continue;
		links[linkCnt++] = next; }

	/*if (coord == Int2{7,1}) {
		cerr << linkCnt << " ";
		for (int i=0; i<linkCnt; ++i) {
			cerr << links[i]; }
		cerr << nl; }*/

	const auto ch = cell(coord);
	if (linkCnt==0) {
		if ('a' <= ch && ch <= 'z') {
			// dead end, but holds a key
			return false; }
		cell(coord, '#');
		return true; }
	else {
		bool allDead{true};
		for (int li=0; li<linkCnt; ++li) {
			allDead = DeadEndProbe(links[li], coord, depth+1) && allDead; }
		if (allDead) {
			if ('a' <= ch && ch <= 'z') {
				// all dead ends from here, but this is a key
				return false; }
			cell(coord, '#');
			return true; }
		else {
			return false; }}}

struct Dest {
	Int2 coord;
	int keys;
	int dist; };

auto Look(Int2 coord, int keys) -> const vector<Dest>& {
	static auto cache = umap<pair<Int2, int>, vector<Dest>, pair_hash>{};
	static auto queue = deque<pair<Int2, int>>{};
	static auto visited = uset<Int2>{};
	static auto tmp = vector<Int2>{};

	if (auto found = cache.find({ coord, keys }); found!=end(cache)) {
		return found->second; }

	auto& out = cache[{ coord, keys }];
	assert(out.size() == 0);

	visited.clear();
	queue.clear();
	queue.push_back({ coord, 0 });
	while (!queue.empty()) {
		auto [pos, dist] = queue.front(); queue.pop_front();
		if (visited.find(pos) != end(visited)) continue;
		visited.insert(pos);

		const auto ch = cell(pos);
		const auto kb = CharToKey(ch);
		if (kb && (keys&kb)==0) {
			out.push_back({ pos, keys|kb, dist }); }
		else {
			OutLinks(pos, keys, tmp);
			for (auto np : tmp) {
				if (visited.find(np) != end(visited)) continue;
				queue.push_back({ np, dist+1 }); }}}

	return out; }
		
int btBest=oo;
auto BT(array<Int2, 4> bots, int keys=0, int steps=0) -> int {
	if (keys == allKeys) {
		if (steps < btBest) {
			btBest = steps; }
		return steps; }
	else {
		if (steps > btBest) {
			return oo; }}

	int best{oo};
	for (int bi=0; bi<4; ++bi) {
		const auto& dests = Look(bots[bi], keys);
		for (const auto& dest : dests) {
			auto newBots = bots; newBots[bi] = dest.coord;
			int rest = BT(newBots, dest.keys, steps+dest.dist);
			best = min(best, rest); }}
	return best; }

void CloseQuads() {
	assert(quadsAreOpen);
	cell(home, '#');
	for (int i=0; i<4; ++i) {
		cell(home + NSWE[i], '#'); }
	quadsAreOpen = false; }

void OpenQuads() {
	assert(!quadsAreOpen);
	cell(home, '@');
	for (int i=0; i<4; ++i) {
		cell(home + NSWE[i], '.'); }
	quadsAreOpen = true; }

int main() {
	{int y{0};
	string tmp;
	map.clear();
	map.resize(81*256, '#');
	while (getline(cin, tmp)) {
		for (int x=0; x<tmp.size(); ++x) {
			const auto coord = Int2(x, y);
			const auto ch = tmp[x];
			if ('a' <= ch && ch <= 'z') {
				allKeys |= (1L<<(ch-'a')); }
			else if (ch == '@') {
				home = coord; }
			map[y*256+x] = ch; }
		++y; }}

	const auto botCoords = array<Int2, 4>{ {
		home + NSWE[0]+NSWE[2],
		home + NSWE[0]+NSWE[3],
		home + NSWE[1]+NSWE[2],
		home + NSWE[1]+NSWE[3], } };

	// optimize a bit by filling in dead ends
	CloseQuads();
	for (int bi{0}; bi<4; ++bi) {
		DeadEndProbe(botCoords[bi]); }
	OpenQuads();

	optional<int> p1{};
	{
		struct state {
			Int2 pos;
			int keys;
			auto operator==(const state& rhs) const -> bool {
				return keys==rhs.keys && pos==rhs.pos; }};
		struct state_hash {
			size_t operator()(const state& s) const {
				return   std::hash<Int2>()(s.pos)
					   ^ std::hash<int>()(s.keys); }};
		auto queue = deque<pair<state, int>>{};
		auto visited = uset<state, state_hash>{};
		queue.push_back({ state{ home, 0 }, 0 });
		auto outs = vector<Int2>{};
		outs.reserve(4);
		while (!queue.empty()) {
			auto& [state, dist] = queue.front();
			if (visited.find(state) == end(visited)) {
				visited.insert(state);
				state.keys |= CharToKey(cell(state.pos));
				if (state.keys == allKeys) {
					p1 = dist;
					break; }
				OutLinks(state.pos, state.keys, outs);
				for (const auto& link : outs) {
					queue.push_back({ { link, state.keys }, dist+1 }); }}
			queue.pop_front(); }}
	cout << p1.value() << nl;

	CloseQuads();
	int p2 = BT(botCoords);
	cout << p2 << nl;
	return 0; }
