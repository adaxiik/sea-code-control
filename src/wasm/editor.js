function init_editor(editor_id)
{
    let editor = ace.edit(editor_id);
    editor.setTheme("ace/theme/monokai");
    editor.session.setMode("ace/mode/c_cpp");
    editor.setKeyboardHandler("ace/keyboard/vscode");
    editor.setValue(`#include <stdio.h>

void _scc_puti(int x);
void _scc_putc(char x);

int main()
{
    int a = 34;
    _scc_puti(a);
    
    a += 35;
    _scc_puti(a);

    _scc_putc('E');
        
    return 0;
}
`, 1); //1 = moves cursor to end

    editor.clearSelection();
    editor.focus();
    editor.setOptions({
        fontSize: "16pt",
        enableBasicAutocompletion: true,
        enableSnippets: true,
        enableLiveAutocompletion: true
    });

    return editor;
}