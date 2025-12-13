# cder wrapper

# Grammar: STATE[ERR|SUC|ERR] [KEY val]*
# Ie: A state followed by key value pairs
# Usage: cder-out-parser <input> <cmd> <var>
# input is the string to parse 
# cmd is the variable which holds output state
# var is the variable which holds output asso array
# The kv_var only make sense if the cmd is SUC
cder-out-parser() {
    local input="$1" cmd_var="$2" kv_var="$3"

    local -n _cmd="$cmd_var"
    local -n _kv="$kv_var"

    # remove surrounding quotes if present
    strip_quotes() {
        local s="$1"
        [[ "$s" =~ ^\".*\"$ ]] && s="${s:1:-1}"
        printf '%s' "$s"
    }

    local words
    read -r -a words <<<"$input" || return 1

    _cmd="${words[0]}"
    _kv=()

    case "$_cmd" in
        HELP|ERR)
            return 0
            ;;
    esac

    (( ${#words[@]} == 1 )) && return 0

    (( (${#words[@]} - 1) % 2 == 0 )) || return 1

    for ((i=1; i<${#words[@]}; i+=2)); do
        _kv["${words[i]}"]="$(strip_quotes "${words[i+1]}")"
    done
}

# cder jump mark
cderm() {
    local out state
    declare -A kv

    out=$(command cder mark get "$@" 2>&1)

    if ! cder-out-parser "$out" state kv; then
        echo "Internal Error: Parse failed, this is not your fault. The output does not obey protocol" >&2
        return 1
    fi

    case "$state" in
        SUC)
            cd "${kv[PATH]}" || return 1
            ;;
        HELP)
            echo "$out"
            ;;
        *)
            echo "$out" >&2
            return 1
            ;;
    esac
}
