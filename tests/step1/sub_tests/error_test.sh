#!/bin/sh

. ../../functions.sh --source-only

# Wrong usage
echo -e "\n${BLUE}\n\tSTEP 1 - Wrong Usage && Errors 🧪\n${Color_Off}"

#Errors
test_error2 "if false; true; then"
test_error2 "if false ; true ; then                          "
test_error2 "echa should not work"
test_error2 "echa should not work"
test_error2 "if echo toto then echo tata fi"
test_error2 "if fi"
delete
echo -e "\n${BLUE}RESULTS -- Wrong Usage && Errors:${Color_Off} $passed/$((passed+failed)) tests passed"
