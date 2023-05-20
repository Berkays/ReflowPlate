import os

Import("env")

board = env.BoardConfig()

profile_flash_size = int(board.get("reserved_profile_space"))


BOARD = env["BOARD"].upper()
BOARD_FAMILY = BOARD[:7]

PROJECT_DIR = env["PROJECT_DIR"]
SRC_DIR = "src"
LIB_DIR = "lib"
CONFIG_DIR = "include"

def join(x, *args):
    return x + "".join(["/" + x for x in args])

BOARD_SRC_DIR = join(SRC_DIR, BOARD)
BOARD_DRIVER_DIR = join("CubeMx", BOARD)

RELATIVE_SRC_DIR = os.path.relpath(SRC_DIR)

SHARED_SRC_DIR = join("src", "shared")
SCREENS_SRC_DIR = join("src", "screens")
MODULES_SRC_DIR = join("src", "modules")

build_flags = [
    join(BOARD_DRIVER_DIR, "Core", "Inc"),
    join(BOARD_DRIVER_DIR, "Drivers", "CMSIS", "Include"),
    join(BOARD_DRIVER_DIR, "Drivers", "CMSIS", "Device", "ST", f'{BOARD_FAMILY}xx', "Include"),
    join(BOARD_DRIVER_DIR, "Drivers", f'{BOARD_FAMILY}xx_HAL_Driver', "Inc"),
    join(BOARD_DRIVER_DIR, "Drivers", f'{BOARD_FAMILY}xx_HAL_Driver', "Inc", "Legacy"),
    CONFIG_DIR,
    SHARED_SRC_DIR,
    SCREENS_SRC_DIR,
    BOARD_SRC_DIR
]

src_filter = [
    join(BOARD_DRIVER_DIR, "Drivers", f'{BOARD_FAMILY}xx_HAL_Driver', "Src"),
    join(BOARD_DRIVER_DIR, "Core", "Src"),
    SHARED_SRC_DIR,
    SCREENS_SRC_DIR,
    BOARD_SRC_DIR
]

lib_dirs = [x.name for x in os.scandir(join(os.getcwd(), LIB_DIR)) if "DS_Store" not in x.name]
module_dirs = [x.name for x in os.scandir(join(os.getcwd(), MODULES_SRC_DIR)) if "DS_Store" not in x.name]

build_flags = build_flags + [join(LIB_DIR, x) for x in lib_dirs] + [join(MODULES_SRC_DIR, x) for x in module_dirs]
src_filter = src_filter + [join(LIB_DIR, x) for x in lib_dirs] + [join(MODULES_SRC_DIR, x) for x in module_dirs]

src_filter = ["+<{0}>".format(join("..", x)) for x in src_filter]
build_flags = ["-I./" + x for x in build_flags]

env.Append(
    SRC_FILTER=src_filter,
    BUILD_FLAGS=build_flags
)

env.Replace(
    LDSCRIPT_PATH=join(".", BOARD_DRIVER_DIR, f'{BOARD}TX_FLASH.ld')
)

product_line = board.get("build.product_line")

env.Append(
    BUILD_FLAGS=[f'-D{product_line}',
    "-D'PROFILE_FLASH_SIZE={0}'".format(hex(profile_flash_size))
]
)