package("ktl")
    set_homepage("https://github.com/kociumba/ktl")
    set_description("ktl")
    set_license("MIT")

    set_kind("library", {headeronly = true})

    set_urls("https://github.com/kociumba/ktl.git")

    on_install(function (package)
        import("package.tools.xmake").install(package)
    end)
package_end()