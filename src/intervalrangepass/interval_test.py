import os
import subprocess
from typing import Set, Tuple
import pytest

CWD = os.path.dirname(os.path.abspath(__file__))
TEST_DIR = os.path.join(CWD, "test")

def discover_tests():
    for file in os.listdir(TEST_DIR):
        if file.endswith(".tip"):
            name = file.split(".")[0]
            yield name

"""
    the content of file is as follow:
    *** interval range analysis for function
    <ir>
     ***
     <analysis result>
"""
def get_ir_and_analysis_result(file: str) -> Tuple[str, Set[str]]:
    with open(file) as f:
        lines = f.readlines()
        ir_lines = []
        analysis_lines = set()
        is_ir = True
        for line in lines:
            if " ***" in line: # the separator of ir and analysis result
                is_ir = False
                continue
            if is_ir:
                ir_lines.append(line)
            else:
                analysis_lines.add(line)
        return "\n".join(ir_lines), analysis_lines

@pytest.mark.parametrize("name", discover_tests())
def test_interval(name: str):
    proc = subprocess.run(f"./runirpass.sh {name}", shell=True, cwd=TEST_DIR, timeout=5)
    if proc.returncode != 0:
        raise Exception(f"runirpass.sh {name} failed")
    res_path = os.path.join(TEST_DIR, name+".irpass")
    exp_path = os.path.join(TEST_DIR, name+".expected")
    res_ir, res_analysis_result = get_ir_and_analysis_result(res_path)
    exp_ir, exp_analysis_result = get_ir_and_analysis_result(exp_path)
    assert res_ir == exp_ir # judge ir is equal
    assert res_analysis_result == exp_analysis_result # judge by set equality because the order of analysis result may be different