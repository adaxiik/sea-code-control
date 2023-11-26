

function init_visualizer(visualizer_id)
{
    let visualizer_div = document.getElementById(visualizer_id);
    let canvas = document.createElement('canvas');
    canvas.id = 'visualizer-canvas';
    visualizer_div.appendChild(canvas);

    let visualizer = new cvisualizer.FabricDrawingModule('visualizer-canvas')

    //Example stackframe 2
    let stackFrame2 = new  cvisualizer.DataModelStructures.StackFrame();
    stackFrame2.frameId = 2;
    stackFrame2.functionName = "Function2";
    stackFrame2.isCollapsed = false;
    //Function variables 2
    let stackFrame2Var1 = new  cvisualizer.DataModelStructures.Variable();
    stackFrame2Var1.variableName = "stackFrame2Var1";
    stackFrame2Var1.dataTypeString = "bool";
    stackFrame2Var1.valueString = "true";
    let stackFrame2Var2 = new  cvisualizer.DataModelStructures.Variable();
    stackFrame2Var2.variableName = "stackFrame2Var2";
    stackFrame2Var2.dataTypeString = "int";
    stackFrame2Var2.valueString = "17";
    let stackFrame2Var3 = new  cvisualizer.DataModelStructures.Variable();
    stackFrame2Var3.variableName = "stackFrame2Var3";
    stackFrame2Var3.dataTypeString = "float";
    stackFrame2Var3.valueString = "3.28";
    let stackFrame2Var4 = new  cvisualizer.DataModelStructures.Array();                    //Array variable
    stackFrame2Var4.variableName = "stackFrame2Array1";
    stackFrame2Var4.dataTypeString = "int";
    stackFrame2Var4.isCollapsed = true;
    stackFrame2Var4.size = 6;
    let stackFrame2Var4atIdx0 = new  cvisualizer.DataModelStructures.Variable();           //Array element
    stackFrame2Var4atIdx0.valueString = "17";
    let stackFrame2Var4atIdx1 = new  cvisualizer.DataModelStructures.Variable();           //Array element
    stackFrame2Var4atIdx1.valueString = "18";
    let stackFrame2Var4atIdx2 = new  cvisualizer.DataModelStructures.Variable();           //Array element
    stackFrame2Var4atIdx2.valueString = "19";
    let stackFrame2Var4atIdx4 = new  cvisualizer.DataModelStructures.Variable();           //Array element
    stackFrame2Var4atIdx4.valueString = "21";
    stackFrame2Var4atIdx4.valueChanged = true;
    stackFrame2Var4.elements[0] = stackFrame2Var4atIdx0;
    stackFrame2Var4.elements[1] = stackFrame2Var4atIdx1;
    stackFrame2Var4.elements[2] = stackFrame2Var4atIdx2;
    stackFrame2Var4.elements[4] = stackFrame2Var4atIdx4;

    let stackFrame2PointerToHeap = new  cvisualizer.DataModelStructures.Variable();          //Pointer variable (pointing to heap)
    stackFrame2PointerToHeap.variableName = "stackFrame2PointerToHeap";
    stackFrame2PointerToHeap.dataTypeString = "int *";
    stackFrame2PointerToHeap.valueString = "heapVar1";
    stackFrame2PointerToHeap.isPointer = true;

    stackFrame2.functionVariables[stackFrame2Var1.variableName] = stackFrame2Var1;
    stackFrame2.functionVariables[stackFrame2Var2.variableName] = stackFrame2Var2;
    stackFrame2.functionVariables[stackFrame2Var3.variableName] = stackFrame2Var3;
    stackFrame2.functionVariables[stackFrame2Var4.variableName] = stackFrame2Var4;
    stackFrame2.functionVariables[stackFrame2PointerToHeap.variableName] = stackFrame2PointerToHeap;
    //Function parameters 2
    let stackFrame2Param1 = new  cvisualizer.DataModelStructures.Variable();
    stackFrame2Param1.variableName = "stackFrame2Param1";
    stackFrame2Param1.dataTypeString = "int";
    stackFrame2Param1.valueString = "273";
    let stackFrame2Param2 = new  cvisualizer.DataModelStructures.Variable();
    stackFrame2Param2.variableName = "stackFrame2Param2";
    stackFrame2Param2.dataTypeString = "double";
    stackFrame2Param2.valueString = "15.893";
    stackFrame2.functionParameters[stackFrame2Param1.variableName] = stackFrame2Param1;
    stackFrame2.functionParameters[stackFrame2Param2.variableName] = stackFrame2Param2;

    //Preparing the heap variables
    let heapValueVar1 = new  cvisualizer.DataModelStructures.Variable();
    heapValueVar1.variableName = "heapVar1";
    heapValueVar1.dataTypeString = "int";
    heapValueVar1.valueString = "113";
    let heapVar1 = new  cvisualizer.DataModelStructures.HeapVariable();
    heapVar1.variable = heapValueVar1;
    let heapValueVar2 = new  cvisualizer.DataModelStructures.Variable();
    heapValueVar2.variableName = "heapVar2";
    heapValueVar2.dataTypeString = "double";
    heapValueVar2.valueString = "16.44";
    let heapVar2 = new  cvisualizer.DataModelStructures.HeapVariable();
    heapVar2.variable = heapValueVar2;
    //Preparing the heap
    let heap = new  cvisualizer.DataModelStructures.Heap();
    heap.heapVariables[heapVar1.variable.variableName] = heapVar1;
    heap.heapVariables[heapVar2.variable.variableName] = heapVar2;

    //Creating the program stack
    let programStack = new  cvisualizer.DataModelStructures.ProgramStack();
    // programStack.stackFrames = new Array<DataModelStructures.StackFrame>();
    programStack.stackFrames = {};

    //Adding the stackframes
    programStack.stackFrames[stackFrame2.frameId] = stackFrame2;
    //Adding the heap
    programStack.heap = heap;


    visualizer.drawProgramStack(programStack);

    return visualizer;
}