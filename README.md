# Game of life

Execute build_app.sh to build application normally.
The built default executable can be found from build.
Run the executable with --help to get more info.

In order to compile to webassembly:
Install emscripten (https://emscripten.org/#).
Activate PATH by adding 'source ./emsdk_env.sh' to .bashrc.
Execute build_wasm.sh.

Running the generated webassembly can be done using emscript server environment with command 'emrun index.html' from the build_wasm directory.


## Playing the game

Press mouse Left Click to make dead cell alive.
Press mouse Right Click to make alive cell dead.

Press P to unpause/pause the Game of life.

Press right arrow key to speed up the game.
Press left arrow key to slow down the game.