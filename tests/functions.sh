#!/bin/sh

RED='\033[1;91m'
GREEN='\033[1;32m'
YELLOW='\033[0;93m'
BLUE='\033[0;36m'
PURPLE='\033[0;35m'
BIWhite='\033[0;97m'
Color_Off='\033[0m'

REF_OUT=".ref.out"
TEST_OUT=".actual.out"

passed=0
failed=0

delete()
{
    rm .actual.out
    rm .ref.out
}

# tests for -c on the binary
test_42sh_string()
{
    bash --posix -c "${@}" > "$REF_OUT"
    ref_exit=$?
    ../../../src/42sh -c "${@}" > "$TEST_OUT"
    test_exit=$?
    diff "$REF_OUT" "$TEST_OUT"
    if [ $? -ne 0 ]; then
        echo -e "👎  ${RED}FAILED${Color_Off} : ./42sh -c "${@}""
        failed=$((failed+1))
    else
        if [ "$test_exit" -ne "$ref_exit" ]; then
            echo -e "🤏 ${YELLOW}BAD ERROR CODE${Color_Off} : ./42sh -c "${@}""
            failed=$((failed+1))
        else
            echo -e "👍 ${GREEN}PASSED${Color_Off} : ./42sh -c "${@}""
            passed=$((passed+1))
        fi
    fi
}

# tests for file tests on the binary
test_42sh_file()
{
    input="${@}"
    bash --posix < "$input" > "$REF_OUT"
    ../../../src/42sh "${@}" > "$TEST_OUT"
    diff "$REF_OUT" "$TEST_OUT"
    if [ $? -ne 0 ]; then
        echo -e "👎 ${RED}FAILED${Color_Off} : ./42sh "${@}""
        failed=$((failed+1))
    else
        echo -e "👍 ${GREEN}PASSED${Color_Off} : ./42sh "${@}""
        passed=$((passed+1))
    fi
}

# tests with env -i
test_42sh_file_env()
{
    input="${@}"
    env -i bash --posix < "$input" > "$REF_OUT"
    env -i ../../../src/42sh "${@}" > "$TEST_OUT"
    diff "$REF_OUT" "$TEST_OUT"
    if [ $? -ne 0 ]; then
        echo -e "👎 ${RED}FAILED${Color_Off} : ./42sh "${@}""
        failed=$((failed+1))
    else
        echo -e "👍 ${GREEN}PASSED${Color_Off} : ./42sh "${@}""
        passed=$((passed+1))
    fi
}


# tests for stind test on the binary
test_42sh_stdin()
{
    bash --posix -c "$(cat ${@})" > "$REF_OUT"
    ref_exit=$?
    ../../../src/42sh < "${@}" > "$TEST_OUT"
    test_exit=$?
    diff "$REF_OUT" "$TEST_OUT"
    if [ $? -ne 0 ]; then
        echo -e "👎 ${RED}FAILED${Color_Off} : ./42sh < "${@}""
        failed=$((failed+1))
    else
        if [ "$test_exit" -ne "$ref_exit" ]; then
            echo -e "🤏 ${YELLOW}BAD ERROR CODE${Color_Off} : ./42sh < "${@}""
            failed=$((failed+1))
        else
            echo -e "👍 ${GREEN}PASSED${Color_Off} : ./42sh < "${@}""
            passed=$((passed+1))
        fi
    fi
}

# test for stdin with PIPE on the binary
test_42sh_stdin2()
{
    bash --posix -c "$(cat ${@})" > "$REF_OUT"
    ref_exit=$?
    cat "${@}" | ../../../src/42sh > "$TEST_OUT"
    test_exit=$?
    diff "$REF_OUT" "$TEST_OUT"
    if [ $? -ne 0 ]; then
        echo -e "👎 ${RED}FAILED${Color_Off} : cat "${@}" | ./42sh"
        failed=$((failed+1))
    else
        if [ "$test_exit" -ne "$ref_exit" ]; then
            echo -e "🤏 ${YELLOW}BAD ERROR CODE${Color_Off} : cat "${@}" | ./42sh"
            failed=$((failed+1))
        else
            echo -e "👍 ${GREEN}PASSED${Color_Off} : cat "${@}" | ./42sh"
            passed=$((passed+1))
        fi
    fi
}

# test if usage printout works
test_error()
{
    echo "Invalid input\nUsage: 42sh [OPTIONS] [SCRIPT] [ARGUMENTS ...]" > "$REF_OUT"

    ../../../src/42sh ${@} 2> "$TEST_OUT"
    test_exit=$?
    diff "$REF_OUT" "$TEST_OUT"
    if [ $? -ne 0 ]; then
        echo -e "👎 ${RED}FAILED${Color_Off} : ./42sh "${@}""
        failed=$((failed+1))
    else
        if [ "$test_exit" -ne 2 ]; then
            echo -e "🤏 ${YELLOW}BAD ERROR CODE${Color_Off} : ./42sh "${@}""
            failed=$((failed+1))
        else
            echo -e "👍 ${GREEN}PASSED${Color_Off} : : ./42sh "${@}""
            passed=$((passed+1))
        fi
    fi
}

# test if grammatical error works
test_error2()
{
    ../../../src/42sh -c "${@}" 2> "$TEST_OUT"
    test_exit=$?
    bash --posix -c "${@}" 2> "$REF_OUT"
    test_ref=$?
    if [ ! -s "$TEST_OUT" ]; then
        echo -e "👎 ${RED}FAILED${Color_Off} : /42sh "${@}""
        failed=$((failed+1))
    else
        if [ "$test_exit" -ne "$test_ref" ]; then
            echo -e "🤏 ${YELLOW}BAD ERROR CODE${Color_Off} : ./42sh "${@}""
            failed=$((failed+1))
        else
            echo -e "👍 ${GREEN}PASSED${Color_Off} : ./42sh "${@}""
            passed=$((passed+1))
        fi
    fi
}

# if 1 returns PASSED else FAILED
test_compare_exit_code()
{
    if [ ${1} -ne 0 ]; then
        echo -e "👎 ${RED}FAILED${Color_Off} : ./42sh "${2}""
        failed=$((failed+1))
    else
        echo -e "👍 ${GREEN}PASSED${Color_Off} : ./42sh "${2}""
        passed=$((passed+1))
    fi
}
