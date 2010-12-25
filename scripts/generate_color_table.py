#!/usr/bin/python

import sys

if (len(sys.argv) < 2):
	fn = '/usr/share/ldraw/LDConfig.ldr'
else:
	fn = sys.argv[1]

f = open(fn)

for line in f:
    if '!COLOUR' in line:
        line = line.strip()
        
        ns = line.split()

        category = ''
        if 'RUBBER' in line:
            category = 'material_rubber'
        elif 'METAL' in line:
            category = 'material_metallic'
        elif 'SPECKLE' in line:
            category = 'material_speckle'
        elif 'GLITTER' in line:
            category = 'material_glitter'
        elif 'LUMINANCE' in line:
            category = 'material_luminant'
        elif 'PEARLESCENT' in line:
            category = 'material_pearlescent'
        elif 'CHROME' in line:
            category = 'material_chrome'
        elif 'ALPHA' in line:
            category = 'material_transparent'
        else:
            category = 'material_normal'

        name = '"' + ns[2].replace('_', ' ')
        idx = int(ns[4])

        color = ns[6][1:]
        edge = ns[8][1:]

        cr = int(color[0:2], 16)
        cg = int(color[2:4], 16)
        cb = int(color[4:6], 16)
        ca = 255

        lumi = 0

        for i in range(len(ns)):
            if ns[i] == 'ALPHA':
                ca = int(ns[i+1])
            elif ns[i] == 'LUMINANCE':
                lumi = int(ns[i+1])

        er = int(edge[0:2], 16)
        eg = int(edge[2:4], 16)
        eb = int(edge[4:6], 16)

        print "{ %20s, {%3d, %3d, %3d, %3d}, {%3d, %3d, %3d, 255}, %2d, %3d, %32s\", 0L }," % (category, cr, cg, cb, ca, er, eg, eb, lumi, idx, name)
