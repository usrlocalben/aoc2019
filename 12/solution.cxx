#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <optional>
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

std::vector<std::string> Split(const std::string& str, char ch) {
	std::vector<std::string> items;
	std::string src(str);
	auto nextmatch = src.find(ch);
	while (true) {
		auto item = src.substr(0, nextmatch);
		items.push_back(item);
		if (nextmatch == std::string::npos) { break; }
		src = src.substr(nextmatch + 1);
		nextmatch = src.find(ch); }
	return items; }


std::vector<int64_t> SplitNums(const std::string& str, char ch=',') {
	std::vector<int64_t> items;
	std::string src(str);
	auto nextmatch = src.find(ch);
	while (true) {
		auto item = src.substr(0, nextmatch);
		items.push_back(stol(item));
		if (nextmatch == std::string::npos) { break; }
		src = src.substr(nextmatch + 1);
		nextmatch = src.find(ch); }
	return items; }

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
		x(x_), y(x_), z(x_){}
	constexpr ivec3(int x_, int y_, int z_) :
		x(x_), y(y_), z(z_) {}
	auto operator+=(const ivec3& other) -> ivec3& { x+=other.x; y+=other.y; z+=other.z; return *this; }
	auto operator-=(const ivec3& other) -> ivec3& { x-=other.x; y-=other.y; z-=other.z; return *this; }
	auto operator*=(const ivec3& other) -> ivec3& { x*=other.x; y*=other.y; z*=other.z; return *this; } };

struct ivec4 {
	int x, y, z, w;
	ivec4() = default;
	constexpr explicit ivec4(int x_) :
		x(x_), y(x_), z(x_), w(x_) {}
	constexpr ivec4(int x_, int y_, int z_, int w_) :
		x(x_), y(y_), z(z_), w(w_) {}
	auto operator+=(const ivec4& other) -> ivec4& { x+=other.x; y+=other.y; z+=other.z; w+=other.w; return *this; }
	auto operator-=(const ivec4& other) -> ivec4& { x-=other.x; y-=other.y; z-=other.z; w-=other.w; return *this; }
	auto operator*=(const ivec4& other) -> ivec4& { x*=other.x; y*=other.y; z*=other.z; w*=other.w; return *this; } };

auto operator+(const ivec2& a, const ivec2& b) -> ivec2 { auto tmp = a; tmp += b; return tmp; }
auto operator-(const ivec2& a, const ivec2& b) -> ivec2 { auto tmp = a; tmp -= b; return tmp; }
auto operator*(const ivec2& a, const ivec2& b) -> ivec2 { auto tmp = a; tmp *= b; return tmp; }
auto operator+(const ivec2& a, int& b) -> ivec2 { auto tmp = a; tmp += ivec2{b,b}; return tmp; }
auto operator*(const ivec2& a, int& b) -> ivec2 { auto tmp = a; tmp *= ivec2{b,b}; return tmp; }
auto operator-(const ivec2& a) -> ivec2 { return ivec2{ -a.x, -a.y }; }
auto operator==(const ivec2& a, const ivec2& b) -> bool { return a.x==b.x && a.y==b.y; }
auto operator==(const ivec3& a, const ivec3& b) -> bool { return a.x==b.x && a.y==b.y && a.z==b.z; }
auto operator==(const ivec4& a, const ivec4& b) -> bool { return a.x==b.x && a.y==b.y && a.z==b.z && a.w==b.w; }
auto operator!=(const ivec2& a, const ivec2& b) -> bool { return a.x!=b.x || a.y!=b.y; }
auto operator!=(const ivec3& a, const ivec3& b) -> bool { return a.x!=b.x || a.y!=b.y || a.z!=b.z; }
auto operator!=(const ivec4& a, const ivec4& b) -> bool { return a.x!=b.x || a.y!=b.y || a.z!=b.z || a.w!=b.w; }


auto abs(ivec2 a) -> ivec2 { return ivec2{ std::abs(a.x), std::abs(a.y) }; }
auto abs(ivec3 a) -> ivec3 { return ivec3{ std::abs(a.x), std::abs(a.y), std::abs(a.z) }; }
auto hadd(ivec2 a) -> int { return a.x + a.y; }
auto hadd(ivec3 a) -> int { return a.x + a.y + a.z; }
auto mdist(ivec2 a, ivec2 b) -> int { return hadd(abs(a - b)); }
auto vmax(ivec2 a, ivec2 b) -> ivec2 { return ivec2{ std::max(a.x, b.x), std::max(a.y, b.y) }; }
auto vmin(ivec2 a, ivec2 b) -> ivec2 { return ivec2{ std::min(a.x, b.x), std::min(a.y, b.y) }; }
auto area(ivec2 a) -> int64_t { return a.x*a.y; }

auto operator<<(ostream& s, const ivec2& i) -> ostream& { return s << "(" << i.x << "," << i.y << ")"; }
auto operator<<(ostream& s, const ivec3& i) -> ostream& { return s << "(" << i.x << "," << i.y << "," << i.z << ")"; }
auto operator<<(ostream& s, const ivec4& i) -> ostream& { return s << "(" << i.x << "," << i.y << "," << i.z << "," << i.w << ")"; }

namespace std {
	template <>
	struct hash<ivec2> {
		auto operator()(const ivec2& a) const -> std::size_t {
			return hash<int>()(a.x) ^ hash<int>()(a.y); }};
	template <>
	struct hash<ivec3> {
		auto operator()(const ivec3& a) const -> std::size_t {
			return hash<int>()(a.x) ^ hash<int>()(a.y) ^ hash<int>()(a.z); }};
	template <>
	struct hash<ivec4> {
		auto operator()(const ivec4& a) const -> std::size_t {
			return hash<int>()(a.x) ^ hash<int>()(a.y) ^ hash<int>()(a.z) ^ hash<int>()(a.w); }}; }

auto LRUD(char ch) -> ivec2 {
	switch (ch) {
	case 'L': return ivec2{ -1, 0 };
	case 'R': return ivec2{  1, 0 };
	case 'U': return ivec2{  0,-1 };
	case 'D': return ivec2{  0, 1 }; }
	std::cerr << "wtf?" << std::endl;
	std::exit(1); }

auto comp3(int a, int b) -> int {
	if (a < b) return -1;
	if (a > b) return 1;
	return 0; }

auto vcomp(ivec3 a, ivec3 b) -> ivec3 {
	return ivec3{ comp3(a.x, b.x), comp3(a.y, b.y), comp3(a.z, b.z) }; }

int main() {
	std::string tmp;
	vector<ivec3> pos;
	vector<ivec3> vel;
	while (getline(std::cin, tmp)) {
		auto segs = Split(tmp, '=');
		int x = stoi(segs[1]);
		int y = stoi(segs[2]);
		int z = stoi(segs[3]);
		pos.emplace_back(ivec3{ x, y, z });
		vel.emplace_back(ivec3{ 0 }); }

	auto copyPos = pos;
	auto copyVel = vel;

	auto Tick = [&]() {
		for (int x=0; x<pos.size(); ++x) {
			for (int y=0; y<pos.size(); ++y) {
				if (x==y) continue;
				if (y<x) continue;

				auto change = vcomp(pos[x], pos[y]);
				vel[x] -= change;
				vel[y] += change; }}

		for (int x=0; x<pos.size(); ++x) {
			pos[x] += vel[x]; }};

	auto TotalEnergy = [&]() {
		int ax{0};
		for (int x=0; x<pos.size(); ++x) {
			ax += hadd(abs(pos[x])) * hadd(abs(vel[x])); }
		return ax; };

	for (int i=0; i<1000; ++i) {
		Tick(); }
	std::cout << "p1: " << TotalEnergy() << "\n";

	pos = copyPos;
	vel = copyVel;
	umap<pair<ivec4, ivec4>, int, pair_hash> seen;
	int iter;


	int64_t lens[3];

	pos = copyPos;
	vel = copyVel;
	seen.clear();
	iter = 0;
	while (1) {
		ivec4 pxxxx{ pos[0].x, pos[1].x, pos[2].x, pos[3].x };
		ivec4 vxxxx{ vel[0].x, vel[1].x, vel[2].x, vel[3].x };

		/*if (xxxx == hit) {
			cout << "hit at " << iter << nl; }*/
		if (auto found = seen.find({ pxxxx, vxxxx }); found != end(seen)) {
			cout << "xxxx repeat " << pxxxx << vxxxx << " [" << found->second << "..." << iter << "]\n";
			lens[0] = iter;
			break; 
			// std::exit(0);
			}
		else {
			seen[{pxxxx,vxxxx}] = iter; }
		Tick();
		++iter; }

	pos = copyPos;
	vel = copyVel;
	seen.clear();
	iter = 0;
	while (1) {
		ivec4 pxxxx{ pos[0].y, pos[1].y, pos[2].y, pos[3].y };
		ivec4 vxxxx{ vel[0].y, vel[1].y, vel[2].y, vel[3].y };

		/*if (xxxx == hit) {
			cout << "hit at " << iter << nl; }*/
		if (auto found = seen.find({ pxxxx, vxxxx }); found != end(seen)) {
			cout << "xxxx repeat " << pxxxx << vxxxx << " [" << found->second << "..." << iter << "]\n";
			lens[1] = iter;
			break; 
			// std::exit(0);
			}
		else {
			seen[{pxxxx,vxxxx}] = iter; }
		Tick();
		++iter; }

	pos = copyPos;
	vel = copyVel;
	seen.clear();
	iter = 0;
	while (1) {
		ivec4 pxxxx{ pos[0].z, pos[1].z, pos[2].z, pos[3].z };
		ivec4 vxxxx{ vel[0].z, vel[1].z, vel[2].z, vel[3].z };

		/*if (xxxx == hit) {
			cout << "hit at " << iter << nl; }*/
		if (auto found = seen.find({ pxxxx, vxxxx }); found != end(seen)) {
			cout << "xxxx repeat " << pxxxx << vxxxx << " [" << found->second << "..." << iter << "]\n";
			lens[2] = iter;
			break; 
			// std::exit(0);
			}
		else {
			seen[{pxxxx,vxxxx}] = iter; }
		Tick();
		++iter; }

	auto p2 = std::lcm(std::lcm(lens[0], lens[1]), lens[2]);
	cout << "p2: " << p2 << nl;

	return 0; }
