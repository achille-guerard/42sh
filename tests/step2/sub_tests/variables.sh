#!/bin/sh

. ../../functions.sh --source-only

echo -e "\n${BLUE}\tSTEP 2 - bonjour les variables  🧪\n${Color_Off}"

test_42sh_string 'a=2'
test_42sh_string 'b=2 c=3'
test_42sh_string 'a=1; echo $a'
test_42sh_string 'b=2 c=3; echo $c'
test_42sh_string 'b=2 c=3; echo $b'
test_42sh_string 'a=3 echo $b'
test_42sh_string 'a=32232 echo $a'
test_42sh_string 'a=32232; echo $a'
test_42sh_string 'abracadabrantesquement=petit; echo $a'

test_42sh_string 'a=ec b=ho; $a$b ta_grand_mere.exe' #FAVORITE TEST EVER
test_42sh_string 'a=23 b=$a c=$b d=$c; echo $a$b$c$d'
test_42sh_string 'a=23 b=$a c=$b d=$c echo $a$b$c$d'
test_42sh_string "a=ec echo $a; a=ac ; echo $'a'c"
test_42sh_string 'a=ro b=main ; echo ${a}${b}c'
test_42sh_file "../files/var_1"
test_42sh_file "../files/var_2"
test_42sh_file "../files/var_3"
test_42sh_file "../files/var_4"
test_42sh_file "../files/var_5"
# tests for $#
test_42sh_file "../files/var_6"

test_42sh_file "../files/var_special"

test_42sh_string 'echo $PWD'
test_42sh_string 'a=2 echo $a; echo $a ; echo $a'

test_42sh_string 'for=toto; echo $for ; for for in for ; do echo $for; done ; \
echo $for'

test_42sh_string 'echo $(echo toto ta grand meeeeeeeere)'
test_42sh_string 'echo `echo toto ta grand meeeeeeeere`'

##those tests can be easily passed by checking the key exists before adding it
#test_42sh_string 'a=2 a=3 a=4 ; echo $a'
#test_42sh_string 'a=2; a=3; a=4; a=4; a=5; echo $a'


#test_42sh_string 'echo $a echo $a' ce test nappartient pas a cette categorie



echo -e "\n${BLUE}RESULTS -- Quotes and tab tests:${Color_Off} $passed/$((passed+failed)) tests passed"
