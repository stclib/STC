#!/usr/bin/env python3

import re
import sys
import os
from os.path import dirname, join as path_join, abspath, basename, exists

top_dir = dirname(abspath(__file__))
extra_paths = [path_join(top_dir, 'include'), path_join(top_dir, '..', 'include')]

def find_file(included_name, current_file):
    current_dir = dirname(abspath(current_file))
    for idir in [current_dir] + extra_paths:
        try_path = path_join(idir, included_name)
        if exists(try_path):
            return abspath(try_path)
    return None


def process_file(
    file_path,
    out_lines=[],
    processed_files=[],
):
    out_lines += "// ### BEGIN_FILE_INCLUDE: " + basename(file_path) + '\n'
    comment_block = False
    with open(file_path, "r") as f:
        for line in f:
            is_comment = comment_block
            if re.search('/\*.*?\*/', line):
                pass
            elif re.search('^\\s*/\*', line):
                comment_block, is_comment = True, True
            elif re.search('\*/', line):
                comment_block = False

            if is_comment:
                continue

            m_inc = re.search('^\\s*# *include\\s*[<"](.+)[>"]', line) if not is_comment else False
            if m_inc:
                inc_name = m_inc.group(1)
                inc_path = find_file(inc_name, file_path)
                if inc_path not in processed_files:
                    if inc_path is not None:
                        processed_files += [inc_path]
                        process_file(
                            inc_path,
                            out_lines,
                            processed_files,
                        )
                    else:
                        # assume it's a system header
                        out_lines += [line]
                continue
            m_once = re.match('^\\s*# *pragma once\\s*', line) if not is_comment else False
            # ignore pragma once; we're handling it here
            if m_once:
                continue
            # otherwise, just add the line to the output
            if line[-1] != '\n':
                line += '\n'
            out_lines += [line]
    out_lines += "// ### END_FILE_INCLUDE: " + basename(file_path) + '\n'
    return (
        "".join(out_lines)
    )


if __name__ == "__main__":
    with open(sys.argv[2], "w", newline='\n') as f:
        print(
            process_file(
                abspath(sys.argv[1]),
                [],
                # We use an include guard instead of `#pragma once` because Godbolt will
                # cause complaints about `#pragma once` when they are used in URL includes.
                [abspath(sys.argv[1])],
            ),
            file=f
        )
