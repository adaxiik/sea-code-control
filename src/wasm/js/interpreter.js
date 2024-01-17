// im really sorry for this dirty code, but i dont enjoy programming in js
// and i dont want to spend more time on this.. take a look at src/*.cpp instead :)

function deleteIfExists(obj) {
    if (obj) {
        obj.delete();
    }
    return null;
}

function hideById(id) {
    document.getElementById(id).hidden = true;
}
function showById(id) {
    document.getElementById(id).hidden = false;
}

function interpreterReset() {
    interpreter = deleteIfExists(interpreter);
    running_interpreter = deleteIfExists(running_interpreter);
    hideById("continue");
    hideById("next");
    showById("run");
    showById("examples");
    showById("precompile-checkbox");
    editor.setReadOnly(false);
}

function addBreakpointsToRunningInterpreter(running_interpreter) {
    let breakpoints = editor.session.getBreakpoints();
    for (let i = 0; i < breakpoints.length; i++) {
        if (breakpoints[i]) {
            running_interpreter.breakpoints().add(i);
        }
    }
}

function removeAllMarkers() {
    // https://stackoverflow.com/questions/65677814/how-to-remove-all-the-existing-highlight-markers-in-ace-editor-using-react
    const markers = editor.session.getMarkers();
    if (markers) {
        const prevMarkersArr = Object.keys(markers);
        for (let item of prevMarkersArr) {
            if (markers[item].clazz === 'line-highlight') {
                editor.session.removeMarker(markers[item].id);
            }
        }
    }
}

var clangAPI = null;
// returns true if the code compiles
function precompile(code) {
    if (clangAPI == null) {
        const apiOptions = {
            async readBuffer(filename) {
                const fileNameWithPrefix = "wasm-clang/" + filename;
                const response = await fetch(fileNameWithPrefix);
                return response.arrayBuffer();
            },

            async compileStreaming(filename) {
                // TODO: make compileStreaming work. It needs the server to use the
                // application/wasm mimetype.
                const fileNameWithPrefix = "wasm-clang/" + filename;
                if (false && WebAssembly.compileStreaming) {
                    return WebAssembly.compileStreaming(fetch(fileNameWithPrefix));
                } else {
                    const response = await fetch(fileNameWithPrefix);
                    return WebAssembly.compile(await response.arrayBuffer());
                }
            },

            hostWrite(s) {
                // port.postMessage({id : 'write', data : s}); 
                stderr_term(s);
            }
        };
        clangAPI = new API(apiOptions);
    }

    let programName = document.getElementById("examples").value.split(".")[0];

    return clangAPI
        .compile({ input: programName + ".c", obj: programName + ".o", contents: editor.getValue() })
        .then(() => {
            console.log("Compilation successful");
            return true;
        })
        .catch((e) => {
            console.log("Compilation failed: " + e);
            return false;
        });

}

var interpreter = null;
var running_interpreter = null;

async function runProgram() {
    interpreter = deleteIfExists(interpreter);
    running_interpreter = deleteIfExists(running_interpreter);

    editor.setReadOnly(true);
    try {
        buffered_stdout = "";
        interpreter = new Module.Interpreter();
        let running_interpreter_creation_result = interpreter.interpret_string(editor.getValue());
        if (running_interpreter_creation_result.is_error()) {
            throw "Error creating interpreter";
        }
        running_interpreter = running_interpreter_creation_result.value();
        addBreakpointsToRunningInterpreter(running_interpreter);

        hideById("run");
        hideById("examples");
        hideById("precompile-checkbox");
        showById("continue");
        showById("next");
        continueExecution();
    } catch (e) {
        console.log(e);
        interpreterReset();
    }
}

function run() {
    if (document.getElementById("precompile").checked) {
        precompile(editor.getValue())
            .then((result) => {
                if (result) {       // compilation successful
                    runProgram();
                }
            });
    } else {
        runProgram();

    }
}

function errorIntoTerm(error) {
    
    function outRed(s) {
        const RED = "\x1b[31m";
        const RESET = "\x1b[0m";
        stdout_term(RED + s + RESET + "\n");
    }

    if (error == Module.InterpreterError.RuntimeError) {
        outRed("Segmentation fault (core dumped)");
    } else if (error == Module.InterpreterError.ReachedUnreachableError) {
        outRed("Illegal instruction (core dumped)");
    } else if (error == Module.InterpreterError.DivisionByZeroError) {
        outRed("Floating point exception (core dumped)");
    } else if (error == Module.InterpreterError.AssertionFailedError) {
        outRed("Assertion failed (core dumped)");
    } else if (error == Module.InterpreterError.MemoryError) {
        outRed("Memory error (core dumped)");
    } else if (error == Module.InterpreterError.DereferenceError) {
        outRed("Memory error (core dumped)");
    }

}

function continueExecution() {
    try {
        removeAllMarkers();

        let result = running_interpreter.continue_execution();
        if (result.is_ok()) {
            console.log("Execution finished");
            interpreterReset();
            return;
        }

        let error = result.get_error();
        if (error == Module.InterpreterError.BreakpointReachedError) {
            console.log("Breakpoint reached");
            let current_line = Module.get_location_row(running_interpreter.current_location());
            editor.gotoLine(current_line + 1);
            editor.session.addMarker(new ace.Range(current_line, 0, current_line, 1), "line-highlight", "fullLine");

            visualizeRunningInterpreter(running_interpreter);

            return;
        }

        console.log("Error during interpreting: " + Module.debug_interpreter_error_as_text(error));
        errorIntoTerm(error);
        interpreterReset();
    } catch (e) {
        console.log(e);
        interpreterReset();
    }
}

function next() {
    try {
        removeAllMarkers();

        let result = running_interpreter.next();
        if (result.is_error()) {
            errorIntoTerm(result.get_error());
            throw Module.debug_interpreter_error_as_text(result.get_error());
        }

        let current_line = Module.get_location_row(running_interpreter.current_location());
        editor.gotoLine(current_line + 1);
        editor.session.addMarker(new ace.Range(current_line, 0, current_line, 1), "line-highlight", "fullLine");
        visualizeRunningInterpreter(running_interpreter);

    } catch (e) {
        console.log(e);
        interpreterReset();
    }

}
