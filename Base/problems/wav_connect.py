import pydub
from pydub import AudioSegment

# 無音データ(重ね合わせ用)
print("混ぜるファイル数を入力!!")
num = int(input())
mix = -1
for i in range(0, num):
    print("ファイル名を入力(拡張子なし)(順番に)")
    s = input()
    wav = AudioSegment.from_file("./" + s + ".wav", format="wav")
    if mix == -1:
      mix = wav
    else:
      mix += wav

mix.export("./connect.wav", format="wav")