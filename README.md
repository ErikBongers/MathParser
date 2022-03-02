# MathParser
##MANUAL

> Present manual as a script or a html page?
  > in case of a script: it should not overwrite the user's script.
> Where to add the manual?
  > question mark button on main page?
  > fold out within main page?
  > on phone?
Math Parser allows arithmetic expressions in a C-style format:
** Basic syntax
//this is a comment line
a=3+4;
b=2a+3; //implicit multiplication is allowed.
a+=2; b /= a;

** Output control
a=1; //
!b=5/6; // -> echo the code in addition to the result
!//echo this comment line

#c=sin(30deg); //suppress output, but still execute the code.
!#d=a+b; //echo the code…but suppress the result
/# //suppress output for the next 4 lines.
a+=1;
a+=3;
a+=PI;
a+=7;
#/
a;

** Units
Distance=10km+1mi;
Imperial=Distance.mi;
Result=(Imperial+5m).km;
Hot=1000K.C.F; //Conversion from K to C to F. Intermediate conversion to C is pointless.
Hot=Hot.C; //set new unit for variable Hot
Implemented units:
Length:
Temperature:
…
** Basic expr:
Expression without assignment:
5*7;
TODO: put result in last line’s result _;

** Functions
Trigonometry: sin, cos, tan, asin, acos, atan
Other: max, min(????), round, abs;
Alternative for abs: |x|
TODO: round() and abs() in combination with units.
TODO: make min and max varargs?
