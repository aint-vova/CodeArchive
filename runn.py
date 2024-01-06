executable_name = "runn.exe"

defines = [
#    "NDEBUG"
]

additional_arguments = [
]

additional_include_folders = [
]

additional_library_folders = [
]

import os
import shutil
import subprocess
import time
import msvcrt

if shutil.which("g++") is None:
    print("Failed to find g++ in PATH.\nInstall GCC through MinGW and add bin folder to PATH.")
    exit()

script_directory = os.path.dirname(os.path.abspath(__file__))

compiler_command = ["g++", "-o", executable_name, "-I", script_directory]

for root, directories, files in os.walk(script_directory):
    for file in files:
        filepath = os.path.relpath(os.path.join(root, file), script_directory).replace("\\", "/")
        if filepath.endswith(".cpp"): compiler_command.append(filepath)

for include_folder in additional_include_folders: compiler_command += ["-I", include_folder]
for library_folder in additional_library_folders: compiler_command += ["-L", library_folder]
for define in defines: compiler_command += ["-D", define]
compiler_command += additional_arguments

print("Building...")

should_launch = True

try:
    output = subprocess.check_output(compiler_command, stderr=subprocess.STDOUT, text=True)
    if output: print(output)
except subprocess.CalledProcessError as e:
    if e.output:
        should_launch = False
        print(e.output)

if should_launch:
    time.sleep(0.1)

    print("Launching...\n")

    process = subprocess.Popen(executable_name, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    for line in process.stdout: print(line, end='')

    print("\nExecution finished!")
else:
    print("\nBuild failed!")

msvcrt.getch()
