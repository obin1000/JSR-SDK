"""Formats the repository's C and C++ code with clang-format (#21).

    python tools/format.py           formats the files in place
    python tools/format.py --check   only reports files that need formatting

It covers the tracked .c, .cpp, .h and .hpp files in src/, include/, test/
and ci/, with the repository's .clang-format. CI runs --check with the
clang-format version below; other versions may format a few lines
differently. Install that version with:

    pip install clang-format==23.1.4

Set CLANG_FORMAT to the clang-format executable if it isn't on PATH.
"""
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DIRECTORIES = ['src', 'include', 'test', 'ci']
EXTENSIONS = {'.c', '.cpp', '.h', '.hpp'}
VERSION = '23.1.4'


def clang_format():
    exe = os.environ.get('CLANG_FORMAT') or shutil.which('clang-format')
    if not exe and os.name == 'nt':
        candidate = Path(os.environ.get('ProgramFiles', r'C:\Program Files')) / 'LLVM/bin/clang-format.exe'
        exe = str(candidate) if candidate.exists() else None
    if not exe:
        sys.exit(f'clang-format not found: pip install clang-format=={VERSION}, or set CLANG_FORMAT')
    return exe


def tracked_files():
    output = subprocess.run(['git', 'ls-files', '--', *DIRECTORIES], cwd=ROOT,
                            check=True, capture_output=True, text=True).stdout
    return [ROOT / name for name in output.splitlines()
            if Path(name).suffix in EXTENSIONS]


def formatted(exe, path, content):
    """Returns content formatted as C++.

    clang-format guesses the language of a .h file from its contents, and
    takes the C++/CLI handles in src/marshals (String ^) for Objective-C,
    which .clang-format has no style for. So every file goes through stdin,
    named as a .hpp file for headers, which clang-format always treats as
    C++.
    """
    name = path.with_suffix('.hpp') if path.suffix == '.h' else path
    return subprocess.run(
        [exe, f'--style=file:{ROOT / ".clang-format"}', f'--assume-filename={name}'],
        input=content, capture_output=True, check=True).stdout


def main():
    check = '--check' in sys.argv[1:]
    exe = clang_format()
    version = subprocess.run([exe, '--version'], check=True, capture_output=True, text=True).stdout.strip()
    print(version)
    if VERSION not in version:
        print(f'note: CI uses clang-format {VERSION}; other versions may disagree on a few lines')

    files = tracked_files()
    unformatted = []
    for path in files:
        original = path.read_bytes()
        result = formatted(exe, path, original)
        if result != original:
            unformatted.append(path.relative_to(ROOT).as_posix())
            if not check:
                path.write_bytes(result)
    if not check:
        print(f'Formatted {len(unformatted)} of {len(files)} files.')
        return 0
    if unformatted:
        print(f'\n{len(unformatted)} of {len(files)} files need formatting:')
        for name in unformatted:
            print(f'  {name}')
        print('Run "python tools/format.py" to format them.')
        return 1
    print(f'All {len(files)} files are formatted.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
