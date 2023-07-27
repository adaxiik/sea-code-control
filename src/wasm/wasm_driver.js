function main(code) {
    try {
        const interpreter = new Module.Interpreter();
        const parsed = interpreter.parse(code);
        console.log(Module.debug_ast_as_puml(parsed));
        const result = interpreter.interpret_parserresult(parsed);
        if (result.is_error()) {
            console.log("ERROR");
        } else {
            console.log("OK");
        }
    } catch (e) {
        console.log(e);
    } finally {
        parser.delete();
    }
    console.log("done");
}

if (process.argv.length < 3) {
    console.log("Usage: node wasm_driver.js <file>");
    process.exit(1);
}

const fs = require("fs");
const filepath = process.argv[2];
const code = fs.readFileSync(filepath, "utf8");

var Module = {
    onRuntimeInitialized: () => main(code)
};

require("./scc.js")(Module);
