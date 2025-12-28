# Specification of Cder output

Version 1.0.1

Specifies how the shell can interact with cder via a simple IPC protocol.

# Definitions

- US: The ascii unit seperator
- RS: The ascii record seperator
- Field: Key-Value pair

# Message structure

The message is sent as a record containing a token stream, 
where each tokens end with a US and each records end with a RS.
Every 2 tokens is a field.

```
# Note: the _ are logical seperator and do not present in the actual message
KEY1_US_VAL1_US_KEY2_US_VAL2_US_RS
```

This record is parsed conceptually as this json:
```json
{
    "KEY1": "VAL1",
    "KEY2": "VAL2"
}
```

Multiple records may be send at once, the RS is what used to seperate them.

Records always contain a field keyed `CODE`, it is analogous to return code but textual.
See [Codes](#codes) for the values of `CODE`.

For info on fields contained in the respond of each commands, see [API](API.md).

The following syntax is used to easier express output in documentations:
```
KEY1="VAL1" KEY2="VAL2" ...
```

## Codes

This section contains the codes used in cder.
Codes do not always related to errors.

### NoError (nul)
The default code, signates successful result.

### Help (help)
Signal used to distinguish 2 kind of 0 return codes,
thus allows shells to not mistakenly process `--help` output.

### ConfigError (config)
Error caused by missing configuration. E.g: Missing `CDER_DB_PATH`.

### FSError (fs)
Error caused by file operations.

### JSONError (json)
Error caused by json.

### CLIError (cli)
Error caused by CLI.
