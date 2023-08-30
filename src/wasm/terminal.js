function init_terminal(terminal_id)
{
    let term = new Terminal({
        cursorBlink: true,
        scrollback: 1000,
    });
    let fitAddon = new FitAddon.FitAddon();
    term.loadAddon(fitAddon);
    term.open(document.getElementById(terminal_id));

    term.fit = fitAddon.fit.bind(fitAddon);
    term.fit();
    return term;
}