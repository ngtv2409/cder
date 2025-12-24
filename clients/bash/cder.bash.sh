#!/bin/bash

# require nameref, asso array (Bash 4+)

# Parse out into an array of records
cder-get-record-array() {
    local -n _records="$1"
    local -n _input="$2"

    records=()
    IFS=$'\x1E' read -r -a _records <<< "$_input"
}
# Parse a record into an asso array
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

cdm() {
    local out=$(command cder mark get $@)
    local -a records

    cder-get-record-array records out 
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
        if [[ ! -v fields[CODE] ]]; then
            echo "Error: Missing mandatory field: CODE" >&2
            return 1
        fi

        if [[ "${fields[CODE]}" != "nul" ]]; then

            if [[ "${fields[CODE]}" == "help" ]]; then
                if [[ ! -v fields[MESSAGE] ]]; then
                    echo "Error: Missing mandatory error field: MESSAGE" >&2
                    return 1
                fi
                echo "${fields[MESSAGE]}"
                return 0
            fi

            local checkkeys=("ACTION" "MESSAGE")

            for k in "${checkkeys[@]}"; do
                if [[ ! -v fields["$k"] ]]; then
                    echo "Error: Missing mandatory error field: $k" >&2
                    return 1
                fi
            done

            echo "Error: Cder error: ${fields[CODE]}" >&2
            echo "Error ${fields[ACTION]}: ${fields[MESSAGE]}" >&2
            return 1
        fi

        if [[ ! -v fields[PATH] ]]; then
            echo "Error: Missing mandatory field: CODE" >&2
            return 1
        fi
        fzfstr+="${fields[PATH]}"$'\n'
        entries+=1
    done
    if (( entries == 0 )); then
        echo "Error: Not found" >&2
    elif (( entries == 1 )); then
        cd "${fzfstr//$'\n'/}"
    else
        local selected=$(echo -n "$fzfstr" | fzf)
        if [[ $selected != "" ]]; then
            cd "$selected"
        fi
    fi
    return 0
}

cdma() {
    local out=$(command cder mark list $@)
    local -a records

    cder-get-record-array records out 
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
        if [[ ! -v fields[CODE] ]]; then
            echo "Error: Missing mandatory field: CODE" >&2
            return 1
        fi

        if [[ "${fields[CODE]}" != "nul" ]]; then

            if [[ "${fields[CODE]}" == "help" ]]; then
                if [[ ! -v fields[MESSAGE] ]]; then
                    echo "Error: Missing mandatory error field: MESSAGE" >&2
                    return 1
                fi
                echo "${fields[MESSAGE]}"
                return 0
            fi

            local checkkeys=("ACTION" "MESSAGE")

            for k in "${checkkeys[@]}"; do
                if [[ ! -v fields["$k"] ]]; then
                    echo "Error: Missing mandatory error field: $k" >&2
                    return 1
                fi
            done

            echo "Error: Cder error: ${fields[CODE]}" >&2
            echo "Error ${fields[ACTION]}: ${fields[MESSAGE]}" >&2
            return 1
        fi

        if [[ ! -v fields[PATH] ]]; then
            echo "Error: Missing mandatory field: CODE" >&2
            return 1
        fi
        fzfstr+="${fields[PATH]}"$'\n'
        entries+=1
    done
    if (( entries == 0 )); then
        echo "Error: Not found" >&2
    elif (( entries == 1 )); then
        cd "${fzfstr//$'\n'/}"
    else
        local selected=$(echo -n "$fzfstr" | fzf)
        if [[ $selected != "" ]]; then
            cd "$selected"
        fi
    fi
    return 0
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
        if [[ ! -v fields[CODE] ]]; then
            echo "Error: Missing mandatory field: CODE" >&2
            return 1
        fi

        if [[ "${fields[CODE]}" != "nul" ]]; then

            if [[ "${fields[CODE]}" == "help" ]]; then
                if [[ ! -v fields[MESSAGE] ]]; then
                    echo "Error: Missing mandatory error field: MESSAGE" >&2
                    return 1
                fi
                echo "${fields[MESSAGE]}"
                return 0
            fi

            local checkkeys=("ACTION" "MESSAGE")

            for k in "${checkkeys[@]}"; do
                if [[ ! -v fields["$k"] ]]; then
                    echo "Error: Missing mandatory error field: $k" >&2
                    return 1
                fi
            done

            echo "Error: Cder error: ${fields[CODE]}" >&2
            echo "Error ${fields[ACTION]}: ${fields[MESSAGE]}" >&2
            return 1
        fi
        local checkkeys=("CAT" "ALIAS" "PATH")

        for k in "${checkkeys[@]}"; do
            if [[ ! -v fields["$k"] ]]; then
                echo "Error: Missing mandatory error field: $k" >&2
                return 1
            fi
        done
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
        if [[ ! -v fields[CODE] ]]; then
            echo "Error: Missing mandatory field: CODE" >&2
            return 1
        fi

        if [[ "${fields[CODE]}" != "nul" ]]; then

            if [[ "${fields[CODE]}" == "help" ]]; then
                if [[ ! -v fields[MESSAGE] ]]; then
                    echo "Error: Missing mandatory error field: MESSAGE" >&2
                    return 1
                fi
                echo "${fields[MESSAGE]}"
                return 0
            fi

            local checkkeys=("ACTION" "MESSAGE")

            for k in "${checkkeys[@]}"; do
                if [[ ! -v fields["$k"] ]]; then
                    echo "Error: Missing mandatory error field: $k" >&2
                    return 1
                fi
            done

            echo "Error: Cder error: ${fields[CODE]}" >&2
            echo "Error ${fields[ACTION]}: ${fields[MESSAGE]}" >&2
            return 1
        fi
        local checkkeys=("CAT" "ALIAS" "PATH")

        for k in "${checkkeys[@]}"; do
            if [[ ! -v fields["$k"] ]]; then
                echo "Error: Missing mandatory error field: $k" >&2
                return 1
            fi
        done
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
        if [[ ! -v fields[CODE] ]]; then
            echo "Error: Missing mandatory field: CODE" >&2
            return 1
        fi

        if [[ "${fields[CODE]}" != "nul" ]]; then

            if [[ "${fields[CODE]}" == "help" ]]; then
                if [[ ! -v fields[MESSAGE] ]]; then
                    echo "Error: Missing mandatory error field: MESSAGE" >&2
                    return 1
                fi
                echo "${fields[MESSAGE]}"
                return 0
            fi

            local checkkeys=("ACTION" "MESSAGE")

            for k in "${checkkeys[@]}"; do
                if [[ ! -v fields["$k"] ]]; then
                    echo "Error: Missing mandatory error field: $k" >&2
                    return 1
                fi
            done

            echo "Error: Cder error: ${fields[CODE]}" >&2
            echo "Error ${fields[ACTION]}: ${fields[MESSAGE]}" >&2
            return 1
        fi
        local checkkeys=("CAT" "ALIAS" "PATH")

        for k in "${checkkeys[@]}"; do
            if [[ ! -v fields["$k"] ]]; then
                echo "Error: Missing mandatory error field: $k" >&2
                return 1
            fi
        done
        echo "Removed ${fields[CAT]}::${fields[ALIAS]}"
    done
    return 0
}
