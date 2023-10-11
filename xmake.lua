set_policy("build.warning", true)
add_rules("mode.debug", "mode.release")
add_requires("criterion")
add_requires("llvm")
set_toolchains("@llvm")

if is_mode("debug") then
    add_defines("DEBUG")
end

target("croc")
    set_kind("binary")
    add_files("src/**.c")
    set_warnings("all")
    add_includedirs("includes")
    add_cflags("-Werror=incompatible-pointer-types")

target("test")
    set_kind("binary")
    add_packages("criterion")
    add_files("src/**.c")
    add_files("tests/*.c")
    add_includedirs("includes")
    remove_files("src/main.c")

includes("scripts/*.lua")
