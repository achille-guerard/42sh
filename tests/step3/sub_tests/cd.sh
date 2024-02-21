#!/bin/sh

. ../../functions.sh -- source-only

echo -e "\n${BLUE}\tSTEP 3 - cd 🧪\n${Color_Off}"

# I. Should Work :
test_42sh_string "cd"
test_42sh_string "cd ."
test_42sh_string "cd ~"
test_42sh_string "cd /"
test_42sh_string "cd /etc////"
test_42sh_string "cd ../../../././././././././"

test_42sh_file_env "../files/test_cd/test_cd_1.sh"
test_42sh_file "../files/test_cd/test_cd_2.sh"
test_42sh_file "../files/test_cd/test_cd_3.sh"
test_42sh_file_env "../files/test_cd/test_cd_4.sh"
test_42sh_file "../files/test_cd/test_cd_5.sh"

test_42sh_string "cd two args"
test_42sh_string "cd this_dir_doesnot_exists"
delete
echo -e "\n${BLUE}RESULTS -- cd 🧪 test:${Color_Off} $passed/$((passed+failed)) tests passed"
