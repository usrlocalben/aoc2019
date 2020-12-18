#include "../lib.hpp"
using namespace std;
#include "../intcode.hpp"

int main(int argc, char**argv) {
	std::string tmp;
	getline(std::cin, tmp);
	auto mem = SplitNums(tmp);

/*
part1

jump +4

#####.#########
  @abcd
#####.#..######

#####...#######

(!c)&&d || !a

       ||
   &       !
 !  d       a
c

NOT C T
AND D T
NOT A J
OR T J
*/

	bool showOutput{false};

	int64_t part1 = -1;
	{auto vm = IntCodeMachine(mem);
	string scode{"NOT C T\nAND D T\nNOT A J\nOR T J\nWALK\n"};
	vm.Recv(scode);
	while (!vm.IsHalted()) {
		vm.Run();
		if (vm.IsWaiting()) {
			cerr << "unexpected wait for input" << nl;
			exit(1); }
		if (vm.HasOutput()) {
			int64_t value = vm.Out();
			if (value >= 128) {
				part1 = value; }
			else {
				if (showOutput) {
					cerr << static_cast<char>(value); }}}}}
	cout << part1 << nl;

/*
part2
    j   x   j
#####.#.##.##.###

#####.###..#..###
*/

	int64_t part2 = -1;
	{auto vm = IntCodeMachine(mem);
	string scode{
		"NOT A J\n"
		"NOT B T\n"
		"OR T J\n"
		"NOT C T\n"
		"OR T J\n"
		"AND D J\n"
		
		"NOT E T\n"
		"AND H T\n"
		"OR E T\n"

		"AND T J\n"
		"RUN\n"};
	vm.Recv(scode);
	while (!vm.IsHalted()) {
		vm.Run();
		if (vm.IsWaiting()) {
			cerr << "unexpected wait for input" << nl;
			exit(1); }
		if (vm.HasOutput()) {
			int64_t value = vm.Out();
			if (value >= 128) {
				part2 = value; }
			else {
				if (showOutput) {
					cerr << static_cast<char>(value); }}}}}

	cout << part2 << nl;
	return 0; }
