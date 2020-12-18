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
	deque<int64_t> inq_{};
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
		inq_.push_back(x); }

	void Recv(string s) {
		for (auto ch : s) {
			inq_.push_back(ch); }}

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
				if (inq_.empty()) {
					state_ = State::Input;
					return; }
				else {
					auto a = LoadAddr(1);
					int64_t v = inq_.front(); inq_.pop_front();
					// cerr << "<S:" << (char)v << ">" << flush;
					Store(a, v);
					state_ = State::Normal;
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
	// Int2{0,1}, Int2{0,-1}, Int2{-1,0}, Int2{1,0} }};  
	Int2{0,-1}, Int2{0,1}, Int2{-1,0}, Int2{1,0} }};

array<string, 4> NSWEname = {{ "N", "S", "W", "E" }};

array<char, 4> UDLRchar = {{ '^', 'v', '<', '>' }};

auto decodeUDLR(char ch) -> optional<int> {
	auto found = find(begin(UDLRchar), end(UDLRchar), ch);
	if (found == end(UDLRchar)) {
		return {}; }
	return distance(begin(UDLRchar), found); }

int main() {
	string tmp;
	getline(cin, tmp);

	auto mem = SplitNums(tmp);

	Int2 bMin{ 0, 0};
	Int2 bMax{ 0, 0};


	auto vm = IntCodeMachine(mem);
	vector<string> map;
	Int2 dPos, dDir;
	map.emplace_back();

	while (!vm.IsHalted()) {
		auto& dst = map.back();

		vm.Run();
		if (vm.HasOutput()) {
			auto ch = static_cast<char>(vm.Out());
			if (ch == '\n') {
				// cerr << "[" << map.back() << "]\n";
				map.emplace_back(); }
			else {
				if (auto bot = decodeUDLR(ch); bot) {
					dPos = Int2( dst.size(), map.size()-1 );
					dDir = NSWE[*bot];
					ch = '#'; }
				dst += ch; }}}
	map.pop_back(); // trailing newline

	const int h = map.size();
	const int w = map[0].size();

	// cerr << dPos;
	// cerr << dDir;

	auto GetPixel = [&](Int2 c) -> char {
		return map[c.y][c.x]; };

	int ax{0};
	for (int y=1; y<h-1; ++y) {
		for (int x=1; x<w-1; ++x) {
			const auto coord = Int2{ x, y };
			if (GetPixel(coord) != '#') continue;
			auto n = GetPixel(coord + Int2{ 0,-1});
			auto s = GetPixel(coord + Int2{ 0, 1});
			auto w = GetPixel(coord + Int2{-1, 0});
			auto e = GetPixel(coord + Int2{ 1, 0});
			if (n=='#' && s=='#' && w=='#' && e=='#') {
				ax += coord.x*coord.y; }}}
	cout << ax << nl;

	vm = IntCodeMachine(mem);
	vm.Store(0, 2);
	vm.Recv("A,B,A,C,B,A,B,C,C,B\n");
	vm.Recv("L,12,L,12,R,4\n");
	vm.Recv("R,10,R,6,R,4,R,4\n");
	vm.Recv("R,6,L,12,L,12\n");
	vm.Recv("n\n");
	// cerr << "begin output loop" << endl;
	while (!vm.IsHalted()) {
		vm.Run();
		if (vm.IsWaiting()) {
			// cerr << "waiting for input?" << endl;
			exit(1); }
		if (vm.HasOutput()) {
			char ch = vm.Out();
			/*cerr << ch;*/ }}

	cout << vm.Out() << nl;

	return 0; }
