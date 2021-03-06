#include "../lib.hpp"
using namespace std;

class IntCodeMachine {
	enum State {
		Normal=0,
		Halted,
		Input };

	vector<int64_t> mem_;
	int pc_{0}, bp_{0};
	int opcode_, mode_[4];
	optional<int64_t> lastOut_{};
	deque<int64_t> inq_{};
	State state_{State::Normal};

public:
	IntCodeMachine(decltype(IntCodeMachine::mem_) m) :
		mem_(m) {}

private:
	void Decode(int64_t op) {
		opcode_ = op%100;
		op /= 100;
		mode_[1] = op%10; op/=10;
		mode_[2] = op%10; op/=10;
		mode_[3] = op%10; op/=10; }

public:
	void Recv(int64_t x) {
		inq_.push_back(x); }

	void Recv(string s) {
		for (auto ch : s) {
			inq_.push_back(ch); }}

	auto IsHalted() const -> bool {
		return state_ == State::Halted; }

	auto IsWaiting() const -> bool {
		return state_ == State::Input; }

	auto LoadValue(int n) -> int64_t {
		auto v = Load(pc_+n);
		if (mode_[n] == 0) {
			v = Load(v); }
		else if (mode_[n] == 1) {
			// nothing
			}
		else if (mode_[n] == 2) {
			v = Load(v+bp_); }
		else {
			cerr << "invalid opcode param mode " << mode_[n] << "!";
			exit(1); }
		return v; }

	auto LoadAddr(int n) -> int {
		int64_t v = Load(pc_+n);
		if (mode_[n] == 0) {
			// do nothing
			}
		else if (mode_[n] == 1) {
			cerr << "invalid mode for LoadAddr";
			exit(1); }
		else if (mode_[n] == 2) {
			v += bp_; }
		else {
			cerr << "invalid opcode param mode " << mode_[n] << "!";
			exit(1); }
		return v; }

	void Run() {
		while (1) {
			Decode(Load(pc_));
			// cerr << "PC:" << pc_ << " OP:" << opcode_ << " mode:" << mode_[1] << "," << mode_[2] << "," << mode_[3] <<"\n";
			switch (opcode_) {
			case 1 : {
				auto a = LoadValue(1);
				auto b = LoadValue(2);
				auto c = LoadAddr(3);
				Store(c, a+b);
				pc_ += 4; }
				break;
			case 2 : {
				auto a = LoadValue(1);
				auto b = LoadValue(2);
				auto c = LoadAddr(3);
				Store(c, a*b);
				pc_ += 4; }
				break;
			case 3 : 
				if (inq_.empty()) {
					state_ = State::Input;
					return; }
				else {
					auto a = LoadAddr(1);
					int64_t v = inq_.front(); inq_.pop_front();
					// cerr << "<S:" << (char)v << ">" << flush;
					Store(a, v);
					state_ = State::Normal;
					pc_ += 2; }
				break;
			case 4 : {
				auto a = LoadValue(1);
				lastOut_ = a;
				// cerr << "VM out " << a << "\n";
				pc_ += 2;
				return; }
			case 5 :
			case 6 : {
				// 5 = jnz, 6 = jz
				auto a = LoadValue(1);
				auto b = LoadValue(2);
				if ((opcode_==5 && a!=0) || (opcode_==6 && a==0)) {
					pc_ = b; }
				else {
					pc_ += 3; }}
				break;
			case 7 : 
			case 8 : {
				// 7 = cmplt, 8 = cmpeq
				auto a = LoadValue(1);
				auto b = LoadValue(2);
				auto c = LoadAddr(3);
				Store(c, (opcode_==7 && a<b) || (opcode_==8 && a==b));
				pc_ += 4; }
				break;
			case 9 : {
				auto a = LoadValue(1);
				bp_ += a;
				pc_ += 2;}
				break;
			case 99 :
				state_ = State::Halted;
				return;
			default :
				cerr << "unknown opcode " << opcode_ << " (pc=" << pc_ << ")\n";
				exit(1); }}}

	void DumpMemory() const {
		cerr << "===== BEGIN MEMORY DUMP =====\n";
		for (auto el : mem_) {
			cerr << el << nl; }
		cerr << "====== END MEMORY DUMP ======\n"; }

	auto Load(int idx) const -> int64_t {
		if (0 <= idx && idx <= mem_.size()) {
			return mem_[idx]; }
		else {
			return 0; }}

	void Store(int idx, int64_t value) {
		if (idx < 0) {
			cerr << "attempted to store @ " << idx << "\n";
			exit(1); }
		if (idx >= mem_.size()) {
			mem_.resize(idx+1, 0); }
		mem_[idx] = value; }

	auto HasOutput() const -> bool {
		return lastOut_.has_value(); }

	auto Out() -> int64_t {
		auto v = *lastOut_;
		lastOut_.reset();
		return v; } };


int main() {
	string tmp;
	getline(cin, tmp);
	auto mem = SplitNums(tmp);

	auto Foo = [&](int phase, int input) {
		auto vm = IntCodeMachine(mem);
		vm.Run();
		vm.Recv(phase);
		vm.Run();
		vm.Recv(input);
		vm.Run();
		return vm.Out(); };

	auto phases = array<int, 5>{};
	iota(begin(phases), end(phases), 0);

	int64_t p1{0};
	do {
		auto amt = Foo(phases[0], 0);
		amt = Foo(phases[1], amt);
		amt = Foo(phases[2], amt);
		amt = Foo(phases[3], amt);
		amt = Foo(phases[4], amt);
		p1 = std::max(p1, amt); }
	while (next_permutation(begin(phases), end(phases)));

	cout << p1 << nl;



	int p2{0};
	iota(begin(phases), end(phases), 5);
	do {
		auto vm0 = IntCodeMachine(mem); vm0.Run(); vm0.Recv(phases[0]);
		auto vm1 = IntCodeMachine(mem); vm1.Run(); vm1.Recv(phases[1]);
		auto vm2 = IntCodeMachine(mem); vm2.Run(); vm2.Recv(phases[2]);
		auto vm3 = IntCodeMachine(mem); vm3.Run(); vm3.Recv(phases[3]);
		auto vm4 = IntCodeMachine(mem); vm4.Run(); vm4.Recv(phases[4]);

		int x = 0;
		int passes = 0;
		while (1) {
			++passes;
			vm0.Run(); if (vm0.IsHalted()) break; vm0.Recv(x);  vm0.Run();  x = vm0.Out();
			vm1.Run(); if (vm1.IsHalted()) break; vm1.Recv(x);  vm1.Run();  x = vm1.Out();
			vm2.Run(); if (vm2.IsHalted()) break; vm2.Recv(x);  vm2.Run();  x = vm2.Out();
			vm3.Run(); if (vm3.IsHalted()) break; vm3.Recv(x);  vm3.Run();  x = vm3.Out();
			vm4.Run(); if (vm4.IsHalted()) break; vm4.Recv(x);  vm4.Run();  x = vm4.Out();
			/*cerr << "pass #" << passes << " " << x << endl;*/ }
		p2 = max(p2, x); }
	while (next_permutation(begin(phases), end(phases)));

	cout << p2 << nl;
	return 0; }
