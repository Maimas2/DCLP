git clone https://gitlab.freedesktop.org/freetype/freetype.git
mkdir freetype-bin

git clone https://github.com/ocornut/imgui.git
cd imgui
git submodule init
cd ..

git clone https://github.com/glfw/glfw.git
cd glfw
git submodule init
cd ..

git clone https://github.com/dacap/clip.git
cd clip
git submodule init
cd ..

bash build-freetype.sh
bash re-build.sh
