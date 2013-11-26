library(geiger)

modelName <- "modelType"

data<-as.numeric(tableData$selectedDataName)
names(data)<-tableData[[1]]
o<-fitContinuous(tree, data, model=modelName,SE=0)
result=o$opt

if (modelName == "OU") {
  resultTable=list("value",z0=result$z0,sigsq=result$sigsq,alpha=result$alpha," "=" ",lnL=result$lnL,AIC=result$aic,AICc=result$aicc)
   resultTree<-transform(tree, "OU", o$opt$alpha)
} else if (modelName == "BM") {
  resultTable=list("value",z0=result$z0,sigsq=result$sigsq," "=" ",lnL=result$lnL,AIC=result$aic,AICc=result$aicc)
  resultTree<-tree
} else if (modelName == "EB") {
  resultTable=list("value",z0=result$z0,sigsq=result$sigsq,a=result$a," "=" ",lnL=result$lnL,AIC=result$aic,AICc=result$aicc)
  resultTree<-transform(tree, "EB", o$opt$a)
}


