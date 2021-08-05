============================================================

      Screenshot Module for DEVHOOK 2006.10.12版
                                       ねこかぶ
============================================================



DEVHOOKでスクリーンショットが出来るようになる追加モジュールです。
このモジュールを導入する事で、DEVHOOKで動作するゲーム、XMBで
画面のスクリーンショットを保存できます。

●導入方法
付属の「nkthread.prx」、「screenshotbmp.prx」ファイルをメモリースティックの
「ms0:/dh/kb」フォルダ内にコピーし、次のファイルを書き換えます。

	・ゲーム中にスクリーンショットを使う場合
	ms0:/dh/150/flash0/kd/pspbtcnf_game.txt
	ms0:/dh/200/flash0/kd/pspbtcnf_game.txt
	ms0:/dh/250/flash0/kd/pspbtcnf_game.txt
	ms0:/dh/260/flash0/kd/pspbtcnf_game.txt
	ms0:/dh/271/flash0/kd/pspbtcnf_game.txt
	ms0:/dh/150F/flash0/kd/pspbtcnf_game.txt

	・XMB中にスクリーンショットを使う場合
	ms0:/dh/150/flash0/kd/pspbtcnf.txt
	ms0:/dh/200/flash0/kd/pspbtcnf.txt
	ms0:/dh/250/flash0/kd/pspbtcnf.txt
	ms0:/dh/260/flash0/kd/pspbtcnf.txt
	ms0:/dh/271/flash0/kd/pspbtcnf.txt
	ms0:/dh/150F/flash0/kd/pspbtcnf.txt

それぞれのテキストファイル内の「ms0:/dh/kd/devhook.prx」「ms0:/dh/kd/umdciso.prx」の次の行に
「ms0:/dh/kd/nkthread.prx」、「ms0:/dh/kd/screenshotbmp.prx」を追加してください。
※必ずnkthread.prxを先に追加すること

例：
～
/kd/isofs.prx
ms0:/dh/kd/devhook.prx
ms0:/dh/kd/umdciso.prx
ms0:/dh/kd/nkthread.prx      #この行を追加する
ms0:/dh/kd/screenshotbmp.prx #この行を追加する
/kd/wlan.prx



●使い方
スクリーンショットを取りたいところでミュージックノートボタン（♪のボタン）を押してください。
またボタンの組み合わせて4分の1サイズや連続キャプチャが出来ます。

	Music			・・・	画面をファイルにキャプチャ/連続キャプチャモードの終了
	Music ＋ Vol-		・・・	画面を4分の1サイズでキャプチャ ※
	L + Music(+ Vol-)	・・・	連続キャプチャモードの開始


　※メモリースティックに書き込み中は電源を切ったりしないでください


●不具合
モノによってはスクリーンショット保存中もゲームが進行してしまい、
うまく画像を保存できない事があります


●更新履歴
2006/7/3	・公開
2006/7/4	・保存先のディレクトリ(ms0:/PSP/PHOTO/capture)が無かった場合に
		　ファイルが保存されなかったのを修正
		・4分の1、連続キャプチャに対応
2006/7/21	・nkthread.prxファイルを追加
2006/7/24	・一部のゲームで起動できなくなっていた問題を修正
2006/10/12	・連続キャプチャが正しく動作していなかったのを修正
		・乗算、除算をビットシフトが使えれば使うように変更
		・module_stop()は使ってないので削除
		・mok氏の修正を適用
			メモリ確保を出来るだけ大きくしてみた


●謝礼
DEVHOOK Ver.0.46 - modified by mok r04aより、メモリ確保部分の修正を適用しました。
mok氏ありがとうございます。


●ソースコードについて
ソースコードの改変は自由です。
ただし改変したソースコードから生成したバイナリを配布する場合は
改変したソースコードも配布することを作者は望みます。



●＝＝＝　免責事項　＝＝＝
　このソフトウェアによるいかなる損害にも作者は一切責任を負いません。
　各自の責任において使用してください。


ねこかぶ
HP     :http://nekokabu.s7.xrea.com/
mail   :nekokabu@gmail.com
