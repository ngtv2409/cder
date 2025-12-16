# Specification of Cder output

Specifies how the shell can interact with cder via a simple IPC protocol.

# Message structure

A message consists of a **TYPE** and its corresponding following 
syntax, read below.

There is imporant note is that everything, *TYPE*, *KEY*, *VAL* 
are seperated by NUL(\0). In other words, the protocol is just a 
collection of words seperated by NUL.

```
OUT\0KEY1\0VAL1\0KEY2\0VAL2
```

Nothing, beside the message field of `MSG` shall contains newline.
Newline character is used to seperate multiple output, for example,
cder may return multiple messages. Newline is `\n` only.
```
OUT\0KEY\0VAL # this is message 1
OUT\0KEY\0VAL # this is message 2
```
The parse step is read each line and do the steps specfied below.

## TYPE(OUT|MSG)

```
OUT <values>
MSG <message>
```

The first word of the output is either OUT or MSG, this field determines 
the type of the message. `OUT` stands before a normal protocol output, 
where it directly followed by values that the shell wrapper can read.
Whereas `MSG` is a special type used to allow human output even when
`-l` has not been specified. This is useful for help messages in aliases.
It allow users to use `--help` in contexts where protocol output is expected.

Multiple `OUT`s can be returned seperated by newline. `MSG` always comes alone.

Upcoming TYPEs may include: `IBEGIN`, `IEND` for interactive mode, which allow
prompts to not prevent the shell from reading protocol output.

### How to handle

- `OUT`: When received, the shell should parse the following words 
   with the rules defined in **Values**.

- `MSG`: When received, the shell should output the following message as is

## Values

An `OUT` type return values in the following manner:
```
[KEY "VALUE"]*
```
That is, a `KEY` followed by a quoted `VALUE`. For instance, a call to 
`mark get` may get something like:
```
OUT\0CODE\0success\0PATH\0path/to/dir\0CAT\0category_found_from\0
```
Where `OUT` let you know that it is followed by values, CODE, PATH, CAT are keys
and "path/to/dir", "category_found_from" are values.

Errors are also returned as an `OUT` type.
```
OUT\0CODE\0"not_found"\0MESSAGE\0"No\0matching\0mark\0${query}\0found\0in\0${category}"
```

## Codes

Todo: standardize codes
