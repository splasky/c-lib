#! /usr/bin/python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
# Last modified: 2018-10-13 11:32:39

import subprocess
import glob


def find_all_tests(excludes=[]):
    """
    find all tests inside tests directory.
    excludes:files won't test
    """
    tests = list(map(lambda p: p.strip('/').split('/')
                     [-1], sorted(glob.glob('tests/test_*'))))

    for exclude in excludes:
        if exclude in tests:
            tests.remove(exclude)

    for test in tests:
        if '.c' in test:
            tests.remove(test)
    return tests


def run_tests():
    """
    running tests files.
    """
    complete = 0
    failed = 0
    failed_tests = []

    tests = find_all_tests()
    print(len(tests))
    for test in tests:
        try:
            ret = subprocess.run('./tests/' + test,
                                 stdout=subprocess.DEVNULL,
                                 )
            ret.check_returncode()
            complete += 1
        except subprocess.CalledProcessError:
            failed += 1
            failed_tests.append(test)

    print("---------------run test result-------------------")
    print("Test finish")
    print("Success:{}".format(complete))
    print("Failed:{}".format(failed))
    print("Failed files:{}".format(failed_tests))


if __name__ == "__main__":
    print("Running unit tests:")
    run_tests()
