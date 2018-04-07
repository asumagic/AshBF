bf = Module.cwrap("brainfuck", "number", ["string", "string"]);
bf_loglevel = Module.cwrap("brainfuck_loglevel", null, ["number"]);
bf_out = console.log;