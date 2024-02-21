#!/bin/sh
. ../../functions.sh --source-only

echo -e "\n${BLUE}\tFIX - Echo 🧪\n${Color_Off}"

test_42sh_string 'echo -EeE \\\\\\\\'
test_42sh_string 'echo -E -e -x -n -e bad option'
test_42sh_string "echo -e -E -e '\\\\\\'"
test_42sh_string "echo -eEe \\\\\\\\"
test_42sh_string "echo -E -E -E -E -e -E -e -e -e -e -E '=> \\\\'"
test_42sh_string "echo -E -E -E -E -e -E -e -e -e -e -e '=> \\\\'"
test_42sh_string "echo -E -E -E -E -e -E -e -e -e -e -e => \\\\"
test_42sh_string "echo -neennne 42\\nsh"

rm '\' '\\'

echo -e "\n${BLUE}RESULTS -- Fix Echo:${Color_Off} $passed/$((passed+failed)) tests passed"
