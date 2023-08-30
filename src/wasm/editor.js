function init_editor(editor_id)
{
    let editor = ace.edit(editor_id);
    editor.setTheme("ace/theme/monokai");
    editor.session.setMode("ace/mode/c_cpp");
    editor.setValue("int a = 10;", 1); //1 = moves cursor to end
    editor.clearSelection();
    editor.focus();
    editor.setOptions({
        fontSize: "16pt"
    });

    return editor;
}