#include <algorithm>
#include <array>
#include <cassert>
#include <deque>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#define uset unordered_set
#define umap unordered_map
using vi = std::vector<int>;
using namespace std;
const std::string nl{"\n"};
constexpr int oo{0x3f3f3f3f};

struct pair_hash {
	template <class T1, class T2>
	size_t operator()(const pair<T1, T2>& p) const {
		return std::hash<T1>()(p.first) ^ std::hash<T2>()(p.second); } };

#pragma pack(1)
template <typename T>
struct vec2 {
	T x, y;
	vec2() = default;
	constexpr explicit vec2(T a) : x(a), y(a) {}
	constexpr vec2(T x_, T y_) : x(x_), y(y_) {}
	auto operator+=(const vec2& rhs) -> vec2& { x+=rhs.x; y+=rhs.y; return *this; }
	auto operator-=(const vec2& rhs) -> vec2& { x-=rhs.x; y-=rhs.y; return *this; }
	auto operator*=(const vec2& rhs) -> vec2& { x*=rhs.x; y*=rhs.y; return *this; }

	auto operator-() const -> vec2 { return vec2{ -x, -y }; }
	auto operator+(vec2 rhs) const -> vec2 { vec2 t{*this}; t+= rhs; return t; }
	auto operator-(vec2 rhs) const -> vec2 { vec2 t{*this}; t-= rhs; return t; }
	auto operator*(vec2 rhs) const -> vec2 { vec2 t{*this}; t*= rhs; return t; }
	auto operator+(int rhs) const -> vec2 { vec2 t{*this}; t+= vec2{rhs}; return t; }
	auto operator-(int rhs) const -> vec2 { vec2 t{*this}; t-= vec2{rhs}; return t; }
	auto operator*(int rhs) const -> vec2 { vec2 t{*this}; t*= vec2{rhs}; return t; }
	auto operator==(vec2 rhs) const -> bool { return x==rhs.x && y==rhs.y; }
	auto operator!=(vec2 rhs) const -> bool { return x!=rhs.x || y!=rhs.y; } };
#pragma pack()

/*
template <typename T>
auto abs(vec2<T> a) -> vec2<T> { return vec2<T>{ std::abs(a.x), std::abs(a.y) }; }

template <typename T>
auto hadd(ivec2 a) -> int { return a.x + a.y; }

auto mdist(ivec2 a, ivec2 b) -> int { return hadd(abs(a - b)); }
auto vmax(ivec2 a, ivec2 b) -> ivec2 { return ivec2{ std::max(a.x, b.x), std::max(a.y, b.y) }; }
auto vmin(ivec2 a, ivec2 b) -> ivec2 { return ivec2{ std::min(a.x, b.x), std::min(a.y, b.y) }; }
auto area(ivec2 a) -> int64_t { return a.x*a.y; }
*/

//auto operator<<(ostream& s, const ivec2& i) -> ostream& { return s << "(" << i.x << "," << i.y << ")"; }

template <typename T>
auto operator<<(ostream& s, const vec2<T>& i) -> ostream& { return s << "(" << static_cast<int>(i.x) << "," << static_cast<int>(i.y) << ")"; }

using cvec2 = vec2<int8_t>;

// auto operator<<(ostream& s, const cvec2& i) -> ostream& { return s << "(" << i.x << "," << i.y << ")"; }

namespace std {
	template <>
	struct hash<cvec2> {
		auto operator()(const cvec2& a) const -> std::size_t {
			auto h = hash<char>()(a.x);
			auto n = hash<char>()(a.y);  n=(n<<6)|(n>>1);  h^=n;
			return h; }};}

auto LRUD(char ch) -> cvec2 {
	switch (ch) {
	case 'L': return cvec2{ -1, 0 };
	case 'R': return cvec2{  1, 0 };
	case 'U': return cvec2{  0,-1 };
	case 'D': return cvec2{  0, 1 }; }
	std::cerr << "wtf?" << std::endl;
	std::exit(1); }



// dir=1 = counter-clockwise
// dir=-1 = clockwise
auto rot90(cvec2 p, int t) -> cvec2 {
	return cvec2( -p.y*t, p.x*t );}

auto botrot(cvec2 dir, int value) -> cvec2 {
	return rot90(dir, value==0?1:-1); }

array<cvec2, 4> NSWE = {{
	// north    south        west         east
	cvec2{0,-1}, cvec2{0,1}, cvec2{-1,0}, cvec2{1,0} }};

array<string, 4> NSWEname = {{ "N", "S", "W", "E" }};

vector<char> map;
int allKeys{0};
cvec2 home;
bool quadsAreOpen{true};

auto CharToKey(char ch) -> int32_t {
	if ('a' <= ch && ch <= 'z') {
		return 1L << (ch-'a'); }
	return 0; }

auto CharToDoor(char ch) -> int32_t {
	if ('A' <= ch && ch <= 'Z') {
		return 1L << (ch-'A'); }
	return 0; }

auto cell(cvec2 coord) -> char {
	auto ofs = static_cast<int>(coord.y)*256 + coord.x;
	return map[ofs]; }

auto cell(cvec2 coord, char ch) -> char {
	auto ofs = static_cast<int>(coord.y)*256 + coord.x;
	return map[ofs] = ch; }

void OutLinks(cvec2 coord, int keys, vector<cvec2>& buf) {
	buf.clear();
	for (int i=0; i<4; ++i) {
		auto next = coord + NSWE[i];
		char ch = cell(next);
		if (ch == '#') continue;
		if (auto dd = CharToDoor(ch); dd>0 && (dd&keys)==0) {
			// door, but no key
			continue; }
		buf.push_back(next); }}

auto DeadEndProbe(cvec2 coord, cvec2 prev=cvec2{-1}, int depth=0) -> bool {
	// cerr << "d:" << depth << nl;
	array<cvec2, 4> links;
	int linkCnt{0};
	for (int i=0; i<4; ++i) {
		const auto next = coord + NSWE[i];
		if (next == prev) continue;
		if (cell(next) == '#') continue;
		links[linkCnt++] = next; }

	/*if (coord == cvec2{7,1}) {
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
	cvec2 coord;
	int keys;
	int dist; };

auto Look(cvec2 coord, int keys) -> const vector<Dest>& {
	static auto cache = umap<pair<cvec2, int>, vector<Dest>, pair_hash>{};
	static auto queue = deque<pair<cvec2, int>>{};
	static auto visited = uset<cvec2>{};
	static auto tmp = vector<cvec2>{};

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
auto BT(array<cvec2, 4> bots, int keys=0, int steps=0) -> int {
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
			best = std::min(best, rest); }}
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
	std::string tmp;
	map.clear();
	map.resize(81*256, '#');
	while (getline(std::cin, tmp)) {
		for (int x=0; x<tmp.size(); ++x) {
			const auto coord = cvec2(x, y);
			const auto ch = tmp[x];
			if ('a' <= ch && ch <= 'z') {
				allKeys |= (1L<<(ch-'a')); }
			else if (ch == '@') {
				home = coord; }
			map[y*256+x] = ch; }
		++y; }}

	const auto botCoords = array<cvec2, 4>{ {
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
			cvec2 pos;
			int keys;
			auto operator==(const state& rhs) const -> bool {
				return keys==rhs.keys && pos==rhs.pos; }};
		struct state_hash {
			size_t operator()(const state& s) const {
				return   std::hash<cvec2>()(s.pos)
					   ^ std::hash<int>()(s.keys); }};
		auto queue = deque<pair<state, int>>{};
		auto visited = uset<state, state_hash>{};
		queue.push_back({ state{ home, 0 }, 0 });
		auto outs = vector<cvec2>{};
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
	if (p1) {
		cout << "p1: " << p1.value() << nl; }
	else {
		cerr << "p1 not found" << endl;
		exit(1); }

	CloseQuads();
	int p2 = BT(botCoords);
	cout << "p2: " << p2 << nl;
	return 0; }
