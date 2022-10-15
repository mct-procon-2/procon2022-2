import pydub
from pydub import AudioSegment
import glob

files = glob.glob("./problems/problem*.wav")
files.sort()

mix = -1
for file in files:
    print(file)

    wav = AudioSegment.from_file(file, format="wav")
    if mix == -1:
      mix = wav
    else:
      mix += wav

mix.export("./problems/connect.wav", format="wav")
