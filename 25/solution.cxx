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

constexpr int BS_EXPLORE = 0;
constexpr int BS_CRACK = 1;

constexpr int CS_DROP = 0;
constexpr int CS_CHECK = 1;

constexpr char IE_ROOM = 0;
constexpr char IE_NOTHING = 1;

const array<string, 4> nsewStrs = {{ "north", "south", "east", "west" }};

const array<string, 5> badItems = {{
	"escape pod", "giant electromagnet",
	"photons", "infinite loop",
	"molten lava" }};

struct Room {
	array<string, 4> outs = {{ "", "", "", "" }}; };

auto FlipDir(int d) -> int {
	if (d==0) return 1;
	if (d==1) return 0;
	if (d==2) return 3;
	/* (d==3)*/ return 2; }

const array<string, 4> dirNames = {{ "north", "south", "east", "west" }};

struct Bot {
	bool done_{false};
	int result_{-1};
	deque<string> buf_;
	string ax_;

	int state_{0};
	string pm1_{"none"};
	string pos_{""};
	int dir_{-1};
	umap<string, Room> map_;
	int seq_{0};
	char expect_{IE_ROOM};
	vs inv_{};

	// cracking...
	bool crackInit_{false};
	int key_{0};
	char stage_{0};

	string roomId_;
	string roomDescr_;
	vector<int> roomDoors_;
	vs roomItems_;

	auto NextUnknown() -> string {
		return string{"unknown"} + to_string(seq_++); }

	void OnRecv(char ch) {
		if (ch == '\n') {
			// cerr << ">>> " << ax_ << nl;

			if (auto pos = ax_.find("get in by"); pos != string::npos) {
				result_ = stoi(ax_.substr(pos+17));
				done_ = true; }

			buf_.push_back(ax_);
			ax_.clear(); }
		else {
			ax_ += ch; }}

	auto Done() {
		return done_; }

	auto Result() const -> int {
		return result_; }

	auto Command() -> string {

		/*
		if (buf_.back() != "Command?") {
			cerr << "expected Command? but saw \"" << buf_.back() << "\"\n";
			exit(1); }
		*/

		if (expect_ == IE_ROOM) {
			int idx=len(buf_);
			while (buf_[--idx].substr(0,2) != "==");
			roomId_.assign(buf_[idx], 3, len(buf_[idx])-6);  idx++;

			roomDescr_.assign(buf_[idx]);  idx++;

			roomDoors_.clear();
			roomItems_.clear();
			char mode=0;
			for (; buf_[idx]!="Command?"; idx++) {
				if (mode==0) {
					if (buf_[idx].find("Doors here lead")==0) {
						mode = 'D'; }
					else if (buf_[idx].find("Items here")==0) {
						mode = 'I'; }
					else if (buf_[idx] == "") {
						mode = 0; }
					else {
						cerr << "unexpected mode text \"" << buf_[idx] << "\"\n";
						exit(1); }}
				else if (mode == 'D') {
					if (buf_[idx] == "") {
						mode = 0; }
					else {
						char ch = buf_[idx][2];
						int idx = string_view{"nsew"}.find(ch);
						if (idx == string::npos) {
							cerr << "DOOR [" << buf_[idx].substr(2) << "]\n";
							cerr << "unknown door, expected n,s,e,w...\n";
							exit(1); }
						if (!(roomId_ == "Security Checkpoint" && idx==3)) {
							roomDoors_.push_back(idx);} }}
				else if (mode == 'I') {
					if (buf_[idx] == "") {
						mode = 0; }
					else {
						auto item = buf_[idx].substr(2);
						// cerr << "ITEM [" << item << "]\n";
						if (find(begin(badItems), end(badItems), item) == end(badItems)) {
							roomItems_.push_back(buf_[idx].substr(2)); }}}}

			// cerr << "NOW IN " << roomId_ << " (expect " << pos_ << ")\n";
			buf_.clear();

			if (pos_ != roomId_) {
				// cerr << "discovered room " << roomId_ << " (was " << pos_ << ")\n";
				if (dir_ != -1) {
					map_[pm1_].outs[dir_] = roomId_; }
				pos_ = roomId_;
				if (map_.find(roomId_) == end(map_)) {
					// cerr << "added new room \"" << roomId_ << "\" with " << roomDoors_.size() << " doors\n";
					Room r;
					for (auto door : roomDoors_) {
						r.outs[door] = NextUnknown(); }
					if (dir_ != -1) {
						r.outs[FlipDir(dir_)] = pm1_; }
					map_[roomId_] = r; }}

			if (roomItems_.size()) {
				expect_ = IE_NOTHING;
				inv_.push_back(roomItems_.back());
				return string{"take "} + roomItems_.back(); }}
		else if (expect_ == IE_NOTHING) {
			/* do nothing after take, drop etc */
			}
		else {
			cerr << "expect_ bad " << expect_ << nl;
			exit(1); }

		if (state_ == BS_EXPLORE) {
			return Explore(); }
		else if (state_ == BS_CRACK) {
			return Crack(); }
		else {
			cerr << "state not implemented " << state_ << nl;
			exit(1); }}

	auto Explore() -> string {
		auto visited = umap<string, string>{};
		auto queue = deque<pair<string, string>>{};
		queue.push_back({ pos_, "" });
		string found{""};
		while (!queue.empty()) {
			auto [here, prev] = queue.front(); queue.pop_front();
			// cerr << "E " << here << nl;
			if (visited.find(here) != end(visited)) continue;
			visited[here] = prev;

			if (here.find("unknown") != string::npos) {
				found = here;
				break; }

			// if (here == "Security Checkpoint") continue;

			for (const auto& adjRoom : map_[here].outs) {
				if (adjRoom != "") {
					// cerr << " door to " << adjRoom << nl;
					queue.push_back({ adjRoom, here }); }}}

		if (found == "") {
			state_ = BS_CRACK;
			return ""; }

		string pm1{""};
		string p = found;
		while (p!=pos_) { pm1 = p, p=visited[p]; }
		// cerr << "will head to " << pm1 << " from " << p << nl;

		int dir = -1;
		for (int i=0; i<4; ++i) {
			if (map_[pos_].outs[i] == pm1) {
				dir = i;
				break; }}
		if (dir == -1) {
			cerr << "could not find " << pm1 << " from " << pos_ << "!\n";
			exit(1); }

		pm1_ = pos_, pos_ = pm1, dir_ = dir;
		expect_ = IE_ROOM;
		return dirNames[dir]; }

	auto Crack() -> string {
		if (pos_ != "Security Checkpoint") {
			return MoveTowardsCheckpoint(); }

		if (!crackInit_) {
			key_ = 0;
			stage_ = CS_DROP;
			crackInit_ = true; }

		switch (stage_) {
		case CS_DROP: {
			string cmd{};
			for (int b=0; b<8; ++b) {
				if (key_&(1<<b)) {
					cmd += string{"drop "} + inv_[b] + "\n"; }}
			stage_ = CS_CHECK;
			expect_ = IE_ROOM;
			return cmd + "west"; }
		case CS_CHECK: {
			/*if (pos_ != "Security Checkpoint") {
				cerr << "success?";
				exit(1); }*/
			string cmd{};
			for (int b=0; b<8; ++b) {
				if (key_&(1<<b)) {
					cmd += string{"take "} + inv_[b] + "\n"; }}
			cmd = cmd.substr(0,len(cmd)-1);
			expect_ = IE_NOTHING;
			stage_ = CS_DROP;
			key_++;
			return cmd; }}

		return "should never reach here"; }

	auto MoveTowardsCheckpoint() -> string {
		auto visited = umap<string, string>{};
		auto queue = deque<pair<string, string>>{};
		queue.push_back({ pos_, "" });
		string found{""};
		while (!queue.empty()) {
			auto [here, prev] = queue.front(); queue.pop_front();
			if (visited.find(here) != end(visited)) continue;
			visited[here] = prev;

			if (here == "Security Checkpoint") {
				found = here;
				break; }

			for (const auto& adjRoom : map_[here].outs) {
				if (adjRoom != "") {
					queue.push_back({ adjRoom, here }); }}}

		string pm1{""};
		string p = found;
		while (p!=pos_) { pm1 = p, p=visited[p]; }
		int dir = -1;
		for (int i=0; i<4; ++i) {
			if (map_[pos_].outs[i] == pm1) {
				dir = i;
				break; }}
		if (dir == -1) {
			cerr << "could not find " << pm1 << " from " << pos_ << "!\n";
			exit(1); }

		pm1_ = pos_, pos_ = pm1, dir_ = dir;
		expect_ = IE_ROOM;
		return dirNames[dir]; } };


int main() {
	//ifstream inp("input.txt");
	std::string tmp;
	getline(cin, tmp);
	auto rom = SplitNums(tmp);

	IntCodeMachine vm(rom);
	Bot bot;
	while (!bot.Done()) {
		vm.Run();
		if (vm.IsWaiting()) {
			vm.Recv(bot.Command() + "\n");
			}
		if (vm.HasOutput()) {
			bot.OnRecv(vm.Out()); }}

	cout << bot.Result() << nl;
	return 0; }
