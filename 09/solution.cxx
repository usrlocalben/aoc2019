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
	
	std::vector<int64_t> mem_;
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
			std::exit(1); }
		return v; }

	auto LoadAddr(int n) -> int {
		int64_t v = Load(pc_+n);
		if (mode_[n] == 0) {
			// do nothing
			}
		else if (mode_[n] == 1) {
			cerr << "invalid mode for LoadAddr";
			std::exit(1); }
		else if (mode_[n] == 2) {
			v += bp_; }
		else {
			cerr << "invalid opcode param mode " << mode_[n] << "!";
			std::exit(1); }
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
				std::cerr << "unknown opcode " << opcode_ << " (pc=" << pc_ << ")\n";
				std::exit(1); }}}

	auto Load(auto idx) const -> int64_t {
		if (0 <= idx && idx <= mem_.size()) {
			return mem_[idx]; }
		else {
			return 0; }}

	void Store(int idx, int64_t value) {
		if (idx < 0) {
			std::cerr << "attempted to store @ " << idx << "\n";
			std::exit(1); }
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
	std::string tmp;
	getline(std::cin, tmp);

	auto mem = SplitNums(tmp);
	auto mem64 = std::vector<int64_t>{};
	mem64.reserve(mem.size());
	for (auto n : mem) {
		mem64.emplace_back(n); }

	auto vm = IntCodeMachine(mem64);
	vm.Recv(1);
	while (1) {
		vm.Run();
		if (vm.HasOutput()) {
			cerr << "<vm> " << vm.Out() << nl; }
		if (vm.IsHalted()) {
			break; }}


	vm = IntCodeMachine(mem64);
	vm.Recv(2);
	while (1) {
		vm.Run();
		if (vm.HasOutput()) {
			cerr << "<vm> " << vm.Out() << nl; }
		if (vm.IsHalted()) {
			break; }}

	// cout << "p2: " << p2 << nl;


	return 0; }
