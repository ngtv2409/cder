Specification of fields included in responds.

See [Protocol Specification](PROTOCOL.md) for an idea on how to read these output.

# General

## Error
Errors always contain the fields **ACTION, MESSAGE**.
- Action: used to provide more infomation on what cder is trying to do.
- Message: The error message.

E.g (does not reflect actual message):
```
CODE=fs ACTION="validating" MESSAGE="No such file or directory"
```

## Info
Info flags such as `--version`, `--help`, `--db-path`, etc. all have simple output.
Containing only **MESSAGE**.

E.g:
```
CODE=help MESSAGE="cder version x.y.z"
```

# Command

## mark 
```
mark [-c --categories <list of categories in quote>, -a --all-categories] <command>
```
Specify the categories to operate with the `--categories` option.
Use all categories with `--all-categories`

The `mark` command containing the following sub: **add, rm, get, list**.

### add 
```
add <alias> <path>
```
Add `path` to the categories under the name `alias`.

On success calls, contains multiple records
containing **CAT, ALIAS, PATH**.

Potential errors:
- cli: empty category list
- fs: path does not exists

### get 
```
get <alias>
```
Search for an alias in the categories.

On success calls, contains multiple records
containing **CAT, ALIAS, PATH**.

Potential errors:
- cli: empty category list
- notfound: alias not exists
- json: validation (rare)

### list 
```
list
```
Get all aliases from the categories.
May output nothing if no aliases exist.

On success calls, contains multiple records
containing **CAT, ALIAS, PATH**.

Potential errors:
- cli: empty category list
- json: validation (rare)

### rm
```
rm <alias>
```
remove alias from category.

On success calls, contains multiple records
containing **CAT, ALIAS**.

Potential errors:
- cli: empty category list 
- json: validation (rare)

