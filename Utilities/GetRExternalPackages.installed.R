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

is.installed <- function(pkg) is.element(pkg, installed.packages()[,1])

install.missing.packages <- function(pkgs, install.func) {
  for(i in 1:length(pkgs)) {
    pkg <- pkgs[i]
    if(!is.installed(pkg)) {
      write(sprintf("  Package '%s' not found; installing...", pkg), file = "")
      install.func(pkg)
    } else {
      write(sprintf("  Package '%s' found", pkg), file = "")
    }
  }
}

cran.install <- function(pkg) {
  install.packages(pkgs = pkg, repos = "http://cran.r-project.org", dependencies = TRUE)
}

bioc.install <- function(pkg) {
  if(!exists("biocLite")) {
    source("http://www.bioconductor.org/biocLite.R")
  }
  biocLite(pkg)
}

# ##########################################################################

install.missing.packages("pls", cran.install)

install.missing.packages("preprocessCore", bioc.install)

install.missing.packages(c("ape", "MASS", "mvtnorm", "msm", "subplex", "deSolve", "colorspace", "digest", "Rcpp", "coda"), cran.install)

install.missing.packages("geiger", function(pkg) install.packages(pkgs = "PACKAGE_PATH/geiger_1.4-5.667.tar.gz", repos=NULL,dependencies=TRUE, type="source"))
