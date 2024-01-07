// var layout_config = {
//     content: [{
//         type: 'row',
//         content: [{
//             type: 'component',
//             isClosable: false,
//             componentName: 'editor',
//         }, {
//             type: 'column',
//             content: [{
//                 type: 'component',
//                 isClosable: false,
//                 componentName: 'visualizer',
//             }, {
//                 type: 'component',
//                 isClosable: false,
//                 componentName: 'terminal',
//             }]
//         }]
//     }]
// };

function createComponent(name)
{
    var component = {
        type: 'component',
        isClosable: false,
        componentName: name,
    };
    return component;
}

function createColumn(components)
{
    var column = {
        type: 'column',
        content: components,
    };
    return column;
}

function createRow(components)
{
    var row = {
        type: 'row',
        content: components,
    };
    return row;
}

const terminal_component = createComponent('terminal');
const visualizer_component = createComponent('visualizer');
const editor_component = createComponent('editor');

const right_column = createColumn([visualizer_component, terminal_component])
const main_row = createRow([editor_component, right_column]);

const layout_config = {
    settings:{
        showPopoutIcon: false,
    },
    content: [main_row]
};

function initLayout(layout_id)
{
    var layout = new GoldenLayout(layout_config, $('#' + layout_id));
    layout.registerComponent('editor', function(container, state) {
        container.getElement().html('<div id="editor" style="height: 100%;"></div>');
    });
    layout.registerComponent('terminal', function(container, state) {
        container.getElement().html('<div id="terminal" style="height: 100%;"></div>');
    });
    layout.registerComponent('visualizer', function(container, state) {
        container.getElement().html('<div id="visualizer" style="height: 100%;"></div>');
    });
    layout.init();

    return layout;
}