Module.onRuntimeInitialized = async _ => {
    Module.api = {
		parseMath: Module.cwrap('parseMath', 'string', ["string"]),
		getMathVersion: Module.cwrap('getMathVersion', 'string', []),
    };

	window.document.title = "Math Parser " + Module.api.getMathVersion();

	Module.parseAfterChange = function(){
		document.getElementById('txtOutput').value = "";
		var result = Module.api.parseMath(document.getElementById('txtInput').value);
		Module.log(result);
		result = JSON.parse(result);
		for(line of result.result)
			{
			let strErrors = "";
			for(e of line.errors)
				{
				strErrors += "  " + e.message;
				}
			strErrors
			Module.print((line.id==="#result#" ? "" : line.id + "=")  + Module.formatFloatString(line.value) + line.unit + (strErrors === ""? "" : "  <<<" + strErrors));
			}
	};
	document.getElementById('txtInput')
		.addEventListener('input', function()
			{
				Module.parseAfterChange();
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