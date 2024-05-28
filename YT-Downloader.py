import os
import youtube_dl

def download_playlist(playlist_url):
    ydl_opts = {
        'format': 'bestvideo[ext=mp4]+bestaudio[ext=m4a]/best[ext=mp4]/best',
        'outtmpl': '%(title)s.%(ext)s'
    }

    with youtube_dl.YoutubeDL(ydl_opts) as ydl:
        ydl.download(playlist_url)

def main():
    playlist_url = input("Enter the YouTube playlist URL: ")
    download_playlist(playlist_url)

if __name__ == "__main__":
    main()
