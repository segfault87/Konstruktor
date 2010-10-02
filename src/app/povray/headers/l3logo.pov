#declare L3Logo = union {
	sphere { <-59, 0, -96>, 6 }
	cylinder { <-59, 0, -96>, <59, 0, -122>, 6 open }
	sphere { <59, 0, -122>, 6 }
	tcylinder { <59, 0, -122>, <59, 0, -84>, 6 open }
	sphere { <59, 0 ,-84>, 6 }
	
	sphere { <-59, 0, -36>, 6 }
	cylinder { <-59, 0, -36>, <-59, 0, 1>, 6 open }
	sphere { <-59, 0, 1>, 6 }
	cylinder { <0, 0, -49>, <0, 0, -25>, 6 open }
	sphere { <0, 0, -25>, 6 }
	sphere { <59, 0, -62>, 6 }
	cylinder { <59, 0, -62>, <59, 0, -24>, 6 open }
	sphere { <59, 0, -24>, 6 }
	cylinder { <-59 ,0 ,-36>, <59, 0, -62>, 6 open }
	
	sphere { <-35.95, 0, 57>, 6 }
	torus { 18.45, 6 clipped_by { plane { <40,0,-9>, 0 } } translate <-40, 0, 39> }
	cylinder { <-44.05, 0, 21>, <35.95, 0, 3>, 6 open }
	torus { 18.45, 6 clipped_by { plane { <-40, 0, 9>, 0 } } translate <40, 0, 21> }
	cylinder { <44.05, 0, 39>, <0, 0, 49>, 6 open }
	sphere { <0, 0, 49>, 6 }
	cylinder { <0, 0, 49>, <0, 0, 34>, 6 open }
	sphere { <0, 0, 34>, 6 }
	
	torus { 18.45, 6 clipped_by { plane { <40, 0, -9>, 0 } } translate <-40, 0, 99> }
	cylinder { <-44.05, 0, 81>, <35.95, 0, 63>, 6 open }
	torus { 18.45, 6 clipped_by { plane { <-40, 0, 9>,0 } } translate <40, 0, 81> }
	cylinder { <44.05, 0, 99>,<-35.95, 0, 117>, 6 open }
	
	scale 4.5 / 128
}

