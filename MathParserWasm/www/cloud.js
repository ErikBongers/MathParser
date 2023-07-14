import * as mp from "./lib.js"

let userSession = {};

export function onSignedIn(googleUserToken) {
    userSession = JSON.parse(mp.getCookie("mathparserSession"));
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
            console.debug("Request complete! response:", jsonUserSession);
            document.getElementById("userName").innerHTML = jsonUserSession.user.name;
            userSession = jsonUserSession;
            mp.setCookie("mathparserSession", JSON.stringify(userSession), 1);
            let scriptId = document.getElementById("scriptSelector").value;
            promptAndUseServerFile(scriptId);
        });
}

async function promptAndUseServerFile(scriptId) {
    let localText = getLocalScript(scriptId);
    let txt = localText;
    if (userSession.sessionId) {
        let cloudText = await downloadScript(scriptId);
        if (cloudText !== localText) {
            if (cloudText !== undefined) {
                if (confirm("Use server file : " + scriptId + "?")) {
                    txt = cloudText;
                }
            } else {
                console.log("cloud text undefined. New file?");
            }
        } else {
            console.log("Server file is da same...");
        }
    }
    let transaction = cm.editor.state.update({ changes: { from: 0, to: cm.editor.state.doc.length, insert: txt } });
    cm.editor.update([transaction]);
}

function uploadScript(scriptId, text) {
    if (!userSession.sessionId) {
        console.log("Not uploading: no session.");
        return;
    }
    let url = new URL("https://europe-west1-ebo-tain.cloudfunctions.net/cloud-script");
    url.searchParams.append("sessionId", userSession.sessionId);
    url.searchParams.append("scriptId", scriptId);
    fetch(url, {
        method: "POST",
        mode: "cors",
        headers: { 'Content-Type': 'plain/text' },
        body: text
    }).then(res => res.json())
        .then(jsonResult => {
            console.log("Upload complete!");
            console.debug("response:", jsonResult);
        });

}

async function downloadScript(scriptId) {
    if (!userSession.sessionId)
        throw new Error("Can't upload: no session.");

    let url = new URL("https://europe-west1-ebo-tain.cloudfunctions.net/cloud-script");
    url.searchParams.append("sessionId", userSession.sessionId);
    url.searchParams.append("scriptId", scriptId);

    const response = await fetch(url, {
        method: "GET",
        mode: "cors",
        headers: { 'Content-Type': 'plain/text' }
    });

    const text = await response.text();
    console.log("download complete!");
    console.debug("response:", text);
    return text;
}

function getLocalScript(scriptId) {
    let txt = "";
    if (scriptId == "start")
        txt = localStorage.savedStartCode;

    else
        txt = localStorage.savedCode;
    return txt;
}


export function saveScript(scriptId) {
    uploadScript(scriptId, cm.editor.state.doc.toString());
    if (scriptId == "start") {
        localStorage.savedStartCode = cm.editor.state.doc.toString();
    } else {
        localStorage.savedCode = cm.editor.state.doc.toString();
    }
}

export function onScriptSwitch() {
    let scriptId = document.getElementById("scriptSelector").value
    if (scriptId == "start") {
        //saveScript("script1");
        promptAndUseServerFile("start");
    } else {
        //saveScript("start");
        promptAndUseServerFile("script1");
    }
}

export function startUp() {
    window.document.title = "Math Parser " + Module.api.getMathVersion();

    cm.setLintSource((view) => {
        afterEditorChange();
        return mp.errorsForLint;
    });

    let txt = getLocalScript("script1");
    let transaction = cm.editor.state.update({ changes: { from: 0, to: cm.editor.state.doc.length, insert: txt } });
    cm.editor.update([transaction]);
}

export function afterEditorChange() {
    let scriptId = document.getElementById("scriptSelector").value
    saveScript(scriptId);
    parseAfterChange(scriptId);
}

function parseAfterChange(scriptId) {
    let result = {};
    let sourceIndex = -1;
    if (scriptId != "start") {
        if (!localStorage.savedStartCode)
            localStorage.savedStartCode = "";
        Module.api.uploadSource("start", localStorage.savedStartCode);
        sourceIndex = Module.api.uploadSource(scriptId, cm.editor.state.doc.toString());
        result = Module.api.parseMath("start", scriptId);
    } else {
        sourceIndex = Module.api.uploadSource(scriptId, cm.editor.state.doc.toString());
        result = Module.api.parseMath("", scriptId);
    }
    mp.outputResult(result, sourceIndex);
}
