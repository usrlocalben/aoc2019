#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <optional>
#include <utility>
#include <vector>

using vi = std::vector<int>;
using namespace std;
const std::string nl{"\n"};

std::vector<std::string> Split(const std::string& str, char ch) {
	std::vector<std::string> items;
	std::string src(str);
	auto nextmatch = src.find(ch);
	while (true) {
		auto item = src.substr(0, nextmatch);
		items.push_back(item);
		if (nextmatch == std::string::npos) { break; }
		src = src.substr(nextmatch + 1);
		nextmatch = src.find(ch); }
	return items; }


std::vector<int> SplitNums(const std::string& str, char ch=',') {
	std::vector<int> items;
	std::string src(str);
	auto nextmatch = src.find(ch);
	while (true) {
		auto item = src.substr(0, nextmatch);
		items.push_back(stoi(item));
		if (nextmatch == std::string::npos) { break; }
		src = src.substr(nextmatch + 1);
		nextmatch = src.find(ch); }
	return items; }


class IntCodeMachine {
	enum State {
		Normal=0,
		Halted,
		Input };
	
	std::vector<int> mem_;
	int pc_{0};
	int opcode_, mode_[3];
	int lastOut_;
	optional<int> lastIn_{};
	State state_{State::Normal};

public:
	IntCodeMachine(std::vector<int> m) :
		mem_(m) {}

private:
	void Decode(int op) {
		opcode_ = op%100;
		op /= 100;
		mode_[0] = op%10; op/=10;
		mode_[1] = op%10; op/=10;
		mode_[2] = op%10; op/=10; }

public:
	void Recv(int x) {
		lastIn_ = x; }

	auto IsHalted() const -> bool {
		return state_ == State::Halted; }

	auto IsWaiting() const -> bool {
		return state_ == State::Input; }

	void Run() {
		while (1) {
			Decode(Load(pc_));
			switch (opcode_) {
			case 1 : {
				int a = Load(pc_+1);  if (mode_[0] == 0) a = Load(a);
				int b = Load(pc_+2);  if (mode_[1] == 0) b = Load(b);
				int c = Load(pc_+3);
				Store(c, a+b);
				pc_ += 4; }
				break;
			case 2 : {
				int a = Load(pc_+1);  if (mode_[0] == 0) a = Load(a);
				int b = Load(pc_+2);  if (mode_[1] == 0) b = Load(b);
				int c = Load(pc_+3);
				Store(c, a*b);
				pc_ += 4; }
				break;
			case 3 : 
				if (!lastIn_) {
					state_ = State::Input;
					return; }
				else {
					int a = Load(pc_+1);
					Store(a, lastIn_.value());
					lastIn_.reset();
					pc_ += 2; }
				break;
			case 4 : {
				int a = Load(pc_+1);  if (mode_[0] == 0) a = Load(a);
				lastOut_ = a;
				pc_ += 2;
				return; }
			case 5 :
			case 6 : {
				// 5 = jnz, 6 = jz
				int a = Load(pc_+1);  if (mode_[0] == 0) a = Load(a);
				int b = Load(pc_+2);  if (mode_[1] == 0) b = Load(b);
				if ((opcode_==5 && a!=0) || (opcode_==6 && a==0)) {
					pc_ = b; }
				else {
					pc_ += 3; }}
				break;
			case 7 : 
			case 8 : {
				// 7 = cmplt, 8 = cmpeq
				int a = Load(pc_+1);  if (mode_[0] == 0) a = Load(a);
				int b = Load(pc_+2);  if (mode_[1] == 0) b = Load(b);
				int c = Load(pc_+3);
				Store(c, (opcode_==7 && a<b) || (opcode_==8 && a==b));
				pc_ += 4; }
				break;
			case 99 :
				state_ = State::Halted;
				return;
			default :
				std::cerr << "unknown opcode " << opcode_ << "\n";
				std::exit(1); }}}

	auto Load(int idx) const -> int {
		if (0 <= idx && idx <= mem_.size()) {
			return mem_[idx]; }
		else {
			return 0; }}

	void Store(int idx, int value) {
		if (idx < 0) {
			std::cerr << "attempted to store @ " << idx << "\n";
			std::exit(1); }
		if (idx >= mem_.size()) {
			mem_.resize(idx+1, 0); }
		mem_[idx] = value; }

	auto Out() const -> int {
		return lastOut_; } };



int main() {
	std::string tmp;
	getline(std::cin, tmp);

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

	int p1{0};
	do {
		auto amt = Foo(phases[0], 0);
		amt = Foo(phases[1], amt);
		amt = Foo(phases[2], amt);
		amt = Foo(phases[3], amt);
		amt = Foo(phases[4], amt);
		p1 = std::max(p1, amt); }
	while (next_permutation(begin(phases), end(phases)));

	cout << "p1: " << p1 << nl;



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
			cout << "pass #" << passes << " " << x << endl; }
		p2 = std::max(p2, x); }
	while (next_permutation(begin(phases), end(phases)));

	cout << "p2: " << p2 << nl;


	return 0; }
