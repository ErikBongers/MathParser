import * as mp from "./lib.js"

Module.onRuntimeInitialized = async _ => {
	Module.api = {
		parseMath: Module.cwrap('parseMath', 'string', ["string"]),
		getMathVersion: Module.cwrap('getMathVersion', 'string', []),
	};

	mp.startUp();
};