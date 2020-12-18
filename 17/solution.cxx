#include "../lib.hpp"
using namespace std;
#include "../intcode.hpp"

array<Int2, 4> NSWE = {{
	// north    south        west         east
	// Int2{0,1}, Int2{0,-1}, Int2{-1,0}, Int2{1,0} }};  
	Int2{0,-1}, Int2{0,1}, Int2{-1,0}, Int2{1,0} }};

array<string, 4> NSWEname = {{ "N", "S", "W", "E" }};

array<char, 4> UDLRchar = {{ '^', 'v', '<', '>' }};

auto decodeUDLR(char ch) -> optional<int> {
	auto found = find(begin(UDLRchar), end(UDLRchar), ch);
	if (found == end(UDLRchar)) {
		return {}; }
	return distance(begin(UDLRchar), found); }

int main() {
	string tmp;
	getline(cin, tmp);

	auto mem = SplitNums(tmp);

	Int2 bMin{ 0, 0};
	Int2 bMax{ 0, 0};


	auto vm = IntCodeMachine(mem);
	vector<string> map;
	Int2 dPos, dDir;
	map.emplace_back();

	while (!vm.IsHalted()) {
		auto& dst = map.back();

		vm.Run();
		if (vm.HasOutput()) {
			auto ch = static_cast<char>(vm.Out());
			if (ch == '\n') {
				// cerr << "[" << map.back() << "]\n";
				map.emplace_back(); }
			else {
				if (auto bot = decodeUDLR(ch); bot) {
					dPos = Int2( dst.size(), map.size()-1 );
					dDir = NSWE[*bot];
					ch = '#'; }
				dst += ch; }}}
	map.pop_back(); // trailing newline

	const int h = map.size();
	const int w = map[0].size();

	// cerr << dPos;
	// cerr << dDir;

	auto GetPixel = [&](Int2 c) -> char {
		return map[c.y][c.x]; };

	int ax{0};
	for (int y=1; y<h-1; ++y) {
		for (int x=1; x<w-1; ++x) {
			const auto coord = Int2{ x, y };
			if (GetPixel(coord) != '#') continue;
			auto n = GetPixel(coord + Int2{ 0,-1});
			auto s = GetPixel(coord + Int2{ 0, 1});
			auto w = GetPixel(coord + Int2{-1, 0});
			auto e = GetPixel(coord + Int2{ 1, 0});
			if (n=='#' && s=='#' && w=='#' && e=='#') {
				ax += coord.x*coord.y; }}}
	cout << ax << nl;

	vm = IntCodeMachine(mem);
	vm.Store(0, 2);
	vm.Recv("A,B,A,C,B,A,B,C,C,B\n");
	vm.Recv("L,12,L,12,R,4\n");
	vm.Recv("R,10,R,6,R,4,R,4\n");
	vm.Recv("R,6,L,12,L,12\n");
	vm.Recv("n\n");
	// cerr << "begin output loop" << endl;
	while (!vm.IsHalted()) {
		vm.Run();
		if (vm.IsWaiting()) {
			// cerr << "waiting for input?" << endl;
			exit(1); }
		if (vm.HasOutput()) {
			char ch = vm.Out();
			/*cerr << ch;*/ }}

	cout << vm.Out() << nl;

	return 0; }
