from xvfbwrapper import Xvfb

# 启动 Xvfb 虚拟显示服务器
vdisplay = Xvfb()
vdisplay.start()

# 在虚拟显示上运行你的程序
import subprocess
subprocess.call(['./Backuper'])

# 程序完成后关闭虚拟显示服务器
vdisplay.stop()
