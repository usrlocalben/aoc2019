#include "../lib.hpp"
using namespace std;
#include "../intcode.hpp"

int main() {
	string tmp;
	getline(cin, tmp);

	auto mem = SplitNums(tmp);
	auto mem64 = vector<int64_t>{};
	mem64.reserve(mem.size());
	for (auto n : mem) {
		mem64.emplace_back(n); }

	auto vm = IntCodeMachine(mem64);
	vm.Recv(1);
	int64_t p1;
	while (1) {
		vm.Run();
		if (vm.HasOutput()) {
			p1 = vm.Out(); }
		if (vm.IsHalted()) {
			break; }}
	cout << p1 << nl;


	vm = IntCodeMachine(mem64);
	vm.Recv(2);
	int64_t p2;
	while (1) {
		vm.Run();
		if (vm.HasOutput()) {
			p2 = vm.Out(); }
		if (vm.IsHalted()) {
			break; }}
	cout << p2 << nl;
	return 0; }
