# ElectronicSho_v0
![Electronic_Sho_全体図](https://github.com/user-attachments/assets/44f45bae-adba-4fe5-b404-774aa6235526)

## 使用技術
Arduino(C/C++),電子回路

## 制作時期
2024年夏

## 概要
笙を電子楽器の形で再現したもの

## 使用例
〇ElectronicSho_Etenraku.mp4  
『平調越殿楽』より、第一行の独奏  
以下のリンクから試聴することも出来ます  
https://drive.google.com/file/d/1ITLJy0-VG6UGjf6nJkPOVr1_WkOjYyIe/view?usp=sharing  
〇ElectronicSho_ad-lib.mp4  
アドリブで演奏したもの  
以下のリンクから試聴することも出来ます  
https://drive.google.com/file/d/19blqC0WjOTID-Sa_yHww3KqB6Vs5NmbD/view?usp=sharing  

## 特徴
1. 雅楽の調律を反映(A=430,三分損益法に基づく調律)
2. 笙の和音(合竹)を実際の笙よりも簡単に演奏しながら、手移り(合竹の変更に関わる各音の変化の規則)も再現できる
3. ゲインは距離センサによって柔軟に操作することができるため、張や息替をある程度再現できる
4. その他機能(後述)によって、実際の笙では不可能な表現を実現できる

## 機能

## 改善点・今後の展望
1. 笙の音作り：単純な正弦波を用いているだけなので、音色を探究する余地が大いにある
2. ノイズ除去：出力にノイズ(距離センサ由来のピッチ感のあるノイズと、諸々の配線に由来する雑音)が混じっているので、取り除く必要がある
3. アナログ/デジタルボタンの実装：周波数つまみやパルスつまみの変化を連続/離散で使い分けできるようにする
4. 基板上で作り直す
5. 笙の和音に関する音楽理論的な理解を深め、どのような表現ができるか探究する
