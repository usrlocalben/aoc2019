#include "../lib.hpp"
using namespace std;
int main() {
	string tmp;
	getline(cin, tmp);
	const auto dirs1 = Split(tmp, ',');
	getline(cin, tmp);
	const auto dirs2 = Split(tmp, ',');

	auto w1 = umap<Int2, int>();
	auto hits = umap<Int2, int>();
	int p2{oo};
	int p1{oo};
	{
		auto c = Int2{0};
		int travel{0};
		for (const auto& dir : dirs1) {
			auto dv = LRUD(dir[0]);
			int  dist = stoi(dir.substr(1));
			for (int cnt{0}; cnt<dist; ++cnt) {
				++travel;
				c += dv;
				w1[c] = travel; }}}
	{
		auto c = Int2{0};
		int travel{0};
		for (const auto& dir : dirs2) {
			auto dv = LRUD(dir[0]);
			int  dist = stoi(dir.substr(1));
			for (int cnt{0}; cnt<dist; ++cnt) {
				++travel;
				c += dv;
				if (w1.find(c) != end(w1)) {
					auto d = mdist(c, Int2{0});
					p1 = min(p1, d);
					p2 = min(p2, w1[c] + travel); }}}}

	cout << p1 << nl;
	cout << p2 << nl;
	return 0; }
