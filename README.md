[![experimental](http://badges.github.io/stability-badges/dist/experimental.svg)](http://github.com/badges/stability-badges)
# MathParser
## Introduction
Mathparser is a mathematical expression language based on C-style syntax.
It was created out of a desire for a simple mathematical script language outside of spreadsheets or online c-compilers or mathlab and other math tools.

## Language reference

### Basic syntax
```
//this is a comment line
a=3+4; //this is a typical statement.
b=2a+3; //implicit multiplication is allowed: same as (2*a)+3;
a+=2; b=a*10; //a statement always ends with a semi-colon, not a new line, so these are two statements.
```

### Output control
```
a=2*7; //will output only the variable and it's result: a=14
7*3; //will only output the result: 21
```

##### Echo
An exclamation mark `!` is used to echo the full expression to the output.
```
!b=60/3; //Will **echo** the code in addition to the result: b=20 b=60/3
!//echo this comment line
!q=100/10; !//some comment  --> output: q=10 q=100/10 //some comment
```
##### Mute
A hashtag `#` is used to mute the the result.
```
#c=sin(30deg); //suppress output, but still execute the code.
!#d=a+b; //execute the statement, echo the code, but mute the result
/# //suppress output for the next 4 lines.
a+=1;
a+=3;
a+=PI;
a+=7;
#/
a;
```
### Units
```
Distance=10km+1mi;
Imperial=Distance.mi;
Result=(Imperial+5m).km;
Hot=1000K.C.F; //Conversion from K to C to F. Intermediate conversion to C is pointless.
Hot=Hot.C; //set new unit for variable Hot
```
#### Implemented units:
* Length: `km, m, mm, ft, mi, th,` [TODO] complete this list
* Temperature: `C, F, K`
* [TODO] complete this list

### Functions
* Trigonometry: `sin, cos, tan, asin, acos, atan`
* Other: `max, min, round, abs`
* Alternative for abs: `|x|` is the same as `abs(x)`

* [TODO]: round() and abs() in combination with units.
* [TODO]: make min and max varargs?
