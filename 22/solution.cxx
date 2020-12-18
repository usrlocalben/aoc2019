/**
 * learned some new things re: modular polynomials, powers
 * https://github.com/metalim/metalim.adventofcode.2019.python/blob/master/22_cards_shuffle.ipynb
 */
#include "../lib.hpp"
using namespace std;

using Int = __int128;

auto mod(Int a, Int b) -> Int {
	Int tmp = a%b;
	return tmp<0?tmp+b:tmp; }

auto pow(Int a, Int b, Int m, Int n) -> pair<Int, Int> {
	if (m==0) {
		return {1, 0}; }
	if (m%2==0) {
		return pow(mod(a*a,n), mod(a*b+b,n), m/2, n); }
	auto [c, d] = pow(a, b, m-1, n);
	return { mod(a*c,n), mod(a*d+b,n) }; }

auto pow(Int a, Int b, Int c) -> Int {
	if (b==0) {
		return 1; }
	if (b%2==0) {
		return pow(mod(a*a,c), b/2, c); }
	return mod(a * pow(a, b-1, c), c); }

using namespace std;
int main() {
	std::string l;
	vs lines;
	while (getline(cin, l)) {
		lines.push_back(l); }

	{int idx = 2019;
	int N = 10007;
	for (const auto& l : lines) {
		auto words = Split(l, ' ');
		auto& w0 = words[0];
		auto& w1 = words[1];
		if (w0=="deal" && w1=="into") {
			idx = N - idx - 1; }
		else if (w0=="deal" && w1=="with") {
			int a = stoi(words[3]);
			idx = mod(idx * a, N); }
		else if (w0=="cut") {
			int a = stoi(words[1]);
			idx = mod(idx - a, N); }
		else {
			cerr << "invalid: " << l << nl;
			exit(1); }}
	cout << idx << nl;}


	Int N = 119315717514047;
	Int many = 101741582076661;
	Int a=1, b=0;
	for (int li=len(lines)-1; li>=0; --li) {
		const auto& line = lines[li];
		auto words = Split(line, ' ');
		auto& w0 = words[0];
		auto& w1 = words[1];
		if (w0=="deal" && w1=="into") {
			a = -a;
			b = N - b - 1; }
		else if (w0=="deal" && w1=="with") {
			Int inc = stol(words[3]);
			Int z = pow(inc,N-2,N);
			a = mod(a*z, N);
			b = mod(b*z, N); }
		else if (w0=="cut") {
			int n = stoi(words[1]);
			b = mod(b+n, N); }
		else {
			cerr << "invalid: " << line << nl;
			exit(1); }}

	tie(a,b) = pow(a, b, many, N);
	auto part2 = static_cast<int64_t>(mod(a*2020+b, N));
	cout << part2 << nl;
	return 0; }
