// Custom Shell Implementation (Capstone Assignment 2)
// Compile: g++ -std=c++17 -Wall -Wextra myshell.cpp -o myshell

#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

using namespace std;

// Job structure
struct Job {
    int id;
    pid_t pid;
    string cmd;
    bool running;
};

static vector<Job> jobs;
static int next_job_id = 1;
static volatile sig_atomic_t child_changed = 0;

void sigchld_handler(int) { child_changed = 1; }

void trim(string &s) {
    size_t p = s.find_last_not_of(" \t\r\n");
    if (p != string::npos) s.erase(p + 1);
    size_t q = s.find_first_not_of(" \t\r\n");
    if (q != string::npos) s.erase(0, q);
    else s.clear();
}

vector<string> split_ws(const string &s) {
    vector<string> out;
    string cur;
    bool in_quote = false;
    char quote_char = '\0';
    for (char c : s) {
        if (!in_quote && (c == '"' || c == '\'')) { in_quote = true; quote_char = c; continue; }
        if (in_quote && c == quote_char) { in_quote = false; continue; }
        if (!in_quote && isspace((unsigned char)c)) {
            if (!cur.empty()) { out.push_back(cur); cur.clear(); }
        } else cur.push_back(c);
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

void print_prompt() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd))) cout << "[myshell] " << cwd << " $ ";
    else cout << "[myshell] $ ";
    cout.flush();
}

void run_command(vector<string> args, bool background) {
    pid_t pid = fork();
    if (pid == 0) {
        vector<char*> argv;
        for (auto &s : args) argv.push_back(&s[0]);
        argv.push_back(nullptr);
        execvp(argv[0], argv.data());
        perror("execvp");
        _exit(127);
    } else if (pid > 0) {
        if (background) {
            cout << "[" << next_job_id++ << "] " << pid << endl;
        } else {
            waitpid(pid, nullptr, 0);
        }
    } else perror("fork");
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    signal(SIGCHLD, sigchld_handler);

    string line;
    while (true) {
        print_prompt();
        if (!getline(cin, line)) break;
        trim(line);
        if (line.empty()) continue;

        bool background = false;
        if (line.back() == '&') { background = true; line.pop_back(); trim(line); }

        vector<string> args = split_ws(line);
        if (args.empty()) continue;

        if (args[0] == "exit") break;
        else if (args[0] == "cd") {
            if (args.size() > 1) chdir(args[1].c_str());
            continue;
        }
        run_command(args, background);
    }
    return 0;
}
