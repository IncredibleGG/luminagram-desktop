# This file is part of Telegram Desktop,
# the official desktop application for the Telegram messaging service.
#
# For license and copyright information please follow this link:
# https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL

# LuminaGram: offline whisper.cpp voice-to-text engine (Windows + Linux).
#
# Unlike lib_prisma, whisper.cpp ships its own CMake, so this drives that build
# via add_subdirectory with the cache vars forced below rather than compiling
# the sources here. init_target() is deliberately NOT called on the whisper /
# ggml targets, which keeps tdesktop's /W4 /WX (-Werror) off third-party code.
#
# Apple keeps its own on-device Apple Speech engine and never builds this, so
# the whole file is a no-op there. Pinned to whisper.cpp tag v1.7.4.

if (NOT APPLE)
    set(whisper_loc ${third_party_loc}/whisper.cpp)

    # All FORCE, so a cache left over from an earlier configure cannot silently
    # re-enable -march=native or turn examples/tests back on.
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "" FORCE)

    # CRITICAL: never optimise for the build machine's CPU. GGML_NATIVE would
    # bake -march=native into the binary and SIGILL on any older CPU the app is
    # shipped to. Pin an explicit, conservative x86-64 baseline instead.
    set(GGML_NATIVE OFF CACHE BOOL "" FORCE)
    set(GGML_AVX    ON  CACHE BOOL "" FORCE)
    set(GGML_AVX2   ON  CACHE BOOL "" FORCE)
    set(GGML_FMA    ON  CACHE BOOL "" FORCE) # ignored by MSVC (implied by AVX2)
    set(GGML_F16C   ON  CACHE BOOL "" FORCE) # ignored by MSVC (implied by AVX2)
    set(GGML_AVX512 OFF CACHE BOOL "" FORCE)

    # No OpenMP runtime dependency, and no dynamically-loaded backends: this is
    # one statically-linked CPU backend inside the Telegram binary.
    set(GGML_OPENMP     OFF CACHE BOOL "" FORCE)
    set(GGML_BACKEND_DL OFF CACHE BOOL "" FORCE)

    # No curl (we download the model ourselves through Qt), and none of the
    # standalone tooling.
    set(WHISPER_CURL           OFF CACHE BOOL "" FORCE)
    set(WHISPER_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(WHISPER_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(WHISPER_BUILD_SERVER   OFF CACHE BOOL "" FORCE)
    set(WHISPER_SDL2           OFF CACHE BOOL "" FORCE)

    # whisper.cpp v1.7.4 and its bundled ggml declare cmake_minimum_required(VERSION
    # 3.5); CMake >= 3.31 on the CI runners rejects a floor below 3.10 as an error
    # ("Compatibility with CMake < 3.10 will be removed"), failing configure. Raise
    # the effective policy floor for the vendored tree (inherited by every nested
    # cmake_minimum_required) so the pinned upstream builds unmodified.
    set(CMAKE_POLICY_VERSION_MINIMUM 3.10)

    add_subdirectory(
        ${whisper_loc}
        ${CMAKE_BINARY_DIR}/whisper.cpp
        EXCLUDE_FROM_ALL)
endif()
