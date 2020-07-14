//////////////////////////////////////////////////////////////////////////////
//
//  WebGL_example_24_GPU_per_vertex.js
//
//  Phong Illumination Model on the GPU - Per vertex shading - Several light sources
//
//  Reference: E. Angel examples
//
//  J. Madeira - November 2017 + November 2018
//
//////////////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------------
//
// Global Variables
//

var gl = null; // WebGL context

var shaderProgram = null;

var triangleVertexPositionBuffer = null;

var triangleVertexNormalBuffer = null;

// The GLOBAL transformation parameters

var globalAngleYY = 0.0;

var globalTz = 0.0;

var cylinderDraw = 1;

// GLOBAL Animation controls

var globalRotationYY_ON = 0;

var globalRotationYY_DIR = 1;

var globalRotationYY_SPEED = 1;

// To allow choosing the way of drawing the model triangles

var primitiveType = null;

// To allow choosing the projection type

var projectionType = 0;

var globalTX = 0;
var globalTY = 0;
var moveMode = true;

// NEW --- The viewer position

// It has to be updated according to the projection type

var pos_Viewer = [ 0.0, 0.0, 0.0, 1.0 ];



//CAMERA

var aspect = null;
var zNear = 1;
var zFar = 2000;
var projectionMatrix = null;



//----------------------------------------------------------------------------
//
// The WebGL code
//

//----------------------------------------------------------------------------
//
//  Rendering
//

// Handling the Vertex Coordinates and the Vertex Normal Vectors

function initBuffers( model ) {

	// Vertex Coordinates

	triangleVertexPositionBuffer = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, triangleVertexPositionBuffer);
	gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(model.vertices), gl.STATIC_DRAW);
	triangleVertexPositionBuffer.itemSize = 3;
	triangleVertexPositionBuffer.numItems =  model.vertices.length / 3;

	// Associating to the vertex shader

	gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute,
			triangleVertexPositionBuffer.itemSize,
			gl.FLOAT, false, 0, 0);

	// Vertex Normal Vectors

	triangleVertexNormalBuffer = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, triangleVertexNormalBuffer);
	gl.bufferData(gl.ARRAY_BUFFER, new Float32Array( model.normals), gl.STATIC_DRAW);
	triangleVertexNormalBuffer.itemSize = 3;
	triangleVertexNormalBuffer.numItems = model.normals.length / 3;

	// Associating to the vertex shader

	gl.vertexAttribPointer(shaderProgram.vertexNormalAttribute,
			triangleVertexNormalBuffer.itemSize,
			gl.FLOAT, false, 0, 0);
}

//----------------------------------------------------------------------------

//  Drawing the model

function drawModel( model,
					mvMatrix,
					primitiveType ) {

	// The the global model transformation is an input

	// Concatenate with the particular model transformations

    // Pay attention to transformation order !!

	//mvMatrix = mult( mvMatrix, translationMatrix( model.tx, model.ty, model.tz ) );

	//mvMatrix = mult( mvMatrix, rotationYYMatrix( 90 ) );
		mvMatrix = mult( mvMatrix, translationMatrix( globalTX, globalTY, 0 ) );

    mvMatrix = mult( mvMatrix, rotationXXMatrix( model.originRotXX ) );
    mvMatrix = mult( mvMatrix, rotationYYMatrix( model.originRotYY ) );
    mvMatrix = mult( mvMatrix, rotationZZMatrix( model.originRotZZ ) );


    mvMatrix = mult( mvMatrix, scalingMatrix( model.sxzoom, model.syzoom, model.szzoom ) );

	mvMatrix = mult( mvMatrix, translationMatrix( model.tx, model.ty, model.tz ) );

    mvMatrix = mult( mvMatrix, rotationYYMatrix( model.rotAngleYY ) );
    mvMatrix = mult( mvMatrix, rotationXXMatrix( model.rotAngleXX) );

    mvMatrix = mult( mvMatrix, translationMatrix( model.txMult, model.tyMult, model.tzMult ) );


	mvMatrix = mult( mvMatrix, scalingMatrix( model.sx, model.sy, model.sz ) );

	// Passing the Model View Matrix to apply the current transformation

	var mvUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");

	gl.uniformMatrix4fv(mvUniform, false, new Float32Array(flatten(mvMatrix)));

	// Associating the data to the vertex shader

	// This can be done in a better way !!

	// Vertex Coordinates and Vertex Normal Vectors

	initBuffers(model);

	// Material properties

	gl.uniform3fv( gl.getUniformLocation(shaderProgram, "k_ambient"),
		flatten(model.kAmbi) );

    gl.uniform3fv( gl.getUniformLocation(shaderProgram, "k_diffuse"),
        flatten(model.kDiff) );

    gl.uniform3fv( gl.getUniformLocation(shaderProgram, "k_specular"),
        flatten(model.kSpec) );

	gl.uniform1f( gl.getUniformLocation(shaderProgram, "shininess"),
		model.nPhong );

    // Light Sources

	var numLights = lightSources.length;

	gl.uniform1i( gl.getUniformLocation(shaderProgram, "numLights"),
		numLights );

	//Light Sources

	for(var i = 0; i < lightSources.length; i++ )
	{
		gl.uniform1i( gl.getUniformLocation(shaderProgram, "allLights[" + String(i) + "].isOn"),
			lightSources[i].isOn );

		gl.uniform4fv( gl.getUniformLocation(shaderProgram, "allLights[" + String(i) + "].position"),
			flatten(lightSources[i].getPosition()) );

		gl.uniform3fv( gl.getUniformLocation(shaderProgram, "allLights[" + String(i) + "].intensities"),
			flatten(lightSources[i].getIntensity()) );
    }

	// Drawing

	// primitiveType allows drawing as filled triangles / wireframe / vertices

	if( primitiveType == gl.LINE_LOOP ) {

		// To simulate wireframe drawing!

		// No faces are defined! There are no hidden lines!

		// Taking the vertices 3 by 3 and drawing a LINE_LOOP

		var i;

		for( i = 0; i < triangleVertexPositionBuffer.numItems / 3; i++ ) {

			gl.drawArrays( primitiveType, 3 * i, 3 );
		}
	}
	else {

		gl.drawArrays(primitiveType, 0, triangleVertexPositionBuffer.numItems);

	}
}

//----------------------------------------------------------------------------

//  Drawing the 3D scene

function drawScene() {




	var pMatrix;

	var mvMatrix = mat4();

	// Clearing the frame-buffer and the depth-buffer

	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.clearColor(0.3, 0.3, 0.3, 1.0);

	// Computing the Projection Matrix

	if( projectionType == 0 ) {

		// For now, the default orthogonal view volume

		// pMatrix = ortho( -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 );
		pMatrix = ortho( -10.0, 10.0, -5.71, 5.71, -10.0, 10.0 );

		// Global transformation !!

		globalTz = 0.0;

		// NEW --- The viewer is on the ZZ axis at an indefinite distance

		pos_Viewer[0] = pos_Viewer[1] = pos_Viewer[3] = 0.0;

		pos_Viewer[2] = 1.0;

		// TO BE DONE !

		// Allow the user to control the size of the view volume
	}
	else {

		// A standard view volume.

		// Viewer is at (0,0,0)

		// Ensure that the model is "inside" the view volume

		pMatrix = perspective( 45, 1, 0.05, 15 );

		// Global transformation !!

		globalTz = -2.5;

		// NEW --- The viewer is on (0,0,0)

		pos_Viewer[0] = pos_Viewer[1] = pos_Viewer[2] = 0.0;

		pos_Viewer[3] = 1.0;

		// TO BE DONE !

		// Allow the user to control the size of the view volume
	}

	// Passing the Projection Matrix to apply the current projection

	var pUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");

	gl.uniformMatrix4fv(pUniform, false, new Float32Array(flatten(pMatrix)));

	// NEW --- Passing the viewer position to the vertex shader

	gl.uniform4fv( gl.getUniformLocation(shaderProgram, "viewerPosition"),
        flatten(pos_Viewer) );

	// GLOBAL TRANSFORMATION FOR THE WHOLE SCENE

	mvMatrix = translationMatrix( 0, 0, globalTz );

	// NEW - Updating the position of the light sources, if required

	// FOR EACH LIGHT SOURCE

	for(var i = 0; i < lightSources.length; i++ )
	{
		// Animating the light source, if defined

		var lightSourceMatrix = mat4();

		if( !lightSources[i].isOff() ) {

			// COMPLETE THE CODE FOR THE OTHER ROTATION AXES

			if( lightSources[i].isRotYYOn() )
			{
				lightSourceMatrix = mult(
						lightSourceMatrix,
						rotationYYMatrix( lightSources[i].getRotAngleYY() ) );
			}
		}

		// NEW Passing the Light Souree Matrix to apply

		var lsmUniform = gl.getUniformLocation(shaderProgram, "allLights["+ String(i) + "].lightSourceMatrix");

		gl.uniformMatrix4fv(lsmUniform, false, new Float32Array(flatten(lightSourceMatrix)));
	}

	// Instantianting all scene models

	for(var i = 0; i < sceneModels.length; i++ )
	{
    if(cylinderDraw == 1) {
  		drawModel( sceneModels[i],
  			   mvMatrix,
  	           primitiveType );
    }
    else {
      if (sceneModels[i].cylinder != 1){
        drawModel( sceneModels[i],
    			   mvMatrix,
    	           primitiveType );
      }
    }
	}

	// NEW - Counting the frames

}

//----------------------------------------------------------------------------
//
//  NEW --- Animation
//

// Animation --- Updating transformation parameters

var lastTime = 0;

function animate() {

	var timeNow = new Date().getTime();

	if( lastTime != 0 ) {

		var elapsed = timeNow - lastTime;

		// Global rotation

		if( globalRotationYY_ON ) {

			globalAngleYY += globalRotationYY_DIR * globalRotationYY_SPEED * (90 * elapsed) / 1000.0;
	    }

		// For every model --- Local rotations

		for(var i = 0; i < sceneModels.length; i++ )
	    {
			if( sceneModels[i].rotXXOn ) {

				sceneModels[i].originRotXX += sceneModels[i].rotXXDir * sceneModels[i].rotXXSpeed * (90 * elapsed) / 1000.0;
			}

			if( sceneModels[i].rotYYOn ) {

				sceneModels[i].originRotYY += sceneModels[i].rotYYDir * sceneModels[i].rotYYSpeed * (90 * elapsed) / 1000.0;
			}

			if( sceneModels[i].rotZZOn ) {

				sceneModels[i].originRotZZ += sceneModels[i].rotZZDir * sceneModels[i].rotZZSpeed * (90 * elapsed) / 1000.0;
			}
		}

		// Rotating the light sources

		for(var i = 0; i < lightSources.length; i++ )
	    {
			if( lightSources[i].isRotYYOn() ) {

				var angle = lightSources[i].getRotAngleYY() + lightSources[i].getRotationSpeed() * (90 * elapsed) / 1000.0;

				lightSources[i].setRotAngleYY( angle );
			}
		}
}

	lastTime = timeNow;
}

//Camera Operations
function radToDeg(r) {
    return r * 180 / Math.PI;
  }

  function degToRad(d) {
    return d * Math.PI / 180;
  }

  var cameraAngleRadians = degToRad(0);
  var fieldOfViewRadians = degToRad(60);

//----------------------------------------------------------------------------

// Timer

function tick() {

	requestAnimFrame(tick);

	drawScene();

	animate();
}

//----------------------------------------------------------------------------

// Handling mouse events

// Adapted from www.learningwebgl.com


var mouseDown = false;

var lastMouseX = null;

var lastMouseY = null;

function handleMouseDown(event) {

    mouseDown = true;

    lastMouseX = event.clientX;

    lastMouseY = event.clientY;
}

function handleMouseUp(event) {

    mouseDown = false;
}

function handleScaleUp(event) {
    if(event.deltaY<0){
        for(var i = 0; i < sceneModels.length; i++ )
        {

            sceneModels[i].sxzoom*= 1.1;

            sceneModels[i].syzoom*= 1.1;

            sceneModels[i].szzoom*= 1.1;
        }
    }
    if(event.deltaY>0){
        for(var i = 0; i < sceneModels.length; i++ )
        {

            sceneModels[i].sxzoom*= 0.9;

            sceneModels[i].syzoom*= 0.9;

            sceneModels[i].szzoom*= 0.9;
        }

    }

}


function handleMouseMove(event) {

    if (!mouseDown) {

      return;
    }

    // Rotation angles proportional to cursor displacement


    var newX = event.clientX;

    var newY = event.clientY;

    var deltaX = newX - lastMouseX;

    //model.rotAngleYY += radians( 10 * deltaX  )

    var deltaY = newY - lastMouseY;

    // model.rotAngleXX += radians( 10 * deltaY  )

    lastMouseX = newX;

    lastMouseY = newY;

    for(var i = 0; i < sceneModels.length; i++ )
    {

        // sceneModels[i].originRotYY += radians( 10 * deltaX  );
        sceneModels[i].originRotYY += radians( 10 * deltaX  );

        sceneModels[i].originRotXX += radians( 10 * deltaY  );
    }

  }
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
//
//  User Interaction
//

function outputInfos(){

}

//----------------------------------------------------------------------------

function setEventListeners(canvas){

    // NEW ---Handling the mouse

	// From learningwebgl.com

    canvas.onmousedown = handleMouseDown;

    document.onmouseup = handleMouseUp;

    document.onmousemove = handleMouseMove;

    canvas.onwheel = handleScaleUp;



    // Dropdown list

	var projection = document.getElementById("projection-selection");

	projection.addEventListener("click", function(){

		// Getting the selection

		var p = projection.selectedIndex;

		switch(p){

			case 0 : projectionType = 0;
				break;

			case 1 : projectionType = 1;
				break;
		}
	});

	// Dropdown list


	// Button events


    document.getElementById("showConnections").onclick = function(){

		if (cylinderDraw == 1) {
            cylinderDraw = 0;
        }
        else{
            cylinderDraw = 1;
        }
	};

    document.getElementById("move-left-button").onclick = function()
    {
				if (moveMode) {
					globalTX += -0.2;
				}
				else {
        for(var i = 0; i < sceneModels.length; i++ )
        {
            sceneModels[i].tx += -0.2;
        }
			}
    };
    document.getElementById("move-right-button").onclick = function()
    {
				if (moveMode) {
					globalTX += 0.2;
				}
				else {
        for(var i = 0; i < sceneModels.length; i++ )
        {
            sceneModels[i].tx += 0.2;
        }
			}
    };
    document.getElementById("move-up-button").onclick = function()
    {
				if (moveMode) {
					globalTY += 0.2;
				}
				else {
        for(var i = 0; i < sceneModels.length; i++ )
        {
            sceneModels[i].ty += 0.2;
        }
			}
    };
    document.getElementById("move-down-button").onclick = function()
    {
				if (moveMode) {
					globalTY += -0.2;
				}
				else {
        for(var i = 0; i < sceneModels.length; i++ )
        {
            sceneModels[i].ty += -0.2;
        }
			}
    };

		document.getElementById("move_method").onclick = function()
    {
        moveMode = !moveMode;
				if (moveMode) document.getElementById("method-text").innerHTML = "<b>Current method:</b> Origin moves";
				else document.getElementById("method-text").innerHTML = "<b>Current method:</b> Origin doesn't move"
    };

	document.getElementById("reset-button").onclick = function()
	{

		sceneModels=[];
		molecules=[];


	}



    document.addEventListener("keypress", function(event){
		var key = event.keyCode; // ASCII
        var _tx,_ty=0;
		switch(key){
			case 97 :
				_tx=-0.2;
				_ty=0.0;
				break;
			case 119:
				_tx=0.0;
				_ty=0.2;
				break;
			case 100:
                _tx=0.2;
				_ty=0.0;
				break;
			case 115:
				_ty=-0.2;
				_tx=0.0;
				break;

		}
		if (moveMode) {
      globalTX += _tx;
			globalTY += _ty;
		}
		else {
			for (var i = 0; i < sceneModels.length; i++){
				sceneModels[i].tx += _tx;
				sceneModels[i].ty += _ty;
			}
		}
	});
}

//----------------------------------------------------------------------------
//
// WebGL Initialization
//

function initWebGL( canvas ) {
	try {

		// Create the WebGL context

		// Some browsers still need "experimental-webgl"

		gl = canvas.getContext("webgl") || canvas.getContext("experimental-webgl");

		// DEFAULT: The viewport occupies the whole canvas

		// DEFAULT: The viewport background color is WHITE

		// NEW - Drawing the triangles defining the model

		primitiveType = gl.TRIANGLES;

		// DEFAULT: Face culling is DISABLED

		// Enable FACE CULLING

		gl.enable( gl.CULL_FACE );

		// DEFAULT: The BACK FACE is culled!!

		// The next instruction is not needed...

		gl.cullFace( gl.BACK );

		// Enable DEPTH-TEST

		gl.enable( gl.DEPTH_TEST );





	} catch (e) {
	}
	if (!gl) {
		alert("Could not initialise WebGL, sorry! :-(");
	}
}

//----------------------------------------------------------------------------

function runWebGL() {

	var canvas = document.getElementById("my-canvas");

	initWebGL( canvas );

	shaderProgram = initShaders( gl );

	setEventListeners(canvas);

	tick();		// A timer controls the rendering / animation

	outputInfos();
}
