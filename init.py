import os

oldwd = os.getcwd()

os.system("git submodule update --init --depth=1")

os.chdir('./WebgpuLib/vendor/dawn/ext/dawn/third_party/')
os.system('git submodule init protobuf')
os.system('git submodule update protobuf')

os.chdir(oldwd + '/WebgpuLib/vendor/emsdk/ext/')
os.system('emsdk.bat install latest')
os.system('emsdk.bat activate latest')
