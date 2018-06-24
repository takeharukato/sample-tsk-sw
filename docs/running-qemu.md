QEMUでの実行方法

## 実行方法

1. make run を実行
1. qemu-system-aarch64 -M virt -cpu cortex-a57 -m 128 -nographic -serial mon:stdio -kernel ../kernel.elf というオプションで実行される
各オプションの意味は以下の通り：
* -M virt   AArch64 virtマシンで実行
* -m 128    128MBメモリを搭載する
* -nographic グラフィック画面を出さない
* -serial mon:stdio シリアルをモニタと標準出力に出力させる
* -kernel ../kernel.elf  kernel.elfを起動する

## 終了方法
  Ctrl-A x のキーシーケンスを入力することで終了します。
  
