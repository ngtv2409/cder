#!/bin/bash

# require nameref, asso array (Bash 4.3+)

# --- Helper functions ---

# Ret $1: Array, Inp $2: Str
# Take a Str containing records seperated by RS
# Create an array containing records
cder-split-record() {
    local -n _records="$1"
    local -n _input="$2"

    records=()
    IFS=$'\x1E' read -r -a _records <<< "$_input"
}
# Ret $1: AssoArray, Inp $2: Str
# Take a Str containing records
# Split into key-value pairs in an AssoArray
#
# Err: 1 when #unit mod 2 != 0
cder-parse-record() {
    local -n _fields="$1"
    local -n _record="$2"

    local -a rawfield

    IFS=$'\x1F' read -r -a rawfield <<< "$_record"

    local len="${#rawfield[@]}"

    if (( len % 2 != 0 )); then
        echo "Error: Expect number of units to be multiples of 2" >&2
        return 1
    fi

    for ((i=0; i<${#rawfield[@]}; i+=2)); do
        _fields["${rawfield[i]}"]="${rawfield[i+1]}"
    done
}
# Inp $1: Array, Inp $2: AssoArray
# Check if fields in $1 exists in $2
#
# Err: 1 if any not exists
cder-exist-fields() {
    local -n _checkkeys="$1"
    local -n _fields="$2"

    for k in "${_checkkeys[@]}"; do
        if [[ ! -v _fields["$k"] ]]; then
            echo "Error: Missing mandatory error field: $k" >&2
            return 1
        fi
    done
    return 0
}
cder-handle-error() {
    local -n _fields="$1"

    if [[ ! -v _fields[CODE] ]]; then
        echo "Error: Missing mandatory field: CODE" >&2
        return 1
    fi

    if [[ "${_fields[CODE]}" != "nul" ]]; then

        if [[ "${_fields[CODE]}" == "help" ]]; then
            if [[ ! -v _fields[MESSAGE] ]]; then
                echo "Error: Missing mandatory error field: MESSAGE" >&2
                return 1
            fi
            echo "${_fields[MESSAGE]}"
            return 0
        fi

        local checkkeys=("ACTION" "MESSAGE")
        cder-exist-fields checkkeys "$1" || return $?

        echo "Error: Cder error: ${_fields[CODE]}" >&2
        echo "Error ${_fields[ACTION]}: ${_fields[MESSAGE]}" >&2
        return 1
    fi
    return 0
}
cder-fzf() {
    local -n _fzfstr="$1"

    echo -n $(\
        echo -n "$_fzfstr"\
        | awk '{\
            line = sprintf("\033[32m%s\033[0m::\033[33m%s\033[0m %s",$1,$2,$3);\
            print line}'\
        | fzf --ansi \
              --bind "j:down,k:up,ctrl-d:half-page-down,ctrl-u:half-page-up,enter:accept" \
              --preview="\
                  echo '{}'\
                  | awk '{print \$NF}'\
                  | xargs tree -C -a -L 1 --dirsfirst --noreport"\
              --disabled --no-input --layout=reverse\
              --color=bg+:-1,pointer:cyan \
              --pointer=$'\u258C\u276F'\
        | awk '{print $2}')
}

# --- Usage ---

# Inp $1: Str
# Get record and do
__cder-select-cd() {
    local -a records

    cder-get-record-array records "$1" 
    local -i entries=0
    local fzfstr=""
    # For each record, if CODE == nul, write to fzfstr
    # Otherwise if any CODE != nul, return 1
    # Fields {CODE, CAT, ALIAS, PATH}
    for rec in "${records[@]}"; do
        local -A fields
        cder-parse-record fields rec
        if [[ "$?" -ne 0 ]]; then
            echo -e "Error: Failed to parse record <$rec>" | cat -v >&2
            return 1
        fi
        # error handling
        cder-handle-error fields || continue

        local checkkeys=("CAT" "ALIAS" "PATH")
        cder-exist-fields checkkeys fields || return $?

        fzfstr+="${fields[CAT]} ${fields[ALIAS]} ${fields[PATH]}"$'\n'
        entries+=1
    done
    if (( entries == 0 )); then
        echo "Error: Not found" >&2
    elif (( entries == 1 )); then
        local path=$(echo "$fzfstr" | awk '{print $3}')
        cd "$path"
    else
        selected=$(cder-fzf fzfstr)
        if [[ $selected != "" ]]; then
            cd "$selected"
        fi
    fi
    return 0
}

cdm() {
    local out=$(command cder mark get "$@")
    __cder-select-cd out
    return $?
}

cdma() {
    local out=$(command cder mark list $@)
    __cder-select-cd out
    return $?
}

cda() {
    local out=$(command cder mark add $@)
    local -a records

    cder-get-record-array records out 
    for rec in "${records[@]}"; do
        local -A fields
        cder-parse-record fields rec
        if [[ "$?" -ne 0 ]]; then
            echo -e "Error: Failed to parse record <$rec>" | cat -v >&2
            return 1
        fi
        # error handling
        cder-handle-error fields || continue

        local checkkeys=("CAT" "ALIAS" "PATH")
        cder-exist-fields checkkeys fields || return $?

        echo "Added ${fields[CAT]}::${fields[ALIAS]}: ${fields[PATH]}"
    done
    return 0
}

cdls() {
    local out=$(command cder mark list $@)
    local -a records

    cder-get-record-array records out 
    for rec in "${records[@]}"; do
        local -A fields
        cder-parse-record fields rec
        if [[ "$?" -ne 0 ]]; then
            echo -e "Error: Failed to parse record <$rec>" | cat -v >&2
            return 1
        fi
        # error handling
        cder-handle-error fields || continue

        local checkkeys=("CAT" "ALIAS" "PATH")
        cder-exist-fields checkkeys fields || return $?

        echo "${fields[CAT]}::${fields[ALIAS]}: ${fields[PATH]}"
    done
    return 0
}

cdrm() {
    local out=$(command cder mark list $@)
    local -a records

    cder-get-record-array records out 
    for rec in "${records[@]}"; do
        local -A fields
        cder-parse-record fields rec
        if [[ "$?" -ne 0 ]]; then
            echo -e "Error: Failed to parse record <$rec>" | cat -v >&2
            return 1
        fi
        # error handling
        cder-handle-error fields || continue

        local checkkeys=("CAT" "ALIAS") || return $?
        cder-exist-fields checkkeys fields
        echo "Removed ${fields[CAT]}::${fields[ALIAS]}"
    done
    return 0
}
