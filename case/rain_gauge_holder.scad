
// left
translate([0, 0,0]) {
    union() {
        difference() {
            cube([14, 14, 35]);
            translate([14/2, 14/2, -1])
            cylinder(d=8.5, h=60, $fn=90);
        }
        translate([0, 14, 0]) {
            difference() {
                cube([8, 14, 14]);
                translate([-1, 14/2, 14/2]) {
                    rotate([0, 90, 0]) {
                        cylinder(d=4.2, h=20, $fn=90);
                    }
                }
            }
        }
    }
}


// right
translate([0, -50, 0]) {
    union() {
        difference() {
            cube([14, 14, 35]);
            translate([14/2, 14/2, -1])
            cylinder(d=8.5, h=60, $fn=90);
        }
        translate([0, -14, 0]) {
            difference() {
                cube([8, 14, 14]);
                translate([-1, 14/2, 14/2]) {
                    rotate([0, 90, 0]) {
                        cylinder(d=4.2, h=20, $fn=90);
                    }
                }
            }
        }
    }
}
