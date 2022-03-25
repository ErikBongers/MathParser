Module.onRuntimeInitialized = async _ => {
    Module.api = {
		parseMath: Module.cwrap('parseMath', 'string', ["string"]),
		getMathVersion: Module.cwrap('getMathVersion', 'string', []),
    };

	window.document.title = "Math Parser " + Module.api.getMathVersion();
	Module.convertErrorToCodeMirror = function(e, doc){
		let pos = doc.line(e.line+1).from + e.pos;
		var hint = {
            message: e.message,
            severity: "error", //"warning"
            from: pos,
            to: pos+1
          };
      return hint;
	};

	Module.resultToString = function (line) {
		if (line.onlyComment == true) {
			return "//" + line.comment;
		}
		let strComment = "";
		if (line.comment != "")
			strComment = " //" + line.comment;
		let strNL = "";
		let strErrors = "";
		for (e of line.errors) {
			strErrors += "  " + e.message;
			Module.parserErrors.push(Module.convertErrorToCodeMirror(e, cm.editor.state.doc));
		}
		let strText = "";
		if (line.text != "")
			strText = " " + line.text;
		let strLine = "";
		let strFormatted = Module.formatFloatString(line.number.significand, line.number.exponent);
		if (line.number.format != "DEC")
			strFormatted = line.number.formatted;
		if (line.mute == false || line.errors.length > 0)
			strLine += (line.id === "#result#" ? "" : line.id + "=") + strFormatted + line.number.unit;
		strLine += strText + (strErrors === "" ? "" : "  <<<" + strErrors);
		strLine += strComment;
		return strLine;
	};

	Module.parseAfterChange = function(){
		Module.parserErrors = [];
		localStorage.savedCode = cm.editor.state.doc.toString();
		var result = Module.api.parseMath(cm.editor.state.doc.toString());
		Module.log(result);
		result = JSON.parse(result);
		var strOutput = "";
		for(line of result.result) {
			let strLine = Module.resultToString(line);
			if (strLine.length > 0)
				strOutput += strLine + "\n";
		}
		let transaction = cm.cmOutput.state.update({changes: {from: 0, to: cm.cmOutput.state.doc.length, insert: strOutput}});
		cm.cmOutput.update([transaction]);
	};

	cm.setLintSource((view) => {
		Module.parseAfterChange();
		return Module.parserErrors;
	});
	if(localStorage.savedCode) {
		let transaction = cm.editor.state.update({changes: {from: 0, to: cm.editor.state.doc.length, insert: localStorage.savedCode}});
		cm.editor.update([transaction]);
		}

	Module.print = (function() 
            {
            var txtOutput = document.getElementById('txtOutput');
            if (txtOutput) txtOutput.value = '';
            
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
    Module.formatFloatString = function(floatString, exponent)
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
		if (exponent !== 0) {
			sResult += "E" + exponent;
        }
		return sResult;
    	};
  };