#!/usr/bin/env python3

# quick dirty script to bench all .c files :)

from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor
import glob
import os
import pathlib
import dataclasses
import sys
import time
from typing import Optional
import subprocess
import enum
import tqdm


FILE_DIR = os.path.dirname(os.path.realpath(__file__))
all_c_files = glob.glob(os.path.join(FILE_DIR, '*.c'))

RUN_TIMEOUT_SECONDS = 5
COMPILER = 'clang'
'../build/interpreter'
INTERPRETER_PATH = pathlib.Path(f'{FILE_DIR}/../build/native/scc').resolve().as_posix()


@dataclasses.dataclass
class RunOutput:
    stdout: str
    stderr: str
    return_code: int
    time_took_seconds: float
    info: str = ''

@dataclasses.dataclass
class CompilerOutput:
    maybe_compiled_program: Optional[RunOutput]
    compilation_time: float
    run_time: float

class Result(enum.Enum):
    PASSED = '✅'
    FAILED = '❌'
    NOT_EVALUATED = '⚠️'

@dataclasses.dataclass
class EvalResult:
    file_path: str
    compiles_without_errors: Result
    interprets_without_errors: Result
    output_matches: Result
    time_compilation: float
    time_native_run: float
    time_interpretation: float


def run_command(program_path: str) -> RunOutput:
    command = f'{program_path}'

    start = time.time()
    process = subprocess.Popen(command, stdin=subprocess.DEVNULL, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    try:
        stdout, stderr = process.communicate(timeout=RUN_TIMEOUT_SECONDS)
        return RunOutput(
            stdout=stdout.decode('utf-8'),
            stderr=stderr.decode('utf-8'),
            return_code=process.returncode,
            time_took_seconds=time.time() - start
        )
    except subprocess.TimeoutExpired:
        process.kill()
        return RunOutput(stdout='', stderr='', return_code=1, time_took_seconds=time.time() - start, info='Timeout')


def compile_file(file_path: str) -> CompilerOutput:
    path_str = pathlib.Path(file_path).resolve().as_posix()
    command = f'{COMPILER} {path_str} -o {path_str}.out -lm -std=c99 -pedantic -Werror=format'
    output = run_command(command)
    return_code = output.return_code

    if return_code != 0:
        return CompilerOutput(None, output.time_took_seconds, None)

    compilation_time = output.time_took_seconds
    run_result = run_command(f'{path_str}.out')
    run_time = run_result.time_took_seconds

    return CompilerOutput(
        maybe_compiled_program=run_result,
        compilation_time=compilation_time,
        run_time=run_time
    )

def interpret_file(file_path: str) -> RunOutput:
    path_str = pathlib.Path(file_path).resolve().as_posix()
    command = f'{INTERPRETER_PATH} {path_str}'
    return run_command(command)

def evaluate_file(file_path: str) -> EvalResult:
    compiler_output = compile_file(file_path)
    interpreter_output = interpret_file(file_path)

    output_matches = Result.NOT_EVALUATED
    if compiler_output.maybe_compiled_program is not None:
        output_matches = Result.PASSED if compiler_output.maybe_compiled_program.stdout == interpreter_output.stdout \
            and compiler_output.maybe_compiled_program.stderr == interpreter_output.stderr else Result.FAILED
    else:
        output_matches = Result.PASSED if interpreter_output.return_code == 1 else Result.FAILED


    return EvalResult(
        file_path,
        Result.PASSED if compiler_output.maybe_compiled_program is not None else Result.FAILED,
        Result.PASSED if interpreter_output.return_code == 0 else Result.FAILED,
        output_matches,
        compiler_output.compilation_time,
        compiler_output.run_time,
        interpreter_output.time_took_seconds
    )



def sort_by_PASSED(results: list[EvalResult]) -> list[EvalResult]:
    v = {Result.PASSED: 0, Result.FAILED: 1, Result.NOT_EVALUATED: 2}
    return sorted(results, key=lambda result: (v[result.compiles_without_errors], v[result.interprets_without_errors], v[result.output_matches]))

def results_to_md(results: list[EvalResult]) -> str:
    results = sort_by_PASSED(results)

    md = '# Evaluation results\n'

    compiles_without_errors = sum(1 for result in results if result.compiles_without_errors == Result.PASSED)
    interprets_without_errors = sum(1 for result in results if result.interprets_without_errors == Result.PASSED)
    correct_output = sum(1 for result in results if result.output_matches == Result.PASSED)

    md += f'## Summary\n'
    md += f'- Compiles without errors: {compiles_without_errors}/{len(results)}\n'
    md += f'- Interprets without errors: {interprets_without_errors}/{len(results)}\n\n'
    md += f'- Output matches: {correct_output}/{len(results)}\n\n'

    md += '| File | Compiles without errors | Interprets without errors | Output matches | Time compilation | Time interpretation |\n'
    md += '| --- | --- | --- | --- | --- | --- |\n'
    for result in results:
        file_name = result.file_path.split('/')[-1]
        file_name = f'[{file_name}](./{file_name})'

        md += f'| {file_name} | {result.compiles_without_errors.value} | {result.interprets_without_errors.value} | {result.output_matches.value} | {result.time_compilation:.3f} + {(result.time_native_run if result.time_native_run else 0):.3f} ({(result.time_compilation + result.time_native_run if result.time_native_run else result.time_compilation):.3f}) | {result.time_interpretation:.3f} |\n'

    return md

eval_results = []

for c_file in tqdm.tqdm(all_c_files, file=sys.stderr):
    eval_results.append(evaluate_file(c_file))

# with ProcessPoolExecutor(max_workers=12) as e:
#     eval_results = list(e.map(evaluate_file, all_c_files))


all_out_files = glob.glob(os.path.join(FILE_DIR, '*.out'))
for out_file in all_out_files:
    os.remove(out_file)

print(results_to_md(eval_results))
