class Atom
{

	constructor(atom,vector,uniqueId)
	{
		this.vector=vector;
		this.atom=atom;
		this.color= this.getAtomColor(atom);
		this.radius= this.getAtomRadius(atom);
		this.id=uniqueId;
		this.uniqueId++;
	}
	//Getter
	getVector()
	{
		return this.vector
	}

	getAtom()
	{
		return this.atom;
	}
	getId()
	{
		return this.id;
	}
	//Methods
	getAtomColor(atom)
	{
		return "#EA162B";
	}

	getAtomRadius(atom)
	{
		return 0.3;
	}

}

class Connection
{

	constructor(position1, position2, type=1)
	{
		this.position1=position1;
		this.position2=position2

		this.absoluteDistance=this.calculateDistance(this.position1,this.position2);

		this.angleZX=getAngle([position1[2],position1[0]],[position2[2],position2[0]]);
		// this.angleXY=getAngle([position1[0],position1[1]],[position2[0],position2[1]]);
		this.angleZY=getAngle([position1[1],position1[2]],[position2[1],position2[2]]);
		this.angleXY=getAngleOrigin([position1[0],position1[1],position1[2]],[position2[0],position2[1],position2[2]]);

		// this.angleZ=
		if(type>3 && type<0) this.type=1;
		else this.type=type;
	}

	removeElementArray(array,index){
			var tmpArray=array;
			return tmpArray.splice(index,1);
	}

	getAngleZX(){
		return this.angleZX;
	}

	getAngleXY(){
		return this.angleZY;
	}

	getAngleZY(){
		return this.angleXY;
	}
	getAtom1()
	{
		return this.position1;
	}

	getAtom2()
	{
		return this.position2;
	}

	getCenter()
	{
		return vec4((this.position2[0]+this.position1[0])/2,(this.position2[1]+this.position1[1])/2,(this.position2[2]+this.position1[2])/2,0.0);
	}
	getType()
	{
		return this.type;
	}
	calculateDistance()
	{
		return Math.sqrt(Math.pow(this.position2[0]-this.position1[0],2)+Math.pow(this.position2[1]-this.position1[1],2)+Math.pow(this.position2[2]-this.position1[2],2));

	}
}

class Test{

	constructor(){

	}
}


class Molecule
{


	constructor(uniqueId)
	{
		this.connectionId=0;
		this.atomId=0;
		this.elementsAtom=[];
		this.elementsConnections=[];
		this.id=uniqueId;

	}

	getAtom(id){
		return this.elementsAtom[id];
	}
	getConnection(id){
		return this.elementsConnections[id];
	}

	setAtom(atom,position)
	{
		this.elementsAtom[this.atomId++]=new Atom(atom,position);
	}
	setConnection(atom1Id, atom2Id, type)
	{
			this.elementsConnections.push(new Connection(this.getAtom(atom1Id).getVector(),this.getAtom(atom2Id).getVector(),type,this.connectionId++));
	}

}
