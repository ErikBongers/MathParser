(function(mod) {
    if (typeof exports == "object" && typeof module == "object") // CommonJS
      mod(require("../../lib/codemirror"));
    else if (typeof define == "function" && define.amd) // AMD
      define(["../../lib/codemirror"], mod);
    else // Plain browser env
      mod(CodeMirror);
  })(function(CodeMirror) {
  "use strict";
  
  CodeMirror.defineMode("math_parser", function() {
  
    return {
      startState: function() {
        return {
          inBlockComment: false
        }
      },

      token: function(stream, state) {
          if(state.inBlockComment)
            return this.parseBlockComment(stream, state);
          var peek = stream.peek();
          if(peek == '#' || peek == '!') {
            stream.next();
            return "output-qualifier";
            }
          else if (peek == '/') {
            stream.next();
            if(stream.peek() == '/') {
              stream.skipToEnd();
              return "comment";
            }
            else if(stream.peek() == '*')
            {
              stream.next();
              state.inBlockComment = true;
              return "comment";
            }
          }
        //TODO: parse identifers and postfixes (units)
        stream.next(); //TODO: don't return on each 'normal' char, but group them into one token.
        return null;
      }, //token()

      parseBlockComment: function(stream, state){
        if(stream.skipTo("*/")){
          stream.next(); // "*"
          stream.next(); // "/"
          state.inBlockComment = false;
        } else {
          stream.skipToEnd();
        }
      return "comment";
      } //parseBlockComment()
    };
  });
  
  CodeMirror.defineMIME("text/mathparser", "mpx");
  
  });