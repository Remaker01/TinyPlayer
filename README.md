## 简易播放器TinyPlayer
采用QT构建的音频播放器

快捷键
+ Ctrl+O：打开音乐文件
+ Ctrl+Shift+O：打开目录，自动添加并扫描目录下的合法文件
+ Alt+O：打开背景图片文件
+ Ctrl+D：删除乐曲
+ F1：打开此帮助
<hr>

1. 支持格式：~~采用QT自带的QMediaPlayer，支持格式与平台有关。~~<br>
2022.4更新：自2.0版改用VLC-qt，支持:
   1. MPEG音频格式(.MP3,.MP2,.MP1)
   2. Windows音频格式(.WAV,.WMA)
   3. 无损音频格式(.FLAC)
   4. AIFF格式(.AIFF,.AIF,.AIFC)
   5. AAC,M4A
   6. AU格式(.au,.snd)
2. 构建平台：
   1. 硬件与系统环境：Intel 第十代酷睿CPU+Windows 10 x64
   2. 软件平台：QT5.12+QT Creator 5+CMake 3.21+MinGW 8.1
3. 功能展望
   1. ~~采用更加好看的界面。~~(部分完成)
   2. ~~设置界面~~(部分完成)
   3. ~~支持更多格式~~(完成)
   4. 关联文件
   5. ~~支持播放线上音乐~~(部分完成)
4. 目前Bug
   1. 部分文件详细信息中有乱码
   2. AAC文件添加进播放列表后无法及时更新信息。
   3. 个别情况下界面会卡死，具体表现位字符及进度条按钮“重影”。**如遇此问题，请尝试点击文件->更换背景->还原默认**

欢迎提出意见与建议。

### For developers:
1. 本项目是CMake工程，如果要改成QMake请记得添加QT的Core,Widgets&VLC-qt的Core模块。
由于gitignore设置，二进制文件无法添加到仓库中，请自行准备VLC-qt：
[VLC-qt下载地址](https://vlc-qt.tano.si/#download)
2. 本项目使用Python爬虫实现网络音乐检索、下载功能，请安装Python 3环境及lxml,pyinstaller库。可使用以下命令：
```
pip install lxml
pip install pyinstaller
```
使用 
```
pyinstaller -F ./src/net_music.py(or pyinstaller ./src/net_music.spec)
pyinstaller -F ./src/down.py
```
命令生成可执行文件，并将文件放在编译后的文件同一目录下