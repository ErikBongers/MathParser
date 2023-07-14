export let errorsForLint = [];

export function clearErrorList() {
	errorsForLint = [];
}

function convertErrorToCodeMirror(e, doc) {
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
		console.error(e);
		throw err;
	}
}

function addErrorsToLint (errors) {
	for (let e of errors) {
		errorsForLint.push(convertErrorToCodeMirror(e, cm.editor.state.doc));
		if (e.stackTrace)
			addErrorsToLint(e.stackTrace);
	}
}

function formatNumber (numb) {
	let strFormatted = "";
	if (numb.fmt == "DEC")
		strFormatted = formatFloatString(numb.sig, numb.exp);
	else
		strFormatted = numb.fmtd;
	strFormatted += numb.u;
	return strFormatted;
}

function formatList (values) {
	let strFormatted = "";
	let strComma = "";
	values.forEach(value => {
		strFormatted += strComma + formatNumber(value);
		strComma = ", ";
	});
	return strFormatted;
}

function formatResult (line) {
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

function lineToString (line) {
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
		console.error(err);
		console.error(line);
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

export function outputResult(result, sourceIndex) {
	console.debug(result);
	clearErrorList();
	var strOutput = "";
	var strResult = "";
	try {
		result = JSON.parse(result); //may throw...
		for (let line of result.result) {
			let strLine = lineToString(line);
			if (strLine.length > 0)
				strOutput += strLine + "\n";
		}
		let lineCnt = 0;
		let lineAlreadyFilled = false;
		for (let line of result.result) {
			if (line.src != sourceIndex)
				continue;
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
	} catch (e) {
		strOutput = e.message + "\n";
		strOutput += e.name + "\n";
		strOutput += e.stack+ "\n";
		strResult = e.message + "\n";
	}
	let transaction = cm.cmOutput.state.update({ changes: { from: 0, to: cm.cmOutput.state.doc.length, insert: strOutput } });
	cm.cmOutput.update([transaction]);
	transaction = cm.cmResult.state.update({ changes: { from: 0, to: cm.cmResult.state.doc.length, insert: strResult } });
	cm.cmResult.update([transaction]);
}
