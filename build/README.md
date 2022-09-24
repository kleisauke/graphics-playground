# Build notes

## Emscripten

### Docker (preferred method)

```bash
$ docker run --rm -v $(pwd):/src emscripten/emsdk:3.1.22 ./build.sh
$ docker run -p 8080:80 --rm -v $(pwd):/src emscripten/emsdk:3.1.22 emrun --port 80 --no_browser /src
# Visit http://localhost:8080/dist/playground.html
```

### Locally (on Linux)

Setup emsdk with:

```bash
# Get the emsdk repo
git clone https://github.com/emscripten-core/emsdk.git

# Enter that directory
cd emsdk

# Fetch the latest version of the emsdk (not needed the first time you clone)
git pull

# Download and install the "tip-of-tree" build (i.e. the very latest binaries)
./emsdk install tot

# Make the "tip-of-tree" build active for the current user (writes .emscripten file)
./emsdk activate tot

# Prefer the default system-installed version of Node.js
NODE=$(which node)
sed -i'.old' "/^NODE_JS/s/= .*/= '${NODE//\//\\/}'/" .emscripten

# Activate PATH and other environment variables in the current terminal
source ./emsdk_env.sh
```

Then build and run with:

```bash
./build.sh
emrun --port 8080 --no_browser .
# Visit http://localhost:8080/dist/playground.html
```

## Windows (with MSVC)

Download https://www.libsdl.org/release/SDL2-devel-2.24.0-VC.zip and unzip to `C:/SDL2-2.24.0`.

Then add this directory to the `CMAKE_PREFIX_PATH`, for example:

```powershell
-DCMAKE_PREFIX_PATH="C:/SDL2-2.24.0"
```
