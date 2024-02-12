
function initVisualizer(visualizer_id)
{
    let visualizer_div = document.getElementById(visualizer_id);
    visualizer_div.innerHTML = "";
    let canvas = document.createElement('canvas');
    canvas.id = 'visualizer-canvas';
    visualizer_div.appendChild(canvas);

    let visualizer = new cvisualizer.FabricDrawingModule('visualizer-canvas')

    // //Example stackframe 2
    // let stackFrame2 = new  cvisualizer.DataModelStructures.StackFrame();
    // stackFrame2.frameId = 2;
    // stackFrame2.functionName = "Function2";
    // stackFrame2.isCollapsed = false;
    // //Function variables 2
    // let stackFrame2Var1 = new  cvisualizer.DataModelStructures.Variable();
    // stackFrame2Var1.variableName = "stackFrame2Var1";
    // stackFrame2Var1.dataTypeString = "bool";
    // stackFrame2Var1.valueString = "true";
    // let stackFrame2Var2 = new  cvisualizer.DataModelStructures.Variable();
    // stackFrame2Var2.variableName = "stackFrame2Var2";
    // stackFrame2Var2.dataTypeString = "int";
    // stackFrame2Var2.valueString = "17";
    // let stackFrame2Var3 = new  cvisualizer.DataModelStructures.Variable();
    // stackFrame2Var3.variableName = "stackFrame2Var3";
    // stackFrame2Var3.dataTypeString = "float";
    // stackFrame2Var3.valueString = "3.28";
    // let stackFrame2Var4 = new  cvisualizer.DataModelStructures.Array();                    //Array variable
    // stackFrame2Var4.variableName = "stackFrame2Array1";
    // stackFrame2Var4.dataTypeString = "int";
    // stackFrame2Var4.isCollapsed = true;
    // stackFrame2Var4.size = 6;
    // let stackFrame2Var4atIdx0 = new  cvisualizer.DataModelStructures.Variable();           //Array element
    // stackFrame2Var4atIdx0.valueString = "17";
    // let stackFrame2Var4atIdx1 = new  cvisualizer.DataModelStructures.Variable();           //Array element
    // stackFrame2Var4atIdx1.valueString = "18";
    // let stackFrame2Var4atIdx2 = new  cvisualizer.DataModelStructures.Variable();           //Array element
    // stackFrame2Var4atIdx2.valueString = "19";
    // let stackFrame2Var4atIdx4 = new  cvisualizer.DataModelStructures.Variable();           //Array element
    // stackFrame2Var4atIdx4.valueString = "21";
    // stackFrame2Var4atIdx4.valueChanged = true;
    // stackFrame2Var4.elements[0] = stackFrame2Var4atIdx0;
    // stackFrame2Var4.elements[1] = stackFrame2Var4atIdx1;
    // stackFrame2Var4.elements[2] = stackFrame2Var4atIdx2;
    // stackFrame2Var4.elements[4] = stackFrame2Var4atIdx4;

    // let stackFrame2PointerToHeap = new  cvisualizer.DataModelStructures.Variable();          //Pointer variable (pointing to heap)
    // stackFrame2PointerToHeap.variableName = "stackFrame2PointerToHeap";
    // stackFrame2PointerToHeap.dataTypeString = "int *";
    // stackFrame2PointerToHeap.valueString = "heapVar1";
    // stackFrame2PointerToHeap.isPointer = true;

    // stackFrame2.functionVariables[stackFrame2Var1.variableName] = stackFrame2Var1;
    // stackFrame2.functionVariables[stackFrame2Var2.variableName] = stackFrame2Var2;
    // stackFrame2.functionVariables[stackFrame2Var3.variableName] = stackFrame2Var3;
    // stackFrame2.functionVariables[stackFrame2Var4.variableName] = stackFrame2Var4;
    // stackFrame2.functionVariables[stackFrame2PointerToHeap.variableName] = stackFrame2PointerToHeap;
    // //Function parameters 2
    // let stackFrame2Param1 = new  cvisualizer.DataModelStructures.Variable();
    // stackFrame2Param1.variableName = "stackFrame2Param1";
    // stackFrame2Param1.dataTypeString = "int";
    // stackFrame2Param1.valueString = "273";
    // let stackFrame2Param2 = new  cvisualizer.DataModelStructures.Variable();
    // stackFrame2Param2.variableName = "stackFrame2Param2";
    // stackFrame2Param2.dataTypeString = "double";
    // stackFrame2Param2.valueString = "15.893";
    // stackFrame2.functionParameters[stackFrame2Param1.variableName] = stackFrame2Param1;
    // stackFrame2.functionParameters[stackFrame2Param2.variableName] = stackFrame2Param2;

    // //Preparing the heap variables
    // let heapValueVar1 = new  cvisualizer.DataModelStructures.Variable();
    // heapValueVar1.variableName = "heapVar1";
    // heapValueVar1.dataTypeString = "int";
    // heapValueVar1.valueString = "113";
    // let heapVar1 = new  cvisualizer.DataModelStructures.HeapVariable();
    // heapVar1.variable = heapValueVar1;
    // let heapValueVar2 = new  cvisualizer.DataModelStructures.Variable();
    // heapValueVar2.variableName = "heapVar2";
    // heapValueVar2.dataTypeString = "double";
    // heapValueVar2.valueString = "16.44";
    // let heapVar2 = new  cvisualizer.DataModelStructures.HeapVariable();
    // heapVar2.variable = heapValueVar2;
    // //Preparing the heap
    // let heap = new  cvisualizer.DataModelStructures.Heap();
    // heap.heapVariables[heapVar1.variable.variableName] = heapVar1;
    // heap.heapVariables[heapVar2.variable.variableName] = heapVar2;

    // //Creating the program stack
    // let programStack = new  cvisualizer.DataModelStructures.ProgramStack();
    // // programStack.stackFrames = new Array<DataModelStructures.StackFrame>();
    // programStack.stackFrames = {};

    // //Adding the stackframes
    // programStack.stackFrames[stackFrame2.frameId] = stackFrame2;
    // //Adding the heap
    // programStack.heap = heap;


    // visualizer.drawProgramStack(programStack);
    console.log("Visualizer initialized");

    return visualizer;
}

// I HATE JS AAA
DataView.prototype.getUint64 = function(byteOffset, littleEndian) {
    // split 64-bit number into two 32-bit parts
    const left =  this.getUint32(byteOffset, littleEndian);
    const right = this.getUint32(byteOffset+4, littleEndian);

    // combine the two 32-bit values
    const combined = littleEndian? left + 2**32*right : 2**32*left + right;

    if (!Number.isSafeInteger(combined))
        console.warn(combined, 'exceeds MAX_SAFE_INTEGER. Precision may be lost');

    return combined;
}

// https://stackoverflow.com/questions/53103695/how-to-read-64-bit-integer-from-an-arraybuffer-dataview-in-javascript
DataView.prototype.getInt64 = function(byteOffset, littleEndian) {
    // delegate to getUint64
    const value = this.getUint64(byteOffset, littleEndian);

    // convert to signed and return
    return littleEndian?
        value > 2**63 - 1 ? value - 2**64 : value
      : value > 2**63 - 1 ? 2**64 - value : value;
}

function drawExportedSnapshot(visualizer, exportedProgramJson)
{
    let exportedProgram = JSON.parse(exportedProgramJson);
    console.log(exportedProgram);

    let programStack = new cvisualizer.DataModelStructures.ProgramStack();

    let isPointer = function(typeIndex) {
        let type = exportedProgram.types[typeIndex];
        if (!type) {
            console.log("ERROR: typeIndex " + typeIndex + " not found");
            return false;
        }
        return type.kind == "pointer";
    }

    let isArray = function(typeIndex) {
        let type = exportedProgram.types[typeIndex];
        if (!type) {
            console.log("ERROR: typeIndex " + typeIndex + " not found");
            return false;
        }
        return type.kind == "array";
    }

    let isStruct = function(typeIndex) {
        let type = exportedProgram.types[typeIndex];
        if (!type) {
            console.log("ERROR: typeIndex " + typeIndex + " not found");
            return false;
        }
        return type.kind == "struct";
    }



    let getTypeName = function(typeIndex) {
        let type = exportedProgram.types[typeIndex];
        if (!type) {
            return "ERROR";
        }

        if (type.kind == "primitive") {
            return type.name;
        } else if (type.kind == "pointer") {
            return getTypeName(type.pointing_to_type_index) + "*";
        }
        else if (type.kind == "array") {
            return getTypeName(type.element_type_index) + "[" + type.size_bytes / exportedProgram.types[type.element_type_index].size_bytes + "]";
        }
        else if (type.kind == "struct") {
            return type.name;
        }

        return "ERROR";
    }

    let variableAddressToName = {};

    let getDataValueFromView = function(view, typeName) {
        if (typeName == "bool") {
            return (view.getUint8(0) == 1) ? "true" : "false";
        } else if (typeName == "char") {
            return String.fromCharCode(view.getUint8(0));
        } else if (typeName == "signed char") {
            return view.getInt8(0);
        } else if (typeName == "short") {
            return view.getInt16(0, true);
        } else if (typeName == "int") {
            return view.getInt32(0, true);
        } else if (typeName == "long" || typeName == "long long") {
            return view.getInt64(0, true);
        } else if (typeName == "unsigned char") {
            return view.getUint8(0);
        } else if (typeName == "unsigned short") {
            return view.getUint16(0, true);
        } else if (typeName == "unsigned int") {
            return view.getUint32(0, true);
        } else if (typeName == "unsigned long" || typeName == "unsigned long long") {
            return view.getUint64(0, true);
        } else if (typeName == "float") {
            return view.getFloat32(0, true);
        } else if (typeName == "double") {
            return view.getFloat64(0, true);
        } else if (typeName == "void") {
            return "You shouldn't be seeing this";
        }

        return "ERROR";


    }


    let variableValueAsString = function(variable) {
        if (!variable.is_initialized) {
            return "uninitialized";
        }

        let bytes = new Uint8Array(variable.allocated_place.data);
        let view = new DataView(bytes.buffer);

        if (isPointer(variable.type_index)) {
            let pointingToAddress = view.getUint64(0, true);
            if (variableAddressToName[pointingToAddress]) {
                return variableAddressToName[pointingToAddress];
            } else {
                return "0x" + pointingToAddress.toString(16);
            }
        }

        let typeName = getTypeName(variable.type_index);
        return getDataValueFromView(view, typeName);
    }

    let createVariableData = function(variable) {
        let address = variable.allocated_place.address;
        variableAddressToName[address] = variable.name;

        let variableData = new cvisualizer.DataModelStructures.Variable();
        variableData.variableName = variable.name;
        variableData.dataTypeString = getTypeName(variable.type_index);
        variableData.isPointer = isPointer(variable.type_index);
        variableData.valueString = variableValueAsString(variable).toString();
        // stackFrame.functionVariables[variable.name] = variableData;
        return variableData;
    }

    let createArrayData = function(variable) {
        let address = variable.allocated_place.address;
        variableAddressToName[address] = variable.name;

        let arrayData = new cvisualizer.DataModelStructures.Array();
        arrayData.variableName = variable.name;
        arrayData.isCollapsed = true;
        let myType = exportedProgram.types[variable.type_index];
        let underlyingType = exportedProgram.types[myType.element_type_index];
        arrayData.size = myType.element_count;
        arrayData.dataTypeString = getTypeName(myType.element_type_index);

        let bytes = new Uint8Array(variable.allocated_place.data);

        for (let i = 0; i < arrayData.size; i++) {
            let dataSlice = bytes.slice(i * underlyingType.size_bytes, (i + 1) * underlyingType.size_bytes);
            let viewSlice = new DataView(dataSlice.buffer);
            let element = new cvisualizer.DataModelStructures.Variable();
            element.valueString = getDataValueFromView(viewSlice, getTypeName(myType.element_type_index)).toString();
            arrayData.elements[i] = element;
        }

        // stackFrame.functionVariables[variable.name] = arrayData;
        return arrayData;
    }

    let createStructData = function(variable) {
        // let stackFrame1StructMemberStruct1 = new DataModelStructures.Struct();    //Member struct variable
        // stackFrame1StructMemberStruct1.variableName = "stackFrame1StructMemberStruct1";
        // stackFrame1StructMemberStruct1.elements = new Array<DataModelStructures.Variable>();
        // stackFrame1StructMemberStruct1.dataTypeString = "struct2";
        // stackFrame1StructMemberStruct1.isCollapsed = true;
        // let stackFrame1StructMember4 = new DataModelStructures.Variable();
        // stackFrame1StructMember4.variableName = "stackFrame1StructMember4";
        // stackFrame1StructMember4.dataTypeString = "int";
        // stackFrame1StructMember4.valueString = "73";

        let address = variable.allocated_place.address;
        variableAddressToName[address] = variable.name;

        let structData = new cvisualizer.DataModelStructures.Struct();
        structData.variableName = variable.name;
        structData.isCollapsed = true;
        structData.dataTypeString = getTypeName(variable.type_index);
        structData.elements = [];
        // {
        //     "fields": [
        //         {
        //             "name": "x",
        //             "offset_bytes": 0,
        //             "type_index": 0
        //         }
        //     ],
        //     "kind": "struct",
        //     "name": "<anonymous>",
        //     "size_bytes": 4
        // }

        let structType = exportedProgram.types[variable.type_index];
        let fields = structType.fields;
        for (let field of fields) {
            let fieldAddress = variable.allocated_place.address + field.offset_bytes;
            let dataSlice = variable.allocated_place.data.slice(field.offset_bytes, field.offset_bytes + exportedProgram.types[field.type_index].size_bytes);

            let fieldVariable = {
                name: field.name,
                type_index: field.type_index,
                is_initialized: true,
                allocated_place: {
                    address: fieldAddress,
                    data: dataSlice
                }
            };
            let d = createData(fieldVariable);
            structData.elements.push(d);
        }

        return structData;
    }

    let createData = function(variable) {
        if (isArray(variable.type_index)) {
            return createArrayData(variable);
        }
        else if (isStruct(variable.type_index)) {
            return createStructData(variable);
        }
         else {
            return createVariableData(variable);
        }
        throw "ERROR";
    }


    let globalVariables = new cvisualizer.DataModelStructures.StackFrame();
    globalVariables.frameId = 0;
    globalVariables.functionName = "Global variables";
    globalVariables.isCollapsed = false;
    for (let global of exportedProgram.global_variables) {
        let d = createData(globalVariables, global);
        stackFrame.functionVariables[global.name] = d;
    }
    programStack.stackFrames[globalVariables.frameId] = globalVariables;


    for (let i = 0; i < exportedProgram.stackframes.length; i++) {
        let exportedStackframe = exportedProgram.stackframes[i];
        let stackFrame = new cvisualizer.DataModelStructures.StackFrame();
        stackFrame.frameId = i + 1; // +1 for global variables
        stackFrame.functionName = exportedStackframe.function_name;
        stackFrame.isCollapsed = false;

        for (let param of exportedStackframe.parameters) {
            // let address = param.allocated_place.address;
            // variableAddressToName[address] = param.name;

            // let variable = new cvisualizer.DataModelStructures.Variable();
            // variable.variableName = param.name;
            // variable.dataTypeString = getTypeName(param.type_index);
            // variable.valueString = variableValueAsString(param);
            // variable.isPointer = isPointer(param.type_index);
            let d = createData(param);
            // stackFrame.functionParameters[param.name] = variable;
            stackFrame.functionParameters[param.name] = d;
        }

        for (let local of exportedStackframe.variables) {
            let d = createData(local);
            stackFrame.functionVariables[local.name] = d;
        }

        programStack.stackFrames[stackFrame.frameId] = stackFrame;
    }

    programStack.heap = new cvisualizer.DataModelStructures.Heap();
    console.log(variableAddressToName);
    visualizer.drawProgramStack(programStack);
}