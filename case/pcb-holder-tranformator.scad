$fn = 190;

translate([60, 0, 0]) {
    cylinder(h=2, d=4);
    cylinder(d=1.1, h=4);
} 

module holder() {
    difference() {
        union() {
            cylinder(d=6, h=6);
            cylinder(d=3, h=8);
        }
        
        translate([0, 0, 5]) {
            cylinder(d=1.5, h=10);
        }
    }
}

// Zugentlastung
translate([-30 -5 - 15, -15]) {
    difference() {
        cube([15, 30, 2]);
        translate([4, 10, -1]) {
            cylinder(d=4, h=10);
        }
        translate([4, 25, -1]) {
            cylinder(d=4, h=10);
        }
    }
    translate([2, 12, 2]) cube([4, 11, 3]);
}



translate([-30 -5,-15 -5 -5 ]) {
    cube([60 + 5, 30 + 10 +5, 2]);
    translate([0, -1.2, 0]) cube([44, 1.2, 20]);
}
translate([30, -15, 0]) {
    holder();
}
translate([30, 15, 0]) {
    holder();
}
translate([-30, 15, 0]) {
    holder();
}
translate([-30, -15, 0]) {
    holder();
}

