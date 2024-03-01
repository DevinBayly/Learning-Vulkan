# intro
This project exists to help learn the complex vulkan graphics spec
# installation instructions

go get a version of the vulkan sdk
```
mkdir vulkan_versions
cd !$
wget https://sdk.lunarg.com/sdk/download/1.2.198.1/linux/vulkansdk-linux-x86_64-1.2.198.1.tar.gz
tar xf *.tar.gz
source 1.2.198.1/setup-env.sh
```
ensure that scons is installed
```
python -m pip install scons
```

make sure glfw3 is around
```
sudo apt update
sudo apt install libglfw3 libglfw3-dev
```

