#include <algorithm>
#include <array>
#include <deque>
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

std::vector<std::string> Split(std::string text, const std::string& delim) {
	std::vector<std::string> items;
	auto nextmatch = text.find(delim);
	while (true) {
		auto item = text.substr(0, nextmatch);
		items.push_back(item);
		if (nextmatch == std::string::npos) { break; }
		text = text.substr(nextmatch + delim.size());
		nextmatch = text.find(delim); }
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

auto operator+(const ivec2& a, const ivec2& b) -> ivec2 { auto tmp = a; tmp += b; return tmp; }
auto operator-(const ivec2& a, const ivec2& b) -> ivec2 { auto tmp = a; tmp -= b; return tmp; }
auto operator*(const ivec2& a, const ivec2& b) -> ivec2 { auto tmp = a; tmp *= b; return tmp; }
auto operator+(const ivec2& a, int& b) -> ivec2 { auto tmp = a; tmp += ivec2{b,b}; return tmp; }
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



// dir=1 = counter-clockwise
// dir=-1 = clockwise
auto rot90(ivec2 p, int t) -> ivec2 {
	return { -p.y*t, p.x*t };}

auto botrot(ivec2 dir, int value) -> ivec2 {
	return rot90(dir, value==0?1:-1); }


int seq{0};
umap<string, int> nameId;
umap<int, string> invNameId;

auto UpsertName(string name) -> int {
	if (auto found = nameId.find(name); found!= end(nameId)) {
		return found->second; }
	int sku = seq++;
	nameId[name] = sku;
	invNameId[sku] = name;
	return sku; }

struct Batch {
	int qty;
	int sku; };

auto operator<<(ostream& s, Batch b) -> ostream& {
	return s << b.qty << "_" << invNameId[b.sku]; }

struct Recipe {
	array<int, 64> in{};
	int sku;
	int yield;
	int64_t needed{0};
	int indegree{0}; };

auto operator<<(ostream& s, Recipe r) -> ostream& {
	s << "<Recipe in=[";
	bool first{true};
	for (int i=0; i<64; ++i) {
		if (r.in[i] == 0) continue;
		if (!first) {
			s << ", "; }
		s << Batch{ r.in[i], i };
		first = false; }
	s << "], out=" << Batch{ r.yield, r.sku } << ">";
	return s; }

std::vector<Recipe> recipes;

uset<int> visited;
void FillIndegree(int n, int d=0) {
	if (d==0) {
		visited.clear(); }
	if (visited.find(n) != end(visited)) {
		return; }
	visited.insert(n);
	auto& recipe = recipes[n];
	for (int i=0; i<64; ++i) {
		if (recipe.in[i] > 0) {
			recipes[i].indegree++;
			FillIndegree(i, d+1); }}}

class Bisector {
	int64_t upper_;
	int64_t lower_;
	int64_t target_;
public:
	Bisector(int64_t u, int64_t t) :
		lower_(0),
		upper_(u),
		target_(t) {}

	auto Pos() const -> int64_t {
		return (lower_ + upper_) / 2; }

	auto Result(int64_t v) {
		// cout << "[" << lower_ << ", " << upper_ << "] " << v << nl;
		if (v < target_) {
			lower_ = Pos(); }
		else if (v > target_) {
			upper_ = Pos(); }}

	auto End() const -> bool {
		return upper_-lower_ <= 1; } };







int main() {
	std::string tmp;
	std::vector<Recipe> rax;
	while (getline(std::cin, tmp)) {
		auto delim = string{" => "};
		auto pos = tmp.find(delim);
		auto left = tmp.substr(0, pos);
		auto right = tmp.substr(pos+delim.size());

		auto segs = Split(left, ", ");

		auto deserializeBatch = [](auto text) {
			auto numname = Split(text, ' ');
			Batch b;
			b.qty = stoi(numname[0]);
			b.sku = UpsertName(numname[1]);
			return b; };

		auto r = std::accumulate(begin(segs), end(segs), Recipe{}, [&](auto ax, auto cur) {
								 auto b = deserializeBatch(cur);
								 ax.in[b.sku] = b.qty;
								 return ax; });

		auto b = deserializeBatch(right);
		r.sku = b.sku;
		r.yield = b.qty;
		// cout << r << nl;
		rax.push_back(r); }

	auto eax = Recipe{};
	eax.sku = nameId["ORE"];
	eax.yield = 1;
	rax.push_back(eax);


	recipes.resize(rax.size());
	for (const auto& r : rax) {
		recipes[r.sku] = r; }


	const int SKU_FUEL = nameId["FUEL"];


	/*for (const auto& r : recipes) {
		cout << r.indegree << ": " << r << nl; }*/

	std::deque<int> queue;
	auto FuelCostInOre = [&](int64_t wanted) {
		for (auto& r : recipes) {
			r.indegree = 0;
			r.needed = 0; }
		FillIndegree(SKU_FUEL);
		visited.clear();

		recipes[SKU_FUEL].needed = wanted;
		queue.push_back(SKU_FUEL);
		while (!queue.empty()) {
			const auto here = queue.front(); queue.pop_front();
			if (visited.find(here) != end(visited)) continue;
			auto& r = recipes[here];
			if (--r.indegree > 0) continue;  // not ready yet
			// cout << "visiting " << invNameId[here] << "\n";

			int64_t numReactions = r.needed / r.yield;
			if (r.needed % r.yield > 0) {
				numReactions++; }
			// cout << "need: " << r.needed << ", yield is " << r.yield << " -- reactions: " << numReactions << nl;

			for (int i=0; i<64; ++i) {
				if (r.in[i] == 0) continue;
				recipes[i].needed += numReactions * r.in[i];
				queue.push_back(i); }}
		return recipes[nameId["ORE"]].needed; };

	auto p1 = FuelCostInOre(1);
	cout << "p1: " << p1 << nl;


	Bisector bisector{ 10000000, 1000000000000 };
	while (!bisector.End()) {
		auto x = bisector.Pos();
		auto ore = FuelCostInOre(x);
		bisector.Result(ore); }

	cout << "p2: " << bisector.Pos() << nl;

	/*for (const auto& r : recipes) {
		cout << r.indegree << " " << r.needed << " " << r << nl; }*/



	return 0; }
