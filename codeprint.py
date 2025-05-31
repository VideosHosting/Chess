import argparse
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument("directory", help="Name of the directory", type=Path)
parser.add_argument("-e", "--exclude", help="Exclusion", nargs='+')
parser.add_argument('-r', '--recursive', help='Recursive', action='store_true')

args = parser.parse_args()

directory: Path = args.directory
exclude: list[str] = list(map(str.lower, args.exclude or []))
recursive: bool = args.recursive

if not directory.is_dir():
    raise ValueError("directory MUST be a directory")

def loop_through_dir(dir: Path):
    for file in dir.iterdir():
        if file.name.lower() in exclude:
            continue

        if recursive and file.is_dir():
            loop_through_dir(file)
            continue

        if file.is_file():
            print(file.absolute())
            print(file.read_text(encoding='utf-8'), end='\n\n')


loop_through_dir(directory)
