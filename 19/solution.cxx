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
