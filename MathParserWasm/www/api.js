window.addEventListener('load', (event) => {
	if(localStorage.savedCode)
		myCodeMirror.doc.setValue(localStorage.savedCode);
  });

Module.onRuntimeInitialized = async _ => {
    Module.api = {
		parseMath: Module.cwrap('parseMath', 'string', ["string"]),
		getMathVersion: Module.cwrap('getMathVersion', 'string', []),
    };

	window.document.title = "Math Parser " + Module.api.getMathVersion();
	Module.convertErrorToCodeMirror = function(e){
		var hint = {
            message: e.message,
            severity: "error", //"warning"
            from: CodeMirror.Pos(e.line, e.pos),
            to: CodeMirror.Pos(e.line, e.pos+1)
          };
      return hint;
	};

	Module.parseAfterChange = function(){
		Module.parserErrors = [];
		cmOutput.setValue("");
		var result = Module.api.parseMath(myCodeMirror.doc.getValue());
		Module.log(result);
		result = JSON.parse(result);
		var strOutput = "";
		for(line of result.result)
			{
			let strComments = "";
			let strNL = "";
			for(cmt of line.comments)
				{
				strComments += strNL + cmt;
				strNL = "\n";
				}
			let strErrors = "";
			for(e of line.errors)
				{
				strErrors += "  " + e.message;
				Module.parserErrors.push(Module.convertErrorToCodeMirror(e));
				}
			let strText = "";
			if(line.text != "")
			  	strText = " " + line.text;
			let strLine = "";
			if(line.mute == false || line.errors.length > 0)
				strLine += (line.id==="#result#" ? "" : line.id + "=")  + Module.formatFloatString(line.value) + line.unit;
			strLine += strText + (strErrors === ""? "" : "  <<<" + strErrors)
			if(strComments.length > 0)
				strOutput+= strComments + "\n";
			if(strLine.length > 0)
				strOutput+= strLine + "\n";
			}
		cmOutput.setValue(strOutput);
	};
//this code requires parseAfterChange() to be defined:
	myCodeMirror.on("change", function(instance, changeObj){
		localStorage.savedCode = myCodeMirror.doc.getValue();
		Module.parseAfterChange(localStorage.savedCode);
});

Module.print = (function() 
            {
            var txtOutput = document.getElementById('txtOutput');
            if (txtOutput) txtOutput.value = ''; // clear browser cache
            
            return function(text) 
                {
                if (arguments.length > 1) 
                    text = Array.prototype.slice.call(arguments).join(' ');
                if (txtOutput) 
                    {
                    txtOutput.value += text + "\n";
                    txtOutput.scrollTop = txtOutput.scrollHeight; // focus on bottom
                    }
                };
            })();
    Module.log = function(text) {
    	console.log(text);
    	};
    Module.formatFloatString = function(floatString)
    	{
		var sFixed = parseFloat(floatString).toFixed(5);
		if(sFixed.search(".") == -1)
			return sFixed;
		var sResult = "";
		var removeLastZero = true;
		[...sFixed].slice().reverse().forEach(function(c) 
			{
			if(c === '.' && removeLastZero) //all decimals are zero > remove dot
				{
				removeLastZero = false;
				return;
				}
			if(c!== '0' || !removeLastZero)
				{
				removeLastZero = false;
				sResult = c + sResult;
				}
			});
		return sResult;
    	};
  };