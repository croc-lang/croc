add_rules("mode.debug", "mode.release")
add_requires("llvm")
set_toolchains("@llvm")

target("croc")
    set_kind("binary")
    add_files("src/*.c")
