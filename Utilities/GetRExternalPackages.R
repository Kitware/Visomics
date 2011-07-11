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
