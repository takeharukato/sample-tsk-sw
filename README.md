タスクスイッチのサンプルプログラム
## 本プログラムについて

本プログラムは, X86-64/aarch64でのタスク（スレッド）切り替えの例を示した
サンプルプログラムです。

## コンパイル手順
トップディレクトリで,以下のコマンドを実行します. 動作確認は, FreeBSD10, CentOS6.2, CentOS7.0で行っています。

***
make <ENTER>
***
しばらく待つとkernelという名前のファイルができます。

## 実行

本プログラムは, ユーザランド上でタスクスイッチの動作を確認するためのサンプルですので, 実機上やシミュレータからブートさせる必要はありません。
シェルのコマンドラインから以下を実行します。

***
./kernel <ENTER>
***

##実行例
本サンプルを実行すると2つのスレッドが自身のスレッド名を画面に表示しな
がらお互いに動作を切り替える様子が観察できます。

```
threadA
threadB
threadA
threadB
```

## aarch64での実行手順
Linux用のaarch64のクロスコンパイラ
(aarch64-none-linux-gnueabi-gcc/aarch64-none-linux-gnueabi-ldなど)への
パスを環境変数PATHに設定し,

```shell-session
export CROSS_COMPILE=aarch64-none-linux-gnueabi-
export CPU=aarch64
```
を実行して環境変数を設定した上で,

```shell-session
make menuconfig
```
を実行し, ``Hardware Abstraction Layer(HAL)''の項目をYに設定し, 
左端の選択表示部分が, [*] になることを確認して, Exitを選択, コンフィグ
レーション情報を保存してください。

その後,
```shell-session
make menuconfig
```
を実行すると```kernel.elf```というaarch64用のサンプルプログラムが生成
されます。

```shell-session
qemu-system-aarch64 -M virt -cpu cortex-a57 -nographic  -kernel
kernel.elf
```
を実行することで, QEmuのaarch64システムシミュレータ上でタスク切り替え
処理が実行されます。

```shell-session:実行例
$ qemu-system-aarch64 -M virt -cpu cortex-a57 -nographic  -kernel
kernel.elf
boot
threadA
threadB
threadC
threadA
threadB
threadA
threadB
threadA
threadB
...
```
