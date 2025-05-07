add_rules("mode.debug", "mode.release")

target("c_scheme")
    set_kind("binary")
    add_files("src/*.c")
    add_cflags("-Wno-parentheses-equality")