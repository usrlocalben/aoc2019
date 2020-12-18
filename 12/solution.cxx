#include "../lib.hpp"
using namespace std;

int main() {
	string tmp;
	vector<Int3> pos;
	vector<Int3> vel;
	while (getline(cin, tmp)) {
		auto segs = Split(tmp, '=');
		int x = stoi(segs[1]);
		int y = stoi(segs[2]);
		int z = stoi(segs[3]);
		pos.emplace_back(Int3{ x, y, z });
		vel.emplace_back(Int3{ 0 }); }

	auto copyPos = pos;
	auto copyVel = vel;

	auto Tick = [&]() {
		for (int x=0; x<pos.size(); ++x) {
			for (int y=0; y<pos.size(); ++y) {
				if (x==y) continue;
				if (y<x) continue;

				auto change = vcomp(pos[x], pos[y]);
				vel[x] -= change;
				vel[y] += change; }}

		for (int x=0; x<pos.size(); ++x) {
			pos[x] += vel[x]; }};

	auto TotalEnergy = [&]() {
		int ax{0};
		for (int x=0; x<pos.size(); ++x) {
			ax += hadd(abs(pos[x])) * hadd(abs(vel[x])); }
		return ax; };

	for (int i=0; i<1000; ++i) {
		Tick(); }
	cout << TotalEnergy() << "\n";

	pos = copyPos;
	vel = copyVel;
	umap<pair<Int4, Int4>, int, pair_hash> seen;
	int iter;


	int64_t lens[3];

	pos = copyPos;
	vel = copyVel;
	seen.clear();
	iter = 0;
	while (1) {
		Int4 pxxxx{ pos[0].x, pos[1].x, pos[2].x, pos[3].x };
		Int4 vxxxx{ vel[0].x, vel[1].x, vel[2].x, vel[3].x };

		/*if (xxxx == hit) {
			cerr << "hit at " << iter << nl; }*/
		if (auto found = seen.find({ pxxxx, vxxxx }); found != end(seen)) {
			// cerr << "xxxx repeat " << pxxxx << vxxxx << " [" << found->second << "..." << iter << "]\n";
			lens[0] = iter;
			break; 
			// exit(0);
			}
		else {
			seen[{pxxxx,vxxxx}] = iter; }
		Tick();
		++iter; }

	pos = copyPos;
	vel = copyVel;
	seen.clear();
	iter = 0;
	while (1) {
		Int4 pxxxx{ pos[0].y, pos[1].y, pos[2].y, pos[3].y };
		Int4 vxxxx{ vel[0].y, vel[1].y, vel[2].y, vel[3].y };

		/*if (xxxx == hit) {
			cerr << "hit at " << iter << nl; }*/
		if (auto found = seen.find({ pxxxx, vxxxx }); found != end(seen)) {
			// cerr << "xxxx repeat " << pxxxx << vxxxx << " [" << found->second << "..." << iter << "]\n";
			lens[1] = iter;
			break; 
			// exit(0);
			}
		else {
			seen[{pxxxx,vxxxx}] = iter; }
		Tick();
		++iter; }

	pos = copyPos;
	vel = copyVel;
	seen.clear();
	iter = 0;
	while (1) {
		Int4 pxxxx{ pos[0].z, pos[1].z, pos[2].z, pos[3].z };
		Int4 vxxxx{ vel[0].z, vel[1].z, vel[2].z, vel[3].z };

		/*if (xxxx == hit) {
			cerr << "hit at " << iter << nl; }*/
		if (auto found = seen.find({ pxxxx, vxxxx }); found != end(seen)) {
			// cerr << "xxxx repeat " << pxxxx << vxxxx << " [" << found->second << "..." << iter << "]\n";
			lens[2] = iter;
			break; 
			// exit(0);
			}
		else {
			seen[{pxxxx,vxxxx}] = iter; }
		Tick();
		++iter; }

	auto p2 = lcm(lcm(lens[0], lens[1]), lens[2]);
	cout << p2 << nl;
	return 0; }
