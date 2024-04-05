function main(code) {
    try {
        const interpreter = new Module.Interpreter();
        let running_interpreter = interpreter.interpret_string(code);
        if (running_interpreter.is_error()) {
            throw "Error creating interpreter";
        }
        running_interpreter = running_interpreter.value();
        const result = running_interpreter.continue_execution();
        if (result.is_error()) {
            throw "Error running interpreter";
        }
    } catch (e) {
        // console.log(e);
        process.exit(1);
    } finally {
        interpreter.delete();
        running_interpreter.delete();
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
