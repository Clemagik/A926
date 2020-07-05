# Audio模块

### alsa 库

```
通过配置buildroot的menuconfig文件， 选择对应的alsa库，
```

#### &gt; alsa 工具使用

```
amixer, aplay, arecord 的使用方法如下：
假设播放的音频文件为 play.wav，录音生成的文件为 record.wav，在开发板上按照以下操作。
(1) 播放音频文件
    $ aplay play.wav

(2) 通过 amic 录音
    $ arecord -D hw:0,0 -c 2 -f S16_LE -r 44100 -d 10 record.wav

    单独使用该命令可以在当前目录下生成 10 秒的录音文件 record.wav。一般在执行 arecord
命令之前，会通过 amixer 设置录音的通道和参数，命令如下：
    $ amixer cset numid=1,iface=MIXER,name='Master Playback Volume' 15
    $ amixer cset numid=2,iface=MIXER,name='Mic Volume' 3

(3) 通过 dmic 录音
    通过 dmic 录 8K 采样率四通道音频数据：
    $ arecord -D hw:0,1 -c 4 -f S16_LE -r 8000 -d 10 record.wav

(4)audio 测试工具说明：
    1.arecord 录音
        -D 参数用于指定音频设备PCM
        hw 的第一个参数用来指定声卡号，第二个参数用于指定设备号
        -c 用于指定声道数
        -f 用于指定数据格式
        -r 用于指定采样频率
        -d 用于指定录音时间
        --help 获取帮助
    2.aplay 放音
        参数设置与 arecord 一致。
```



