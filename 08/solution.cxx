#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <numeric>
#include <optional>
#include <utility>
#include <vector>
#include <unordered_set>

#define uset unordered_set
using vi = std::vector<int>;
using namespace std;
const std::string nl{"\n"};
constexpr int oo{0x3f3f3f3f};


int main() {
	std::string tmp;
	getline(std::cin, tmp);

	auto len = tmp.size();
	constexpr int w = 25;
	constexpr int h = 6;
	constexpr int area{ w * h };
	// cout << "layers: " << (len/area) << "r" << (len%area) << "\n";
	const int layers = len/area;
	assert(len%area==0);

	/*
	uset<char> uniq;
	for (auto ch : tmp) {
		uniq.insert(ch); }
	for (auto ch : uniq) {
		cout << "ch(" << ch << ")"; }
	std::exit(0);
	*/

	int bestLayer{-1};
	int bestZeros{oo};
	for (int li=0; li<layers; ++li) {
		int ax{0};
		auto region = tmp.data() + area * li;
		for (int i=0; i<area; ++i) {
			ax += static_cast<int>(region[i]=='0'); }
		// cout << "layer #" << li << " zeros=" << ax << "\n";
		if (ax < bestZeros) {
			bestZeros = ax;
			bestLayer = li; }}

	auto cnts = array<int, 3>{ { 0, 0, 0 } };
	for (int i{0}; i<area; ++i) {
		auto region = tmp.data() + area * bestLayer;
		auto ch = region[i];
		cnts[ch-'0']++; }
	int p1{ cnts[1] * cnts[2] };
	cout << "p1: " << p1 << nl;
	

	auto buf = array<char, w*h>{};
	std::fill(begin(buf), end(buf), '2');
	for (int li=0; li<layers; ++li) {
		auto region = tmp.data() + area*li;
		for (int i=0; i<area; ++i) {
			buf[i] = buf[i] == '2' ? region[i] : buf[i]; }}

	for (int y=0; y<h; ++y) {
		for (int x=0; x<w; ++x) {
			cout << (buf[y*w+x]=='1'?'#':' '); }
		cout << "\n"; }


	// cout << "p2: " << p2 << nl;


	return 0; }
