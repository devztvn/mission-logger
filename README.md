# Mission Logger

A minimal Linux CLI system for collecting telemetry during a run ("mission") and packaging it into a structured archive.

Built in C++ on Arch Linux.

---

## What this project does

This project consists of two command-line tools:

### 1. telemetry_collector
- Starts a new mission
- Creates a mission directory
- Accepts telemetry input as key=value pairs
- Writes timestamped telemetry to `telemetry.jsonl`
- Logs mission lifecycle events to `events.log`

Each telemetry sample is stored as a single JSON object per line, making it easy to stream, parse, and analyze later.

### 2. mission_archiver
- Reads a completed mission directory
- Extracts mission start time, end time, and sample count
- Generates a `metadata.json` summary
- Compresses the entire mission into a `.tar.gz` archive

This mimics how robotics, simulation, and backend systems package and preserve run data.


## Project structure
```text
mission_logger/
├── src/
│   ├── telemetry_collector.cpp
│   └── mission_archiver.cpp
├── build/
│   ├── telemetry_collector
│   └── mission_archiver
├── missions/
│   └── mission_YYYY-MM-DD_HHMMSS/
│       ├── telemetry.jsonl
│       ├── events.log
│       └── metadata.json
├── archives/
│   └── mission_YYYY-MM-DD_HHMMSS.tar.gz

```

## Build instructions

Compile both tools from the project root:
```bash
g++ -std=c++20 -O2 -Wall -Wextra -pedantic src/telemetry_collector.cpp -o build/telemetry_collector  
g++ -std=c++20 -O2 -Wall -Wextra -pedantic src/mission_archiver.cpp -o build/mission_archiver

```

## Usage

### Run telemetry collection
```bash
./build/telemetry_collector
```
Enter telemetry samples in the terminal:

speed=1.2 battery=87 temp=41.3 x=12.4 y=-3.1  
speed=1.3 battery=86 temp=41.4 x=13.1 y=-3.0  
END

This creates a new mission directory under `missions/`.


### Archive a mission
```bash
./build/mission_archiver missions/mission_YYYY-MM-DD_HHMMSS --result success --notes "test run"
```
This generates `metadata.json` and a compressed archive in `archives/`.


## Why this exists

This project was built to practice:
- Linux-native CLI tooling
- File and directory management
- Timestamped logging
- Telemetry-style data collection
- Post-run archiving and metadata generation

It reflects patterns used in robotics systems, game servers, and backend infrastructure.



## Author

Steven Smith
GitHub: https://github.com/ztvndev

