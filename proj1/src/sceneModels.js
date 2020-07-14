//////////////////////////////////////////////////////////////////////////////
//
//  For instantiating the scene models.
//
//  J. Madeira - November 2018
//
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//
//  Constructors
//
atomColorList = {
	"H":[[1,1,1],[1,1,1],[1,1,1]],
	"C":[[0.2,0.2,0.2],[0.5,0.5,0.5],[0.7,0.7,0.7]],
	"O":[[0.5,0.0,0.0],[1.0,0.0,0.0],[1.0,0.5,0.5]],
	"N":[[0.0,0.0,0.5],[0.0,0.0,1.0],[0.5,0.5,1.0]],
	"P":[[1.0,0.7,0.0],[1.0,0.7,0.0],[1.0,1.0,0.5]],
	"S":[[1.0,1.0,0.0],[1.0,1.0,0.0],[1.0,1.0,0.5]],
	"F":[[0.4,1.0,0.0],[0.4,1.0,0.0],[0.5,1.0,0.5]],
	"Cl":[[0.0,1.0,0.0],[0.0,1.0,0.0],[0.5,1.0,0.5]],
	"Br":[[0.5,0.0,0.0],[0.7,0.0,0.0],[0.2,0.2,0.2]],
	"I":[[0.5,0.0,1.0],[1.0,0.0,1.0],[1.0,0.5,1.0]],
	"Na":[[0.5,0.0,1.0],[1.0,0.0,1.0],[1.0,0.3,1.0]],
}

atomSizeList = {
	"H": 0.25,
	"C": 0.3,
	"O": 0.3,
	"N": 0.3,
	"P": 0.35,
	"S": 0.32,
	"F": 0.28,
	"Cl": 0.34,
	"Br": 0.4,
	"I": 0.45,
	"Na": 0.45,
}

var sceneModels = [];
var molecules = [];

function emptyModelFeatures() {

	// EMPTY MODEL

	this.vertices = [];

	this.normals = [];

	// Transformation parameters

	// Displacement vector

	this.tx = 0.0;

	this.ty = 0.0;

	this.tz = 0.0;


	// this.tx_transaction = 0.0;
	//
	// this.ty_transaction = 0.0;
	//
	// this.tz_transaction = 0.0;



	this.txMult = 0.0;

	this.tyMult = 0.0;

	this.tzMult = 0.0;


	this.originRotXX =0.0;

	this.originRotYY =0.0;

	this.originRotZZ =0.0;

	// Rotation angles

	this.rotAngleXX = 0.0;

	this.rotAngleYY = 0.0;

	this.rotAngleZZ = 0.0;


	//MouseRotation

	this.rotMouseXX =0.0;

	this.rotMouseYY =0.0;

	this.rotMouseZZ =0.0;

	// Scaling factors

	this.sx = 1.0;

	this.sy = 1.0;

	this.sz = 1.0;

	//Scaling factors for zoom
	this.sxzoom = 1.0;

	this.syzoom = 1.0;

	this.szzoom = 1.0;


	// Animation controls

	this.rotXXOn =false;

	this.rotYYOn =false;

	this.rotZZOn = false;

	this.rotXXSpeed = 1.0;

	this.rotYYSpeed = 1.0;

	this.rotZZSpeed = 1.0;

	this.rotXXDir = 1;

	this.rotYYDir = 1;

	this.rotZZDir = 1;

	// Material features


	this.kAmbi = [ 0.2,0.2,0.2 ];

	this.kDiff = [ 0.5,0.5,0.5 ];

	this.kSpec = [ 0.7,0.7,0.7 ];

	this.nPhong = 100;

	this.cilinder = 0;
}


function simpleCubeModel( ) {

	var cube = new emptyModelFeatures();

	cube.vertices = [

		-1.000000, -1.000000,  1.000000,
		 1.000000,  1.000000,  1.000000,
		-1.000000,  1.000000,  1.000000,
		-1.000000, -1.000000,  1.000000,
		 1.000000, -1.000000,  1.000000,
		 1.000000,  1.000000,  1.000000,
         1.000000, -1.000000,  1.000000,
		 1.000000, -1.000000, -1.000000,
		 1.000000,  1.000000, -1.000000,
         1.000000, -1.000000,  1.000000,
         1.000000,  1.000000, -1.000000,
         1.000000,  1.000000,  1.000000,
        -1.000000, -1.000000, -1.000000,
        -1.000000,  1.000000, -1.000000,
         1.000000,  1.000000, -1.000000,
        -1.000000, -1.000000, -1.000000,
         1.000000,  1.000000, -1.000000,
         1.000000, -1.000000, -1.000000,
        -1.000000, -1.000000, -1.000000,
		-1.000000, -1.000000,  1.000000,
		-1.000000,  1.000000, -1.000000,
		-1.000000, -1.000000,  1.000000,
		-1.000000,  1.000000,  1.000000,
		-1.000000,  1.000000, -1.000000,
		-1.000000,  1.000000, -1.000000,
		-1.000000,  1.000000,  1.000000,
		 1.000000,  1.000000, -1.000000,
		-1.000000,  1.000000,  1.000000,
		 1.000000,  1.000000,  1.000000,
		 1.000000,  1.000000, -1.000000,
		-1.000000, -1.000000,  1.000000,
		-1.000000, -1.000000, -1.000000,
		 1.000000, -1.000000, -1.000000,
		-1.000000, -1.000000,  1.000000,
		 1.000000, -1.000000, -1.000000,
		 1.000000, -1.000000,  1.000000,
	];

	computeVertexNormals( cube.vertices, cube.normals );

	return cube;
}


function cubeModel( subdivisionDepth = 0 ) {

	var cube = new simpleCubeModel();

	midPointRefinement( cube.vertices, subdivisionDepth );

	computeVertexNormals( cube.vertices, cube.normals );

	return cube;
}


function sphereModel( subdivisionDepth = 2 ) {

	var sphere = new simpleCubeModel();

	midPointRefinement( sphere.vertices, subdivisionDepth );

	moveToSphericalSurface( sphere.vertices )

	computeVertexNormals( sphere.vertices, sphere.normals );

	return sphere;
}

function cylinderModel( subdivisionDepth = 2 ) {

	var cylinder = new simpleCubeModel();

	midPointRefinement( cylinder.vertices, subdivisionDepth );

	turnIntoCylinder( cylinder.vertices )

	computeVertexNormals( cylinder.vertices, cylinder.normals );

	cylinder.cylinder = 1;

	return cylinder;
}

function getSphere(radius = 0.2){
	var sphere = sphereModel(4);
	sphere.sx = radius; sphere.sy = radius; sphere.sz = radius;
	return sphere;
}

function getCylinder(radius = 0.2, length = 0.5){
	var cylinder = cylinderModel(4);
	cylinder.sx = radius; cylinder.sy = radius; cylinder.sz =length;
	return cylinder;
}

function getCylinderMult(numberOfCylinders = 2, radius = 0.05, length = 0.5, distance = 0.05){
	var cylinders = [];
	if (distance <= radius)
		distance = radius
	for (var i = 0; i < numberOfCylinders; i++){
		cylinders[i] = getCylinder(radius/numberOfCylinders,length);
		if (numberOfCylinders % 2 == 0) {
			if (i % 2 == 0)
				cylinders[i].txMult = distance * ((i / 2) + 1);
			else
				cylinders[i].txMult = (-1) * distance * ((i / 2) + 1);
			}
		else{
			if (i % 2 == 0){
				cylinders[i].txMult = distance * (i / 2);
			}
			else{
				cylinders[i].txMult = (-1) * distance * (parseInt(i / 2) + 1);
			}
		}
	}
	return cylinders;
}

function loadFile(f){
	var file = f.files[0];

	var reader = new FileReader();
	var molecule = new Molecule(1);
	sceneModels = [];


	reader.onload = function( progressEvent ){
		var s = "";
		//var tokens = this.result.split("/\s\s*/");
		var tokens = this.result.split("/");
		var atomTokens = tokens[0].split(/\s\s*/);
		var connectionTokens = tokens[1].replace( /\n/g, " " ).split(/\s\s*/);
		for(var i = 0; i < atomTokens.length - 1; i += 4){
			molecule.setAtom(atomTokens[i],vec4(parseFloat(atomTokens[i+1]),parseFloat(atomTokens[i+2]),parseFloat(atomTokens[i+3]),0.0));

		}
		for(var i = 1; i < connectionTokens.length - 2; i += 3){
			molecule.setConnection(parseInt(connectionTokens[i]),parseInt(connectionTokens[i+1]),parseInt(connectionTokens[i+2]));

		}
		molecules.push(molecule);
		moleculesToModels();
	}
	reader.readAsText(file);

}

function readMolecule(thismolecule)
{
	// var reader = new FileReader();
	var molecule = new Molecule(1);
	sceneModels = [];

	var my_molecule;
	var connections;
	ethane=[
		'C', 0.0, 0.0, 0.0,
		'H', -1.0, -0.2, -0.2,
		'H', 0.0, -0.2, 1.0,
		'H', 1.0, -0.2, -0.2,
		'H', 0.0, 1.0, 0.0
	];
	ethane_connections=[
		0, 1, 1,
		0, 2, 1,
		0, 3, 1,
		0, 4, 1
	];

	buthane=[
		'C',0.5,-0.5,0.0,
		'C',-1.5,-0.5,0.0,
		'C',1.5,0.5,0.0,
		'C',-0.5,0.5,0.0,
		'H',-1.5,-1,0.5,
		'H',-1.5,-1,-0.5,
		'H',-2,0.0,0.0,
		'H',-0.5,1,0.5,
		'H',-0.5,1,-0.5,
		'H',0.5,-1,0.5,
		'H',0.5,-1,-0.5,
		'H',1.5,1,0.5,
		'H',1.5,1,-0.5,
		'H',2,0.0,0.0,
	];

	buthane_connections=[
		0,2,1,
		0,3,1,
		3,1,1,
		1,4,1,
		1,5,1,
		1,6,1,
		3,7,1,
		3,8,1,
		0,9,1,
		0,10,1,
		2,11,1,
		2,12,1,
		2,13,1
	];

	methane=[
		'C',0.0,0.0,0.0,
		'H',-1.0,-0.2,-0.2,
		'H',0.0,-0.2,1.0,
		'H',1.0,-0.2,-0.2,
		'H',0.0,1.0,0.0
	];
	methane_connections=[
		0,1,1,
		0,2,1,
		0,3,1,
		0,4,1
	];

	propane=[
		'C',0.0,0.5,0.0,
		'C',-1,-0.5,0.0,
		'C',1,-0.5,0.0,
		'H',0.0,1,0.5,
		'H',0.0,1,-0.5,
		'H',-1,-1,0.5,
		'H',-1,-1,-0.5,
		'H',-1.5,0.0,0.0,
		'H',1,-1,0.5,
		'H',1,-1,-0.5,
		'H',1.5,0.0,0.0
	];

	propane_connections=[
		0,3,1,
		0,4,1,
		1,5,1,
		1,6,1,
		1,7,1,
		2,8,1,
		2,9,1,
		2,10,1,
		0,1,1,
		0,2,1

	];
	switch (thismolecule) {
		case 'buthane':
			my_molecule=buthane;
			connections=buthane_connections;
			break;
		case 'ethane':
			my_molecule=ethane;
			connections=ethane_connections;
			break;
		case 'methane':
			my_molecule=methane;
			connections=methane_connections;
			break;
		case 'propane':
			my_molecule=propane;
			connections=propane_connections;
			break;
		default:
			break;
	}

	for(var i = 0; i < my_molecule.length - 1; i += 4){
		molecule.setAtom(my_molecule[i],vec4(my_molecule[i+1],my_molecule[i+2],my_molecule[i+3],0.0));

	}
	for(var i = 0; i < connections.length - 2; i += 3){
		molecule.setConnection(connections[i],connections[i+1],connections[i+2]);

	}
	molecules.push(molecule);
	moleculesToModels();
}

function moleculesToModels(n = null){
	if (n == null || n > molecules.length) n = molecules.length;
	var distance = 4;
	for (var i = 0; i < n; i++){
		var auxtx=0;
		if (n % 2 == 0) {
			if (i % 2 == 0)
				auxtx = distance * ((i / 2) + 1) - distance/2;
			else{
				auxtx = (-1) * (distance * (parseInt(i / 2) + 1) - distance/2);
			}
		}
		else{
			if (i % 2 == 0){
				auxtx = distance * (i / 2);
			}
			else{
				auxtx = (-1) * distance * (parseInt(i / 2) + 1);
			}
		}
		var atomLength = molecules[i].elementsAtom.length;
		for (var j = 0; j < atomLength; j++){
			var atom = molecules[i].getAtom(j);
			sphere = new getSphere(atomSizeList[atom.getAtom()]);
			sphere.tx=atom.vector[0];
			sphere.ty=atom.vector[1];
			sphere.tz=atom.vector[2];
			sphere.kAmbi = atomColorList[atom.getAtom()][0];
			sphere.kDiff = atomColorList[atom.getAtom()][1];
			sphere.kSpec = atomColorList[atom.getAtom()][2];
			sphere.tx += auxtx;
			sceneModels.push(sphere)
		}
		var connectionLength = molecules[i].elementsConnections.length;
		for (var k = 0; k < connectionLength; k++){
			var connection = molecules[i].getConnection(k);
			cylinder = new getCylinderMult(connection.getType(),0.08,connection.calculateDistance()/2);
			for (var j = 0; j < cylinder.length; j++){
				cylinder[j].tx+=connection.getCenter()[0];
				cylinder[j].ty+=connection.getCenter()[1];
				cylinder[j].tz+=connection.getCenter()[2];
				cylinder[j].rotAngleYY= connection.getAngleZX();
				cylinder[j].rotAngleZZ = connection.getAngleXY();
				cylinder[j].rotAngleXX = connection.getAngleZY();
				cylinder[j].tx += auxtx;
				sceneModels.push(cylinder[j]);
			}
		}
	}
}
