#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>

using namespace std;

static string now_iso()
{
    using namespace chrono;
    auto tp = system_clock::now();
    auto t = system_clock::to_time_t(tp);
    auto ms = duration_cast<milliseconds>(tp.time_since_epoch()) % 1000;

    tm tmv{};
    localtime_r(&t, &tmv);

    stringstream ss;
    ss << put_time(&tmv, "%Y-%m-%dT%H:%M:%S") << "."
       << setw(3) << setfill('0') << ms.count();
    return ss.str();
}

static string make_mission_id()
{
    using namespace chrono;
    auto tp = system_clock::now();
    auto t = system_clock::to_time_t(tp);

    tm tmv{};
    localtime_r(&t, &tmv);

    stringstream ss;
    ss << "mission_" << put_time(&tmv, "%Y-%m-%d_%H%M%S");
    return ss.str();
}

static vector<pair<string, string>> parse_kv_line(const string &line)
{
    vector<pair<string, string>> kvs;
    string token;
    stringstream ss(line);

    while (ss >> token)
    {
        auto pos = token.find('=');
        if (pos == string::npos)
        {
            continue;
        }

        string k = token.substr(0, pos);
        string v = token.substr(pos + 1);
        if (!k.empty() && !v.empty())
        {
            kvs.push_back({k, v});
        }
    }
    return kvs;
}

static bool looks_number(const string &s)
{
    bool dot = false;
    size_t i = 0;

    if (s.empty())
    {
        return false;
    }

    if (s[0] == '-' || s[0] == '+')
    {
        i = 1;
        if (i >= s.size())
        {
            return false;
        }
    }

    for (; i < s.size(); i++)
    {
        if (s[i] == '.')
        {
            if (dot)
            {
                return false;
            }
            dot = true;
            continue;
        }
        if (!isdigit((unsigned char)s[i]))
        {
            return false;
        }
    }
    return true;
}

int main(int argc, char **argv)
{
    string out_root = "missions";
    string mission_id = make_mission_id();

    filesystem::path mission_dir = filesystem::path(out_root) / mission_id;
    filesystem::create_directories(mission_dir);

    filesystem::path telemetry_path = mission_dir / "telemetry.jsonl";
    filesystem::path events_path = mission_dir / "events.log";

    ofstream telemetry(telemetry_path);
    ofstream events(events_path);

    string start_time = now_iso();

    cout << "mission_id: " << mission_id << "\n";
    cout << "type telemetry lines like: speed=1.2 battery=87 x=12.4 y=-3.1\n";
    cout << "type: END to finish\n";

    events << now_iso() << " INFO mission_start id=" << mission_id << "\n";

    string line;
    long long samples = 0;

    while (true)
    {
        cout << "> ";
        if (!getline(cin, line))
        {
            break;
        }

        if (line == "END")
        {
            break;
        }

        auto kvs = parse_kv_line(line);
        if (kvs.empty())
        {
            events << now_iso() << " WARN ignored_line\n";
            continue;
        }

        string t = now_iso();

        telemetry << "{";
        telemetry << "\"t\":\"" << t << "\"";

        for (auto &kv : kvs)
        {
            telemetry << ",\"" << kv.first << "\":";
            if (looks_number(kv.second))
            {
                telemetry << kv.second;
            }
            else
            {
                telemetry << "\"" << kv.second << "\"";
            }
        }

        telemetry << "}\n";
        telemetry.flush();

        samples++;
    }

    string end_time = now_iso();
    events << now_iso() << " INFO mission_end id=" << mission_id << " samples=" << samples << "\n";

    cout << "saved:\n";
    cout << "  " << telemetry_path.string() << "\n";
    cout << "  " << events_path.string() << "\n";
    cout << "samples: " << samples << "\n";
    cout << "start: " << start_time << "\n";
    cout << "end:   " << end_time << "\n";

    return 0;
}

