#!/usr/bin/python2

import re, glob, os, subprocess, tempfile, json



def handle_layouts(layouth_fn, layoutc_fn, infojson_fn, config_fn):
    config = open(config_fn, "r").read()
    cols = int(re.findall(r"#define\s+MATRIX_COLS\s+([0-9]+)\n", config)[0])
    rows = int(re.findall(r"#define\s+MATRIX_ROWS\s+([0-9]+)\n", config)[0])

    f = open(infojson_fn, "r")
    js = json.loads(f.read())['layouts']
    f.close()

    d = open(layouth_fn, "r").read().replace("\\\n", " ")
    dd = re.findall(r'#define\s+(LAYOUT[a-zA-Z0-9_]*)\s*\(([^\)]*)\)', d)
    temp_fn = ".__temp.c"
    separator = "%%%%%%"
    f = open(temp_fn, "w")
    f.write("#include \"" + layouth_fn + "\"\n")
    for l in dd:
        le = len(l[1].split(","))
        f.write(separator + l[0] + separator + "\n" + l[0] + "(" + (",".join([str(i) for i in range(le)])) + ")\n")
    f.close()
    i = os.path.split(os.path.abspath(layouth_fn))[0]
    includes = []
    while True:
        includes.append(i)
        i, j = os.path.split(i)
        if j == "keyboards" or not i:
            break
    for i in "-Iusers/default -I. -Itmk_core -Iquantum -Iquantum/keymap_extras -Iquantum/audio -Iquantum/process_keycode -Iquantum/api -Idrivers -Idrivers/avr -Itmk_core/protocol -Itmk_core/common -Itmk_core/common/avr -Itmk_core/protocol/lufa -Ilib/lufa -Idrivers/avr".split(" "):
        if i.startswith("-I"):
            includes.append("../../../../" + i[2:])
    p = subprocess.Popen(["gcc", "-E"] + ["-I" + i for i in includes] + ["-include", os.path.join(os.path.split(os.path.abspath(layouth_fn))[0], "config.h"), temp_fn], stdout=subprocess.PIPE)
    data = p.communicate()[0].split(separator)
    os.unlink(temp_fn)
    del data[0]
    laynames = []
    keybname = re.sub(r'[^a-zA-Z0-9_]', '_', layoutc_fn)
    for i in range(0, len(data), 2):
        lname = data[i]
        desc = "}".join(data[i+1].split("{", 1)[1].split("}")[:-1])
        desc = [i.replace("{", "").replace("}", "") for i in re.findall("\{[^}]*\}", desc)]
        desc = [[j.strip() for j in i.split(",")] for i in desc]
        arr = []
        for row in range(len(desc)):
            for col in range(len(desc[row])):
                if desc[row][col] != "KC_NO":
                    ind = int(desc[row][col])
                    while len(arr) <= ind:
                        arr.append('')
                    arr[ind] = (row, col)
        layname = keybname + "_" + re.sub(r'[^a-zA-Z0-9_]', '_', lname)
        print("struct key_def " + layname + "_keys[] = {")
        for i in range(len(arr)):
            item = js[lname]['layout'][i]
            x = item['x']
            y = item['y']
            w = item['w'] if 'w' in item else 1
            h = item['h'] if 'h' in item else 1
            print ("    { .row = %d, .col = %d, .x = %f, .y = %f, .w = %f, .h = %f }," % (arr[i][0], arr[i][1], x, y, w, h))
        print("};")
        laynames.append((layname, lname))
    print("struct lay_def " + keybname + "_lays[] = {")
    for layname, lname in laynames:
        print("    {")
        print("        .lay_name = \"" + lname + "\",")
        print("        .n_keys = sizeof(" + layname + "_keys) / sizeof(" + layname + "_keys[0]),")
        print("        .keys = " + layname + "_keys,")
        print("    },")
    print("};")
    return keybname, cols, rows


def find_layouts(starting_dir):
    print("#include \"kbd_defs.h\"")
    keebs = []
    for root, dirs, files in os.walk(starting_dir):
        for fil in files:
            layouth_fn = os.path.join(root, fil)
            if layouth_fn.endswith(".h"):
                spl1 = os.path.split(layouth_fn[:-2])
                spl2 = os.path.split(spl1[0])
                if spl1[1] == spl2[1]:
                    infojson_fn = os.path.join(spl1[0], "info.json")
                    config_fn = os.path.join(spl1[0], "config.h")
                    layoutc_fn = "keyboards/xwhatsit" + layouth_fn[len(starting_dir):-2] + ".c"
                    keybname, cols, rows = handle_layouts(layouth_fn, layoutc_fn, infojson_fn, config_fn)
                    keebs.append((keybname, layoutc_fn, cols, rows))
    print("struct kbd_def keyboards[] = {")
    for keybname, layoutc_fn, cols, rows in keebs:
        print("    {")
        print("        .kbd_name = \"" + layoutc_fn + "\",")
        print("        .n_layouts = sizeof(" + keybname + "_lays)/sizeof(" + keybname + "_lays[0]),")
        print("        .layouts = " + keybname + "_lays,")
        print("        .cols = %d," % (cols,))
        print("        .rows = %d," % (rows,))
        print("    },")
    print("};")
    print("int n_keyboards = sizeof(keyboards)/sizeof(keyboards[0]);")


find_layouts("../..")


