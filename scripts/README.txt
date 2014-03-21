To create a new custom R analysis, drop your R script in this directory.
You will also need to create a corresponding .xml file to describe your
new analysis.  This xml file must have the same base filename
(the part before the file extension) as your R script.

See add.R and add.xml within the examples subdirectory for an example
on how to create your own custom analyses.

== Documentation for the XML schema used by this system ==

analysis tag attributes:
  name: This is how your analysis will be represented in the GUI.
        Make sure that each custom R analysis has a unique name.

input tag attributes:
  name:     This is the name of the R variable that represents the input of your
            script.
  type:     What type of input your script requires.  Currently, the only
            supported types of input are Table or Tree.
  metadata: Set this attribute to "false" if you don't want a table's metadata
            to be loaded as part of the input of the analysis.  Otherwise, the
            whole table will be passed to the analysis (metadata and all).

outputs tag
This is simply a container tag for the outputs of your script.  Your script
may generate one or more outputs.

output tag attributes:
  name: This is the name of an R variable that represents an output of your
        script.
  type: What type of data this output is.  Currently, the only supported types
        of output are Table or Tree.

parameters tag
Container tag for the parameters of your script.  If your script does not use
parameters, then you do not need to include this tag.

parameter tag attributes:
  name: The name of this parameter in your R script.  Note that this name will
        be replaced by the actual value selected by the user when your analysis
        runs.  This works via string replacement.  Take care to give your
        parameters unique names so that no other part of your script is
        unintentionally modified.
  type: What type of parameter this is.  Currently supported types are
        Integer, Double, String, Enum, Range, and Column.

parameter subtags:
title:       what this parameter will be called within the GUI.
description: a summary for the user as to what effect this parameter has within
             your analysis.
default:     The initial value for this parameter, before the user modifies it.
min:         minimum value for this parameter (Integer & Double types only).
max:         maximum value for this parameter (Integer & Double types only).
option:      one of the available options for this parameter (Enum type only).
table:       Only used for Column parameters.  Indicates which table to use.
