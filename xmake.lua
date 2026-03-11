add_rules("mode.debug", "mode.release")
add_repositories("vendor vendor")

add_requires("imgui v1.92.6-docking", {configs = {freetype = true, glfw = true, opengl3 = true}})
add_requires("ktl 5436759", "csvparser 2.5.1", "tinyfiledialogs 3.15.1")

set_languages("cxx23")

if is_mode("release") then 
    set_policy("build.optimization.lto", true)
end

target("wt_custom")
    add_files("src/**.cpp", "src/**.cc")
    add_headerfiles("src/**.h", "src/**.hpp")
    add_packages("imgui", "csvparser", "tinyfiledialogs", "opengl", "ktl")

    if is_plat("windows") then
        add_cxxflags("/Zc:__cplusplus")
    end

    add_rules("utils.bin2obj", {extensions = ".ttf"})
    add_files("assets/Roboto-Regular.ttf")

    before_build(function (target)
        local hash = os.iorun("git rev-parse --short HEAD"):trim()
        local commits = os.iorun("git rev-list --count HEAD"):trim()

        
        local build_file = path.join(target:scriptdir(), ".build_cache")
        if not os.isfile(build_file) then
            io.writefile(build_file, "0")
        end

        local n = tonumber(io.readfile(build_file)) or 0
        n = n + 1
        io.writefile(build_file, tostring(n))

        local local_inc = n
        local full_version = string.format("r%s.%s+%s", commits, hash, local_inc)

        target:add("defines", "BUILD_IDENTIFIER=\"" .. full_version .. "\"")
    end)
