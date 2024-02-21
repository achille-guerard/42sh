#!/bin/sh

. ../../functions.sh --source-only

echo -e "\n${BLUE}\tSTEP 2 - Quotes and tab tests 🧪\n${Color_Off}"

test_42sh_string 'echo ""'
test_42sh_string 'echo "Hello, World!"'
test_42sh_string 'cat "../files/test1.sh"'
test_42sh_string 'echo "\\t" '
test_42sh_string 'echo "\\\\\\\t"'
test_42sh_string 'echo \\\\\\\t'
test_42sh_string "echo word\tsplit"

test_42sh_string "echo word'\t'split"
test_42sh_string "echo 'wo"""rd\tsp"lit'"

test_42sh_string "echo ''"
test_42sh_string 'echo ""'
test_42sh_string 'echo "word\\n" '
test_42sh_string 'echo "\""'
test_42sh_string 'echo "\\""'
test_42sh_string 'echo "word\
split"'

test_42sh_string 'echo "word\


split"'
test_42sh_string '/run/current-system/sw/bin/ec\
ho toto'
test_42sh_string "echo \#escaped \"#\"quoted not\#first #commented"
test_42sh_string 'echo 2\>a'
test_42sh_string 'echo -E "\\\\\\\"'
test_42sh_string 'echo \$toto'
test_42sh_string 'echo "\$toto"'
test_42sh_string "echo '\$toto'"
test_42sh_file "../files/test7.sh"

echo -e "\n${BLUE}RESULTS -- Quotes and tab tests:${Color_Off} $passed/$((passed+failed)) tests passed"
