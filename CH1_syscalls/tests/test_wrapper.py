#!/usr/bin/env python3

import os
import tempfile

from testsupport import run, subtest, warn, ensure_library, test_root
from re import search


def main() -> None:
    # Get test abspath
    lib = ensure_library("librw_1.so")
    with tempfile.TemporaryDirectory() as tmpdir:

        # Generate a small random file
        with open(f"{tmpdir}/rdn.txt", "wb") as fp:
            fp.write(os.urandom(4096))

        test_empty = test_root().joinpath("test_empty")
        if not test_empty.exists():
            run(["make", "-C", str(test_root()), str(test_empty)])

        # Copy the file with the system's glibc
        with subtest("Run cp with system glibc"):
            run(["cp", f"{tmpdir}/rdn.txt", f"{tmpdir}/rdn.glibc.txt"])

        # Copy files with LD_PRELOADed librw.so.1
        with subtest(f"Run cp with {lib} preloaded"):
            with open(f"{tmpdir}/stderr", "w+") as stderr:
                run(
                    ["cp", f"{tmpdir}/rdn.txt", f"{tmpdir}/rdn.librw.1.txt"],
                    extra_env={"LD_PRELOAD": str(lib)},
                    stderr=stderr,
                )
            with open(f"{tmpdir}/stderr", "r") as stderr:
                for l in stderr.readlines():
                    if "ERROR: ld.so:" in l:
                        warn(l)
                        exit(1)

        # Check that glibc and librw.so.1 give the same result
        with subtest("Check that both resulting files are identical"):
            run(["cmp", f"{tmpdir}/rdn.glibc.txt", f"{tmpdir}/rdn.librw.1.txt"])

        # Check that librw_1.so provides read() and write() symbols
        with subtest("Check that read and write functions are available in librw_1.so"):
            with open(f"{tmpdir}/stdout", "w+") as stdout:
                run(["nm", "-D", "--defined-only", str(lib)], stdout=stdout)
            ok = 0
            with open(f"{tmpdir}/stdout", "r") as stdout:
                for l in stdout.readlines():
                    if l.endswith(" T read\n"):
                        ok += 1
                    elif l.endswith(" T write\n"):
                        ok += 1
                if ok != 2:
                    warn(f"{str(lib)} is not providing read, write, or both!")
                    exit(1)

        # # Check that the nbytes == 0 optimisation is implemented
        with subtest(f"Check that {lib} avoids the syscall if nothing will be read/written"):
            run(
                [ "strace", "-o", f"{tmpdir}/strace.log",
                f"{test_empty}", f"{tmpdir}/rdn.txt" ],
                extra_env={"LD_PRELOAD": str(lib)}
            )
            with open(f"{tmpdir}/strace.log", "r") as fp:
                for l in fp.readlines():
                    m = search('(read|write)\(\d+, \S*, 0\)\s+= 0', l)
                    if m is not None:
                        warn(f"{str(lib)} permits zero length reads or writes")
                        warn(f"{l}")
                        exit(1)

if __name__ == "__main__":
    main()
