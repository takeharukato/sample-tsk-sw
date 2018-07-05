# QEMUでの実行方法

## 実行方法

1. make run を実行
   実行されるコマンドは, 以下の通りです。
   * AArch64版
      qemu-system-aarch64 -M virt -cpu cortex-a57 -m 128 -nographic
      -serial mon:stdio -kernel ../kernel.elf 
	  各オプションの意味は以下の通りです:
	  * -M virt   AArch64 virtマシンで実行
	  * -m 128    128MiBメモリ搭載環境として起動
	  * -nographic グラフィック画面を出さない
	  * -serial mon:stdio シリアルをモニタと標準出力に出力させる
	  * -kernel ../kernel.elf  kernel.elfを起動する
   * X64版
      qemu-system-x86_64 -m 128 -nographic -nographic -serial
      mon:stdio -cdrom kernel.iso
      れます。
	  各オプションの意味は以下の通りです:
	  * -m 128     128MiBメモリ搭載環境として起動
	  * -nographic グラフィック画面を出さない
	  * -serial mon:stdio シリアルをモニタと標準出力に出力させる
	  * -cdrom kernel.iso  kernel.iso(Grub2からkernel.elfを起動するた
        めのCDROMイメージ）から起動する

## 終了方法
  Ctrl-A x のキーシーケンスを入力することで終了します。
  
