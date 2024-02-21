#!/bin/sh

. ../../functions.sh --source-only

echo -e "\n${BLUE}\tSTEP 2 - Redirections 🧪\n${Color_Off}"

# test '>' and '>|'

../../../src/42sh -c "echo toto > titi"
bash --posix -c "echo toto > titi2"
diff titi2 titi
test_compare_exit_code $? "[echo toto > titi]"

# test '<'
../../../src/42sh -c "cat < titi" > $TEST_OUT
bash --posix -c "cat < titi" > $REF_OUT
diff $TEST_OUT $REF_OUT
test_compare_exit_code $? "[cat < titi]"

# test '>>' 
../../../src/42sh -c "echo toto2 >> titi" > $TEST_OUT
bash --posix -c "echo toto2 >> titi2" > $REF_OUT
diff titi2 titi
test_compare_exit_code $? "[echo toto >> titi]"
diff $REF_OUT $TEST_OUT
    #to compare if the outputs are the same
test_compare_exit_code $? "[echo toto >> titi OUTPUTS]"
rm titi titi2

# test '>&'
test_42sh_string "echo toto2 >& 1"

# test '<&' 
echo "Next test is for <&"
test_42sh_file ../files/test_redir4.sh

# test '<>'
echo "Next is cat <> file:" 
test_42sh_file ../files/test_redir1.sh

# double redirection >
echo "Next is echo toto > mbappé > titi:" 
test_42sh_file ../files/test_redir2.sh

test_42sh_string "echo popo 2> lala" # check if lala is empty
#rm lala tmp
test_42sh_string "if true; then echo tata >&3; cat <&3; fi 3<> lala"
rm lala

echo -e "\n${BLUE}RESULTS -- Redirections:${Color_Off} $passed/$((passed+failed)) tests passed"
