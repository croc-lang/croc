add_rules("mode.debug", "mode.release")
add_requires("criterion")
add_requires("llvm")
set_toolchains("@llvm")

target("croc")
    set_kind("binary")
    add_files("src/**.c")
    set_warnings("all")

target("test")
    set_kind("binary")
    add_packages("criterion")
    add_files("src/**.c")
    add_files("tests/*.c")
    remove_files("src/main.c")

includes("scripts/*.lua")
