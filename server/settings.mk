# Libraries
LIBS=commons pthread readline m

# Custom libraries' paths
SHARED_LIBPATHS=../so-commons-library
STATIC_LIBPATHS=

# Compiler flags
CDEBUG=-g -Wall -DDEBUG -fdiagnostics-color=always
CRELEASE=-O3 -Wall -DNDEBUG
