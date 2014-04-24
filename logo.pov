// a pentagram prism
// Final: povray -Q11 Height=1024 Width=1024 +A0.1 +J
// Dev: povray +D +P +Ua +Q4

#include "colors.inc"
#include "glass.inc"
camera {
  location <8,35,-9> * .1
  look_at 0
  right<1,0,0>
}
light_source {<-2, 28, 8> *.1 color White area_light <1,0,0> <0,0,1> 8 8}
light_source {<-8, 3, 2> color White area_light <1,0,0> <0,0,1> 8 8}

global_settings {max_trace_level 16}

#declare tex = texture {
			pigment{color Col_Glass_Bluish}
			finish {F_Glass8}
		}

#declare x1 = 0.47;
#declare x2 = 0.293;
#declare z1 = 0.5;
#declare z2 = 0.154;
#declare z3 = 0.4;

#declare len = 3;
#declare h = 1;

#declare p1low = <0, 0, z1>;
#declare p2low = <-x1, 0, z2>;
#declare p3low = <-x2, 0, -z3>;
#declare p4low = <x2, 0, -z3>;
#declare p5low = <x1, 0, z2>;

#declare p1hi = <0, h, z1>;
#declare p2hi = <-x1, h, z2>;
#declare p3hi = <-x2, h, -z3>;
#declare p4hi = <x2, h, -z3>;
#declare p5hi = <x1, h, z2>;

#declare p1mid = <0, h*0.5, z1> * <-len, 1, -len>;
#declare p2mid = <-x1, h*0.5, z2> * <-len, 1, -len>;
#declare p3mid = <-x2, h*0.5, -z3> * <-len, 1, -len>;
#declare p4mid = <x2, h*0.5, -z3> * <-len, 1, -len>;
#declare p5mid = <x1, h*0.5, z2> * <-len, 1, -len>;

mesh {
	// cap
	triangle {<0, 0, 0>, p1low, p2low}
	triangle {<0, 0, 0>, p2low, p3low}
	triangle {<0, 0, 0>, p3low, p4low}
	triangle {<0, 0, 0>, p4low, p5low}
	triangle {<0, 0, 0>, p5low, p1low}

	// cap
	triangle {<0, h, 0>, p1hi, p2hi}
	triangle {<0, h, 0>, p2hi, p3hi}
	triangle {<0, h, 0>, p3hi, p4hi}
	triangle {<0, h, 0>, p4hi, p5hi}
	triangle {<0, h, 0>, p5hi, p1hi}

	// fin
	triangle {p1mid, p3low, p4low}
	triangle {p1mid, p3low, p3hi}
	triangle {p1mid, p3hi, p4hi}
	triangle {p1mid, p4low, p4hi}

	// fin
	triangle {p2mid, p4low, p5low}
	triangle {p2mid, p4low, p4hi}
	triangle {p2mid, p4hi, p5hi}
	triangle {p2mid, p5low, p5hi}

	// fin
	triangle {p3mid, p1low, p5low}
	triangle {p3mid, p1low, p1hi}
	triangle {p3mid, p1hi, p5hi}
	triangle {p3mid, p5low, p5hi}

	// fin
	triangle {p4mid, p1low, p2low}
	triangle {p4mid, p1low, p1hi}
	triangle {p4mid, p1hi, p2hi}
	triangle {p4mid, p2low, p2hi}

	// fin
	triangle {p5mid, p3low, p2low}
	triangle {p5mid, p3low, p3hi}
	triangle {p5mid, p3hi, p2hi}
	triangle {p5mid, p2low, p2hi}

	texture { tex }
	interior {I_Glass_Caustics1 ior 1.4}
}

plane { <0,1,0>, -0.01 pigment {White}}
