#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#define uset unordered_set
#define umap unordered_map
using vi = std::vector<int>;
using namespace std;
const std::string nl{"\n"};
constexpr int oo{0x3f3f3f3f};

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


std::vector<int64_t> SplitNums(const std::string& str, char ch=',') {
	std::vector<int64_t> items;
	std::string src(str);
	auto nextmatch = src.find(ch);
	while (true) {
		auto item = src.substr(0, nextmatch);
		items.push_back(stol(item));
		if (nextmatch == std::string::npos) { break; }
		src = src.substr(nextmatch + 1);
		nextmatch = src.find(ch); }
	return items; }

struct ivec2 {
	int x, y;
	ivec2() = default;
	constexpr explicit ivec2(int x_) :
		x(x_), y(x_){}
	constexpr ivec2(int x_, int y_) :
		x(x_), y(y_) {}
	auto operator+=(const ivec2& other) -> ivec2& { x+=other.x; y+=other.y; return *this; }
	auto operator-=(const ivec2& other) -> ivec2& { x-=other.x; y-=other.y; return *this; }
	auto operator*=(const ivec2& other) -> ivec2& { x*=other.x; y*=other.y; return *this; } };

auto operator+(const ivec2& a, const ivec2& b) -> ivec2 { auto tmp = a; tmp += b; return tmp; }
auto operator-(const ivec2& a, const ivec2& b) -> ivec2 { auto tmp = a; tmp -= b; return tmp; }
auto operator*(const ivec2& a, const ivec2& b) -> ivec2 { auto tmp = a; tmp *= b; return tmp; }
auto operator+(const ivec2& a, int& b) -> ivec2 { auto tmp = a; tmp += ivec2{b,b}; return tmp; }
auto operator*(const ivec2& a, int& b) -> ivec2 { auto tmp = a; tmp *= ivec2{b,b}; return tmp; }
auto operator-(const ivec2& a) -> ivec2 { return ivec2{ -a.x, -a.y }; }
auto operator==(const ivec2& a, const ivec2& b) -> bool { return a.x==b.x && a.y==b.y; }
auto operator!=(const ivec2& a, const ivec2& b) -> bool { return a.x!=b.x || a.y!=b.y; }


auto abs(ivec2 a) -> ivec2 { return ivec2{ std::abs(a.x), std::abs(a.y) }; }
auto hadd(ivec2 a) -> int { return a.x + a.y; }
auto mdist(ivec2 a, ivec2 b) -> int { return hadd(abs(a - b)); }
auto vmax(ivec2 a, ivec2 b) -> ivec2 { return ivec2{ std::max(a.x, b.x), std::max(a.y, b.y) }; }
auto vmin(ivec2 a, ivec2 b) -> ivec2 { return ivec2{ std::min(a.x, b.x), std::min(a.y, b.y) }; }
auto area(ivec2 a) -> int64_t { return a.x*a.y; }

auto operator<<(ostream& s, const ivec2& i) -> ostream& { return s << "(" << i.x << "," << i.y << ")"; }

namespace std {
	template <>
	struct hash<ivec2> {
		auto operator()(const ivec2& a) const -> std::size_t {
			return hash<int>()(a.x) ^ hash<int>()(a.y); }};}

auto LRUD(char ch) -> ivec2 {
	switch (ch) {
	case 'L': return ivec2{ -1, 0 };
	case 'R': return ivec2{  1, 0 };
	case 'U': return ivec2{  0,-1 };
	case 'D': return ivec2{  0, 1 }; }
	std::cerr << "wtf?" << std::endl;
	std::exit(1); }


class IntCodeMachine {
	enum State {
		Normal=0,
		Halted,
		Input };
	
	std::vector<int64_t> mem_;
	int pc_{0}, bp_{0};
	int opcode_, mode_[4];
	optional<int64_t> lastOut_{};
	optional<int64_t> lastIn_{};
	State state_{State::Normal};

public:
	IntCodeMachine(decltype(IntCodeMachine::mem_) m) :
		mem_(m) {}

private:
	void Decode(int64_t op) {
		opcode_ = op%100;
		op /= 100;
		mode_[1] = op%10; op/=10;
		mode_[2] = op%10; op/=10;
		mode_[3] = op%10; op/=10; }

public:
	void Recv(int64_t x) {
		lastIn_ = x; }

	auto IsHalted() const -> bool {
		return state_ == State::Halted; }

	auto IsWaiting() const -> bool {
		return state_ == State::Input; }

	auto LoadValue(int n) -> int64_t {
		auto v = Load(pc_+n);
		if (mode_[n] == 0) {
			v = Load(v); }
		else if (mode_[n] == 1) {
			// nothing
			}
		else if (mode_[n] == 2) {
			v = Load(v+bp_); }
		else {
			cerr << "invalid opcode param mode " << mode_[n] << "!";
			std::exit(1); }
		return v; }

	auto LoadAddr(int n) -> int {
		int64_t v = Load(pc_+n);
		if (mode_[n] == 0) {
			// do nothing
			}
		else if (mode_[n] == 1) {
			cerr << "invalid mode for LoadAddr";
			std::exit(1); }
		else if (mode_[n] == 2) {
			v += bp_; }
		else {
			cerr << "invalid opcode param mode " << mode_[n] << "!";
			std::exit(1); }
		return v; }

	void Run() {
		while (1) {
			Decode(Load(pc_));
			// cout << "PC:" << pc_ << " OP:" << opcode_ << " mode:" << mode_[1] << "," << mode_[2] << "," << mode_[3] <<"\n";
			switch (opcode_) {
			case 1 : {
				auto a = LoadValue(1);
				auto b = LoadValue(2);
				auto c = LoadAddr(3);
				Store(c, a+b);
				pc_ += 4; }
				break;
			case 2 : {
				auto a = LoadValue(1);
				auto b = LoadValue(2);
				auto c = LoadAddr(3);
				Store(c, a*b);
				pc_ += 4; }
				break;
			case 3 : 
				if (!lastIn_) {
					state_ = State::Input;
					return; }
				else {
					auto a = LoadAddr(1);
					Store(a, lastIn_.value());
					state_ = State::Normal;
					lastIn_.reset();
					pc_ += 2; }
				break;
			case 4 : {
				auto a = LoadValue(1);
				lastOut_ = a;
				// cerr << "VM out " << a << "\n";
				pc_ += 2;
				return; }
			case 5 :
			case 6 : {
				// 5 = jnz, 6 = jz
				auto a = LoadValue(1);
				auto b = LoadValue(2);
				if ((opcode_==5 && a!=0) || (opcode_==6 && a==0)) {
					pc_ = b; }
				else {
					pc_ += 3; }}
				break;
			case 7 : 
			case 8 : {
				// 7 = cmplt, 8 = cmpeq
				auto a = LoadValue(1);
				auto b = LoadValue(2);
				auto c = LoadAddr(3);
				Store(c, (opcode_==7 && a<b) || (opcode_==8 && a==b));
				pc_ += 4; }
				break;
			case 9 : {
				auto a = LoadValue(1);
				bp_ += a;
				pc_ += 2;}
				break;
			case 99 :
				state_ = State::Halted;
				return;
			default :
				std::cerr << "unknown opcode " << opcode_ << " (pc=" << pc_ << ")\n";
				std::exit(1); }}}

	void DumpMemory() const {
		cout << "===== BEGIN MEMORY DUMP =====\n";
		for (auto el : mem_) {
			cout << el << nl; }
		cout << "====== END MEMORY DUMP ======\n"; }

	auto Load(auto idx) const -> int64_t {
		if (0 <= idx && idx <= mem_.size()) {
			if (idx == 392) {
				// cout << "read 392! replacing " << mem_[392] << " with " << mem_[388] << nl;
				return mem_[388]; }
			return mem_[idx]; }
		else {
			return 0; }}

	void Store(int idx, int64_t value) {
		if (idx < 0) {
			std::cerr << "attempted to store @ " << idx << "\n";
			std::exit(1); }
		if (idx >= mem_.size()) {
			mem_.resize(idx+1, 0); }
		mem_[idx] = value; }

	auto HasOutput() const -> bool {
		return lastOut_.has_value(); }

	auto Out() -> int64_t {
		auto v = *lastOut_;
		lastOut_.reset();
		return v; } };

// dir=1 = counter-clockwise
// dir=-1 = clockwise
auto rot90(ivec2 p, int t) -> ivec2 {
	return { -p.y*t, p.x*t };}

auto botrot(ivec2 dir, int value) -> ivec2 {
	return rot90(dir, value==0?1:-1); }


int main() {
	std::string tmp;
	getline(std::cin, tmp);

	auto mem = SplitNums(tmp);

	umap<ivec2, char> map;
	ivec2 bMax{-oo,-oo };

	auto vm = IntCodeMachine(mem);
	while (1) {
		vm.Run();
		if (vm.IsHalted()) {
			cerr << "<vm> halted" << nl;
			break; }

		auto px = vm.Out();
		vm.Run();
		auto py = vm.Out();
		vm.Run();
		auto tileId = vm.Out();

		auto pos = ivec2(px, py);
		bMax = vmax(bMax, pos);
		map[pos] = tileId; }
	auto bEnd = bMax + ivec2{1,1};

	constexpr int T_EMPTY = 0;
	constexpr int T_WALL = 1;
	constexpr int T_BLOCK = 2;
	constexpr int T_PADDLE = 3;
	constexpr int T_BALL = 4;

	int p1{0};
	for (int y=0; y<bEnd.y; ++y) {
		for (int x=0; x<bEnd.x; ++x) {
			int tid;
			if (auto found = map.find(ivec2{ x, y }); found!=end(map)) {
				tid = found->second; }
			else {
				tid = 0; }
			p1 += (tid==T_BLOCK);
			cout << tid;}
		cout << "\n"; }
	cout << "p1: " << p1 << nl;

	vm = IntCodeMachine(mem);
	vm.Store(0, 2); // play for free
	map.clear();
	int score{0};
	int frameId{0};
	while (1) {
		// cerr << "run...\n";
		vm.Run();
		if (vm.IsHalted()) {
			cerr << "<vm> halted" << nl;
			break; }
		if (vm.IsWaiting()) {
			cerr << "f#" << frameId << nl;

			/*
	for (int y=0; y<bEnd.y; ++y) {
		for (int x=0; x<bEnd.x; ++x) {
			int tid;
			if (auto found = map.find(ivec2{ x, y }); found!=end(map)) {
				tid = found->second; }
			else {
				tid = 0; }
			cout << tid;}
		cout << "\n"; }*/

			frameId++;
			vm.Recv(1); // neutral input won't cause padle update
			// map.clear();
			continue;
		}
		int i0 = vm.Out(); vm.Run();
		int i1 = vm.Out(); vm.Run();
		int i2 = vm.Out();
		if (i0==-1 && i1==0) {
			cout << "score: " << i2 << nl;
			score = i2; }
		else {
			map[ivec2(i0, i1)] = i2; }}
		//cerr << "[" << i0 << ", " << i1 << ", " << i2 << "]\n"; }


	// vm.DumpMemory();


	return 0; }
