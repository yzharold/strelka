#
# Strelka - Small Variant Caller
# Copyright (c) 2009-2016 Illumina, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#

################################################################################
##
## file CMakeLists.txt
##
################################################################################

# variant scoring and indel error files:
file (GLOB JSON_CONFIG_LIST [a-zA-Z0-9]*.json empiricalVariantScoring/models/[a-zA-Z0-9]*.json)

install(FILES ${JSON_CONFIG_LIST} DESTINATION ${THIS_CONFIGDIR})
