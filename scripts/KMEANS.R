# Order the data
ind <- order(input_table[[1]])

for(i in 1:length(input_table)) {
  input_table[[i]] = input_table[[i]][ind]
} 

toMatrix <- function(columnList) {
  sample <- matrix(nrow=length(columnList[[1]]), ncol=0)
  for(i in 2:length(columnList)) {
    
    # Replace an columns contain characters with zeros
    if (typeof(columnList[[i]]) == 'character') {
      sample <- cbind(sample, c(rep(0, length(columnList[[i]]))))
    }
    else {
      sample <- cbind(sample,  columnList[[i]])
    }
  }

  return(sample)
}

metabData <- toMatrix(input_table)

metabDatat <- t(metabData)
km<-kmeans(metabDatat, kmeans_centers, iter.max = kmeans_iter_max, nstart = kmeans_number_of_random_start, algorithm = "kmeans_algorithm")
kmCenters<-km$centers 
kmCluster<-km$cluster
kmWithinss<-km$withinss
kmSize<-km$size

collectColumnIds <- function(columnList, clusterId) {

  ids = numeric(0)
  for(i in 1:length(columnList)) {
    column = columnList[i]
    if (column[[1]] == clusterId ) {
      ids <- c(ids, i)
    }
  }

  return(ids)
}

swapClusterIds <- function(columnList, columnIds1, value1, columnIds2, value2) {
  for(i in columnIds1) {
    columnList[i][[1]] = value2
  }

  for(i in columnIds2) {
    columnList[i][[1]] = value1
  }
  return(columnList)
}


renumber <- function(columnList) {
  processedClusterIds <- numeric(0)
  futureClusterId <- 1
  for(i in 1:length(columnList)) {
    column = columnList[i]
    currentClusterId = column[[1]]

    if (currentClusterId %in% processedClusterIds )
      next

    rightColumnIds <- collectColumnIds(columnList, currentClusterId)
    leftColumnIds <- collectColumnIds(columnList, futureClusterId)
    columnList <- swapClusterIds(columnList, rightColumnIds, currentClusterId, leftColumnIds, futureClusterId)
    processedClusterIds <- c(processedClusterIds, currentClusterId)
    futureClusterId <- futureClusterId + 1
  }

  return(columnList)
}

cluster <- list()
clusterNames <- names(input_table)
for (i in 1:length(kmCluster)) {
  name <- paste(rawToChar(as.raw(64+i)), clusterNames[i+1], sep=":")
  cluster[name] <- kmCluster[i]
}

cluster <- renumber(cluster)
cluster <- c("Cluster number", renumber(cluster))
