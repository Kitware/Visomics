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

RerrValue<-0;
meanMethod<- "mean_method"

if(meanMethod == 'Geometric') { 
  avgInit<-2^rowMeans(log2(sample1Array))
  avgFinal<-2^rowMeans(log2(sample2Array))
} else {
  avgInit<-rowMeans(sample1Array)
  avgFinal<-rowMeans(sample2Array) 
}

log2FC<-(log2(avgFinal)-log2(avgInit))
FCFun <- function(x) {
    if(!is.finite(x)){RerrValue <<- 2; return(-NaN)}
    if (x<0) {return(-1/(2^x))} else {return(2^x)}}
foldChangeValues<-sapply(log2FC, FCFun)

# add counters to labels
labels = input_table[[1]]
for(i in 1:length(labels)) {
  labels[i] = paste(i, labels[i], sep=": ")
}

foldChange <- list(labels, "Average Initial"=avgInit, "Average Final"=avgFinal,
  "Fold Change"=foldChangeValues)

foldChangePlot <- list(labels, "Fold Change"=foldChangeValues)
