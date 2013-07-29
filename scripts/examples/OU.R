library(geiger)
data<-as.numeric(input_table$column_name)
names(data)<-input_table[[1]]
o<-fitContinuous(input_tree, data, model="OU",SE=0)
result=o$opt
OU_table=list(parameter="value",z0=result$z0,sigsq=result$sigsq,alpha=result$alpha," "=" ",lnL=result$lnL,AIC=result$aic,AICc=result$aicc)
OU_tree<-transform(input_tree, "OU", o$opt$alpha)
