library(geiger)
data<-as.numeric(input_table$column_name)
names(data)<-input_table[[1]]
o<-fitContinuous(input_tree, data, model="EB",SE=0)
result=o$opt
EB_table=list(parameter="value",z0=result$z0,sigsq=result$sigsq,a=result$a," "=" ",lnL=result$lnL,AIC=result$aic,AICc=result$aicc)
EB_tree<-transform(input_tree, "EB", o$opt$a)
