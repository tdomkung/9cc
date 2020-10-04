#!/bin/bash
assert() {
   expected="$1"
   input="$2"

   ./9cc "$input" >tmp.s
   cc -o tmp tmp.s
   ./tmp
   actual="$?"

   if [ "$actual" = "$expected" ]; then
      echo "$input => $actual"
   else
      echo "$inupt => $expected expected, but got $actual"
      exit 1
   fi
}

assert 0 0
assert 42 42
assert 21 "5+20-4"
assert 41 " 12  + 34 - 5  "
assert 47 "5+6*7"
assert 15 "5*(9-6)"
assert 4 '(3+5)/2'
assert 5  '-5+10'
assert 9  '-5+ +10- -4'
#assert 100  '--100'  # TODO: tokenize で失敗
assert  1  '5<4+7'
assert  0  '5-4<=-4'
assert  1  '6>=11-5'
assert  1  '5+5*3==20'
assert  1  '5/5!=2'
echo OK
