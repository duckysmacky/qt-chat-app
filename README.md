# QT Chat App

A simple chat application written in C++ using the QT framework

## About

This is a simple project featuring a client-server chat application, which supports accounts (registration/login), private chats and groups in form of text messages. It runs over pure TCP communaction and utilizes custom packets for inner communication.

This is an educational project being mainly devloped as an university assignment, but still is a passion project with effort being put it to make a functional product and a solid learning experience.

The documentation for the source code can be viewed in the [project's github pages](https://duckysmacky.github.io/qt-chat-app).

## Build

The project uses CMake as the build system. There is also a `CMakePresets.json` file provided for easier build of the project.

### Windows

On Windows the project uses MSVC as the compiler and NMake as the generator. 
Windows presets:

- `windows-msvc-debug`
- `windows-msvc-release`

Building with either of those profiles is **required** since they define the necessary settings for the project, like the `$QT_PREFIX_PATH`.

#### 1. Activate Visual Studio environment

In order to build the project on Windows, you will have to activate the Visual Studio environment by running `scripts/devshell.bat` before building.
This script sets up the environment variables required to use MSVC compiler on Windows.

```bash
.\scripts\devshell.bat
```

#### 2. Setup project with CMake

```bash
cmake --preset=<preset>
```

#### 3. Build project with CMake

Build both client and server:

```bash
cmake --build --preset=<preset>
```

Or build specific target:

```bash
cmake --build --preset=<preset> --target <target>
```

Where:
- `target`: `client` or `server`

### Linux/macOS

On Linux/macOS, the project uses GCC as the compiler and Makefile as the generator.

Linux/macOS presets:

- `linux-gcc-debug`
- `linux-gcc-release`

Building with either of those presets is **optional** since on these platforms the default QT directory is already set. But they are still here for convenience.

#### 1. Setup project

##### No preset:

```bash
cmake -S . -B build
```

##### Using a preset:

```bash
cmake --preset=<preset>
```

#### 2. Build project

##### No preset:

```bash
cmake --build build/
```

##### Using a preset:

```
cmake --build --preset=<preset>
```

##### Specific target:

```
cmake --build --preset=<preset> --target <target>
cmake --build build/ --target <target>
```

Where:
- `target`: `client` or `server`

### Cross-platform using Just

There is a `Justfile` provided for easier build of the project. If you want to use it, make sure Just is installed on your system.

- On Windows: `just` will automatically run builds through `scripts/devshell.bat` (MSVC + Qt env)
- On Linux/macOS: `just` will build the project normally

#### Build project

```bash
just build <preset> <target>
```

Where:
- `preset`: `debug` or `release`
- `target`: `client`, `server` or `all`

Or just leave empty to build everything under `debug` preset

#### Clean

```bash
just clean
```

## Run

Generated binaries will be located in the `build/` directory, under `build/release` or `build/debug` respectively.

### Client

Simply run `client_app` from the `build/release` or `build/debug` directory.

### Server binary

Server requires for a PostgreSQL database to be running. 

#### 1. Start PostgreSQL

You can start it with one of the following commands:

```bash
just db
docker compose up -d db
```

#### 2. Start Server

Simply run `server_app` from the `build/release` or `build/debug` directory.

### Server image

Server can be ran inside a Docker container. The `docker-compose.yml` file will automatically build the image and run the container.

```bash
docker compose up -d
```

Optionally you can build the image manually:

```bash
docker build -t chat-app-server:latest .
```