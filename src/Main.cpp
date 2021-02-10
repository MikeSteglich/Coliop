/***********************************************************************
 *  This code is part of Coliop-IDE
 *
 *  Copyright (C) Mike Steglich
 *  Technical University of Applied Sciences
 *  Wildau, Germany
 *
 *  Coliop is a project of the Technical University of
 *  Applied Sciences Wildau.
 *  Please visit the project homepage <www.coliop.org>
 *
 *  Coliop is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Coliop is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/


#include "ColiopApp.hh"
#include "ColiopUi.hh"


int main(int argc, char *argv[])
{
    ColiopApp col(argc, argv);
    return col.exec();
}
