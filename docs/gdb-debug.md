gdbでのデバッグ方法

## 実行手順
以下の手順を実行する。

1. make run-debug
2. 別のターミナルから aarch64-none-elf-gdb を実行
3. symbol-file kernel.elf を実行して, シンボルをロード
4. b _start を実行してエントリアドレスでブレークするよう指定
5. target remote localhost:1234 を実行してQEMUに接続
6. continueを実行して開始

## 実行例

```shell-session
$ aarch64-none-elf-gdb
GNU gdb (GDB) 7.12.50.20160823-git
Copyright (C) 2016 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "--host=x86_64-pc-linux-gnu --target=aarch64-none-elf".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word".
(gdb) symbol-file kernel.elf
Reading symbols from kernel.elf...done.
(gdb) b _start
Breakpoint 1 at 0x80000: file start.S, line 19.
(gdb) target remote localhost:1234
Remote debugging using localhost:1234
warning: No executable has been specified and target does not support
determining executable automatically.  Try using the "file" command.
0x0000000000000000 in ?? ()

```
