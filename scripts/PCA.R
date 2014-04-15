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
PCAData <- df[numeric_rows,]
PCAData <- PCAData[sapply(PCAData, function(x) all(!is.na(x)))]

# run PCA
pc1<-prcomp(PCAData, scale.=F, center=T, retx=T)
pcaRot<-pc1$rot

data<-summary(pc1)
numcol=ncol(data$importance)
OutputData<-unlist(data[6],use.names=FALSE)

stddev = data.frame(t(OutputData[((1:numcol)*3)-2]))
row.names(stddev) <- "Std Dev"
colnames(stddev) <- colnames(pcaRot)

loadingvector = c(1:numcol-1)
perload = data.frame(t(OutputData[((1:numcol)*3)-1]))
perload = rbind(loadingvector, perload)
row.names(perload) <- c("Loading Vector", "Percent Loading")
colnames(perload) <- colnames(pcaRot)

sumperload = data.frame(t(OutputData[((1:numcol)*3)]))
sumperload = rbind(loadingvector, sumperload)
row.names(sumperload) <- c("Loading Vector", "Cumulative Percent Loading")
colnames(sumperload) <- colnames(pcaRot)

projection<-pc1$x
row.names(projection) <- t(row_names)
