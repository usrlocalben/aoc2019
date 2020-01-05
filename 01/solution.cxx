#include <algorithm>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

using namespace std;

int FuelCost(int m) {
	return std::max(0, (m/3)-2); }

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
	std::cout << "p1: " << p1 << "\n";
	std::cout << "p2: " << p2 << "\n";
	return 0; }
