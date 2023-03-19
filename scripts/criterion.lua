package("criterion")
    set_homepage("https://github.com/Snaipe/Criterion")
    set_description("A cross-platform C and C++ unit testing framework for the 21st century")
    set_license("MIT")

    add_urls("https://github.com/Snaipe/Criterion/releases/download/v$(version)/criterion-$(version).tar.xz", { version = function (version) return version:sub(2) end })
    add_urls("https://github.com/Snaipe/Criterion.git")
    add_versions("v2.4.1", "d0f86a8fc868e2c7b83894ad058313023176d406501a4ee8863e5357e31a80e7")

    add_deps("meson", "ninja")

    on_install(function (package)
        import("package.tools.meson").install(package)
    end)

    on_test(function (package)
        assert(package:check_csnippets({test = [[
            Test(misc, passing) {
                cr_assert(1);
            }
        ]]}, {configs = {languages = "c11"}, includes = "criterion/criterion.h"}))
    end)
