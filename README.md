# Learning emscripten

Execute build_app.sh to build application normally.
The built default executable can be found from build.

In order to compile to webassembly:
Install emscripten (https://emscripten.org/#).
Activate PATH by adding 'source ./emsdk_env.sh' to .bashrc.

Running the generated webassembly can be done using emscript server environment with command 'emrun index.html' in the build_wasm directory.