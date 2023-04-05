Module.onRuntimeInitialized = async _ => {
    Module.api = {
		parseMath: Module.cwrap('parseMath', 'string', ["string"]),
		getMathVersion: Module.cwrap('getMathVersion', 'string', []),
    };

	window.document.title = "Math Parser " + Module.api.getMathVersion();

	Module.convertErrorToCodeMirror = function (e, doc) {
		try {
			let start = doc.line(e.range.startLine + 1).from + e.range.startPos;
			let end = doc.line(e.range.endLine + 1).from + e.range.endPos;
			var hint = {
				message: e.message,
				severity: "error", //"warning"
				from: start,
				to: end
			};
			if (e.type == "W")
				hint.severity = "warning";
			return hint;
		}
		catch (err) {
			console.log(e);
			throw err;
		}
	};

	Module.addErrorsToLint = function (errors) {
		for (e of errors) {
			Module.errorsForLint.push(Module.convertErrorToCodeMirror(e, cm.editor.state.doc));
			if (e.stackTrace)
				Module.addErrorsToLint(e.stackTrace);
		}
	};

	Module.formatNumber = function (numb) {
		let strFormatted = "";
		if (numb.fmt == "DEC")
			strFormatted = Module.formatFloatString(numb.sig, numb.exp);
		else
			strFormatted = numb.fmtd;
		strFormatted += numb.u;
		return strFormatted;
	};

	Module.formatList = function (values) {
		let strFormatted = "";
		let strComma = "";
		values.forEach(value => {
			strFormatted += strComma + Module.formatNumber(value);
			strComma = ", ";
		});
		return strFormatted;
	};

	Module.resultToString = function (line) {
		if (line.onlyComment == true) {
			return "//" + line.comment;
		}
		let strComment = "";
		if (line.comment != "")
			strComment = " //" + line.comment;
		let strNL = "";
		try {
			Module.addErrorsToLint(line.errors);
		}
		catch (err) {
			console.log(line);
        }
		//just for top level errors in the output window:
		let strErrors = "";
		for (e of line.errors) {
			strErrors += "  " + e.message;
		}
		let strText = "";
		if (line.text != "")
			strText = " " + line.text;
		let strLine = "";
		let strFormatted = "";
		if (line.type == "NUMBER") {
			strFormatted = Module.formatNumber(line.number);
		}
		else if (line.type == "TIMEPOINT")
			strFormatted = line.date.formatted;
		else if (line.type == "DURATION") {
			strFormatted = line.duration.years + " years, " + line.duration.months + " months, " + line.duration.days + " days";
		}
		else if (line.type == "LIST") {
			strFormatted = Module.formatList(line.values);
		}

		if (line.mute == false || line.errors.length > 0)
			strLine += (line.id === "_" ? "" : line.id + "=") + strFormatted;
		strLine += strText + (strErrors === "" ? "" : "  <<<" + strErrors);
		strLine += strComment;
		return strLine;
	};

	Module.parseAfterChange = function(){
		Module.errorsForLint = [];
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
		return Module.errorsForLint;
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

	Module.log = function (text) {
    	console.log(text);
    	};

	Module.formatFloatString = function (floatString, exponent)
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