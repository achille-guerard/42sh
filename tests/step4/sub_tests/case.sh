#!/bin/sh

. ../../functions.sh --source-only

# if, else, true tests
echo -e "\n${BLUE}\tSTEP 4 - Case command tests 🧪\n${Color_Off}"

test_42sh_string "case prout in esac"
test_42sh_string "case prout in prout) echo toto;; esac"
test_42sh_string "case prout in caca) echo toto;; prout) echo tata;; esac"
test_42sh_file ../files/beteuXL.sh
test_42sh_file ../files/caca.sh
test_42sh_file ../files/error.sh
test_42sh_file ../files/test
test_42sh_file ../files/a
test_42sh_file ../files/tata
delete

echo -e "\n${BLUE}RESULTS -- Case command tests:${Color_Off} $passed/$((passed+failed)) tests passed"
