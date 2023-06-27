import argparse
import difflib
import os
import pathlib
import subprocess
import sys
import tempfile
import textwrap
import uuid
import yaml


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "filename", type=pathlib.Path, help="yaml test configuration"
    )
    return parser.parse_args()


class Tester():

    def __init__(self, args):
        self.data = yaml.safe_load(args.filename.read_text())
        self.args = args

    def run(self):
        with tempfile.TemporaryDirectory() as workdir:
            for filename, content in self.data["files"].items():
                filename = workdir / pathlib.Path(filename)
                filename.parent.mkdir(exist_ok=True, parents=True)
                filename.write_text(content)
            self.workdir = workdir
            status = [
                self.run_test(testcase)
                for testcase in self.data["tests"]
            ]
        return 0 if all(status) else 1

    def run_test(self, testcase):
        cmd = testcase["command"]
        sys.stdout.write("*" * 80 + "\n")
        sys.stdout.write(f"# {cmd.strip()}\n")
        sys.stdout.write("*" * 80 + "\n")
        env = dict(os.environ)
        env["WORKDIR"] = self.workdir
        p = subprocess.run(
            cmd,
            shell=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            env=env,
            cwd=self.workdir
        )
        stdout = p.stdout.decode("utf-8", errors="replace")
        stderr = p.stderr.decode("utf-8", errors="replace")
        if p.returncode != 0:
            sys.stdout.write("*" * 80 + "\n")
            sys.stdout.write(f"FAIL: {p.returncode}")
            sys.stdout.write(stderr)
            sys.stdout.write("*" * 80 + "\n")
        if "assert" in testcase:
            try:
                cb = self.assertion_fct(testcase["assert"])
            except Exception as e:
                sys.stdout.write(f"ERROR: {e.__class__.__name__}: {e}\n")
                return False
            status = cb(testcase, stdout, stderr)
        else:
            status = [
                self.assertion_diff(testcase["expected"][name], result)
                for name, result in (("stdout", stdout), ("stderr", stderr))
                if name in testcase["expected"]
            ]
            if not status:
                sys.stdout.write("No expected provided\n")
            status = status and all(status)
        sys.stdout.write("\n")
        return status

    def assertion_diff(self, expect, result):
        expect = expect.format(WORKDIR=self.workdir)
        if result == expect:
            return True
        diff = difflib.Differ().compare(
            expect.splitlines(keepends=True), result.splitlines(keepends=True)
        )
        sys.stdout.write("--- expected\n")
        sys.stdout.write("+++ result\n")
        sys.stdout.writelines(diff)
        return False

    def assertion_fct(self, script):
        fct_name = f"cb_{uuid.uuid4().hex}"
        fct = (
            "def %s(testcase, stdout, stderr):\n"
            "    try:\n"
            "%s\n"
            "    except Exception as e:\n"
            '        sys.stdout.write(f"ERROR: {e.__class__.__name__}: {e}")\n'
            "        return False\n"
            "    return True\n"
        ) % (fct_name, textwrap.indent(script, 8 * " "))
        exec(fct, globals(), self.__dict__)
        return getattr(self, fct_name)


def main():
    return Tester(parse_args()).run()


if __name__ == "__main__":
    sys.exit(main())
