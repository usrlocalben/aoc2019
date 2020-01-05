#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#define uset std::unordered_set
#define umap std::unordered_map
using vi = std::vector<int>;
using namespace std;
constexpr int INF{0x3f3f3f3f};
const std::string nl{"\n"};

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


std::vector<int> SplitNum(const std::string& str, char ch=',') {
	std::vector<int> items;
	std::string src(str);
	auto nextmatch = src.find(ch);
	while (true) {
		auto item = src.substr(0, nextmatch);
		items.push_back(stoi(item));
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

auto operator+(const ivec2& a, const ivec2& b) -> ivec2 { auto tmp = a; tmp += b; return tmp; }
auto operator-(const ivec2& a, const ivec2& b) -> ivec2 { auto tmp = a; tmp -= b; return tmp; }
auto operator*(const ivec2& a, const ivec2& b) -> ivec2 { auto tmp = a; tmp *= b; return tmp; }
auto operator*(const ivec2& a, int& b) -> ivec2 { auto tmp = a; tmp *= ivec2{b,b}; return tmp; }
auto operator-(const ivec2& a) -> ivec2 { return ivec2{ -a.x, -a.y }; }
auto operator==(const ivec2& a, const ivec2& b) -> bool { return a.x==b.x && a.y==b.y; }
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
			return hash<int>()(a.x) ^ hash<int>()(a.y); }};}

auto LRUD(char ch) -> ivec2 {
	switch (ch) {
	case 'L': return ivec2{ -1, 0 };
	case 'R': return ivec2{  1, 0 };
	case 'U': return ivec2{  0,-1 };
	case 'D': return ivec2{  0, 1 }; }
	std::cerr << "wtf?" << std::endl;
	std::exit(1); }

/*
	ivec2 bmin{INF, INF};
	ivec2 bmax{-INF, -INF};
	{ivec2 c{0,0};
	for (const auto& dir : dirs1) {
		auto dv = LRUD(dir[0]);
		int  dist = stoi(dir.substr(1));
		c += dv * dist;
		bmin = vmin(bmin, c);
		bmax = vmax(bmax, c); }}
*/


uset<int> hits;
int lower, upper;

void R(int num, int pd, bool hasRepeating, int width=0) {
	if (width==6) {
		if (hasRepeating) {
			if (lower <= num && num <= upper) {
				std::cout << num << "\n";
				hits.insert(num); }}}
	else if (width==0) {
		for (int d=0; d<=9; ++d) {
			R(d, d, false, width+1); }}
	else {
		for (int d=pd; d<=9; ++d) {
			R(num*10+d, d, hasRepeating||(d==pd), width+1); }}}


	

int main() {
	std::string tmp;
	getline(std::cin, tmp);
	const auto nums = SplitNum(tmp, '-');
	lower = nums[0];
	upper = nums[1];

	R(0, 0, false, 0);
	auto p1{hits.size()};

	int p2{0};
	auto cnts = std::array<int, 10>{};
	for (auto hit : hits) {
		std::fill(begin(cnts), end(cnts), 0);
		auto original = hit;
		for (int i=0; i<6; ++i) {
			int d = hit%10;
			hit/=10;
			cnts[d]++; }
		for (int i=0; i<10; ++i) {
			if (cnts[i] == 2) {
				++p2;
				break; }}}


	std::cout << "p1: " << p1 << nl;
	std::cout << "p2: " << p2 << nl;

	return 0; }
