# Specification of Cder output

Specifies how the shell can interact with cder

There are a few scenarios:

- Success: cder will echo SUC and followed by the fields 
  depending on the specific commands. Field names are all uppercase
  For example: 
  `cder mark get myproj`
  `SUC PATH path/to/myproject`
- Error: cder will echo ERR as the first word.
  For example: 
  `ERR Error: error message`
- Help: cder will echo HELP before the help message, this is necessary because 
  help has 0 error code and may be mistaken as a SUC call.
