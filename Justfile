set windows-shell := ["powershell.exe", "-NoLogo", "-NoProfile", "-Command"]

preset_prefix := if os() == "windows" { "windows-msvc" } else { "linux-gcc" }

default: build

clean:
    cmake -E rm -rf build

db:
    docker compose up -d db

build profile="debug" component="all":
    @just _validate_profile {{ profile }}
    @just _validate_component {{ component }}
    @just _build-{{ if os() == "windows" { "windows" } else { "unix" } }} {{ profile }} {{ component }}

debug component="all":
    just build debug {{ component }}

release component="all":
    just build release {{ component }}

all profile="debug":
    just build {{ profile }} all

client profile="debug":
    just build {{ profile }} client

server profile="debug":
    just build {{ profile }} server

_validate_profile profile:
    @{{ if profile == "debug" { "cmake -E true" } else if profile == "release" { "cmake -E true" } else { "cmake -E echo Invalid profile '" + profile + "'. Use debug|release. && cmake -E false" } }}

_validate_component component:
    @{{ if component == "all" { "cmake -E true" } else if component == "client" { "cmake -E true" } else if component == "server" { "cmake -E true" } else { "cmake -E echo Invalid component '" + component + "'. Use all|client|server. && cmake -E false" } }}

_build-unix profile component:
    cmake --preset {{ preset_prefix }}-{{ profile }}
    cmake --build --preset {{ preset_prefix }}-{{ profile }}{{ if component == "all" { "" } else if component == "client" { " --target client_app" } else { " --target server_app" } }}

_build-windows profile component:
    & .\scripts\devshell.bat cmake --preset {{ preset_prefix }}-{{ profile }}
    & .\scripts\devshell.bat cmake --build --preset {{ preset_prefix }}-{{ profile }}{{ if component == "all" { "" } else if component == "client" { " --target client_app" } else { " --target server_app" } }}
