#include "../lib.hpp"
using namespace std;

int seq{0};
umap<string, int> nameId;
umap<int, string> invNameId;

auto UpsertName(string name) -> int {
	if (auto found = nameId.find(name); found!= end(nameId)) {
		return found->second; }
	int sku = seq++;
	nameId[name] = sku;
	invNameId[sku] = name;
	return sku; }

struct Batch {
	int qty;
	int sku; };

auto operator<<(ostream& s, Batch b) -> ostream& {
	return s << b.qty << "_" << invNameId[b.sku]; }

struct Recipe {
	array<int, 64> in{};
	int sku;
	int yield;
	int64_t needed{0};
	int indegree{0}; };

auto operator<<(ostream& s, Recipe r) -> ostream& {
	s << "<Recipe in=[";
	bool first{true};
	for (int i=0; i<64; ++i) {
		if (r.in[i] == 0) continue;
		if (!first) {
			s << ", "; }
		s << Batch{ r.in[i], i };
		first = false; }
	s << "], out=" << Batch{ r.yield, r.sku } << ">";
	return s; }

vector<Recipe> recipes;

uset<int> visited;
void FillIndegree(int n, int d=0) {
	if (d==0) {
		visited.clear(); }
	if (visited.find(n) != end(visited)) {
		return; }
	visited.insert(n);
	auto& recipe = recipes[n];
	for (int i=0; i<64; ++i) {
		if (recipe.in[i] > 0) {
			recipes[i].indegree++;
			FillIndegree(i, d+1); }}}

class Bisector {
	int64_t upper_;
	int64_t lower_;
	int64_t target_;
public:
	Bisector(int64_t u, int64_t t) :
		lower_(0),
		upper_(u),
		target_(t) {}

	auto Pos() const -> int64_t {
		return (lower_ + upper_) / 2; }

	auto Result(int64_t v) {
		// cout << "[" << lower_ << ", " << upper_ << "] " << v << nl;
		if (v < target_) {
			lower_ = Pos(); }
		else if (v > target_) {
			upper_ = Pos(); }}

	auto End() const -> bool {
		return upper_-lower_ <= 1; } };


int main() {
	string tmp;
	vector<Recipe> rax;
	while (getline(cin, tmp)) {
		auto delim = string{" => "};
		auto pos = tmp.find(delim);
		auto left = tmp.substr(0, pos);
		auto right = tmp.substr(pos+delim.size());

		auto segs = Split(left, ", ");

		auto deserializeBatch = [](auto text) {
			auto numname = Split(text, ' ');
			Batch b;
			b.qty = stoi(numname[0]);
			b.sku = UpsertName(numname[1]);
			return b; };

		auto r = accumulate(ALL(segs), Recipe{},
		                    [&](auto ax, auto cur) {
		                        auto b = deserializeBatch(cur);
		                        ax.in[b.sku] = b.qty;
		                        return ax; });

		auto b = deserializeBatch(right);
		r.sku = b.sku;
		r.yield = b.qty;
		// cout << r << nl;
		rax.push_back(r); }

	auto eax = Recipe{};
	eax.sku = nameId["ORE"];
	eax.yield = 1;
	rax.push_back(eax);


	recipes.resize(rax.size());
	for (const auto& r : rax) {
		recipes[r.sku] = r; }


	const int SKU_FUEL = nameId["FUEL"];

	/*for (const auto& r : recipes) {
		cout << r.indegree << ": " << r << nl; }*/

	deque<int> queue;
	auto FuelCostInOre = [&](int64_t wanted) {
		for (auto& r : recipes) {
			r.indegree = 0;
			r.needed = 0; }
		FillIndegree(SKU_FUEL);
		visited.clear();

		recipes[SKU_FUEL].needed = wanted;
		queue.push_back(SKU_FUEL);
		while (!queue.empty()) {
			const auto here = queue.front(); queue.pop_front();
			if (visited.find(here) != end(visited)) continue;
			auto& r = recipes[here];
			if (--r.indegree > 0) continue;  // not ready yet
			// cout << "visiting " << invNameId[here] << "\n";

			int64_t numReactions = r.needed / r.yield;
			if (r.needed % r.yield > 0) {
				numReactions++; }
			// cout << "need: " << r.needed << ", yield is " << r.yield << " -- reactions: " << numReactions << nl;

			for (int i=0; i<64; ++i) {
				if (r.in[i] == 0) continue;
				recipes[i].needed += numReactions * r.in[i];
				queue.push_back(i); }}
		return recipes[nameId["ORE"]].needed; };

	auto p1 = FuelCostInOre(1);
	cout << p1 << nl;


	Bisector bisector{ 10000000, 1000000000000 };
	while (!bisector.End()) {
		auto x = bisector.Pos();
		auto ore = FuelCostInOre(x);
		bisector.Result(ore); }

	cout << bisector.Pos() << nl;
	return 0; }
