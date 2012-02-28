LIBRARY()

BUILD(USE_INTERNAL_STL)

ENABLE(NOUTIL)
ENABLE(NO_COMPILER_WARNINGS)

SRCDIR(
    util/private/stl/stlport-${CURRENT_STLPORT_VERSION}/src
    util/private/stl/stlport-${CURRENT_STLPORT_VERSION}/stlport
)

IF (CURRENT_STLPORT_VERSION EQUAL 5.1.4)
	SRCS(
		#allocators.cpp
		bitset.cpp
		cxa.c
		string.cpp
	)
ENDIF (CURRENT_STLPORT_VERSION EQUAL 5.1.4)

SRCS(
	ctype.cpp
	num_put_float.cpp
	locale_catalog.cpp
	numpunct.cpp
	c_locale.c
	dll_main.cpp
	locale_impl.cpp
	ostream.cpp
	codecvt.cpp
	facets_byname.cpp
	messages.cpp
	sstream.cpp
	collate.cpp
	fstream.cpp
	monetary.cpp
	stdio_streambuf.cpp
	complex.cpp
	ios.cpp
	num_get.cpp
	complex_io.cpp
	iostream.cpp
	num_get_float.cpp
	strstream.cpp
	complex_trig.cpp
	istream.cpp
	num_put.cpp
	time_facets.cpp
)
SET(IDE_FOLDER "util")

END()
