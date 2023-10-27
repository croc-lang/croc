set_policy("build.warning", true)
add_rules("mode.debug", "mode.release")
add_requires("criterion")
add_requires("llvm")
set_toolchains("@llvm")

if is_mode("debug") then
    add_defines("DEBUG")
end

add_includedirs("includes")
set_warnings("all")
add_cflags("-Werror=incompatible-pointer-types")
add_cflags("-Wno-use-after-free")

target("croc")
    set_kind("binary")
    add_files("src/**.c")

target("test")
    set_kind("binary")
    add_packages("criterion")
    add_files("src/**.c")
    add_files("tests/*.c")
    remove_files("src/main.c")

includes("scripts/*.lua")
