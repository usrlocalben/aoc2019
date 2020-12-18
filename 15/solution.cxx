#include "../lib.hpp"
using namespace std;

class IntCodeMachine {
	enum State {
		Normal=0,
		Halted,
		Input };
	
	vector<int64_t> mem_;
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
			exit(1); }
		return v; }

	auto LoadAddr(int n) -> int {
		int64_t v = Load(pc_+n);
		if (mode_[n] == 0) {
			// do nothing
			}
		else if (mode_[n] == 1) {
			cerr << "invalid mode for LoadAddr";
			exit(1); }
		else if (mode_[n] == 2) {
			v += bp_; }
		else {
			cerr << "invalid opcode param mode " << mode_[n] << "!";
			exit(1); }
		return v; }

	void Run() {
		while (1) {
			Decode(Load(pc_));
			// cerr << "PC:" << pc_ << " OP:" << opcode_ << " mode:" << mode_[1] << "," << mode_[2] << "," << mode_[3] <<"\n";
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
				cerr << "unknown opcode " << opcode_ << " (pc=" << pc_ << ")\n";
				exit(1); }}}

	void DumpMemory() const {
		cerr << "===== BEGIN MEMORY DUMP =====\n";
		for (auto el : mem_) {
			cerr << el << nl; }
		cerr << "====== END MEMORY DUMP ======\n"; }

	auto Load(int idx) const -> int64_t {
		if (0 <= idx && idx <= mem_.size()) {
			if (idx == 392) {
				// cerr << "read 392! replacing " << mem_[392] << " with " << mem_[388] << nl;
				return mem_[388]; }
			return mem_[idx]; }
		else {
			return 0; }}

	void Store(int idx, int64_t value) {
		if (idx < 0) {
			cerr << "attempted to store @ " << idx << "\n";
			exit(1); }
		if (idx >= mem_.size()) {
			mem_.resize(idx+1, 0); }
		mem_[idx] = value; }

	auto HasOutput() const -> bool {
		return lastOut_.has_value(); }

	auto Out() -> int64_t {
		auto v = *lastOut_;
		lastOut_.reset();
		return v; } };


array<Int2, 4> NSWE = {{
	// north    south        west         east
	Int2{0,1}, Int2{0,-1}, Int2{-1,0}, Int2{1,0} }};

array<string, 4> NSWEname = {{ "N", "S", "W", "E" }};


int main() {
	string tmp;
	getline(cin, tmp);

	auto mem = SplitNums(tmp);

	Int2 bMin{ 0, 0};
	Int2 bMax{ 0, 0};
	umap<Int2, char> map;
	Int2 pos{ 0, 0 };
	map[pos] = 1;
	optional<Int2> oxygenPos{};

	constexpr char T_UNKNOWN = 0;
	constexpr char T_NORMAL = 1;
	constexpr char T_WALL = 2;

	auto DumpMap = [&]() {
		for (int yy=bMax.y; yy>=bMin.y; --yy) {
			for (int xx=bMin.x; xx<=bMax.x; ++xx) {
				auto ppp = Int2{xx,yy};
				if (pos == ppp) {
					cerr << "D"; }
				else if (oxygenPos.has_value() && oxygenPos.value()==ppp) {
					cerr << "O"; }
				else {
					auto t = map[ppp];
					if (t == T_UNKNOWN) {
						cerr << "?"; }
					else if (t == T_NORMAL) {
						cerr << " "; }
					else {
						cerr << "#"; }}}
			cerr << "\n"; }};

	auto vm = IntCodeMachine(mem);

	uset<Int2> visited;
	umap<Int2, pair<Int2, char>> steps;
	deque<pair<Int2, int>> queue;
	vector<char> path;
	while (1) {
		// DumpMap();
		// cerr << "----------------------------\n";
				

		visited.clear();
		steps.clear();
		queue.clear();
		queue.push_back({ pos, 0 });
		optional<Int2> target{};
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
			// cerr << "search complete\n";
			break; }

		Int2 t = *target;
		// cerr << "target is " << t << nl;
		path.clear();
		while (t != pos) {
			// cerr << steps[t].first << ", " << NSWEname[steps[t].second] << "\n";
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

	if (0) {// && oxygenPos) {
		cerr << "oxygen at " << oxygenPos.value() << nl; }

	// DumpMap();

	visited.clear();
	queue.clear();
	queue.push_back({ Int2{0,0}, 0 });
	while (!queue.empty()) {
		auto [p, dist] = queue.front(); queue.pop_front();
		if (visited.find(p) != end(visited)) continue;
		visited.insert(p);
		if (p == oxygenPos.value()) {
			cout << dist << "\n";
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
		maxDist = max(maxDist, dist);
		for (int di{0}; di<4; ++di) {
			auto pp = p + NSWE[di];
			if (map[pp] == T_WALL) continue;
			if (visited.find(pp) != end(visited)) continue;
			steps[pp] = { p, di };
			queue.push_back({ pp, dist+1 }); }}

	cout << maxDist << nl;
	return 0; }
