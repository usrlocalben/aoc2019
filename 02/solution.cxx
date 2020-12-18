#include "../lib.hpp"
using namespace std;

class IntCodeMachine {
	
	vector<int64_t> mem_;
	int pc_{0};

public:
	IntCodeMachine(vector<int64_t> m) :
		mem_(m) {}

	void Run() {
		bool halted{false};
		while (!halted) {
			const int op = Load(pc_);
			switch (op) {
			case 1 :
				{int a = Load(Load(pc_+1));
				int b = Load(Load(pc_+2));
				Store(Load(pc_+3), a+b);}
				pc_ += 4;
				break;
			case 2 :
				{int a = Load(Load(pc_+1));
				int b = Load(Load(pc_+2));
				Store(Load(pc_+3), a*b);}
				pc_ += 4;
				break;
			case 99 :
				halted = true;
				break;
			default :
				cerr << "unknown opcode " << op << nl;
				exit(1); }}}

	auto Load(int idx) const -> int {
		if (0 <= idx && idx <= mem_.size()) {
			return mem_[idx]; }
		else {
			return 0; }}

	void Store(int idx, int value) {
		if (idx < 0) {
			cerr << "attempted to store @ " << idx << "\n";
			exit(1); }
		if (idx >= mem_.size()) {
			mem_.resize(idx+1, 0); }
		mem_[idx] = value; }};


int main() {
	string tmp;
	getline(cin, tmp);
	auto mem = SplitNums(tmp);

	auto vm = IntCodeMachine(mem);
	vm.Store(1, 12);
	vm.Store(2, 2);
	vm.Run();
	cout << vm.Load(0) << nl;

	for (int x=0; x<100; ++x) {
		for (int y=0; y<100; ++y) {
			auto vm = IntCodeMachine(mem);
			vm.Store(1, x);
			vm.Store(2, y);
			vm.Run();
			auto ans = vm.Load(0);
			if (ans == 19690720) {
				cout << (x*100+y) << nl; }}}

	return 0; }
