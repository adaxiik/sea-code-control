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

function create_component(name)
{
    var component = {
        type: 'component',
        isClosable: false,
        componentName: name,
    };
    return component;
}

function create_column(components)
{
    var column = {
        type: 'column',
        content: components,
    };
    return column;
}

function create_row(components)
{
    var row = {
        type: 'row',
        content: components,
    };
    return row;
}

const terminal_component = create_component('terminal');
const visualizer_component = create_component('visualizer');
const editor_component = create_component('editor');

const right_column = create_column([visualizer_component, terminal_component])
const main_row = create_row([editor_component, right_column]);

const layout_config = {
    settings:{
        showPopoutIcon: false,
    },
    content: [main_row]
};

function init_layout(layout_id)
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