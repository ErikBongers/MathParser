Module.onRuntimeInitialized = async _ => {
    Module.api = {
      getMessage: Module.cwrap('getMessage', 'string', ["string"]),
    };
    console.log("api created:" + Module.api.getMessage("I was created."));
  };