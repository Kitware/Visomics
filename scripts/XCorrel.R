# get size of input data
rows <- 1:length(input_table[[1]])
cols <- 1:length(input_table)

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
df_stripped <- df[numeric_rows,]
df_stripped <- df_stripped[sapply(df_stripped, function(x) all(!is.na(x)))]

# run cross correlation to generate results
methodName <- "correlation_method"
correl<-cor(df_stripped, method=methodName)
