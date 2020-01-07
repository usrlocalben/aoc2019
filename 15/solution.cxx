#include <algorithm>
#include <array>
#include <cassert>
#include <deque>
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

array<ivec2, 4> NSWE = {{
	// north    south        west         east
	ivec2{0,1}, ivec2{0,-1}, ivec2{-1,0}, ivec2{1,0} }};

array<string, 4> NSWEname = {{ "N", "S", "W", "E" }};


int main() {
	std::string tmp;
	getline(std::cin, tmp);

	auto mem = SplitNums(tmp);

	ivec2 bMin{ 0, 0};
	ivec2 bMax{ 0, 0};
	umap<ivec2, char> map;
	ivec2 pos{ 0, 0 };
	map[pos] = 1;
	optional<ivec2> oxygenPos{};

	constexpr char T_UNKNOWN = 0;
	constexpr char T_NORMAL = 1;
	constexpr char T_WALL = 2;

	auto DumpMap = [&]() {
		for (int yy=bMax.y; yy>=bMin.y; --yy) {
			for (int xx=bMin.x; xx<=bMax.x; ++xx) {
				auto ppp = ivec2{xx,yy};
				if (pos == ppp) {
					cout << "D"; }
				else if (oxygenPos.has_value() && oxygenPos.value()==ppp) {
					cout << "O"; }
				else {
					auto t = map[ppp];
					if (t == T_UNKNOWN) {
						cout << "?"; }
					else if (t == T_NORMAL) {
						cout << " "; }
					else {
						cout << "#"; }}}
			cout << "\n"; }};

	auto vm = IntCodeMachine(mem);

	uset<ivec2> visited;
	umap<ivec2, pair<ivec2, char>> steps;
	deque<pair<ivec2, int>> queue;
	vector<char> path;
	while (1) {
		// DumpMap();
		// cout << "----------------------------\n";
				

		visited.clear();
		steps.clear();
		queue.clear();
		queue.push_back({ pos, 0 });
		optional<ivec2> target{};
		while (!queue.empty()) {
			auto [p, dist] = queue.front(); queue.pop_front();
			if (visited.find(p) != end(visited)) continue;
			visited.insert(p);
			if (map[p] == 0) {
				target = p;
				break; }
			for (int di{0}; di<4; ++di) {
				auto pp = p + NSWE[di];
				if (map[pp] == T_WALL) continue;
				if (visited.find(pp) != end(visited)) continue;
				steps[pp] = { p, di };
				queue.push_back({ pp, dist+1 }); }}

		if (!target.has_value()) {
			cout << "search complete\n";
			break; }

		ivec2 t = *target;
		// cout << "target is " << t << nl;
		path.clear();
		while (t != pos) {
			// cout << steps[t].first << ", " << NSWEname[steps[t].second] << "\n";
			path.push_back(steps[t].second);
			t = steps[t].first; }

		for (int pi=path.size()-1; pi>=0; --pi) {
			auto nextPos = pos + NSWE[path[pi]];
			bMin = vmin(bMin, nextPos);
			bMax = vmax(bMax, nextPos);
			vm.Recv(path[pi] + 1);  // NSWE 1234
			vm.Run();
			int status = vm.Out();
			switch (status) {
				case 0 :
					assert(pi==0);
					map[nextPos] = T_WALL;
					break;
				case 2 :
					oxygenPos = nextPos;
				case 1 :
					map[nextPos] = T_NORMAL;
					pos = nextPos;
					break; }}}

	if (oxygenPos) {
		cout << "oxygen at " << oxygenPos.value() << nl; }

	DumpMap();

	visited.clear();
	queue.clear();
	queue.push_back({ ivec2{0,0}, 0 });
	while (!queue.empty()) {
		auto [p, dist] = queue.front(); queue.pop_front();
		if (visited.find(p) != end(visited)) continue;
		visited.insert(p);
		if (p == oxygenPos.value()) {
			cout << "p1: " << dist << "\n";
			break; }
		for (int di{0}; di<4; ++di) {
			auto pp = p + NSWE[di];
			if (map[pp] == T_WALL) continue;
			if (visited.find(pp) != end(visited)) continue;
			steps[pp] = { p, di };
			queue.push_back({ pp, dist+1 }); }}

	visited.clear();
	queue.clear();
	queue.push_back({ oxygenPos.value(), 0 });
	int maxDist{-oo};
	while (!queue.empty()) {
		auto [p, dist] = queue.front(); queue.pop_front();
		if (visited.find(p) != end(visited)) continue;
		visited.insert(p);
		maxDist = std::max(maxDist, dist);
		for (int di{0}; di<4; ++di) {
			auto pp = p + NSWE[di];
			if (map[pp] == T_WALL) continue;
			if (visited.find(pp) != end(visited)) continue;
			steps[pp] = { p, di };
			queue.push_back({ pp, dist+1 }); }}

	cout << "p2: " << maxDist << nl;
	return 0; }
