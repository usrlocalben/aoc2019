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
	optional<int64_t> lastIn_{};
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
		lastIn_ = x; }

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
			// cout << "PC:" << pc_ << " OP:" << opcode_ << " mode:" << mode_[1] << "," << mode_[2] << "," << mode_[3] <<"\n";
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
				if (!lastIn_) {
					state_ = State::Input;
					return; }
				else {
					auto a = LoadAddr(1);
					Store(a, lastIn_.value());
					lastIn_.reset();
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
