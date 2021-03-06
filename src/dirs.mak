# Software License Agreement
# 
#     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
# 
#     This file is part of the Orange C Compiler package.
# 
#     The Orange C Compiler package is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
# 
#     The Orange C Compiler package is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
# 
#     You should have received a copy of the GNU General Public License
#     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
# 
#     contact information:
#         email: TouchStone222@runbox.com <David Lindauer>
# 

ifeq "$(COMPILER)" "gcc-linux"

DIRS := sqlite3 util objlib netlib obrc ocpp ogrep oasm dlhex dlle dlmz dlpe dlpm olib olink \
       oimplib omake onm orc occ coff2ieee
else

ifneq "$(ORANGEC_ONLY)" "YES"
OCCIL_DIRS := netlib
endif

DIRS := sqlite3 util objlib $(OCCIL_DIRS) obrc ocpp ogrep oasm dlhex dlle dlmz dlpe dlpm olib olink \
       oimplib omake onm orc ocl occ coff2ieee help MSDNHelp
endif
