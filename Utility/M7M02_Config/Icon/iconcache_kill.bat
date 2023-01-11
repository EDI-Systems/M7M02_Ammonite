% 进入图标缓存目录%
cd /d %userprofile%\AppData\Local\Microsoft\Windows\Explorer

%关闭Windows资源管理器explorer%
taskkill /f /im explorer.exe

%延时3s%
ping -n 3 127.0.0.1>nul

%删除图标缓存数据库%
attrib -h iconcache_.db
del iconcache_.db /a

%延时3s%
ping -n 3 127.0.0.1>nul

%重启Windows资源管理器explorer%
start explorer