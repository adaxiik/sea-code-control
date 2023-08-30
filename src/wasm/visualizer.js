
class Visualizer {
    constructor(visualizer_id) {
        this.stage = new Konva.Stage({
            container: visualizer_id,
            width: 1,
            height: 1,
            draggable: true
        });
        this.layer = new Konva.Layer();
        this.stage.add(this.layer);
        this.layer.draw();

        var circle = new Konva.Circle({
            x: 100,
            y: 100,
            radius: 70,
            fill: 'red',
            stroke: 'black',
            strokeWidth: 4
        });

        this.layer.add(circle);
        
    }

    fit() {
        this.stage.width(window.innerWidth);
        this.stage.height(window.innerHeight);
        this.draw();
    }

    draw() {
        this.layer.draw();
    }
}