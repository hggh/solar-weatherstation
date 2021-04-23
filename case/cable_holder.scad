difference() {
    translate([0, 10, 0]) {
        cube([20, 2, 39]);
    }
    translate([20 / 2, 30, 10]) {
        rotate([90, 0, 0]) {
            // Bohrung befestigung an Gitter
            cylinder(d=5.8, h=60, $fn=180);
        }
    }
} 
