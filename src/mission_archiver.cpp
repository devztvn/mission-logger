#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

using namespace std;

static bool read_first_last_t(const filesystem::path &telemetry_path, string &first_t, string &last_t, long long &count)
{
    ifstream in(telemetry_path);
    if (!in)
    {
        return false;
    }

    string line;
    count = 0;
    while (getline(in, line))
    {
        auto pos = line.find("\"t\":\"");
        if (pos == string::npos)
        {
            continue;
        }
        pos += 5;
        auto end = line.find("\"", pos);
        if (end == string::npos)
        {
            continue;
        }
        string t = line.substr(pos, end - pos);
        if (count == 0)
        {
            first_t = t;
        }
        last_t = t;
        count++;
    }
    return count > 0;
}

static int run_cmd(const string &cmd)
{
    return system(cmd.c_str());
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "usage: mission_archiver <mission_dir> [--result success|fail] [--notes text]\n";
        return 1;
    }

    filesystem::path mission_dir = argv[1];
    string result = "unknown";
    string notes = "";

    for (int i = 2; i < argc; i++)
    {
        string a = argv[i];
        if (a == "--result" && i + 1 < argc)
        {
            result = argv[++i];
        }
        else if (a == "--notes" && i + 1 < argc)
        {
            notes = argv[++i];
        }
    }

    filesystem::path telemetry_path = mission_dir / "telemetry.jsonl";
    filesystem::path meta_path = mission_dir / "metadata.json";

    if (!filesystem::exists(mission_dir) || !filesystem::is_directory(mission_dir))
    {
        cout << "mission_dir not found\n";
        return 1;
    }

    if (!filesystem::exists(telemetry_path))
    {
        cout << "missing telemetry.jsonl\n";
        return 1;
    }

    string start_time, end_time;
    long long samples = 0;

    if (!read_first_last_t(telemetry_path, start_time, end_time, samples))
    {
        cout << "failed to parse telemetry timestamps\n";
        return 1;
    }

    string mission_id = mission_dir.filename().string();

    ofstream meta(meta_path);
    meta << "{\n";
    meta << "  \"mission_id\": \"" << mission_id << "\",\n";
    meta << "  \"start_time\": \"" << start_time << "\",\n";
    meta << "  \"end_time\": \"" << end_time << "\",\n";
    meta << "  \"result\": \"" << result << "\",\n";
    meta << "  \"samples\": " << samples << ",\n";
    meta << "  \"notes\": \"" << notes << "\"\n";
    meta << "}\n";
    meta.close();

    filesystem::create_directories("archives");

    string archive_name = "archives/" + mission_id + ".tar.gz";
    string cmd = "tar -czf \"" + archive_name + "\" -C \"" + mission_dir.parent_path().string() + "\" \"" + mission_id + "\"";

    int rc = run_cmd(cmd);
    if (rc != 0)
    {
        cout << "tar failed\n";
        return 1;
    }

    cout << "wrote:\n";
    cout << "  " << meta_path.string() << "\n";
    cout << "archived:\n";
    cout << "  " << archive_name << "\n";

    return 0;
}

