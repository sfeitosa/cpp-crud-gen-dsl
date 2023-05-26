PROJECT(swmanager)

cmake_minimum_required(VERSION 3.22)

IF (UNIX)
	SET(BW_OS_DIR linux) 
	SET(BW_INC_LIBS curl ssl pthread crypto)
	SET(BW_PQ_LIBS pq)
	SET(BW_SQLITE_LIBS sqlite3 pthread)
	SET(BW_INC_DIRS /usr/include/postgresql /usr/include/libnfnetlink
	/usr/include/libnetfilter_queue)
	SET(CMAKE_CXX_FLAGS "-Wall -Werror -Wunused -Wshadow -pipe -fPIC" CACHE
	STRING "Flags globais" FORCE)
ELSE (UNIX)
	SET(BW_OS_DIR win32)
	SET(BW_INC_LIBS libeay32 ws2_32)
	SET(BW_PQ_LIBS libpq libeay32)
	SET(BW_SQLITE_LIBS sqlite3 libeay32)
	SET(BW_INC_DIRS /OpenSSL/include)
	SET(BW_LINK_DIRS /OpenSSL/lib)
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF (UNIX)

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE Release CACHE STRING
		"Tipo da compilacao: None Debug Release ReWithDebInfo" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

SET(CMAKE_CXX_FLAGS_DEBUG "-g -DBWDEBUG" CACHE STRING
    "Flags para Debug")

ADD_DEFINITIONS(
    -DUSE_STL_STRING
    -DUSE_STL_VECTOR
    -DUSE_STL_MAP
    -DETCDIR="etc/"
    -DLIBDIR="lib/"
    -DBW_ROOT_DIR=""
    -DBW_LIBDIR="lib/"
)

INCLUDE_DIRECTORIES(
    ${BW_INC_DIRS}
    libs/http
    libs/utils
    libs/cgi
    libs/utils/${BW_OS_DIR}
    libs/database
    libs/database/${BW_OS_DIR}
    libs/${BW_OS_DIR}
    app/${BW_OS_DIR}
    app
)

LINK_DIRECTORIES(
    ${BW_LINK_DIRS}
)

ADD_LIBRARY(bwutils
    libs/utils/bwerror.cpp
    libs/utils/bwprimstring.cpp
    libs/utils/bwprimlist.cpp
    libs/utils/bwlist.cpp
    libs/utils/bwstring.cpp
    libs/utils/bwutils.cpp
    libs/utils/bwkeypair.cpp
    libs/utils/bwkeylist.cpp
    libs/utils/bwconfig.cpp
    libs/utils/bwabstractthread.cpp
    libs/utils/${BW_OS_DIR}/bwthread.cpp
    libs/utils/${BW_OS_DIR}/bwmutex.cpp
    libs/utils/bwurl.cpp
    libs/utils/bwtemplate.cpp
    libs/utils/bwlog.cpp
    libs/utils/bwmailer.cpp
    libs/utils/utils.cpp
    libs/utils/web.cpp
    libs/utils/bwurlhandler.cpp
    libs/utils/bwurlmail.cpp
)

ADD_LIBRARY(bwdatabase
    libs/database/bwdb.cpp
    libs/database/bwdatabase.cpp
    libs/database/${BW_OS_DIR}/bwdbload.cpp
    libs/database/bwresultset.cpp
    libs/database/bwdbregistry.cpp
    libs/database/bwdbrelation.cpp
)

ADD_LIBRARY(pgsql SHARED
    libs/database/bwdbpgsql.cpp
)

TARGET_LINK_LIBRARIES(pgsql bwdatabase bwutils ${BW_PQ_LIBS})

ADD_LIBRARY(sqlite SHARED
    libs/database/bwdbsqlite.cpp
)

TARGET_LINK_LIBRARIES(sqlite bwdatabase bwutils ${BW_SQLITE_LIBS})

ADD_LIBRARY(bwhttp
    libs/http/bwhttprequest.cpp
    libs/http/bwhttpresponse.cpp
    libs/http/bwhttp.cpp
    libs/http/bwform.cpp
)

ADD_LIBRARY(bwcgi
    libs/cgi/bwhtmlform.cpp
    libs/cgi/bwajaxhtmlform.cpp
    libs/cgi/bwdbhtmlform.cpp
    libs/cgi/bwajaxdbhtmlform.cpp
    libs/cgi/bwregcontainer.cpp
    libs/cgi/bwajaxregcontainer.cpp
    libs/cgi/bwreporthtmlform.cpp
    libs/cgi/bwhtmltable.cpp
    libs/cgi/bwajaxhtmltable.cpp
    libs/cgi/bwdbhtmltable.cpp
    libs/cgi/bwajaxdbhtmltable.cpp
    libs/cgi/bwhtmlnav.cpp
    libs/cgi/bwformaction.cpp
    libs/cgi/bwformactionfactory.cpp
    libs/cgi/bwaccessprofile.cpp
    app/bwreportaction.cpp 
    # app/bwclientaction.cpp 
)

# FILE(GLOB BW_SPECIFIC_PLATFORM "libs/${BW_OS_DIR}/*.cpp")

# ADD_LIBRARY(bwspecific
#     ${BW_SPECIFIC_PLATFORM}
# )

ADD_EXECUTABLE(swm.cgi
        swmanager.cpp
)

TARGET_LINK_LIBRARIES(swm.cgi bwhttp bwcgi bwdatabase bwutils ${BW_INC_LIBS})

ADD_EXECUTABLE(swmsale.cgi
        custom/swmsale.cpp
)

TARGET_LINK_LIBRARIES(swmsale.cgi bwhttp bwcgi bwdatabase bwutils ${BW_INC_LIBS})

