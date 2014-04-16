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

sample1Array <- t(as.matrix(input_table[sample1_range,]))
sample2Array <- t(as.matrix(input_table[sample2_range,]))

errValue<-0
pValue <- sapply( seq(length=nrow(sample1Array)),
function(x) {summary(aov(sample1Array[x,] ~ sample2Array[x,]))[[1]][[1,"Pr(>F)"]] })
log2FC<-log2(rowMeans(sample1Array))-log2(rowMeans(sample2Array))
FCFun <- function(x){
if(!is.finite(x)){RerrValue <<- 2; return(-NaN)}
if (x<0) {return(-1/(2^x))} else {return(2^x)}}
foldChange<-sapply(log2FC, FCFun)

# add counters to labels
labels = colnames(input_table)
for(i in 1:length(labels)) {
  labels[i] = paste(i, labels[i], sep=": ")
}

ANOVA_table <- data.frame("P-Value"=pValue)
rownames(ANOVA_table) <- labels

ANOVA_volcano <- data.frame("Fold Change (Sample 1 -> Sample 2)"=foldChange, "P-Value"=pValue)
rownames(ANOVA_volcano) <- labels
