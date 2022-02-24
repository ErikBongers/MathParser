(function(mod) {
    if (typeof exports == "object" && typeof module == "object") // CommonJS
      mod(require("../../lib/codemirror"));
    else if (typeof define == "function" && define.amd) // AMD
      define(["../../lib/codemirror"], mod);
    else // Plain browser env
      mod(CodeMirror);
  })(function(CodeMirror) {
    "use strict";
  
    function validator(text, options) {
        console.log("validating...\n");
        return CodeMirror.mathparser.Module.parserErrors;
    }
  
    CodeMirror.registerHelper("lint", "math_parser", validator);
  
  });
  