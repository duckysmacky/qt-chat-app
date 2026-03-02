# QT Chat App

Это клиент-серверное приложение, написанное на C++20 с использованием фреймворка Qt6.

Это учебный университетский проект.

## Сборка

Проект использует CMake в качестве системы сборки. Также для удобства предусмотрен файл `CMakePresets.json`.

### Windows

На Windows проект использует MSVC как компилятор и NMake как генератор.  
Пресеты для Windows:

- `windows-msvc-debug`
- `windows-msvc-release`

Сборка с одним из этих профилей **обязательна**, так как они задают необходимые параметры проекта, например `$QT_PREFIX_PATH`.

#### 1. Активируйте окружение Visual Studio

Чтобы собрать проект на Windows, перед сборкой нужно активировать окружение Visual Studio, запустив `scripts/devshell.bat`.
Этот скрипт настраивает переменные окружения, необходимые для использования компилятора MSVC на Windows.

```bash
.\scripts\devshell.bat
```

#### 2. Сконфигурируйте проект с помощью CMake

```bash
cmake --preset=<preset>
```

#### 3. Соберите проект с помощью CMake

Собрать и клиент, и сервер:

```bash
cmake --build --preset=<preset>
```

Или собрать конкретную цель:

```bash
cmake --build --preset=<preset> --target <target>
```

Где:
- `target`: `client` или `server`

### Linux/macOS

На Linux/macOS проект использует GCC как компилятор и Makefile как генератор.

Пресеты Linux/macOS:

- `linux-gcc-debug`
- `linux-gcc-release`

Сборка с этими пресетами **не обязательна**, так как на этих платформах директория Qt по умолчанию уже задана. Но они оставлены для удобства.

#### 1. Конфигурация проекта

##### Без пресета:

```bash
cmake -S . -B build
```

##### С пресетом:

```bash
cmake --preset=<preset>
```

#### 2. Сборка проекта

##### Без пресета:

```bash
cmake --build build/
```

##### С пресетом:

```bash
cmake --build --preset=<preset>
```

##### Конкретная цель:

```bash
cmake --build --preset=<preset> --target <target>
cmake --build build/ --target <target>
```

Где:
- `target`: `client` или `server`

### Кроссплатформенно через Just

Для удобной сборки в проекте есть `Justfile`. Если хотите использовать его, убедитесь, что `just` установлен в системе.

- На Windows: `just` автоматически запускает сборку через `scripts/devshell.bat` (окружение MSVC + Qt)
- На Linux/macOS: `just` собирает проект обычным образом

#### Сборка проекта

```bash
just build <preset> <target>
```

Где:
- `preset`: `debug` или `release`
- `target`: `client`, `server` или `all`

Либо можно оставить параметры пустыми, чтобы собрать всё в профиле `debug`.

#### Очистка

```bash
just clean
```

## Запуск

Собранные бинарные файлы находятся в директории `build/`.

### Клиент

Просто запустите `client_app` из директории `build/`.

### Сервер (бинарник)

Для сервера требуется запущенная база данных PostgreSQL.

#### 1. Запустите PostgreSQL

Можно запустить одной из команд:

```bash
just db
docker compose up -d db
```

#### 2. Запустите сервер

Просто запустите `server_app` из директории `build/`.

### Сервер (Docker image)

Сервер можно запускать внутри Docker-контейнера. Файл `docker-compose.yml` автоматически соберёт образ и запустит контейнер.

```bash
docker compose up -d
```

При необходимости образ можно собрать вручную:

```bash
docker build -t chat-app-server:latest .
```
