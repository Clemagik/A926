## Watchdog应用

## 命令说明

```
watchdog [-t N[ms]] [-T N[ms]] [-F] DEV

    Periodically write to watchdog device DEV

    Options:

            -T N    Reboot after N seconds if not reset (default 60)
            -t N    Reset every N seconds (default 30)
            -F      Run in foreground
    Use 500ms to specify period in milliseconds
```

## 命令运行

```
watchdog -t 30 /dev/watchdog
```

执行命令后30ms重启系统。

