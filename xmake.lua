add_rules("mode.debug", "mode.release")
set_toolchains("@llvm")

target("croc")
    set_kind("binary")
    add_files("src/*.c")
