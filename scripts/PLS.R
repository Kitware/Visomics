library("pls", warn.conflicts=FALSE)

# get size of input data
rows <- 1:length(input_table[[1]])
cols <- 1:length(input_table)

# get row names too (assumed to be first column)
row_names <- input_table[[1]]

# convert data to numeric if it isn't already
for (col in cols) {
    input_table[[col]] <- suppressWarnings(as.numeric(input_table[[col]]))
}

# convert to data frame format
df <- data.frame(input_table)

# detect rows that consist solely of non-numeric data
numeric_rows <- integer(0)
for (row in rows) {
    keep_this_row <- FALSE
    for (col in cols) {
        if (!is.na(df[row,col])) {
            keep_this_row <- TRUE
            break
        }
    }
    if (keep_this_row) {
        numeric_rows <- append(numeric_rows, row)
    }
}

# strip out any non-numerical elements
input_table <- df[numeric_rows,]
input_table <- input_table[sapply(input_table, function(x) all(!is.na(x)))]

predictorArray <- as.matrix(input_table[, predictorGroup])
responseArray <- as.matrix(input_table[, responseGroup])

PLSdata <- data.frame(response=I(responseArray), predictor=I(predictorArray))

methodName <- "algorithm"
PLSresult <- plsr(response ~ predictor, data = PLSdata, method = methodName)
if(exists("PLSresult")) {
  RerrValue<-1
}else{
  RerrValue<-0
}
scores <- t(PLSresult$scores)
scores_transposed <- t(scores)
yScores <- t(PLSresult$Yscores)
yScores_transposed <- t(yScores)
loadings <- PLSresult$loadings[,]
loadingWeights <- PLSresult$loading.weights[,]
yLoadings <- PLSresult$Yloadings[,]
