#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <deque>
#include <iostream>
#include <numeric>
#include <string>
#include <string_view>
#include <tuple>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#define uset unordered_set
#define umap unordered_map
using vs = std::vector<std::string>;
using vi = std::vector<int>;
using pii = std::pair<int, int>;
using psi = std::pair<std::string, int>;
using vii = std::vector<pii>;
const int oo = 0x3f3f3f3f;
const std::string nl{"\n"};
#define ALL(x) begin(x), end(x)
#define B(x) begin(x)
#define E(x) end(x)
#define fi first
#define se second
constexpr auto kPi = 3.14169265359F;

template <typename T>
auto len(const T& c) -> int { return static_cast<int>(c.size()); }


struct pair_hash {
	template <class T1, class T2>
	size_t operator()(const std::pair<T1, T2>& p) const {
		return std::hash<T1>()(p.first) ^ std::hash<T2>()(p.second); } };

auto comp3(int a, int b) -> int {
	if (a < b) return -1;
	if (a > b) return 1;
	return 0; }

struct Int2 {
	int x, y;
	Int2() = default;
	constexpr explicit Int2(int x_) :
		x(x_), y(x_){}
	constexpr Int2(int x_, int y_) :
		x(x_), y(y_) {}
	auto operator+=(Int2 other) -> Int2& { x+=other.x; y+=other.y; return *this; }
	auto operator-=(Int2 other) -> Int2& { x-=other.x; y-=other.y; return *this; }
	auto operator*=(Int2 other) -> Int2& { x*=other.x; y*=other.y; return *this; }

	auto key() const -> int64_t {
		return (int64_t(x)<<32)|uint32_t(y); }

	static
	auto fromkey(int64_t a) -> Int2 {
		return { int(a>>32), int(a&0xffffffff) }; } };

struct Int3 {
	int x, y, z;
	Int3() = default;
	constexpr explicit Int3(int x_) :
		x(x_), y(x_), z(x_){}
	constexpr Int3(int x_, int y_, int z_) :
		x(x_), y(y_), z(z_) {}
	auto operator+=(Int3 other) -> Int3& { x+=other.x; y+=other.y; z+=other.z; return *this; }
	auto operator-=(Int3 other) -> Int3& { x-=other.x; y-=other.y; z-=other.z; return *this; }
	auto operator*=(Int3 other) -> Int3& { x*=other.x; y*=other.y; z*=other.z; return *this; }

	auto xy() const -> Int2 { return { x, y }; }};

struct Int4 {
	int x, y, z, w;
	Int4() = default;
	constexpr explicit Int4(int x_) :
		x(x_), y(x_), z(x_), w(x_){}
	constexpr Int4(int x_, int y_, int z_, int w_) :
		x(x_), y(y_), z(z_), w(w_) {}
	auto operator+=(Int4 other) -> Int4& { x+=other.x; y+=other.y; z+=other.z; w+=other.w; return *this; }
	auto operator-=(Int4 other) -> Int4& { x-=other.x; y-=other.y; z-=other.z; w-=other.w; return *this; }
	auto operator*=(Int4 other) -> Int4& { x*=other.x; y*=other.y; z*=other.z; w*=other.w; return *this; }};

auto operator- (Int2 a) -> Int2 { return { -a.x, -a.y }; }
auto operator+ (Int2 a, Int2 b) -> Int2 { a += b; return a; }
auto operator- (Int2 a, Int2 b) -> Int2 { a -= b; return a; }
auto operator* (Int2 a, Int2 b) -> Int2 { a *= b; return a; }
auto operator* (Int2 a, int b) -> Int2 { a *= Int2{b,b}; return a; }

auto operator==(Int2 a, Int2 b) -> bool { return a.x==b.x && a.y==b.y; }
auto operator==(Int3 a, Int3 b) -> bool { return a.x==b.x && a.y==b.y && a.z==b.z; }
auto operator==(Int4 a, Int4 b) -> bool { return a.x==b.x && a.y==b.y && a.z==b.z && a.w==b.w; }
auto operator!=(Int2 a, Int2 b) -> bool { return a.x!=b.x || a.y!=b.y; }
auto operator!=(Int3 a, Int3 b) -> bool { return a.x!=b.x || a.y!=b.y || a.z!=b.z; }
auto operator!=(Int4 a, Int4 b) -> bool { return a.x!=b.x || a.y!=b.y || a.z!=b.z || a.w!=b.w; }

auto abs(Int2 a) -> Int2 { return { std::abs(a.x), std::abs(a.y) }; }
auto abs(Int3 a) -> Int3 { return { std::abs(a.x), std::abs(a.y), std::abs(a.z) }; }
auto hadd(Int2 a) -> int { return a.x + a.y; }
auto hadd(Int3 a) -> int { return a.x + a.y + a.z; }
auto mdist(Int2 a, Int2 b) -> int { return hadd(abs(a - b)); }
auto vmax(Int2 a, Int2 b) -> Int2 { return { std::max(a.x, b.x), std::max(a.y, b.y) }; }
auto vmin(Int2 a, Int2 b) -> Int2 { return { std::min(a.x, b.x), std::min(a.y, b.y) }; }
auto vcomp(Int3 a, Int3 b) -> Int3{ return { comp3(a.x,b.x), comp3(a.y,b.y), comp3(a.z,b.z) }; }
auto area(Int2 a) -> int64_t { return a.x*a.y; }

namespace std {
	template <>
	struct hash<Int2> {
		auto operator()(Int2 a) const -> std::size_t {
			return hash<int>()(a.x) ^ hash<int>()(a.y); }};
	template <>
	struct hash<Int3> {
		auto operator()(Int3 a) const -> std::size_t {
			return hash<int>()(a.x) ^ hash<int>()(a.y) ^ hash<int>()(a.z); }};
	template <>
	struct hash<Int4> {
		auto operator()(Int4 a) const -> std::size_t {
			return hash<int>()(a.x) ^ hash<int>()(a.y) ^ hash<int>()(a.z) ^ hash<int>()(a.w); }}; }

auto operator<<(std::ostream& s, Int2 item) -> decltype(s) {
	return s << "<Int2 " << item.x << ", " << item.y << ">"; }
auto operator<<(std::ostream& s, Int3 item) -> decltype(s) {
	return s << "<Int3 " << item.x << ", " << item.y << ", " << item.z << ">"; }

void Split(const std::string& str, char delim, std::vector<std::string>& out) {
	std::string src(str);
	auto nextmatch = src.find(delim);
	std::size_t cnt{0};
	while (true) {
		auto item = src.substr(0, nextmatch);
		if (cnt < out.size()) {
			out[cnt].assign(src.substr(0, nextmatch)); }
		else {
			out.emplace_back().assign(src.substr(0, nextmatch)); }
		++cnt;
		if (nextmatch == std::string::npos) { break; }
		src = src.substr(nextmatch + 1);
		nextmatch = src.find(delim); }
	out.resize(cnt); }

auto Split(const std::string& str, char ch) -> std::vector<std::string> {
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

auto Split(std::string text, const std::string& delim) -> std::vector<std::string> {
	std::vector<std::string> items;
	auto nextmatch = text.find(delim);
	while (true) {
		auto item = text.substr(0, nextmatch);
		items.push_back(item);
		if (nextmatch == std::string::npos) { break; }
		text = text.substr(nextmatch + delim.size());
		nextmatch = text.find(delim); }
	return items; }

auto SplitNums(const std::string& str, char ch=',') -> std::vector<int64_t> {
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



struct CharMap {
	std::vector<char> data;
	const int stride;
	const int height;

	auto at(Int2 coord) const -> char {
		assert(0 <= coord.y && coord.y <= height);
		assert(0 <= coord.x && coord.x <= stride);
		return data[coord.y*stride + coord.x]; } };


auto LoadCharMatrix(std::istream& is) -> CharMap {
	std::string l;
	std::vector<std::string> lines;
	while (getline(is, l)) {
		lines.push_back(l); }
	const int stride = len(lines[0]);
	const int height = len(lines);
	std::vector<char> map(stride*height);
	for (int y=0; y<height; ++y) {
		for (int x=0; x<stride; ++x) {
			map[y*stride + x] = lines[y][x]; }}
	return { move(map), stride, height }; };


template <typename Tv, typename Td>
auto inrange(const Tv& x, const Td& a, const Td& b) -> bool { return a<=x && x<=b; }

auto signsep(int x) -> pii {
	if (x < 0) {
		return { -x, -1 }; }
	return { x, 1 }; }

auto LRUD(char ch) -> Int2 {
	switch (ch) {
	case 'L': return Int2{ -1, 0 };
	case 'R': return Int2{  1, 0 };
	case 'U': return Int2{  0,-1 };
	case 'D': return Int2{  0, 1 }; }
	std::cerr << "LRUD wtf?" << std::endl;
	std::exit(1); }


// dir=1 = counter-clockwise
// dir=-1 = clockwise
auto rot90(Int2 p, int t) -> Int2 {
	return { -p.y*t, p.x*t };}

auto botrot(Int2 dir, int value) -> Int2 {
	return rot90(dir, value==0?1:-1); }
