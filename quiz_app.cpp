// quiz_app.cpp
// Self-contained console quiz app (no external libs).
// Save as quiz_app.cpp

#include <bits/stdc++.h>
using namespace std;
using ll = long long;

struct Question { string text; array<string,4> opts; int correct; };
struct LBEntry { string name; int score; int total; string date; };

string trim(const string &s){
    size_t a=s.find_first_not_of(" \t\r\n"); if(a==string::npos) return "";
    size_t b=s.find_last_not_of(" \t\r\n"); return s.substr(a,b-a+1);
}

// Load questions from a simple text format:
// each line: question|||opt1|||opt2|||opt3|||opt4|||correctIndex
vector<Question> load_questions(const string &fname="questions.txt"){
    vector<Question> list;
    ifstream ifs(fname);
    if(!ifs) return list;
    string line;
    while(getline(ifs,line)){
        if(trim(line).empty()) continue;
        vector<string> parts;
        size_t pos=0;
        while(true){
            size_t p=line.find("|||",pos);
            if(p==string::npos){ parts.push_back(line.substr(pos)); break; }
            parts.push_back(line.substr(pos,p-pos));
            pos = p+3;
        }
        if(parts.size()<6) continue;
        Question q; q.text = parts[0];
        for(int i=0;i<4;i++) q.opts[i] = parts[1+i];
        try{ q.correct = stoi(parts[5]); } catch(...) { q.correct = 0; }
        list.push_back(q);
    }
    return list;
}

bool save_questions(const vector<Question> &list, const string &fname="questions.txt"){
    ofstream ofs(fname);
    if(!ofs) return false;
    for(auto &q: list){
        string qt=q.text; for(char &c:qt) if(c=='\n' || c=='\r') c=' ';
        ofs<<qt;
        for(int i=0;i<4;i++){ string o=q.opts[i]; for(char &c:o) if(c=='\n' || c=='\r') c=' '; ofs<<"|||"<<o; }
        ofs<<"|||"<<q.correct<<"\n";
    }
    return true;
}

vector<LBEntry> load_leaderboard(const string &fname="leaderboard.txt"){
    vector<LBEntry> v; ifstream ifs(fname); if(!ifs) return v;
    string line;
    while(getline(ifs,line)){
        if(trim(line).empty()) continue;
        vector<string> parts; size_t pos=0;
        while(true){
            size_t p=line.find("|||",pos);
            if(p==string::npos){ parts.push_back(line.substr(pos)); break; }
            parts.push_back(line.substr(pos,p-pos)); pos = p+3;
        }
        if(parts.size()<4) continue;
        LBEntry e; e.name = parts[0];
        try{ e.score = stoi(parts[1]); } catch(...) { e.score = 0; }
        try{ e.total = stoi(parts[2]); } catch(...) { e.total = 0; }
        e.date = parts[3];
        v.push_back(e);
    }
    return v;
}

bool save_leaderboard(const vector<LBEntry> &v, const string &fname="leaderboard.txt"){
    ofstream ofs(fname);
    if(!ofs) return false;
    for(auto &e: v){
        string n=e.name; for(char &c:n) if(c=='\n' || c=='\r') c=' ';
        ofs<<n<<"|||"<<e.score<<"|||"<<e.total<<"|||"<<e.date<<"\n";
    }
    return true;
}

string now_str(){
    auto t = chrono::system_clock::now();
    time_t tt = chrono::system_clock::to_time_t(t);
    char buf[64];
    strftime(buf,sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&tt));
    return string(buf);
}

string safe_getline(){ string s; getline(cin,s); return s; }

// non-blocking read with timeout using std::async
bool readline_with_timeout(string &out, int seconds){
    auto fut = async(launch::async, [](){
        string s; getline(cin,s); return s;
    });
    if(fut.wait_for(chrono::seconds(seconds)) == future_status::ready){
        out = fut.get();
        return true;
    }
    return false;
}

void show_instructions(){
    cout<<"\nInstructions:\n";
    cout<<" - Each correct answer gives 1 point.\n";
    cout<<" - Enter option number (1-4) and press Enter.\n";
    cout<<" - If time runs out, the question is marked wrong.\n\n";
}

void view_leaderboard(){
    auto lb = load_leaderboard();
    if(lb.empty()){ cout<<"\nNo leaderboard entries yet.\n"; return; }
    cout<<"\nLeaderboard:\n";
    int i=1; for(auto &e:lb) cout<<i++<<". "<<e.name<<" - "<<e.score<<" / "<<e.total<<" ("<<e.date<<")\n";
    cout<<"\n";
}

void manage_questions(){
    while(true){
        cout<<"\nManage Questions:\n1. List\n2. Add\n3. Delete\n4. Clear all\n0. Back\nChoice: ";
        string ch = safe_getline();
        if(ch=="1"){
            auto qs = load_questions();
            if(qs.empty()){ cout<<"No questions.\n"; continue; }
            int i=1; for(auto &q:qs){
                cout<<i++<<". "<<q.text<<"\n";
                for(int k=0;k<4;k++) cout<<"   "<<(k+1)<<") "<<q.opts[k]<<(k==q.correct?" [correct]":"")<<"\n";
            }
        } else if(ch=="2"){
            Question q; cout<<"Enter question text: "; q.text = safe_getline();
            for(int i=0;i<4;i++){ cout<<"Option "<<(i+1)<<": "; q.opts[i]=safe_getline(); }
            cout<<"Correct option number (1-4): "; string c = safe_getline();
            int ci=1; try{ ci = stoi(c); } catch(...) { ci=1; } if(ci<1||ci>4) ci=1;
            q.correct = ci-1;
            auto qs = load_questions();
            qs.push_back(q);
            save_questions(qs);
            cout<<"Added.\n";
        } else if(ch=="3"){
            auto qs = load_questions();
            if(qs.empty()){ cout<<"No questions.\n"; continue; }
            cout<<"Enter question number to delete: "; string s = safe_getline();
            int id=0; try{id=stoi(s);}catch(...){id=0;}
            if(id<=0 || id>(int)qs.size()){ cout<<"Invalid.\n"; continue; }
            qs.erase(qs.begin()+id-1);
            save_questions(qs);
            cout<<"Deleted.\n";
        } else if(ch=="4"){
            cout<<"Clear ALL questions? (y/n): "; string s = safe_getline();
            if(!s.empty() && (s[0]=='y' || s[0]=='Y')){ save_questions(vector<Question>()); cout<<"Cleared.\n"; }
        } else if(ch=="0") break;
        else cout<<"Invalid.\n";
    }
}

void import_export(){
    cout<<"\n1. Export questions to file\n2. Import from file\n0. Back\nChoice: ";
    string ch = safe_getline();
    if(ch=="1"){
        auto qs = load_questions();
        cout<<"Enter export filename (e.g. export.txt): ";
        string fn = safe_getline(); if(fn.empty()) fn="export.txt";
        if(save_questions(qs, fn)) cout<<"Exported to "<<fn<<"\n"; else cout<<"Failed.\n";
    } else if(ch=="2"){
        cout<<"Enter import filename: ";
        string fn = safe_getline();
        ifstream ifs(fn);
        if(!ifs){ cout<<"Failed to open.\n"; return; }
        vector<Question> incoming; string line;
        while(getline(ifs,line)){
            if(trim(line).empty()) continue;
            vector<string> parts; size_t pos=0;
            while(true){
                size_t p=line.find("|||",pos);
                if(p==string::npos){ parts.push_back(line.substr(pos)); break; }
                parts.push_back(line.substr(pos,p-pos)); pos = p+3;
            }
            if(parts.size()<6) continue;
            Question q; q.text = parts[0];
            for(int i=0;i<4;i++) q.opts[i]=parts[1+i];
            try{ q.correct = stoi(parts[5]); } catch(...) { q.correct = 0; }
            incoming.push_back(q);
        }
        if(incoming.empty()){ cout<<"No valid items.\n"; return; }
        auto qs = load_questions(); qs.insert(qs.end(), incoming.begin(), incoming.end()); save_questions(qs);
        cout<<"Imported "<<incoming.size()<<" questions.\n";
    }
}

void play_quiz(){
    auto qs = load_questions();
    if(qs.empty()){ cout<<"No questions. Add some first.\n"; return; }
    cout<<"Number of questions to attempt (max "<<qs.size()<<", default 5): ";
    string s = safe_getline(); int num=5; try{ if(!s.empty()) num=stoi(s); } catch(...) { num=5; }
    if(num<=0) num=5; if(num>(int)qs.size()) num = qs.size();
    cout<<"Time per question in seconds (default 20): ";
    s = safe_getline(); int t=20; try{ if(!s.empty()) t=stoi(s); } catch(...) { t=20; }
    cout<<"Enter your name: "; string player = safe_getline(); if(player.empty()) player="Anonymous";
    vector<int> idx(qs.size()); iota(idx.begin(), idx.end(), 0);
    random_device rd; mt19937 g(rd()); shuffle(idx.begin(), idx.end(), g);
    idx.resize(num);
    int score=0;
    for(int i=0;i<num;i++){
        const auto &q = qs[idx[i]];
        cout<<"\nQuestion "<<(i+1)<<" / "<<num<<"\n";
        cout<<q.text<<"\n";
        for(int j=0;j<4;j++) cout<<"  "<<(j+1)<<". "<<q.opts[j]<<"\n";
        cout<<"Answer (1-4). You have "<<t<<"s: ";
        string ans; bool got = readline_with_timeout(ans, t);
        if(!got){ cout<<"\nTime's up!\n"; continue; }
        ans = trim(ans);
        int a=0; try{ a = stoi(ans); } catch(...) { a=0; }
        if(a-1 == q.correct){ cout<<"Correct!\n"; score++; } else { cout<<"Wrong. Correct: "<<(q.correct+1)<<". "<<q.opts[q.correct]<<"\n"; }
    }
    cout<<"\nQuiz finished. Score: "<<score<<" / "<<num<<"\n";
    cout<<"Save to leaderboard? (y/n): ";
    string c = safe_getline();
    if(!c.empty() && (c[0]=='y' || c[0]=='Y')){
        auto lb = load_leaderboard(); LBEntry e{player, score, num, now_str()}; lb.push_back(e);
        sort(lb.begin(), lb.end(), [](const LBEntry &a, const LBEntry &b){
            if(a.score != b.score) return a.score > b.score; return a.date > b.date;
        });
        save_leaderboard(lb);
        cout<<"Saved.\n";
    }
}

int main(){
    // If questions.txt doesn't exist, create 3 default questions
    ifstream t("questions.txt");
    if(!t){
        vector<Question> def;
        Question q1; q1.text="Which constructor is called when an object is passed by value to a function?"; q1.opts={"Default constructor","Parameterized constructor","Copy constructor","Move constructor"}; q1.correct=1;
        Question q2; q2.text="Which language is primarily used for AI development?"; q2.opts={"Python","C","Java","C++"}; q2.correct=0;
        Question q3; q3.text="Who developed C++?"; q3.opts={"Dennis Ritchie","James Gosling","Bjarne Stroustrup","Guido van Rossum"}; q3.correct=2;
        def.push_back(q1); def.push_back(q2); def.push_back(q3);
        save_questions(def);
    }
    cout<<"Online Quiz Game (C++ console) — ready\n";
    while(true){
        cout<<"\nMenu:\n1. Play Quiz\n2. Instructions\n3. Leaderboard\n4. Manage Questions\n5. Import/Export\n0. Exit\nChoice: ";
        string ch = safe_getline();
        if(ch=="1") play_quiz();
        else if(ch=="2") show_instructions();
        else if(ch=="3") view_leaderboard();
        else if(ch=="4") manage_questions();
        else if(ch=="5") import_export();
        else if(ch=="0") { cout<<"Goodbye.\n"; break; }
        else cout<<"Invalid.\n";
    }
    return 0;
}
