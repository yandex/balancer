print "#define TYPELIST_1(T1) TTypeList<T1, TNone>"

def genT(i, j):
    s = ""

    for k in range(i, j + 1):
        s += "T" + str(k)

        if k != j:
            s += ", "

    return s

for i in range(2, 15):
    s = "#define TYPELIST_" + str(i) + "("

    s += genT(1, i)
    s += ") TTypeList<T1, TYPELIST_" + str(i - 1) + "("
    s += genT(2, i)
    s += ") >"

    print s
