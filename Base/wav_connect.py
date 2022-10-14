import pydub
from pydub import AudioSegment

# 無音データ(重ね合わせ用)
print("混ぜるファイル数を入力!!")
num = int(input())
mix = -1
for i in range(0, num):
    print("ファイル名を入力(拡張子なし)(順番に)")
    print("problems/", end="")
    s = input()
    wav = AudioSegment.from_file("./problems/" + s + ".wav", format="wav")
    if mix == -1:
      mix = wav
    else:
      mix += wav

mix.export("./problems/connect.wav", format="wav")