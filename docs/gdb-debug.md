# gdbでのデバッグ方法

## 本文書について

本文書では, GNU Debugger(GDB)とQEmuのシステムシミュレータを用いたリモー
トデバッグ手順について説明します。

## `${HOME}/.gdbinit`ファイルの準備

まず, `${HOME}/.gdbinit`ファイルにデバッグ支援用のマクロを追記します。

`tools/gdb/_gdbinit`にデバッグ作業を手助けするための`.gdbinit`(GDB用のマクロ)が入っています。
本ファイルの内容を`${HOME}/.gdbinit`に追記してから, `gdb`を起動することで
以下のコマンドマクロが使えるようになります。 

* `aarch64_target` --- AArch64ターゲット(aarch64)向けの環境を設定します。
* `x64_target` --- x64ターゲット(i386:x86-64:intel)向けの環境を設定します。
* `remote_target` --- QEMUのリモートターゲット(localhostのポート:1234番)
  に接続します。
* `load_symbol` --- デバッグシンボルを読み込みます。
* `six` -- 1命令ステップし, 次に実行される命令($pcの指し示す命令)を表示
します。
* `set_aarch64_vectors` --- AArch64版の割込み/例外ベクタにブレークポイン
  トを設定します。
* `set_x64_vectors`  --- x64版の割込み/例外エントリにブレークポイントを設定します。
* `aarch64_setup` --- aarch64版のデバッグを行うための典型的な前処理を行います.
* `x64_setup` --- x64版のデバッグを行うための典型的な前処理を行います.

`aarch64_setup`, `x64_setup`は, ターゲットの設定, リモートターゲットへ
の接続, デバッグシンボルの読込み, 割込み/例外エントリにブレークポイン
ト設定までの一連の処理をそれぞれのターゲット向けに実行します。

## 実行手順

GDBとQEmuを用いたデバッグの手順を以下に示します:

1. `make run-debug`を実行します。
1. 別のターミナルから `gdb`を実行します。
   * AArch64版の場合 --- `aarch64-none-elf-gdb` を実行してください。
   * x64版の場合 --- `x64-elf-gdb`(以下の「x64版のgdbのコンパイル手順」
     参照)を実行してください。 
1. `gdb`のプロンプトから`x64_target`または`aarch64_target`を実行し, ターゲットを選択します。
1. `gdb`のプロンプトから`load_symbol`を実行してシンボル情報を読み込みます。
1. `gdb`のプロンプトから`remote_target`を実行してQEmuに接続します。
1. `gdb`のプロンプトから`set_aarch64_vectors`または`set_x64_vectors` を実行し, 例外/割込み
   エントリ処理にブレークポイントを設定します(任意)。
1. `gdb`のプロンプトからエントリアドレスでブレークするよう指定します(任意)。
   * AArch64版の場合 ---  カーネルエントリアドレスである`_start`にブレー
     クポイントを設定するために, `b _start`を実行します。
   * x64版の場合 ---  64bitモード(long モード)に移行後のアドレスにブレー
     クポイントを設定するためにC言語のエントリ関数である`boot_main`にブレー
     クポイントを設定します。このために, `b boot_main`を実行します。
1. `gdb`のプロンプトからデバッグの目的に応じて他のブレークポイントなどを設定します。
1. `gdb`のプロンプトから`six`または`continue`を実行してデバッグを開始します。
   * AArch64版の場合 --- QEmuは, カーネルの開始アドレス(`_start`)で停止してい
     るので, `six` を実行してデバッグを開始します。
   * x64版の場合 --- QEmuは, カーネル読み込み前のBIOS内で停止している
     ので, `continue` を実行してデバッグを開始します。

    

## 実行例
AArch64, x64のいずれのカーネルをデバッグする場合でも以下のコマンドを実
行して, QEmuをgdbからの接続待機状態にします。

```shell-session
$ make run-debug
```

別の端末から以下に示す手順でgdbを実行しQEmuに接続します。

### AArch64版のカーネルをデバッグする際の手順の例
AArch64版のカーネルをデバッグする際の手順の例を以下に示します:

```shell-session
$ aarch64-none-elf-gdb
GNU gdb (GDB) 7.12.50.20160823-git
Copyright (C) 2016 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later
<http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show
copying"
and "show warranty" for details.
This GDB was configured as "--host=x86_64-pc-linux-gnu
--target=aarch64-none-elf".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word".
(gdb) aarch64_target
The target architecture is assumed to be aarch64
(gdb) load_symbol
(gdb) remote_target
start () at boot.S:26
26              msr   daifset, #(AARCH64_DAIF_FIQ | AARCH64_DAIF_IRQ)
/* Disable FIQ and IRQ */
(gdb) set_aarch64_vectors
Breakpoint 1 at 0x4000f784: file vector.S, line 174.
Breakpoint 2 at 0x4000f848: file vector.S, line 182.
Breakpoint 3 at 0x4000f90c: file vector.S, line 190.
Breakpoint 4 at 0x4000f9d0: file vector.S, line 198.
Breakpoint 5 at 0x4000fa94: file vector.S, line 206.
Breakpoint 6 at 0x4000fb54: file vector.S, line 213.
Breakpoint 7 at 0x4000fc14: file vector.S, line 220.
Breakpoint 8 at 0x4000fcd4: file vector.S, line 228.
Breakpoint 9 at 0x4000fd94: file vector.S, line 235.
Breakpoint 10 at 0x4000fe58: file vector.S, line 243.
Breakpoint 11 at 0x4000ff1c: file vector.S, line 251.
Breakpoint 12 at 0x4000ffe0: file vector.S, line 259.
Breakpoint 13 at 0x400100a4: file vector.S, line 268.
Breakpoint 14 at 0x40010168: file vector.S, line 276.
Breakpoint 15 at 0x4001022c: file vector.S, line 284.
Breakpoint 16 at 0x400102f0: file vector.S, line 292.
(gdb) b _start
Breakpoint 17 at 0x40000000: file boot.S, line 26.
(gdb) six
28              mrs   x0, CurrentEL
=> 0x40000004 <start+4>:        mrs     x0, currentel
(gdb)
```
### x64版のカーネルをデバッグする際の手順の例

x64版のカーネルをデバッグする際の手順の例を以下に示します:

```shell-session
$ x64-elf-gdb
GNU gdb (GDB) 8.2
Copyright (C) 2018 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later
<http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "--host=x86_64-pc-linux-gnu
--target=x86_64-elf".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word".
(gdb) x64_target
The target architecture is assumed to be i386:x86-64:intel
(gdb) load_symbol
(gdb) remote_target
0x0000000000000000 in ?? ()
(gdb) set_x64_vectors
Breakpoint 67 at 0xffff80000011a760: file vector.S, line 14.
Breakpoint 68 at 0xffff80000011a769: file vector.S, line 21.
Breakpoint 69 at 0xffff80000011a772: file vector.S, line 28.
Breakpoint 70 at 0xffff80000011a77b: file vector.S, line 35.
Breakpoint 71 at 0xffff80000011a784: file vector.S, line 42.
Breakpoint 72 at 0xffff80000011a78d: file vector.S, line 49.
Breakpoint 73 at 0xffff80000011a796: file vector.S, line 56.
Breakpoint 74 at 0xffff80000011a79f: file vector.S, line 63.
Breakpoint 75 at 0xffff80000011a7a8: file vector.S, line 70.
Breakpoint 76 at 0xffff80000011a7af: file vector.S, line 76.
Breakpoint 77 at 0xffff80000011a7b8: file vector.S, line 83.
Breakpoint 78 at 0xffff80000011a7bf: file vector.S, line 89.
Breakpoint 79 at 0xffff80000011a7c6: file vector.S, line 95.
Breakpoint 80 at 0xffff80000011a7cd: file vector.S, line 101.
Breakpoint 81 at 0xffff80000011a7d4: file vector.S, line 107.
Breakpoint 82 at 0xffff80000011a7db: file vector.S, line 113.
Breakpoint 83 at 0xffff80000011a7e4: file vector.S, line 120.
Breakpoint 84 at 0xffff80000011a7ed: file vector.S, line 127.
Breakpoint 85 at 0xffff80000011a7f4: file vector.S, line 133.
Breakpoint 86 at 0xffff80000011a7fd: file vector.S, line 140.
Breakpoint 87 at 0xffff80000011a806: file vector.S, line 147.
Breakpoint 88 at 0xffff80000011a80f: file vector.S, line 154.
Breakpoint 89 at 0xffff80000011a818: file vector.S, line 161.
Breakpoint 90 at 0xffff80000011a821: file vector.S, line 168.
Breakpoint 91 at 0xffff80000011a82a: file vector.S, line 175.
Breakpoint 92 at 0xffff80000011a833: file vector.S, line 182.
--Type <RET> for more, q to quit, c to continue without paging--
Breakpoint 93 at 0xffff80000011a83c: file vector.S, line 189.
Breakpoint 94 at 0xffff80000011a845: file vector.S, line 196.
Breakpoint 95 at 0xffff80000011a84e: file vector.S, line 203.
Breakpoint 96 at 0xffff80000011a857: file vector.S, line 210.
Breakpoint 97 at 0xffff80000011a860: file vector.S, line 217.
Breakpoint 98 at 0xffff80000011a869: file vector.S, line 224.
Breakpoint 99 at 0xffff80000011b112: file vector.S, line 1687.
(gdb) b boot_main
Breakpoint 100 at 0xffff800000118120: file boot64.c, line 55.
(gdb) continue
Continuing.

Breakpoint 100, boot_main (magic=920085129,
mbaddr=18446603336222360616) at boot64.c:55
55          uint64_t __attribute__ ((unused)) mbaddr) {
```

## x64版のgdbのコンパイル手順
GDBのリモートデバッグ機能とQEmuを接続するためには,
`tools/gdb/gdb-8.2-qemu-x86-64.patch`をGDBに適用してGDBを
構築し直す必要があります。

QEmuのリモートデバッグ機能に対応したGDBの構築方法を以下に示します。

1. `http://ftp.gnu.org/gnu/gdb/gdb-8.2.tar.gz` からGDBのソースアーカイブを取得します。
1. `tar xf gdb-8.2.tar.gz` を実行してソースを展開します。
1. `pushd gdb-8.2`を実行して, ソースディレクトリに移動します。
1. `./configure --prefix=${HOME}/cross/x64
   --program-prefix="x64-elf-"`を実行します。
1. `make` を実行します。
1. `mkdir -p ${HOME}/cross/x64`
1. `make install` を実行します。
1. `popd` を実行します。

GDBは, x64-elfターゲットをサポートしていないので, --program-prefixオプ
ションを付けることでコマンド名をかえています。
上記実行後, `${HOME}/cross/x64/x64-elf-gdb`を用いてQEmuのx86-64ターゲット上
のプログラムをリモートデバッグすることができます。

