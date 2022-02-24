(function(mod) {
    if (typeof exports == "object" && typeof module == "object") // CommonJS
      mod(require("../../lib/codemirror"));
    else if (typeof define == "function" && define.amd) // AMD
      define(["../../lib/codemirror"], mod);
    else // Plain browser env
      mod(CodeMirror);
  })(function(CodeMirror) {
  "use strict";
  
  var parser = function() {
  
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
          var ret = this.parseOutputQualifiersAndComments(stream, state, peek);
          if(ret)
            return ret;
          if(stream.match(/[a-zA-Z_][a-zA-Z_0-9]*/))
            return "variable-2";
          else if(stream.match(/[0-9]+\.?[0-9]*/))
            return "number";
          else if(stream.match("<<<")) {
            stream.skipToEnd();
            return "error";
          }
          stream.next(); //TODO: don't return on each 'normal' char, but group them into one token.
          return null;
      }, //token()

      parseOutputQualifiersAndComments: function(stream, state, peek){
        if(peek == '#') {
          stream.next();
          if(stream.peek() == '/'){
            stream.next();
          }
          return "output-qualifier";
        }
        else if(peek == '!'){
          stream.next();
          return "output-qualifier";
        }
        else if (peek == '/') {
          stream.next();
          if(stream.peek() == '/') {
            stream.skipToEnd();
            return "comment";
          }
          else if(stream.peek() == '*'){
            stream.next();
            state.inBlockComment = true;
            return "comment";
          }
          else if(stream.peek() == '#'){
            stream.next();
            return "output-qualifier";
          }
        }
        else
          return null;

      }, //parseOutputQualifiers()

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
  };

  CodeMirror.defineMode("math_parser", parser);
  CodeMirror.defineMIME("text/mathparser", "mpx");
  
  });