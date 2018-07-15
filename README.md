# タスクスイッチのサンプルプログラム
## 本プログラムについて

本プログラムは, X64/AArch64でのタスク（スレッド）切り替えの例を示した
サンプルプログラムです。

## X64 での実行手順
### コンパイル手順
トップディレクトリで,以下のコマンドを実行します.
動作確認は, FreeBSD10, CentOS6.2, CentOS7.0で行っています。

```shell-session
$ make
```
しばらく待つとkernel.elfという名前のファイルができます。

### 実行手順

本プログラムは, ユーザランド上でタスクスイッチの動作を確認するためのサ
ンプルですので, 実機上やシミュレータからブートさせる必要はありません。

```shell-session
$ make menuconfig
```
を実行し, ``Hardware Abstraction Layer(HAL)``の項目を`N`に設定し, 
左端の選択表示部分が, `[ ]` になることを確認して, `Exit`を選択, コンフィ
グレーション情報を保存してください。 

その後,
```shell-session
$ make
```
を実行することで, ```kernel.elf```というx64用のサンプルプログラムが生
成されます。

次に, シェルのコマンドラインから以下を実行します:

```shell-session
$ ./kernel.elf
```
本サンプルを実行すると2つのスレッドが自身のスレッド名を画面に表示しな
がらお互いに動作を切り替える様子が観察できます。実行例は以下の通りです。

```
threadA
threadB
threadA
threadB
```

## AArch64での実行手順
### コンパイル手順
Linux用のAArch64のクロスコンパイラ
(aarch64-none-linux-gnueabi-gcc/aarch64-none-linux-gnueabi-ldなど)への
パスを環境変数PATHに設定し,

```shell-session
$ export CROSS_COMPILE=aarch64-none-linux-gnueabi-
$ export CPU=aarch64
```
を実行して環境変数を設定した上で,

```shell-session
$ make menuconfig
```
を実行し, ``Hardware Abstraction Layer(HAL)``の項目を`Y`に設定し, 
左端の選択表示部分が, `[*]` になることを確認して, `Exit`を選択, コンフィ
グレーション情報を保存してください。 

その後,
```shell-session
$ make
```
を実行すると```kernel.elf```というAArch64用のサンプルプログラムが生成
されます。

### 実行手順

以下のコマンドを実行することで, QEmuのAArch64システムシミュレータ上で
タスク切り替え処理が実行されます。
```shell-session
$ qemu-system-aarch64 -M virt -cpu cortex-a57 -nographic  -kernel
kernel.elf
```

実行例を以下に示します:

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
