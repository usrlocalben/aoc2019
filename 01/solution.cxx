#include "../lib.hpp"
using namespace std;

int FuelCost(int m) {
	return max(0, (m/3)-2); }

int FuelCostEx(int m) {
	auto fuel = FuelCost(m);
	if (fuel > 0) {
		fuel += FuelCostEx(fuel); }
	return fuel; }

int main() {
	int p1{0}, p2{0}, x;
	while (cin >> x) {
		p1 += FuelCost(x);
		p2 += FuelCostEx(x); }
	cout << p1 << nl;
	cout << p2 << nl;
	return 0; }
