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
			return hash<int8_t>()(a.x) ^ hash<int8_t>()(a.y); }};}

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

auto deKey(char ch) -> int32_t {
	if ('a' <= ch && ch <= 'z') {
		return 1L << (ch-'a'); }
	return 0; }

auto deDoor(char ch) -> int32_t {
	if ('A' <= ch && ch <= 'Z') {
		return 1L << (ch-'A'); }
	return 0; }

auto isWall(char ch) -> bool {
	return ch == '#'; }

auto cell(cvec2 coord) -> char {
	auto ofs = static_cast<int>(coord.y)*256 + coord.x;
	return map[ofs]; }
auto cell(cvec2 coord, char ch) -> char {
	auto ofs = static_cast<int>(coord.y)*256 + coord.x;
	return map[ofs] = ch; }

void outLinks(cvec2 coord, int keys, vector<cvec2>& buf) {
	buf.clear();
	for (int i=0; i<4; ++i) {
		auto next = coord + NSWE[i];
		char ch = cell(next);
		if (ch == '#') continue;
		if (auto dd = deDoor(ch); dd>0 && (dd&keys)==0) {
			// door, but no key
			continue; }
		buf.push_back(next); }}

auto DeadEndProbe(cvec2 coord, cvec2 prev, int depth=0) -> bool {
	// cerr << "d:" << depth << nl;
	array<cvec2, 4> links;
	int linkCnt{0};
	for (int i=0; i<4; ++i) {
		const auto next = coord + NSWE[i];
		if (next == prev) continue;
		if (cell(next) == '#') continue;
		links[linkCnt++] = next; }

	if (coord == cvec2{7,1}) {
		cerr << linkCnt << " ";
		for (int i=0; i<linkCnt; ++i) {
			cerr << links[i]; }
		cerr << nl; }

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


int main() {
	cvec2 home;
	int allKeys{0};
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

	cell(home, '#');
	for (int i=0; i<4; ++i) {
		cell(home + NSWE[i], '#'); }
	DeadEndProbe(home+NSWE[0]+NSWE[2], cvec2{-1,-1});
	DeadEndProbe(home+NSWE[0]+NSWE[3], cvec2{-1,-1});
	DeadEndProbe(home+NSWE[1]+NSWE[2], cvec2{-1,-1});
	DeadEndProbe(home+NSWE[1]+NSWE[3], cvec2{-1,-1});
	cell(home, '@');
	for (int i=0; i<4; ++i) {
		cell(home + NSWE[i], '.'); }


	/*for (int y{0}; y<81; ++y) {
		for (int x{0}; x<81; ++x) {
			const auto coord = cvec2(x, y);
			cout << cell(coord); }
		cout << "\n"; }*/

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
				state.keys |= deKey(cell(state.pos));
				if (state.keys == allKeys) {
					p1 = dist;
					break; }
				outLinks(state.pos, state.keys, outs);
				for (const auto& link : outs) {
					queue.push_back({ { link, state.keys }, dist+1 }); }}
			queue.pop_front(); }}
	if (p1) {
		cout << "p1: " << p1.value() << nl; }
	else {
		cerr << "p1 not found" << endl;
		exit(1); }

	cell(home, '#');
	for (int i=0; i<4; ++i) {
		cell(home + NSWE[i], '#'); }

	optional<int> p2{};
	{
		constexpr int NB{4};
		struct state {
			array<cvec2, NB> pos;
			int keys;
			auto operator==(const state& rhs) const -> bool {
				return    keys==rhs.keys
				       && pos[0]==rhs.pos[0]
				       && pos[1]==rhs.pos[1]
				       && pos[2]==rhs.pos[2]
				       && pos[3]==rhs.pos[3]; }};
		struct state_hash {
			size_t operator()(const state& s) const {
				return   std::hash<cvec2>()(s.pos[0])
					   ^ std::hash<cvec2>()(s.pos[1])
					   ^ std::hash<cvec2>()(s.pos[2])
					   ^ std::hash<cvec2>()(s.pos[3])
					   ^ std::hash<int>()(s.keys); }};
		state init{
			{ { home+NSWE[0]+NSWE[2],
				home+NSWE[0]+NSWE[3],
				home+NSWE[1]+NSWE[2],
				home+NSWE[1]+NSWE[3] } },
			0 };
		auto queue = deque<pair<state, int>>{};
		queue.emplace_back(init, 0);
		auto visited = uset<state, state_hash>{};
		array<vector<cvec2>, 4> outBufs{};
		while (!queue.empty()) {
			auto& [state, dist] = queue.front();
			cerr << "." << flush;
			if (visited.find(state) == end(visited)) {
				visited.insert(state);
				// gather keys, if any
				for (int i{0}; i<NB; ++i) {
					state.keys |= deKey(cell(state.pos[i])); }
				// check target
				if (state.keys == allKeys) {
					p2 = dist;
					break; }
				// movements
				outLinks(state.pos[0], state.keys, outBufs[0]);
				outLinks(state.pos[1], state.keys, outBufs[1]);
				outLinks(state.pos[2], state.keys, outBufs[2]);
				outLinks(state.pos[3], state.keys, outBufs[3]);
				if (outBufs[0].empty()) { outBufs[0].push_back(state.pos[0]); }
				if (outBufs[1].empty()) { outBufs[1].push_back(state.pos[1]); }
				if (outBufs[2].empty()) { outBufs[2].push_back(state.pos[2]); }
				if (outBufs[3].empty()) { outBufs[3].push_back(state.pos[3]); }
				for (int xa=0; xa<outBufs[0].size(); xa++) {
				for (int xb=0; xb<outBufs[1].size(); xb++) {
				for (int xc=0; xc<outBufs[2].size(); xc++) {
				for (int xd=0; xd<outBufs[3].size(); xd++) {
					auto ns = state;
					state.pos[0] = outBufs[0][xa];
					state.pos[1] = outBufs[1][xb];
					state.pos[2] = outBufs[2][xc];
					state.pos[3] = outBufs[3][xd];
					if (visited.find(ns) == end(visited)) {
						queue.emplace_back(ns, dist+1); }}}}}}
			queue.pop_front(); }}

	if (p2) {
		cout << "p2: " << p2.value() << nl; }
	else {
		cerr << "p2 not found" << endl;
		exit(1); }
	return 0; }
