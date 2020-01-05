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

public:
	IntCodeMachine(std::vector<int> m) :
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
				std::cerr << "unknown opcode " << op << "\n";
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
	vm.Store(1, 12);
	vm.Store(2, 2);
	vm.Run();
	std::cout << "p1: " << vm.Load(0) << "\n";


	for (int x=0; x<100; ++x) {
		for (int y=0; y<100; ++y) {
			auto vm = IntCodeMachine(mem);
			vm.Store(1, x);
			vm.Store(2, y);
			vm.Run();
			auto ans = vm.Load(0);
			if (ans == 19690720) {
				std::cout << "p2: " << (x*100+y) << "\n"; }}}

	return 0; }
