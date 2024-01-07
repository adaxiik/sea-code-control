function initEditor(editor_id)
{
    let editor = ace.edit(editor_id);
    editor.setTheme("ace/theme/monokai");
    editor.session.setMode("ace/mode/c_cpp");
    editor.setKeyboardHandler("ace/keyboard/vscode");
//     editor.setValue(`#include <stdio.h>

// void _scc_puti(int x);
// void _scc_putc(char x);

// int main()
// {
//     int a = 34;
//     _scc_puti(a);
    
//     a += 35;
//     _scc_puti(a);

//     for (int i = 'a'; i < 'z'; i+=1)
//     {
//         _scc_putc(i);
//     }
//     _scc_putc('\\n');
        
//     return 0;
// }
// `, 1); //1 = moves cursor to end

    editor.clearSelection();
    editor.focus();
    editor.setOptions({
        fontSize: "16pt",
        enableBasicAutocompletion: true,
        enableSnippets: true,
        enableLiveAutocompletion: true
    });

    // https://ourcodeworld.com/articles/read/1052/how-to-add-toggle-breakpoints-on-the-ace-editor-gutter
    editor.on("guttermousedown", function(e) {
        var target = e.domEvent.target;
    
        if (target.className.indexOf("ace_gutter-cell") == -1){
            return;
        }
    
        if (!editor.isFocused()){
            return; 
        }
    
        if (e.clientX > 25 + target.getBoundingClientRect().left){
            return;
        }
    
        var breakpoints = e.editor.session.getBreakpoints(row, 0);
        var row = e.getDocumentPosition().row;
    
        // If there's a breakpoint already defined, it should be removed, offering the toggle feature
        if(typeof breakpoints[row] === typeof undefined){
            e.editor.session.setBreakpoint(row);
        }else{
            e.editor.session.clearBreakpoint(row);
        }
    
        e.stop();
    });

    // https://github.com/ajaxorg/ace/issues/1282
    editor.on("change", function (e) {
        if (e.lines.length > 1 && (e.action==='insert' || e.action==='remove')){
            const breakpointsArrayOld = editor.session.getBreakpoints();
            let breakpointsArrayNew = [];
    
            const amountOfLinesAffected = e.lines.length - 1;
            const startRow = e.start.row;
            const endRow = e.end.row;
    
            for (const key of Object.keys(breakpointsArrayOld)) {
                let breakpointRow = parseInt(key)
    
                if (e.action==='insert') {  // new lines
                    if (breakpointRow > startRow ){
                        // breakpoint forward
                        breakpointsArrayNew[breakpointRow + amountOfLinesAffected] = "ace_breakpoint"
                    }
                    else {
                        // unaffected by insert
                        breakpointsArrayNew[breakpointRow] = "ace_breakpoint"
                    }
                }
                else if (e.action==='remove') {  // removed lines
                    if (breakpointRow > startRow && breakpointRow <= endRow ){
                        // breakpoint removed
                    }
                    else if (breakpointRow >= endRow ){
                        // breakpoint behind
                        breakpointsArrayNew[breakpointRow - amountOfLinesAffected] = "ace_breakpoint"
                    }
                    else {
                        // unaffected by remove
                        breakpointsArrayNew[breakpointRow] = "ace_breakpoint"
                    }
                }
            }
    
            // remove all old breakpoints
            for (const key of Object.keys(breakpointsArrayOld)) {
                let breakpointRow = parseInt(key)
                editor.session.clearBreakpoint(breakpointRow);
            }
    
            // add all new breakpoints
            for (const key of Object.keys(breakpointsArrayNew)) {
                let breakpointRow = parseInt(key)
                editor.session.setBreakpoint(breakpointRow);
            }
        }
    })
    return editor;
}