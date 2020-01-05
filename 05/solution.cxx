#include <algorithm>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

using vi = std::vector<int>;
using namespace std;

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
	
	std::vector<int> mem_;
	int pc_{0};
	int opcode_, mode_[3];
	std::vector<int> args_;
	int ai_{0};

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
	void Arg(int x) {
		args_.push_back(x); }

	void Run() {
		bool halted{false};
		while (!halted) {
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
			case 3 : {
				int a = Load(pc_+1);
				if (ai_ >= args_.size()) {
					std::cerr << "premature end of args\n";
					std::exit(1); }
				Store(a, args_[ai_++]);
				pc_ += 2; }
				break;
			case 4 : {
				int a = Load(pc_+1);  if (mode_[0] == 0) a = Load(a);
				std::cout << "VM: " << a << "\n";
				pc_ += 2; }
				break;
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
				halted = true;
				break;
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
		mem_[idx] = value; }};



int main() {
	std::string tmp;
	getline(std::cin, tmp);

	auto mem = SplitNums(tmp);

	auto vm = IntCodeMachine(mem);
	vm.Arg(1);
	vm.Run();

	// std::cout << "p1: " << vm.Load(0) << "\n";
	//
	vm = IntCodeMachine(mem);
	vm.Arg(5);
	vm.Run();

	return 0; }
