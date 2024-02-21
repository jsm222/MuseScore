# SPDX-License-Identifier: GPL-3.0-only
# MuseScore-CLA-applies
#
# MuseScore
# Music Composition & Notation
#
# Copyright (C) 2021 MuseScore BVBA and others
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

set(MODULE iex_musicxml)

set(MODULE_QRC musicxml.qrc)

include(${CMAKE_CURRENT_LIST_DIR}/internal/musicxml/musicxml.cmake)
find_package(LibXml2 REQUIRED)
include_directories("${LIBXML2_INCLUDE_DIRS}")

set(MODULE_SRC
    ${MUSICXML_SRC}
    ${CMAKE_CURRENT_LIST_DIR}/musicxmlmodule.cpp
    ${CMAKE_CURRENT_LIST_DIR}/musicxmlmodule.h
    ${CMAKE_CURRENT_LIST_DIR}/imusicxmlconfiguration.h
    )

if (MUSICXML_NO_INTERNAL)
    set(MODULE_DEF ${MODULE_DEF} -DMUSICXML_NO_INTERNAL -DMUSICXML_NO_INTERACTIVE -DMUSICXML_NO_VALIDATION)
else()
    set(MODULE_SRC ${MODULE_SRC}
        ${CMAKE_CURRENT_LIST_DIR}/internal/musicxmlconfiguration.cpp
        ${CMAKE_CURRENT_LIST_DIR}/internal/musicxmlconfiguration.h
        ${CMAKE_CURRENT_LIST_DIR}/internal/musicxmlreader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/internal/musicxmlreader.h
        ${CMAKE_CURRENT_LIST_DIR}/internal/musicxmlwriter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/internal/musicxmlwriter.h
        ${CMAKE_CURRENT_LIST_DIR}/internal/mxlwriter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/internal/mxlwriter.h
        )
find_package(LibXml2 REQUIRED)
include_directories("${LIBXML2_INCLUDE_DIRS}")
add_definitions(-DMUSICXML_CATALOG_FILE="${CMAKE_INSTALL_PREFIX}/${Mscore_SHARE_NAME}${Mscore_INSTALL_NAME}schema/catalog.xml" -DMUSICXML_SCHEMA="${CMAKE_INSTALL_PREFIX}/${Mscore_SHARE_NAME}${Mscore_INSTALL_NAME}schema/musicxml.xsd")

endif()

set(MODULE_LINK
    engraving
    "${LIBXML2_LIBRARIES}"
    )

include(SetupModule)

if (MUE_BUILD_IMPORTEXPORT_TESTS)
    add_subdirectory(tests)
endif()
