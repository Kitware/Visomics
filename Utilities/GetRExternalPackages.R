# ##########################################################################
#
#  Program: Visomics
#
#  Copyright (c) Kitware Inc.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0.txt
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
# ##########################################################################

# Run: R CMD BATCH GetRExternalPackages.R
# Note: It's recommended to run this script with administrative/root privileges

is.installed <- function(mypkg) is.element(mypkg, installed.packages()[,1])

write("Checking for R package 'pls'...", file = "")
if( !is.installed("pls") ) {
  write("  Package 'pls' not found; installing...", file = "")
  install.packages(pkgs="pls", repos="http://cran.r-project.org")
} else {
  write("  Package 'pls' found", file = "")
}

write("Checking for R package 'preprocessCore'...", file = "")
if( !is.installed("preprocessCore") ) {
  write("  Package 'preprocessCore' not found; installing...", file = "")
  source("http://www.bioconductor.org/biocLite.R")
  biocLite("preprocessCore")
} else {
  write("  Package 'preprocessCore' found", file = "")
}

write("Checking for R package 'geiger'...", file = "")
if( !is.installed("geiger") ) {
  write("  Package 'geiger' not found; installing...", file = "")
  install.packages(pkgs="geiger", repos="http://cran.r-project.org",dependencies=TRUE)
} else {
  write("  Package 'geiger' found", file = "")
}

