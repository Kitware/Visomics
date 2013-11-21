# Order the data
ind <- order(input_table[[1]])

for(i in 1:length(input_table)) {
  input_table[[i]] = input_table[[i]][ind]
} 

filter <- function(columnList, columnIndexes) {
  write(typeof(columnList), stderr())
  sample <- matrix(nrow=length(columnList[[1]]), ncol=0)
  for(i in columnIndexes) {
    
    # Replace an columns contain characters with zeros
    if (typeof(columnList[[i+1]]) == 'character') {
      sample <- cbind(sample, c(rep(0, length(columnList[[i+1]]))))
    }
    else {
      sample <- cbind(sample,  columnList[[i+1]])
    }
  }

  return(sample)
}

sample1Array <- filter(input_table, sample1_range)
sample2Array <- filter(input_table, sample2_range)


errValue<-0
pValue <- sapply( seq(length=nrow(sample1Array)),
function(x) {summary(aov(sample1Array[x,] ~ sample2Array[x,]))[[1]][[1,"Pr(>F)"]] })
log2FC<-log2(rowMeans(sample1Array))-log2(rowMeans(sample2Array))
FCFun <- function(x){
if(!is.finite(x)){RerrValue <<- 2; return(-NaN)}
if (x<0) {return(-1/(2^x))} else {return(2^x)}}
foldChange<-sapply(log2FC, FCFun)

# add counters to labels
labels = input_table[[1]]
for(i in 1:length(labels)) {
  labels[i] = paste(i, labels[i], sep=": ")
}

ANOVA_table <- list(labels, "P-Value"=pValue)

ANOVA_volcano <- list(labels, "Fold Change (Sample 1 -> Sample 2)"=foldChange, "P-Value"=pValue)



