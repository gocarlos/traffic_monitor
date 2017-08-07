#!/usr/bin/env python3
import pafy

from vigilatore import vigilatore
# from tests import test_data

def download_test_video():
    # Download move from youtube
    url = "https://www.youtube.com/watch?v=gP5PupjD2po"
    video = pafy.new(url)
    best = video.getbest(preftype="mp4")
    filename = best.download(quiet=False)

    # Rename it
    for file in os.listdir("."):
        if file.endswith(".mp4"):
            os.rename(file, 'movie.mp4')
    print('test video downloaded')

if __name__ == '__main__':
    # Download data
    download_test_video()



