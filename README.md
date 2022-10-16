# procon2022

## CNN,RNNはダメでした（いろいろ工夫してる途中であきらめた）

Baseにあるwavread.cpp,solve.exeは元々のアイディアのもの
（微小な間隔が一致する部分を見つけるというアイディア）

wavread2.cpp,solve[2~6].exeは旭川高専のアイディアをもとに作ったもの


Base/に入って
cmd上で


> ruby trans.rb info

でサーバから問題の情報をとる


> ruby trans.rb get 1

で分割データ1つ受け取る


> solve5

で問題を解く
(result.txtが出力)


> answer

でresult.txtを修正できる


> ruby trans.rb post

で回答を提出
