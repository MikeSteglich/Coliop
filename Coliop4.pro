##########################################################################
#  This code is part of Coliop-IDE
#
#  Copyright (C) Mike Steglich - Technical University of Applied Sciences
#  Wildau, Germany
#
#  Coliop is a project of the Technical University of
#  Applied Sciences Wildau
#  Please visit the project homepage <www.coliop.org>
#
#  Coliop is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#
#  Coliop is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
#  License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, see <http://www.gnu.org/licenses/>.
#
##########################################################################

QT      += gui widgets printsupport
QT      += xml

#QMAKE_MAC_SDK = macosx10.15
#QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

TARGET = Coliop
TEMPLATE = app

CONFIG += c++17

DESTDIR = bin/
OBJECTS_DIR = obj
MOC_DIR = obj
UI_DIR = obj
RCC_DIR = obj


SOURCES +=\
    src/ClpAbout.cpp \
    src/ClpProblem.cpp \
    src/CmplHighlighter.cpp \
    src/ColiopApp.cpp \
    src/Main.cpp \
    src/ColiopUi.cpp \
    src/CmplRunner.cpp

HEADERS  += \
    src/ClpAbout.hh \
    src/ClpProblem.hh \
    src/CmplHighlighter.hh \
    src/ColiopApp.hh \
    src/ColiopUi.hh \
    src/CmplRunner.hh

FORMS    += src/coliopui.ui \
    src/clpabout.ui

RESOURCES += \
    Ressources/clpRessources.qrc
