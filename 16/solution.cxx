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

constexpr array<int, 4> pat = {{ 0, 1, 0, -1 }};

struct PatternGen {
	int pos_{0};
	int rem_;
	int width_;

	void Begin(int n) {
		if (n==0) {
			width_ = n;
			rem_ = 0;
			pos_ = 1; }
		else {
			width_ = n;
			rem_ = n-1;
			pos_ = 0; }}

	auto Next() -> int {
		int value = pat[pos_];
		if (--rem_ < 0) {
			rem_ = width_;
			pos_ = (pos_+1)&0x03; }
		return value; } };


struct PatternGen2 {
	int pos_{0};
	int rem_;
	int width_;

	void Begin(int n) {
		if (n==0) {
			width_ = n+1;
			rem_ = 1;
			pos_ = 1; }
		else {
			width_ = n+1;
			rem_ = n;
			pos_ = 0; }}

	auto Next() -> pair<int, int> {
		pair<int,int> rtn = { pat[pos_], rem_ };
		rem_ = width_;
		pos_ = (pos_+1)&0x03;
		return rtn; }};
	 

int main() {
	std::string tmp;
	getline(std::cin, tmp);

	vector<char> ba, bb;
	ba.resize(tmp.size());
	bb.resize(tmp.size());
	char* src = ba.data();
	char* dst = bb.data();

	auto baseSize = static_cast<int>(tmp.size());

	{int idx=0;
	for (const auto ch : tmp) {
		src[idx++] = ch - '0'; }}

	PatternGen pgen;
	for (int p=0; p<100; ++p) {
		cout << "." << flush;
		for (int x{0}; x<baseSize; ++x) {
			pgen.Begin(x);
			int ax{0};
			for (int y{0}; y<baseSize; ++y) {
				ax += src[y] * pgen.Next(); }
			dst[x] = abs(ax)%10; }
		swap(src, dst); }

	cout << "p1: ";
	for (int i{0}; i<8; ++i) {
		cout << char(src[i]+'0'); }
	cout << endl;

				
	vector<char> ca, cb;
	ca.reserve(baseSize*10000);
	cb.reserve(baseSize*10000);
	{for (const auto ch : tmp) {
		ca.push_back(ch - '0'); }}
	for (int i=0; i<10000; ++i) {
		cb.insert(end(cb), begin(ca), end(ca)); }
	ca = cb;

	src = ca.data();
	dst = cb.data();

	PatternGen2 pg2;

	for (int p=0; p<100; ++p) {
		cerr << "." << flush;
		for (int x{0}, siz=ca.size(); x<siz; ++x) {
		// for (int x=ca.size()-1, siz=ca.size(); x>=0; --x) {
			if (x%1000==0)
			cerr << x << "." << flush;

			pg2.Begin(x);
			int ax{0}, y{0};
			while (y<siz) {
				auto [n, rep] = pg2.Next();
				rep = min(rep, siz-y);
				if (n == 0) {
					y+=rep; }
				else {
					while (rep--) ax += n*src[y++]; }
				/*else if (n == 1) {
					while (rep--) ax += src[y++]; }
				else if (n ==-1) {
					while (rep--) ax -= src[y++]; }*/
			}

			cout << ax << "\n";
			int final = abs(ax)%10;
			// cout << char(final+'0');
			dst[x] = final; }
		swap(src, dst); }

	return 0; }
