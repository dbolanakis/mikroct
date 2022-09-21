$fn=50;
// Create an enclosure using 4 cylinders connected to each other with hull()

// Dimension of enclosure
CylinderRadius=5; //radious of enclosure corners
Width=110;  //x //first version 110
Length=60;  //y //first version 60
Height=60;  //z //first version 60
WallThickness=3;
lid_thickness=3;
lid_lip=WallThickness/2;

ScrewSockets_diameter=9;
hole_diameter=3; //screw's size if using lid


lid_tolerance=0.5;
lid_HEIGHT = 13.5;


module Chassis(x,y,z,h,r){
    translate([ x , y, z])   cylinder(r=r,h=h);
    translate([-x,  y, z])   cylinder(r=r,h=h);
    translate([-x, -y, z])   cylinder(r=r,h=h);
    translate([ x, -y, z])   cylinder(r=r,h=h);
}

// Width and Lenght should be divided by 2 because it draws the object over zero, while height is applied on possitive Z axes only
// CylinderRadious should be subtracted from Width and Length of the box, otherwise the outer dimensions will become larger
// Test the following line to see the Chassis of the box
//!hull()  Chassis(x=Width/2-CylinderRadius,y=Length/2-CylinderRadius,z=0,h=Height,r=CylinderRadius);



difference(){ // remove from Chassis the Hollow and the Lip
    hull(){ //create Chassis of enclosure
        Chassis(
            x=Width/2 -CylinderRadius,
            y=Length/2 - CylinderRadius,
            z=0,
            h=Height,
            r=CylinderRadius);
    }

    hull(){ //create inside Hollow of enclosure (i.e., the same as Chassis of enclosure decreased by WallThickness)
    //HOLLOWS STARTS FROM Z=WallThickness   
        Chassis(
            x=Width/2 - CylinderRadius - WallThickness,
            y=Length/2 - CylinderRadius - WallThickness,
            z=WallThickness, //thickness of bottom area
            h=Height,
            r=CylinderRadius);
    }
    //*
    hull(){ //lip inside the enclosure
    //LID STARTS FROM Z=Height of box minus lid_thickness    
        Chassis(
            x=Width/2 - CylinderRadius - lid_lip,
            y=Length/2 - CylinderRadius - lid_lip,
            z=Height - lid_thickness, //leave a gap from the upper arrea of the box, equal to lid_thickness
            h=Height, // just not to confuse OpenSCAD
            r=CylinderRadius);
    }  //*/
    
    //Terminal Block
    translate([ -Width/2 - WallThickness/2, 0, Height/2]) rotate([0,90,0]) cylinder(h=WallThickness*2, d=12);
}
//*


difference(){
    Chassis( //Screw Sockets (place them inside the Wall/thickness/2 because the box is aligned over zero of axis, and then subtract the raius of the Screw Sockets, i.e. diameter/2)
        x=Width/2 - WallThickness/2  - ScrewSockets_diameter/2,
        y=Length/2  - WallThickness/2  - ScrewSockets_diameter/2,
        z=WallThickness - 0.5, //place them 0.5mm inside the bottom wall
        h=Height - WallThickness - lid_thickness + 0.5,
        r=ScrewSockets_diameter/2);
    
    Chassis( //screw holes
        x=Width/2 - WallThickness/2 - ScrewSockets_diameter/2,
        y=Length/2 - WallThickness/2 - ScrewSockets_diameter/2,
        z=WallThickness , 
        h=Height - WallThickness - lid_thickness + 0.5 ,
        r=hole_diameter/2);
}

//LID STARTS HERE !!!! place ! mark before the difference to stl the LID only
#difference(){
union() {
    difference(){
      difference(){
        hull(){ //lid
            Chassis(
                x=Width/2 - CylinderRadius - WallThickness/2 - lid_tolerance,
                y=Length/2 - CylinderRadius - WallThickness/2 - lid_tolerance,
                z=Height - lid_thickness + 5, //+ 5, //move lid up
                h=lid_HEIGHT, //lid_thickness,
                r=CylinderRadius);   
        }
        hull(){ //lid hole
            Chassis(
                x=Width/2 - CylinderRadius - WallThickness/2 - lid_tolerance - lid_lip,
                y=Length/2 - CylinderRadius - WallThickness/2 - lid_tolerance - lid_lip,
                z=Height - lid_thickness + 5 , //+ 5, //move lid up
                h=13, //lid_thickness,
                r=CylinderRadius);   
        }
    }
    Chassis( //lid
        x=Width/2 - WallThickness/2 - ScrewSockets_diameter/2,
        y=Length/2 - WallThickness/2 - ScrewSockets_diameter/2,
        z=Height - lid_thickness +5, 
        h=Height - WallThickness - lid_thickness + lid_tolerance,
        r=hole_diameter/2 + lid_tolerance); 
    }    
    difference(){
      difference(){
        hull(){ //lid
            Chassis(
                x=Width/2 - CylinderRadius - WallThickness/2 - lid_tolerance,
                y=Length/2 - CylinderRadius - WallThickness/2 - lid_tolerance,
                z=Height - lid_thickness + 5, //+ 5, //move lid up
                h=lid_HEIGHT, //lid_thickness,
                r=CylinderRadius);   
        }
        hull(){ //lid hole
            Chassis(
                x=Width/2 - CylinderRadius - WallThickness/2 - lid_tolerance - lid_lip,
                y=Length/2 - CylinderRadius - WallThickness/2 - lid_tolerance - lid_lip,
                z=Height - lid_thickness + 5 , //+ 5, //move lid up
                h=13, //lid_thickness,
                r=CylinderRadius);   
        }
    }
    Chassis( //lid
        x=Width/2 - WallThickness/2 - ScrewSockets_diameter/2,
        y=Length/2 - WallThickness/2 - ScrewSockets_diameter/2,
        z=Height - lid_thickness +5, 
        h=Height - WallThickness - lid_thickness + lid_tolerance,
        r=hole_diameter/2 + lid_tolerance); 
    }

    difference(){
        Chassis( //Screw Sockets (place them inside the Wall/thickness/2 because the box is aligned over zero of axis, and then subtract the raius of the Screw Sockets, i.e. diameter/2)
            x=Width/2 - WallThickness/2  - ScrewSockets_diameter/2,
            y=Length/2  - WallThickness/2  - ScrewSockets_diameter/2,
            z=Height - lid_thickness + 5, //+ 5, //move up to the lid 
                //z=WallThickness - 0.5, //place them 0.5mm inside the bottom wall
            h=lid_HEIGHT /*Height - WallThickness - lid_thickness + 0.5*/,
            r=ScrewSockets_diameter/2);
        
        Chassis( //screw holes
            x=Width/2 - WallThickness/2 - ScrewSockets_diameter/2,
            y=Length/2 - WallThickness/2 - ScrewSockets_diameter/2,
             z=Height - lid_thickness, //+ 5, //move up to the lid
                //z=WallThickness , 
            h=Height - WallThickness - lid_thickness + 0.5 ,
            r=hole_diameter/2);
    }
    
////////////////// create equal distance with the outer of the box
    difference(){
      difference(){
        hull(){ //lid
            Chassis(
                x=Width/2 - CylinderRadius  - lid_tolerance,
                y=Length/2 - CylinderRadius  - lid_tolerance,
                z=Height - lid_thickness + 5 + lid_thickness, //+ 5, //move lid up
                h=lid_HEIGHT - lid_thickness,
                r=CylinderRadius);   
        }
        hull(){ //lid hole
            Chassis(
                x=Width/2 - CylinderRadius - WallThickness/2 - lid_tolerance - lid_lip,
                y=Length/2 - CylinderRadius - WallThickness/2 - lid_tolerance - lid_lip,
                z=Height - lid_thickness + 5 , //+ 5, //move lid up
                h=lid_HEIGHT-0.5, //lid_thickness,
                r=CylinderRadius);   
        }
    }
    Chassis( //lid
        x=Width/2 - WallThickness/2 - ScrewSockets_diameter/2,
        y=Length/2 - WallThickness/2 - ScrewSockets_diameter/2,
        z=Height - lid_thickness +5, 
        h=Height - WallThickness - lid_thickness + lid_tolerance,
        r=hole_diameter/2 + lid_tolerance); 
    }     
}

//// Switch hole for LID
    //Terminal Block
    translate([ Width/2 - 10, 0, Height-lid_thickness+5]) rotate([0,0,90]) cylinder(h=lid_HEIGHT, d=6);
}
