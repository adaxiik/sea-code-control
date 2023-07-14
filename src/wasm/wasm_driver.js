function main(code) {
    try {
        // const code = `
        // int main() {
        //     return 0;
        // }`;

        const interpreter = new Module.Interpreter();
        console.log(Module.debug_ast_as_puml(interpreter.parse(code)));
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
