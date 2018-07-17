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
1. `gdb`のプロンプトから`set_aarch64_vectors`または`set_x64_vectors` を実行し, 例外/割込み
   エントリ処理にブレークポイントを設定します(任意)。
1. `gdb`のプロンプトから`b _start`を実行してエントリアドレスでブレーク
   するよう指定します(任意)。
1. `gdb`のプロンプトからデバッグの目的に応じて他のブレークポイントなどを設定します。
1. `gdb`のプロンプトから`continue`を実行して, デバッグを開始します。

## 実行例

AArch64版のカーネルをデバッグする際の手順の例を以下に示します:

```shell-session
$ aarch64-none-elf-gdb
GNU gdb (GDB) 7.12.50.20160823-git
Copyright (C) 2016 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "--host=x86_64-pc-linux-gnu
--target=aarch64-none-elf".
Type "show configuration" for configuration details.
For bug reporting instructions, please see: <http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at: <http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word".
(gdb) aarch64_target
The target architecture is assumed to be aarch64
(gdb) load_symbol
(gdb) b _start
Breakpoint 1 at 0x40000000: file boot.S, line 22.
(gdb) remote_target
warning: No executable has been specified and target does not support
determining executable automatically.  Try using the "file" command.
start () at boot.S:22
22              msr     daifset, #(AARCH64_DAIF_FIQ |
AARCH64_DAIF_IRQ) /* Disable FIQ and IRQ */
(gdb)
```

## x64版のgdbのコンパイル手順
GDBのリモートデバッグ機能とQEmuを接続するためには,
`tools/gdb/gdb-7.10-qemu-x86-64.patch`をGDBに適用してGDBを
構築し直す必要があります。

QEmuのリモートデバッグ機能に対応したGDBの構築方法を以下に示します。

1. `http://ftp.gnu.org/gnu/gdb/gdb-7.10.tar.gz` からGDBのソースアーカイブを取得します。
1. `tar xf gdb-7.10.tar.gz` を実行してソースを展開します。
1. `pushd gdb-7.10`を実行して, ソースディレクトリに移動します。
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

