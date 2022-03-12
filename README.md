[![experimental](http://badges.github.io/stability-badges/dist/experimental.svg)](http://github.com/badges/stability-badges)
# MathParser
## Introduction
Mathparser is a mathematical expression language based on C-style syntax.
It was created out of a desire for a simple mathematical script language outside of spreadsheets, online c-compilers, mathlab and other math tools.
I think it may feel more familiar to programmers that those other tools.

## Language reference

### Basic syntax
```
//this is a comment line
a=3+4; //this is a typical statement.
b=2a+3; //implicit multiplication is allowed: same as (2*a)+3;
a+=2; b=a*10; //a statement always ends with a semi-colon, not a new line, so these are two statements.
0xFF; //hexadecimal notation for decimal 255
0b101; //binary notation for decimal 5
123e4; //scientific notation
```
### Operators
In addition to the usual `+ - * /` operators, there are:
```
a+=7; //same as a=a+7, also works with - * /
a=3^2; //exponent operator
a=|-3|; //absolute value operator
a++; //increment operator: same as a=a+1 or a+=1;
a--; //decrement operator.
```
### Output control
```
a=2*7; //will output only the variable and it's result: a=14
7*3; //will only output the result: 21
```

##### Echo
An exclamation mark `!` is used to echo the full expression to the output.
```
!b=60/3; //Will echo the code together with the result: b=20 b=60/3
!//this comment line will appear on the output
!q=100/10; !//some comment  --> output: q=10 q=100/10 //some comment
```
##### Mute
A hashtag `#` is used to mute the result.
```
#c=sin(30deg); //suppress output, but still execute the code.
!#d=a+b; //execute the statement, echo the code, but mute the result
/# //suppress output for the next 4 lines, with intermediate results
a+=1;
a+=3;
a+=PI;
a+=7;
#/
a; //finally, the result will be output
```
### Units
```
Distance=10km+1mi; //Distance will be expressed in the first unit: km.
Imperial=Distance.mi; //Conversion
Result=(Imperial+5m).km; //Conversion of an expression.
Hot=1000K.C.F; //Conversion from K to C to F. Note that the intermediate conversion to C is pointless.
Hot.C; //Output the value of Hot in Celcius. Note that the variable Hot remains in K!
Hot=Hot.C; //Convert Hot to Celcius.
Hot.=C; //Does the same as the above line.

```
#### Implemented units:
* Angle: `rad, deg`
* Length: `km, m, cm, mm, um (micron), in, ft, mi, thou, yd` [TODO] complete this list
* Temperature: `C, F, K`
* Mass (weight): `kg, g, mg, t, lb (lbs), oz, N` (note that for convenience no distinction is made between weight and mass)
* Volume: `L, ml, gal, pt`

### Functions
* Trigonometry: `sin, cos, tan, asin, acos, atan`
* Other: `max(val1, val2,...), min(val1,val2,...), round, abs`
* `|x|` is the same as `abs(x)`

* [TODO]: round() and abs() in combination with units.
* [TODO]: make min and max varargs?

### Constants
Currently only PI. (also in lower case)

## Technical
The main parser project is **MathParserDll** and is written in C++. It is a homebrew recursive descent parser (I think) with 1 look-ahead and 1 push-back.
Parser errors are handled gracefully.
### Projects
* **MathParserDll** main parser project.
* **MathParserWASM** is a web project that compiles the dll into a WASM file. It is the main user interface.
* **MathParserWPF** a Windows desktop GUI for the parser dll. 
* **MathParserLib** C# version. [![deprecated](http://badges.github.io/stability-badges/dist/deprecated.svg)](http://github.com/badges/stability-badges)
* **MathParserLibTests** C# tests. [![deprecated](http://badges.github.io/stability-badges/dist/deprecated.svg)](http://github.com/badges/stability-badges)

Those who are into parser development may want to take a look at the `.ebnf` file (which stands for *Erik's BNF* :) )

## Build stack
All projects in **Visual Studio**, except for the WASM project, which is build in **VS Code**.
The WASM compilation obviously uses `emscripten`.
In addition `rollup` is used to bundle the CodeMirror 6 online editor.
Note that a separate parser is written for CodeMirror syntax highlighting.
The (json) output of the C++ parser is fed to CodeMirror's *linter* for error annotation.
## Try it
There's a online version available at [Google Cloud Platform](https://storage.googleapis.com/mathparser/index.html).

## Disclaimer
This is a personal project, for fun.  
Don't let your rocket launches depend on it.
