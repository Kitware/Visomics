# convert input to data frame format
df <- data.frame(input_table)

# strip out any non-numerical elements
df_stripped <- df[sapply(df, is.numeric)]

# run cross correlation to generate results
methodName <- "correlation_method"
correl<-cor(df_stripped, method=methodName)
