export let errorsForLint = [];
export function clearErrorList() {
	errorsForLint = [];
}

export function convertErrorToCodeMirror(e, doc) {
	try {
		let start = doc.line(e.range.startLine + 1).from + e.range.startPos;
		let end = doc.line(e.range.endLine + 1).from + e.range.endPos;
		let hint = {
			message: e.message,
			severity: "error", //"warning"
			from: start,
			to: end
		}
		if (e.type == "W")
			hint.severity = "warning";
		return hint;
	}
	catch (err) {
		console.log(e);
		throw err;
	}
}

export function addErrorsToLint (errors) {
	for (let e of errors) {
		errorsForLint.push(convertErrorToCodeMirror(e, cm.editor.state.doc));
		if (e.stackTrace)
			addErrorsToLint(e.stackTrace);
	}
}

export function formatNumber (numb) {
	let strFormatted = "";
	if (numb.fmt == "DEC")
		strFormatted = formatFloatString(numb.sig, numb.exp);
	else
		strFormatted = numb.fmtd;
	strFormatted += numb.u;
	return strFormatted;
}

export function formatList (values) {
	let strFormatted = "";
	let strComma = "";
	values.forEach(value => {
		strFormatted += strComma + formatNumber(value);
		strComma = ", ";
	});
	return strFormatted;
}

export function formatResult (line) {
	let strFormatted = "";
	if (line.type == "NUMBER" || line.type == "N") {
		strFormatted = formatNumber(line.number);
	}
	else if (line.type == "TIMEPOINT" || line.type == "T")
		strFormatted = line.date.formatted;
	else if (line.type == "DURATION" || line.type == "D") {
		strFormatted = line.duration.years + " years, " + line.duration.months + " months, " + line.duration.days + " days";
	}
	else if (line.type == "LIST" || line.type == "L") {
		strFormatted = formatList(line.values);
	}
	return strFormatted;
}

export function lineToString (line) {
	if (line.onlyComment == true) {
		return "//" + line.comment;
	}
	let strComment = "";
	if (line.comment != "")
		strComment = " //" + line.comment;
	let strNL = "";
	try {
		addErrorsToLint(line.errors);
	}
	catch (err) {
		console.log(err);
		console.log(line);
	}
	//just for top level errors in the output window:
	let strErrors = "";
	for (let e of line.errors) {
		strErrors += "  " + e.message;
	}
	let strText = "";
	if (line.text != "")
		strText = " " + line.text;
	let strLine = "";
	let strFormatted = formatResult(line);

	if (line.mute == false || line.errors.length > 0)
		strLine += (line.id === "_" ? "" : line.id + "=") + strFormatted;
	strLine += strText + (strErrors === "" ? "" : "  <<<" + strErrors);
	strLine += strComment;
	return strLine;
}

export function ResultToString (line) {
	if (line.onlyComment == true) {
		return line.comment;
	}
	let strComment = "";
	if (line.comment != "")
		strComment = line.comment + ": ";
	let strLine = "";
	strLine += strComment;
	strLine += (line.id === "_" ? "" : line.id + "=") + formatResult(line);
	return strLine;
}

function formatFloatString (floatString, exponent) {
	var sFixed = parseFloat(floatString).toFixed(5);
	if (sFixed.search(".") == -1)
		return sFixed;
	var sResult = "";
	var removeLastZero = true;
	[...sFixed].slice().reverse().forEach(function (c) {
		if (c === '.' && removeLastZero) //all decimals are zero > remove dot
		{
			removeLastZero = false;
			return;
		}
		if (c !== '0' || !removeLastZero) {
			removeLastZero = false;
			sResult = c + sResult;
		}
	});
	if (exponent !== 0) {
		sResult += "E" + exponent;
	}
	return sResult;
}

export function log (text) {
	console.log(text);
}
export function onSignedIn (googleUserToken) {

	let userSession = JSON.parse(getCookie("mathparserSession"));
	let params = "";
	if (userSession && userSession.sessionId)
		params = "?sessionId=" + encodeURIComponent(userSession.sessionId);
	fetch("https://europe-west1-ebo-tain.cloudfunctions.net/get-session" + params, {
		method: "POST",
		mode: "cors",
		headers: { 'Content-Type': 'application/json' },
		body: JSON.stringify(googleUserToken)
	}).then(res => res.json())
		.then(jsonUserSession => {
			console.log("Request complete! response:", jsonUserSession);
			document.getElementById("userName").innerHTML = jsonUserSession.user.name;
			userSession = jsonUserSession;
			setCookie("mathparserSession", JSON.stringify(userSession), 1);
		});

}
export function setCookie(name, value, days) {
	var expires = "";
	if (days) {
		let date = new Date();
		date.setTime(date.getTime() + (days * 24 * 60 * 60 * 1000));
		expires = "; expires=" + date.toUTCString();
	}
	document.cookie = name + "=" + (value || "") + expires + "; path=/";
}
export function getCookie (name) {
	var nameEQ = name + "=";
	var ca = document.cookie.split(';');
	for (let i = 0; i < ca.length; i++) {
		let c = ca[i];
		while (c.charAt(0) == ' ') c = c.substring(1, c.length);
		if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length, c.length);
	}
	return null;
}

export function loadScript(scriptId) {
	let txt = "";
	if (scriptId == "start")
		txt = localStorage.savedStartCode;
	else
		txt = localStorage.savedCode;

	if (localStorage.savedCode) {
		let transaction = cm.editor.state.update({ changes: { from: 0, to: cm.editor.state.doc.length, insert: txt } });
		cm.editor.update([transaction]);
	}
}

export function saveScript(scriptId) {
	if (scriptId == "start") {
		localStorage.savedStartCode = cm.editor.state.doc.toString();
	} else {
		localStorage.savedCode = cm.editor.state.doc.toString();
	}
}

export function afterEditorChange () {
	let scriptId = document.getElementById("scriptSelector").value
	saveScript(scriptId);
	parseAfterChange();
}

export function parseAfterChange () {
	clearErrorList();
	var result = Module.api.parseMath(cm.editor.state.doc.toString());
	log(result);
	result = JSON.parse(result);
	var strOutput = "";
	var strResult = "";
	for (let line of result.result) {
		let strLine = lineToString(line);
		if (strLine.length > 0)
			strOutput += strLine + "\n";
	}
	let lineCnt = 0;
	let lineAlreadyFilled = false;
	for (let line of result.result) {
		//goto the next line in output
		while (lineCnt < line.line) {
			strResult += "\n";
			lineCnt++;
			lineAlreadyFilled = false;
		}
		let strValue = ResultToString(line);
		if (lineAlreadyFilled)
			strResult += " | ";
		strResult += strValue;
		lineAlreadyFilled = true;
	}
	let transaction = cm.cmOutput.state.update({ changes: { from: 0, to: cm.cmOutput.state.doc.length, insert: strOutput } });
	cm.cmOutput.update([transaction]);
	transaction = cm.cmResult.state.update({ changes: { from: 0, to: cm.cmResult.state.doc.length, insert: strResult } });
	cm.cmResult.update([transaction]);
}

export function onScriptSwitch () {
	let scriptId = document.getElementById("scriptSelector").value
	if (scriptId == "start") {
		saveScript("script1");
		loadScript("start");
	} else {
		saveScript("start");
		loadScript("script1");
	}
}

export function startUp () {
	window.document.title = "Math Parser " + Module.api.getMathVersion();

	cm.setLintSource((view) => {
		afterEditorChange();
		return errorsForLint;
	});

	loadScript("script1");
}

