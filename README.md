# Babe
## Tiny Qt Babe Music Player

#### Babe is a tiny Qt music player to keep you favorite songs at hand

https://milohr.github.io/BabeIt/

Babe will handle your whole music collection, letting you create custom playlists to organize your music.

You can also filter your music by artist, title, album, genre, date and location. Babe let's you babe-mark your favorite YouTube music videos into your local collection by using the Chromium extension. 

Think of Babe as the playlist where all your favorite tracks at the moment are. And when needed you also have an integrated collection manager.


(a previous unfinished gtk3 version is still in my repository and I plan to turn it into a simple version of Noise for the Pantheon desktop of elementaryOS) 



##### you need to have [ taglib - knotification lib - qt 5.8 libs ]installed in order to compile this app from source
any extra help to package this app is welcome and I'm willing to solve any doubts or questions about it.


<h3> To run: </h3>
qmake && make && ./Babe



* Planned Features :
  ** youtube-dl streamming [5%]
  ** babe chrome-chromium extension for youtube-dl supported sites [90%]
  ** krunner integration[0%]

* Expected Features :
  ** simple music collection manager [100%]
  ** artist and lyrics info [100%] 
  
  
<h3> Features : </h3> 

    -Babe has three different view modes: 
    * the mini mode keeps on top of the windows and its size is just 200x200 px ,just displaying the art and playback controls
    * the playlist mode is unobtrusive and displays a list of your songs
    * the collection view let's you browse your whole music collection by tracks, artists and albums, also let's you get information about the current song playing.
    
    -search keys to filter the results: 
    
    * location:  (example: to see all the tracks from the YouTube extension: "location:youtube")
    * artist: (get all matching artists)
    * album: (get all matching albums)    
    * title: (get all matching titles)
    * genre: (get all matching genres)
    
    -quickly append search results, albums, artists and tracks to the main playlist or save them to a specific playlist
    
    -create colored mood tags for tracks that modify the main playlist color
    
    -use the chromium extension to babe your favorite youtube music videos and Babe will fetch the art and metadata of the track
    
    -quickly move between albums/artists with the playAll button that shows on hover in all the artworks
    
    -get native kde notifications for the plasma desktop
    
    -add songs to a queued special playlist if you want to play some track next
    

<h3> Playlist Mode </h3>
![alt tag](https://github.com/milohr/babe-qt/blob/master/screenshots/playlist_mode.png?raw=true) 

<h3> Mini Mode </h3>
![alt tag](https://github.com/milohr/babe-qt/blob/master/screenshots/mini_mode.png?raw=true)

<h3> Collection View </h3>
![alt tag](https://github.com/milohr/babe-qt/blob/master/screenshots/collection_view.png?raw=true)

<h3> Albums View </h3>
![alt tag](https://github.com/milohr/babe-qt/blob/master/screenshots/albums_view.png?raw=true) 

<h3> Artists View </h3>
![alt tag](https://github.com/milohr/babe-qt/blob/master/screenshots/artists_view.png?raw=true) 

<h3> Playlists View </h3>
![alt tag](https://github.com/milohr/babe-qt/blob/master/screenshots/playlists_view.png?raw=true) 

<h3> Settings View </h3>
![alt tag](https://github.com/milohr/babe-qt/blob/master/screenshots/settings_view.png?raw=true) 

<h3> About View </h3>
![alt tag](https://github.com/milohr/babe-qt/blob/master/screenshots/about_view.png?raw=true) 

<h3> Chromium Extension </h3>
![alt tag](https://github.com/milohr/babe-qt/blob/master/screenshots/chromium_extension.png?raw=true) 

