Import("env")

board = env.BoardConfig()

reserved_profile_space = int(board.get("reserved_profile_space"))

env.Replace(AS="$CC", ASCOM="$ASPPCOM")

env.Append(
    ASFLAGS=["-x", "assembler-with-cpp"],

    CCFLAGS=[
        "-Os",
        "-ffunction-sections",
        "-fdata-sections",
        "--gc-sections",
        "-Wall",
        "-fno-common",
        "-mthumb",
        "-nostdlib",
        "-fno-exceptions",
        "-fno-rtti",
        "USE_HAL_DRIVER"
    ],

    LINKFLAGS=[
        "-mthumb",
        "--specs=nano.specs",
        "--specs=nosys.specs",
        "-Wl,--defsym=LD_RESERVED_PROFILE_SPACE=%d"  % reserved_profile_space
    ],

    LIBS=["c", "gcc", "m", "stdc++", "nosys"],
)

# copy CCFLAGS to ASFLAGS (-x assembler-with-cpp mode)
env.Append(ASFLAGS=env.get("CCFLAGS", [])[:])

flash_size = board.get("upload.maximum_size"); 
board.update("upload.maximum_size", flash_size - reserved_profile_space)
