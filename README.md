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
   5. AAC
2. 构建平台：
   1. 硬件与系统环境：Intel 第十代酷睿CPU+Windows 10 x64
   2. 软件平台：QT5.12+QT Creator 5+CMake 3.21+MinGW 8.1
3. 功能展望
   1. ~~采用更加好看的界面。~~(部分完成)
   2. 设置界面
   3. ~~支持更多格式~~(完成)
   4. 关联文件
   5. ~~增加播放按钮~~(完成)
   6. 支持播放线上音乐
   7. ~~自动加载上次退出时的播放列表~~(完成)
4. 目前Bug
   1. 部分文件详细信息中有乱码
   2. 某些文件(主要是MP3)在少数情况下点击进度条会从头开始
   3. AAC文件添加进播放列表后无法及时更新信息。
   4. 个别情况下界面会卡死，具体表现位字符及进度条按钮“重影”。**如遇此问题，请尝试点击文件->更换背景->还原默认**
   

欢迎提出意见与建议。

### For developers:
本项目是CMake工程，如果要改成QMake请记得添加QT的Core,Widgets&VLC-qt的Core模块。
由于gitignore设置，二进制文件无法添加到仓库中，请自行准备VLC-qt：
VLC-qt下载地址[(Windows-mingw)](https://github.com/vlc-qt/vlc-qt/releases/download/1.1.0/VLC-Qt_1.1.0_win32_mingw.7z)
[Windows-MSVC]()