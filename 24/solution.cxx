#include "../lib.hpp"
using namespace std;

const int dim = 5;
const int slice = dim*dim;
const int thick = 512;
const int mid = thick/2;

int main() {
	string l;

	int init{0};
	vector<char> map(slice*thick, '.');
	auto tmp = map;
	for (int y=0; getline(cin,l); ++y) {
		for (int x=0; x<5; ++x) {
			init |= (l[x]=='#') << (y*5+x);
			map[mid*slice + y*dim +x] = l[x]; }}

	const array<Int2, 4> dirs = {{ {1,0}, {-1,0}, {0,-1}, {0,1} }};

	int a, b;
	auto Calc1 = [&]() {
		b = 0;
		for (int y=0; y<5; ++y) {
		for (int x=0; x<5; ++x) {
			Int2 coord{ x, y };
			int cellBit = 1<<(y*5+x);

			int adjBugs=0;
			for (auto dir : dirs) {
				Int2 adjCoord = coord + dir;
				if (0<=adjCoord.y && adjCoord.y<5 &&
					0<=adjCoord.x && adjCoord.x<5) {
					int otherBit = 1<<(adjCoord.y*5+adjCoord.x);
					adjBugs += (a&otherBit)!=0; }}

			bool cellBug = (a&cellBit)!=0;
			bool result;
			if (cellBug) {
				result = adjBugs==1; }
			else {
				result = (1<=adjBugs && adjBugs <=2); }
			if (result) {
				b |= cellBit; }}}};

	uset<int> states{};
	a = init;
	while (states.find(a) == states.end()) {
		states.insert(a);
		Calc1();
		swap(a,b); }
	cout << a << nl;


	umap<int64_t, uset<pair<int64_t, int>, pair_hash>> outs;

	// outlinks for z=0 towards z=-1
	for (int y=-2; y<=2; ++y) {
		for (int x=-2; x<=2; ++x) {
			if (x==0 && y==0) continue;
			Int2 pos{x,y};
			for (const auto& dir : dirs) {
				int z = 0;
				Int2 next = pos + dir;
				if (next == Int2{0,0}) continue;
				else if (next.x < -2) next = {-1, 0}, z = -1;
				else if ( 2 < next.x) next = { 1, 0}, z = -1;
				else if (next.y < -2) next = { 0,-1}, z = -1;
				else if ( 2 < next.y) next = { 0, 1}, z = -1;
				outs[pos.key()].insert({next.key(), z});
				if (z==-1) {
					// invert for links towards z+1
					outs[next.key()].insert({pos.key(), 1}); }}}}

	auto Peek = [&](Int3 c) -> char {
		return map[(c.z+mid)*slice + ((-c.y)+2)*dim + (c.x+2)]; };

	auto Poke = [&](Int3 c, char ch) -> void {
		tmp[(c.z+mid)*slice + ((-c.y)+2)*dim + (c.x+2)] = ch; };

	auto Calc2 = [&]() {
		for (int z=-200; z<=200; ++z) {
			for (int y=-2; y<=2; ++y) {
				for (int x=-2; x<=2; ++x) {
					if (x==0 && y==0) continue;
					const Int3 pos{x, y, z};

					int adjBugCnt = 0;
					auto& others = outs[pos.xy().key()];
					for (const auto [k, dz] : others) {
						auto foo = Int2::fromkey(k);
						const Int3 adj{ foo.x, foo.y, pos.z+dz };
						adjBugCnt += Peek(adj)=='#'; }

					bool bug = Peek(pos)=='#';
					bool result;
					if (bug) {
						result = adjBugCnt==1; }
					else {
						result = (adjBugCnt==1 || adjBugCnt==2); }
					Poke(pos, result?'#':'.'); }}}
		swap(tmp, map); };

	for (int n=0; n<200; ++n) {
		Calc2(); }
	auto part2 = accumulate(ALL(map), 0, [](auto ax, auto ch) { return ax + (ch=='#'); });
	cout << part2 << nl;
	return 0; }
