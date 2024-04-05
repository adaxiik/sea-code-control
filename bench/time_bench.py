#!/usr/bin/env python3
import os
import pathlib
import random
import sys
import time

from matplotlib.ticker import ScalarFormatter

def create_parity_program(parity: str) -> str:
    return '''
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    const char *data;
    size_t size;
} String;

bool string_equals(const String *a, const String *b)
{
    if (a->size != b->size) {
        return false;
    }

    for (size_t i = 0; i < a->size; ++i) {
        if (a->data[i] != b->data[i]) {
            return false;
        }
    }

    return true;
}

typedef struct {
    String *data;
    size_t size;
    size_t capacity;
} StringStack;

void create_stringstack(StringStack *stack)
{
    size_t s = sizeof(String);
    stack->data = (String *)malloc(s * 8);
    stack->size = 0;
    stack->capacity = 8;
}

void resize_stack_if_needed(StringStack *stack)
{
    if (stack->size != stack->capacity) {
        return;
    }

    stack->capacity *= 2;
    size_t s = sizeof(String);
    String *new_data = (String *)malloc(s* stack->capacity);

    for (size_t i = 0; i < stack->size; ++i) {
        new_data[i] = stack->data[i];
    }

    free(stack->data);
    stack->data = new_data;
}

void push_stringstack(StringStack *stack, String *str)
{
    resize_stack_if_needed(stack);
    stack->data[stack->size] = *str;
    stack->size += 1;
}

String pop_stringstack(StringStack *stack)
{
    stack->size -= 1;
    return stack->data[stack->size];
}

bool is_empty_stringstack(const StringStack *stack)
{
    return stack->size == 0;
}

void destroy_stringstack(StringStack *stack)
{
    free(stack->data);
}

bool next_tag(const char **str, String *tag)
{
    const char *start = *str;
    while (*start != '<' && *start != '\\0'){
        start += 1;
    }

    if (*start == '\\0') {
        return false;
    }

    const char *end = start + 1;
    while (*end != '>') {
        end += 1;
    }

    tag->data = start + 1;
    tag->size = end - start - 1;
    *str = end + 1;

    return true;
}

bool check_parity(const char *str)
{
    StringStack stack;
    create_stringstack(&stack);

    String current_tag;

    while (next_tag(&str, &current_tag)) {
        bool is_closing = current_tag.data[0] == '/';
        if (!is_closing) {
            push_stringstack(&stack, &current_tag);
        } else {
            String closing_tag;
            closing_tag.data = current_tag.data + 1;
            closing_tag.size = current_tag.size - 1;

            String open_tag = pop_stringstack(&stack);
            if (!string_equals(&open_tag, &closing_tag)) {
                destroy_stringstack(&stack);
                return false;
            }
        }
    }

    if (!is_empty_stringstack(&stack)) {
        destroy_stringstack(&stack);
        return false;
    }

    destroy_stringstack(&stack);
    return true;
}

int main()
{
    const char *input = "'''+ parity + '''";

    if (check_parity(input)){
        puts("Correct");
    }
    else{
        puts("Incorrect");
    }
    return 0;
}
'''


def save_program(program: str, file_path: str):
    with open(file_path, 'w') as f:
        f.write(program)

def compile_program(file_path: str, compiler: str = 'gcc') -> None:
    os.system(f'{compiler} {file_path} -o {file_path[:-2]}.out -std=c99 -pedantic -Wall -Wextra -Werror -Wno-overlength-strings')



def measure_time(path: str, num_iter: int = 10) -> float:
    times = []
    for _ in range(num_iter):
        start = time.time()
        rc = os.system(f'{path}')
        if rc != 0:
            return None
        end = time.time()
        times.append(end - start)

    return sum(times) / len(times)


def random_tag() -> str:
    return random.choice(['div', 'span', 'p', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'a', 'img', 'input', 'button'])

def generate_tag_tree(depth: int, count: int) -> str:
    if depth == 0:
        return ''

    tag = random_tag()
    children = [generate_tag_tree(depth - 1, count) for _ in range(count)]
    return f'<{tag}>{"".join(children)}</{tag}>'


FILE_DIR = os.path.dirname(os.path.realpath(__file__))
INTERPRETER_PATH_NATIVE = pathlib.Path(f'{FILE_DIR}/../build/native/scc').resolve().as_posix()
INTERPRETER_PATH_WASM = pathlib.Path(f'{FILE_DIR}/../build/wasm/wasm_driver.js').resolve().as_posix()

import matplotlib
import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd


def create_plot(datapath: str):
    matplotlib.use('pdf')
    # matplotlib.rcParams.update({
    #     "pgf.texsystem": "pdflatex",
    #     'font.family': 'serif',
    #     'text.usetex': True,
    #     'pgf.rcfonts': False,
    #     'font.size' : 11,
    #     # 'text.latex.preamble': r'\newcommand{\mathdefault}[1][]{}'
    # })


    sns.set_theme()
    sns.set_context('paper')
    # sns.set_context('talk')
    sns.set_style('whitegrid')
    sns.set_palette('colorblind')

    # df = pd.DataFrame(data, columns=['Překladač', 'Hloubka', 'Čas [s]'])
    df = pd.read_csv(datapath)
    df['Čas [s]'] = df['Čas [s]'].astype(float)
    df['Hloubka'] = df['Hloubka'].astype(int)

    df = df[(df['Překladač'] != 'gcc -O1') & (df['Překladač'] != 'tcc')]


    plt.figure(figsize=(12, 6))
    plt.yscale('log')
    sns.lineplot(data=df, x='Hloubka', y='Čas [s]', hue='Překladač', marker='o')
    # sns.scatterplot(data=df, x='Hloubka', y='Čas [s]', hue='Překladač', marker='o')

    # plt.gca().yaxis.set_major_formatter(ScalarFormatter())
    # plt.gca().ticklabel_format(axis='y', style='plain')

    # plt.show()

    plt.savefig('time_bench.pdf')
    plt.close()


if __name__ == '__main__':

    # create_plot('bench.txt')
    # exit(0)

    programs = [(depth, create_parity_program(generate_tag_tree(depth, 2))) for depth in range(2, 18)]

    print(f'Překladač,Hloubka,Čas [s]', file=sys.stderr)
    for compiler in ['gcc -O0', 'gcc -O2', 'scc x86_64', 'scc wasm via node']:
        for i, (n, program) in enumerate(programs):
            save_program(program, 'check_parity.c')

            if compiler == 'scc x86_64':
                time_ = measure_time(f'{INTERPRETER_PATH_NATIVE} check_parity.c --mute', 5)
                print(f'{compiler},{n},{time_}', file=sys.stderr)
                continue

            if compiler == 'scc wasm via node':
                time_ = measure_time(f'node {INTERPRETER_PATH_WASM} check_parity.c ', 5)
                if time_ is None:
                    continue
                print(f'{compiler},{n},{time_}', file=sys.stderr)
                continue

            compile_program('check_parity.c', compiler)
            time_ = measure_time('./check_parity.out', 5)
            os.system(f'rm check_parity.out')

            print(f'{compiler},{n},{time_}', file=sys.stderr)


