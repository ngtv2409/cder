# cder wrapper

# cder jump mark
cderm ()
{
    out=$(command cder mark get $@)
    if [ $? -eq 0 ]; then
        cd $out
    fi
}
