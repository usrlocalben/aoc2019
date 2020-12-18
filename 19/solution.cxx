#include "../lib.hpp"
using namespace std;
#include "../intcode.hpp"

int main(int argc, char**argv) {
	string tmp;
	getline(cin, tmp);

	auto mem = SplitNums(tmp);

	// Int2 bMin{ 0, 0};
	// Int2 bMax{ 0, 0};

	vector<string> map;

	auto InBeam = [&](Int2 coord) -> bool {
		auto vm = IntCodeMachine(mem);
		vm.Recv(coord.x);
		vm.Recv(coord.y);
		vm.Run();
		return vm.Out() == 1; };

	int cnt{0};
	for (int y=0; y<50; ++y) {
		for (int x=0; x<50; ++x) {
			const auto coord = Int2(x,y);
			const auto in = InBeam(coord);
			cerr << (in?'#':'.');
			cnt += in; }
		cerr << nl; }

	cout << "p1: " << cnt << nl;

	// (923, 1141) found by probing manually
	int p2{0};
	for (int y=0; y<100; ++y) {
		for (int x=0; x<100; ++x) {
			const auto coord = Int2(x,y) + Int2(atoi(argv[1]), atoi(argv[2]));
			const auto in = InBeam(coord);
			p2 += in;
			cerr << (in?'#':'.'); }
		cerr << nl; }

	cout << "p2: " << p2 << nl;

	/*for (int i=0; i<mem.size(); ++i) {
		if (mem[i] != vm.Load(i)) {
			cerr << "mem @ " << i << " orig=" << mem[i] << ", vm=" << vm.Load(i) << "\n"; }}*/

	return 0; }
