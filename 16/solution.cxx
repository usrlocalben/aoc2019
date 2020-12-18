#include "../lib.hpp"
using namespace std;

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
	string tmp;
	getline(cin, tmp);

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
		for (int x{0}; x<baseSize; ++x) {
			pgen.Begin(x);
			int ax{0};
			for (int y{0}; y<baseSize; ++y) {
				ax += src[y] * pgen.Next(); }
			dst[x] = abs(ax)%10; }
		swap(src, dst); }

	for (int i{0}; i<8; ++i) {
		cout << char(src[i]+'0'); }
	cout << endl;


	auto at = [&](int idx) -> char {
		return tmp[idx%650]-'0'; };

	int start = stoi(tmp.substr(0, 7));
	int rlen = 650*10000;
	int wlen = rlen - start;
	vector<char> ca(wlen, 0);
	vector<char> cb(wlen, 0);
	int64_t ax=0;
	for (int i=start; i<rlen; ++i) {
		ax += at(i);
		ca[i-start] = at(i); }

	auto ones = [](int64_t x) -> int {
		auto m = x % 10;
		return m<0?m+x:m; };

	for (int n=0; n<100; ++n) {
		int64_t nax=0;
		for (int i=0; i<wlen; ++i) {
			int num = ones(ax);
			cb[i] = num;
			nax += num;
			ax -= ca[i]; }
		swap(ca,cb);
		ax = nax;
		nax=0;}

	for (int n=0; n<8; ++n) {
		cout << int(ca[n]); }
	cout << nl;
	return 0; }
