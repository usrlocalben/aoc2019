#include "../lib.hpp"
using namespace std;
#include "../intcode.hpp"

int main() {
	string tmp;
	getline(cin, tmp);
	auto mem = SplitNums(tmp);

	umap<Int2, char> paint;
	uset<Int2> painted;
	Int2 cPos{0,0};
	Int2 cDir{0,1};
	Int2 bMin{ oo, oo };
	Int2 bMax{-oo,-oo };

	auto runBot = [&]() {
		auto vm = IntCodeMachine(mem);
		while (1) {
			// cout << "P:" << cPos << " D:" << cDir << endl;
			bMin = vmin(bMin, cPos);
			bMax = vmax(bMax, cPos);

			int color;
			if (auto found = paint.find(cPos); found==end(paint)) {
				color = 0; }
			else {
				color = found->second; }

			vm.Recv(color);
			vm.Run();
			if (vm.IsHalted()) {
				break; }
			if (vm.HasOutput()) {
				// cerr << "<vm> " << vm.Out() << nl;
				painted.insert(cPos);
				paint[cPos] = vm.Out(); }
			vm.Run();
			// if (vm.IsHalted()) { cerr << "<vm> halted! 2\n"; exit(1);}
			if (vm.HasOutput()) {
				// cerr << "<vm> " << vm.Out() << nl;
				cDir = botrot(cDir, vm.Out()); }
			cPos += cDir; }};

	runBot();
	int p1 = painted.size();
	cout << "p1: " << p1 << nl;

	paint.clear();
	painted.clear();
	cPos = Int2{ 0, 0 };
	cDir = Int2{ 0, 1 };
	bMin = Int2{ oo, oo };
	bMax = Int2{-oo,-oo };
	paint[cPos] = 1;
	runBot();

	cout << bMin << " -> " << bMax << nl;
	auto dim = bMax-bMin;
	for (int y=0; y<dim.y+1; ++y) {
		for (int x=0; x<dim.x; ++x) {
			auto pos = Int2{ x, dim.y-y } + bMin;

			int color;
			if (auto found = paint.find(pos); found==end(paint)) {
				color = 0; }
			else {
				color = found->second; }

			cout << (color==0?'.':'#'); }
		cout << nl; }

	return 0; }
