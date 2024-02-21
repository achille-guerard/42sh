#!/bin/sh

. ../../functions.sh --source-only

# should be an error
test_42sh_stdin3()
{
    bash --posix -c "$(cat ${@})" > "$REF_OUT"
    ref_exit=$?
    cat "${@}" | ../../../src/42sh > "$TEST_OUT"
    test_exit=$?
    if [ $ref_exit -eq $test_exit ]; then
        echo -e "👍 ${GREEN}PASSED${Color_Off} : cat "${@}" | ./42sh"
        passed=$((passed+1))
    else
        diff "$REF_OUT" "$TEST_OUT"
        echo -e "👎 ${RED}FAILED${Color_Off} : cat "${@}" | ./42sh"
        failed=$((failed+1))
    fi
}
test_42sh_stdin3 "./files/test_un_quotes.sh"
delete
