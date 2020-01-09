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

struct ivec2 {
	int x, y;
	ivec2() = default;
	constexpr explicit ivec2(int x_) :
		x(x_), y(x_){}
	constexpr ivec2(int x_, int y_) :
		x(x_), y(y_) {}
	auto operator+=(const ivec2& other) -> ivec2& { x+=other.x; y+=other.y; return *this; }
	auto operator-=(const ivec2& other) -> ivec2& { x-=other.x; y-=other.y; return *this; }
	auto operator*=(const ivec2& other) -> ivec2& { x*=other.x; y*=other.y; return *this; } };

struct ivec3 {
	int x, y, z;
	ivec3() = default;
	constexpr explicit ivec3(int x_) :
		x(x_), y(x_), z(x_) {}
	constexpr ivec3(int x_, int y_, int z_) :
		x(x_), y(y_), z(z_) {}
	constexpr ivec3(ivec2 a, int z_) :
		x(a.x), y(a.y), z(z_) {}
	constexpr auto xy() const -> ivec2 {
		return ivec2{ x, y }; }
	auto operator+=(const ivec3& other) -> ivec3& { x+=other.x; y+=other.y; return *this; }
	auto operator-=(const ivec3& other) -> ivec3& { x-=other.x; y-=other.y; return *this; }
	auto operator*=(const ivec3& other) -> ivec3& { x*=other.x; y*=other.y; return *this; } };

auto operator+(const ivec2& a, const ivec2& b) -> ivec2 { auto tmp = a; tmp += b; return tmp; }
auto operator-(const ivec2& a, const ivec2& b) -> ivec2 { auto tmp = a; tmp -= b; return tmp; }
auto operator*(const ivec2& a, const ivec2& b) -> ivec2 { auto tmp = a; tmp *= b; return tmp; }
auto operator+(const ivec2& a, int& b) -> ivec2 { auto tmp = a; tmp += ivec2{b,b}; return tmp; }
auto operator*(const ivec2& a, int& b) -> ivec2 { auto tmp = a; tmp *= ivec2{b,b}; return tmp; }
auto operator-(const ivec2& a) -> ivec2 { return ivec2{ -a.x, -a.y }; }
auto operator==(const ivec2& a, const ivec2& b) -> bool { return a.x==b.x && a.y==b.y; }
auto operator==(const ivec3& a, const ivec3& b) -> bool { return a.x==b.x && a.y==b.y && a.z==b.z; }
auto operator!=(const ivec2& a, const ivec2& b) -> bool { return a.x!=b.x || a.y!=b.y; }


auto abs(ivec2 a) -> ivec2 { return ivec2{ std::abs(a.x), std::abs(a.y) }; }
auto hadd(ivec2 a) -> int { return a.x + a.y; }
auto mdist(ivec2 a, ivec2 b) -> int { return hadd(abs(a - b)); }
auto vmax(ivec2 a, ivec2 b) -> ivec2 { return ivec2{ std::max(a.x, b.x), std::max(a.y, b.y) }; }
auto vmin(ivec2 a, ivec2 b) -> ivec2 { return ivec2{ std::min(a.x, b.x), std::min(a.y, b.y) }; }
auto area(ivec2 a) -> int64_t { return a.x*a.y; }

auto operator<<(ostream& s, const ivec2& i) -> ostream& { return s << "(" << i.x << "," << i.y << ")"; }

namespace std {
	template <>
	struct hash<ivec2> {
		auto operator()(const ivec2& a) const -> std::size_t {
			return hash<int>()(a.x) ^ hash<int>()(a.y); }};
	template <>
	struct hash<ivec3> {
		auto operator()(const ivec3& a) const -> std::size_t {
			return hash<int>()(a.x) ^ (hash<int>()(a.y)<<1) ^ (hash<int>()(a.z)>>1); }};}

auto LRUD(char ch) -> ivec2 {
	switch (ch) {
	case 'L': return ivec2{ -1, 0 };
	case 'R': return ivec2{  1, 0 };
	case 'U': return ivec2{  0,-1 };
	case 'D': return ivec2{  0, 1 }; }
	std::cerr << "wtf?" << std::endl;
	std::exit(1); }



// dir=1 = counter-clockwise
// dir=-1 = clockwise
auto rot90(ivec2 p, int t) -> ivec2 {
	return { -p.y*t, p.x*t };}

auto botrot(ivec2 dir, int value) -> ivec2 {
	return rot90(dir, value==0?1:-1); }

array<ivec2, 4> NSWE = {{
	// north    south        west         east
	ivec2{0,-1}, ivec2{0,1}, ivec2{-1,0}, ivec2{1,0} }};

array<string, 4> NSWEname = {{ "N", "S", "W", "E" }};

vector<string> map;

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

auto cell(ivec2 coord) -> char {
	return map[coord.y][coord.x]; }
auto cell(ivec2 coord, char ch) -> char {
	return map[coord.y][coord.x] = ch; }
auto cell(ivec3 coord) -> char {
	return cell(coord.xy()); }

auto outLinks(ivec2 coord, int keys) -> const vector<ivec2>& {
	static vector<ivec2> buf;
	buf.clear();
	for (int i=0; i<4; ++i) {
		auto next = coord + NSWE[i];
		char ch = cell(next);
		if (ch == '#') continue;
		if (auto dd = deDoor(ch); dd>0 && (dd&keys)==0) {
			// door, but no key
			continue; }
		buf.push_back(next); }

	/*{
		char ch = cell(coord);
		if (auto dk=deKey(ch); dk>0 && (dk&coord.z)==0) {
			buf.push_back(coord.xy(), coord.z|dk); }}*/
	return buf; }


int main() {
	ivec2 home;
	std::string tmp;
	int allKeys{0};
	while (getline(std::cin, tmp)) {
		int y = map.size();
		for (int x=0; x<tmp.size(); ++x) {
			const ivec2 coord{ x, y };
			char ch = tmp[x];
			if ('a' <= ch && ch <= 'z') {
				allKeys |= (1L<<(ch-'a')); }
			else if (ch == '@') {
				home = coord; }}
		map.push_back(tmp); }

	optional<int> found{};

	{
		auto queue = deque<pair<ivec3, int>>{};
		auto visited = uset<ivec3>{};
		queue.push_back({ ivec3{ home, 0 }, 0 });
		while (!queue.empty()) {
			auto [pos, dist] = queue.front(); queue.pop_front();
			if (visited.find(pos) != end(visited)) continue;
			visited.insert(pos);
			pos.z |= deKey(cell(pos));
			if (pos.z == allKeys) {
				found = dist;
				break; }
			for (const auto& link : outLinks(pos.xy(), pos.z)) {
				queue.push_back({ ivec3{link,pos.z}, dist+1 }); }}}
	if (found) {
		cout << "p1: " << found.value() << nl; }
	else {
		cerr << "p1 not found" << endl;
		exit(1); }

	cell(home, '#');
	for (int i=0; i<4; ++i) {
		cell(home + NSWE[i], '#'); }

	for (const auto& ln : map) {
		cout << ln << nl; }

	found.reset();
	{
		constexpr int NB{4};
		struct state {
			array<ivec2, NB> pos;
			int keys;
			auto operator==(const state& rhs) const -> bool {
				return    keys==rhs.keys
				       && pos[0]==rhs.pos[0]
				       && pos[1]==rhs.pos[1]
				       && pos[2]==rhs.pos[2]
				       && pos[3]==rhs.pos[3]; }};
		struct state_hash {
			size_t operator()(const state& s) const {
				return   std::hash<ivec2>()(s.pos[0])
					   ^ std::hash<ivec2>()(s.pos[1])
					   ^ std::hash<ivec2>()(s.pos[2])
					   ^ std::hash<ivec2>()(s.pos[3])
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
		while (!queue.empty()) {
			auto& [state, dist] = queue.front();
			if (visited.find(state) == end(visited)) {
				visited.insert(state);
				// gather keys, if any
				for (int i{0}; i<NB; ++i) {
					state.keys |= deKey(cell(state.pos[i])); }
				// check target
				if (state.keys == allKeys) {
					found = dist;
					break; }
				// movements
				for (int i{0}; i<NB; ++i) {
					auto ns = state;
					for (auto link : outLinks(state.pos[i], state.keys)) {
						ns.pos[i] = link;
						if (visited.find(ns) == end(visited)) {
							queue.emplace_back(ns, dist+1); }}}}
			queue.pop_front(); }}

	if (found) {
		cout << "p2: " << found.value() << nl; }
	else {
		cerr << "p2 not found" << endl;
		exit(1); }
	return 0; }
