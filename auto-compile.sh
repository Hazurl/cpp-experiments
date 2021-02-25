# Recompile when any source file is modified

compile_program() {
    echo -e \\n\\033[34m$(head -c $(tput cols) < /dev/zero | tr '\0', '=')\\033[0m
    date +"[%T] Compilation...%n"

    meson compile
}

cd build
compile_program

inotifywait -m -r -e close_write,moved_to,create --format '%w%f' "${MONITORDIR}" ../source ../include ../meson.build | 
while read events
do 
    compile_program
done
