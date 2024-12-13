import sys
import subprocess
import os


SRC_DIR = "src"

if os.name == "nt":
    CC = "clang.exe"
    OUTPUT = "pascal.exe"
else:
    CC = "clang"
    OUTPUT = "pascal"


def get_c_files() -> list[str]:
    files = []
    for item in os.listdir(SRC_DIR):
        item = os.path.join(SRC_DIR, item)
        if os.path.isfile(item) and item.endswith(".c"):
            files.append(item)
    return files


def build():
    files = get_c_files()
    command = [CC] + files + ["-o", OUTPUT, "-g"]
    subprocess.run(command, check=True)


def run():
    subprocess.run([OUTPUT], check=True)


cmd = {
    "build": build,
    "run": run,
}

for arg in sys.argv[1:]:
    if arg in cmd:
        cmd[arg]()
    else:
        print(f"unknown command {arg}")
