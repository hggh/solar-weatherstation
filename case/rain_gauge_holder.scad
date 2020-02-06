
union() {
    difference() {
        cube([14, 14, 40]);
        translate([14/2, 14/2, -1])
        cylinder(d=8.5, h=60, $fn=90);
    }
    translate([0, -14, 0]) {
        difference() {
            cube([14, 14, 14]);
            translate([-1, 14/2, 14/2]) {
                rotate([0, 90, 0]) {
                    cylinder(d=4, h=20, $fn=90);
                }
            }
        }
    }
}
