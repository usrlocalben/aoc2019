#include "../lib.hpp"
using namespace std;

uset<int> hits;
int lower, upper;

void R(int num, int pd, bool hasRepeating, int width=0) {
	if (width==6) {
		if (hasRepeating) {
			if (lower <= num && num <= upper) {
				//cerr << num << "\n";
				hits.insert(num); }}}
	else if (width==0) {
		for (int d=0; d<=9; ++d) {
			R(d, d, false, width+1); }}
	else {
		for (int d=pd; d<=9; ++d) {
			R(num*10+d, d, hasRepeating||(d==pd), width+1); }}}


int main() {
	string tmp;
	getline(cin, tmp);
	const auto nums = SplitNums(tmp, '-');
	lower = nums[0];
	upper = nums[1];

	R(0, 0, false, 0);
	auto p1{hits.size()};

	int p2{0};
	auto cnts = array<int, 10>{};
	for (auto hit : hits) {
		fill(ALL(cnts), 0);
		auto original = hit;
		for (int i=0; i<6; ++i) {
			int d = hit%10;
			hit/=10;
			cnts[d]++; }
		for (int i=0; i<10; ++i) {
			if (cnts[i] == 2) {
				++p2;
				break; }}}

	cout << p1 << nl;
	cout << p2 << nl;
	return 0; }
