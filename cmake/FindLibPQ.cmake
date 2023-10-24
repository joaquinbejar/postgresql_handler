# FindLibPQ.cmake

# Use pkg-config to get hints about paths
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_LIBPQ QUIET libpq)
endif()

# Include dir
find_path(LibPQ_INCLUDE_DIR
        NAMES
        libpq-fe.h
        HINTS
        ${PC_LIBPQ_INCLUDEDIR}
        ${PC_LIBPQ_INCLUDE_DIRS}
        /opt/homebrew/opt/libpq/include
        PATH_SUFFIXES
        postgresql
)

# Libraries
find_library(LibPQ_LIBRARY
        NAMES
        pq
        HINTS
        ${PC_LIBPQ_LIBDIR}
        ${PC_LIBPQ_LIBRARY_DIRS}
        /opt/homebrew/opt/libpq/lib
)

# Create imported target
if(LibPQ_INCLUDE_DIR AND LibPQ_LIBRARY)
    add_library(LibPQ::PQ UNKNOWN IMPORTED)
    set_target_properties(LibPQ::PQ PROPERTIES
            IMPORTED_LOCATION "${LibPQ_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${LibPQ_INCLUDE_DIR}"
    )
endif()

# Version information
set(LibPQ_VERSION
        ${PC_LIBPQ_VERSION}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibPQ
        FOUND_VAR
        LibPQ_FOUND
        REQUIRED_VARS
        LibPQ_INCLUDE_DIR
        LibPQ_LIBRARY
        VERSION_VAR
        LibPQ_VERSION
)
