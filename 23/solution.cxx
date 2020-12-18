#include "../lib.hpp"
using namespace std;
#include "../intcode.hpp"

int main(int argc, char**argv) {
	string tmp;
	getline(cin, tmp);
	auto rom = SplitNums(tmp);

	vector<IntCodeMachine> vms;
	for (int i=0; i<50; ++i) {
		vms.push_back(rom);
		auto& vm = vms.back();
		vm.Recv(i); }

	array<int64_t, 50> mlen{};
	array<int64_t, 50*3> mdat{};

	uset<int64_t> natsent{};
	int64_t natx{0}, naty{0};
	int64_t part1{-1};
	while (1) {
		int idleCnt=0;
		for (int i=0; i<50; ++i) {
			auto& vm = vms[i];
			vm.Run();
			if (vm.IsWaiting()) {
				idleCnt++;
				vm.Recv(-1);
				continue; }
			if (vm.HasOutput()) {
				int64_t value = vm.Out();
				mdat[i*3+mlen[i]] = value;
				mlen[i]++;
				if (mlen[i] == 3) {
					mlen[i] = 0;
					auto addr = mdat[i*3+0];
					auto mx = mdat[i*3+1];
					auto my = mdat[i*3+2];
					if (0<=addr && addr<50) {
						// cerr << "vm" << i << " -> " << addr << " (" << mx <<", " << my << ")\n";
						vms[addr].Recv(mx);
						vms[addr].Recv(my); }
					else if (addr == 255) {
						// cerr << "NAT packet received\n";
						if (part1 == -1) {
							cout << my << nl;
							part1 = my; }
						natx = mx;
						naty = my; }
					else {
						cerr << "vm" << i << " -> " << addr << " (" << mx <<", " << my << ")\n";
						exit(1); }}}}
		if (idleCnt == 50) {
			if (natsent.find(naty) != natsent.end()) {
				cout << naty << nl;
				exit(0); }
			// cerr << "IDLE\n";
			natsent.insert(naty);
			vms[0].Recv(natx);
			vms[0].Recv(naty); }}


	// cout << part2 << nl;
	return 0; }
