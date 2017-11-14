CREATE TABLE ARTISTS
(
artist   TEXT  ,
artwork TEXT ,
wiki    TEXT,
PRIMARY KEY(artist)
) ;

CREATE TABLE ALBUMS
(
album   TEXT ,
artist  TEXT,
artwork TEXT,
wiki    TEXT,
PRIMARY KEY(album, artist),
FOREIGN KEY(artist) REFERENCES artists(artist)
) ;

CREATE TABLE TAGS
(
tag TEXT NOT NULL,
context TEXT,

PRIMARY KEY(tag)
) ;

CREATE TABLE MOODS
(
mood TEXT PRIMARY KEY
) ;


CREATE TABLE PLAYLISTS
(
playlist       TEXT PRIMARY KEY ,
addDate DATE NOT NULL
) ;

CREATE TABLE SOURCES_TYPES
(
id   INTEGER PRIMARY KEY ,
name TEXT NOT NULL
) ;

CREATE TABLE SOURCES
(
url              TEXT PRIMARY KEY ,
SOURCE_TYPES_id INTEGER NOT NULL,
FOREIGN KEY(SOURCE_TYPES_id) REFERENCES SOURCES_TYPES(id)
) ;

CREATE TABLE TRACKS
(
url TEXT ,
sources_url TEXT  ,
track   INTEGER ,
title   TEXT NOT NULL,
artist  TEXT NOT NULL,
album    TEXT NOT NULL,
duration    INTEGER  ,
comment     TEXT,
played      INTEGER  ,
babe    INTEGER NOT NULL,
stars       INTEGER NOT NULL,
releaseDate DATE ,
addDate     DATE NOT NULL,
lyrics     TEXT ,
genre      TEXT,
art        TEXT,
wiki    TEXT,
PRIMARY KEY (url),
FOREIGN KEY(sources_url) REFERENCES SOURCES(url),
FOREIGN KEY(album, artist) REFERENCES albums(album, artist)
) ;


CREATE TABLE TRACKS_MOODS
(
mood  TEXT NOT NULL ,
url TEXT NOT NULL ,
FOREIGN KEY(mood) REFERENCES MOODS(mood),
FOREIGN KEY(url) REFERENCES TRACKS(url)

) ;

CREATE TABLE TRACKS_TAGS
(
tag  TEXT NOT NULL ,
url TEXT NOT NULL ,
PRIMARY KEY (tag, url),
FOREIGN KEY(tag) REFERENCES TAGS(tag),
FOREIGN KEY(url) REFERENCES TRACKS(url)

) ;

CREATE TABLE ARTISTS_TAGS
(
tag  TEXT NOT NULL ,
artist TEXT NOT NULL ,
PRIMARY KEY (tag, artist),
FOREIGN KEY(tag) REFERENCES TAGS(tag),
FOREIGN KEY(artist) REFERENCES ARTISTS(artist)

) ;

CREATE TABLE ALBUMS_TAGS
(
tag  TEXT NOT NULL ,
album TEXT NOT NULL ,
artist TEXT NOT NULL,
PRIMARY KEY (tag, album, artist),
FOREIGN KEY(tag) REFERENCES TAGS(tag),
FOREIGN KEY(album, artist) REFERENCES ALBUMS(album, artist)
) ;

CREATE TABLE PLAYLISTS_MOODS
(
playlist  TEXT NOT NULL ,
mood TEXT NOT NULL ,
PRIMARY KEY (playlist, mood),
FOREIGN KEY(playlist) REFERENCES PLAYLISTS(playlist),
FOREIGN KEY(mood) REFERENCES MOODS(mood)

) ;

CREATE TABLE TRACKS_PLAYLISTS
(
playlist TEXT NOT NULL ,
url      TEXT NOT NULL ,
addDate DATE NOT NULL,
PRIMARY KEY (playlist, url),
FOREIGN KEY(playlist) REFERENCES PLAYLISTS(playlist),
FOREIGN KEY(url) REFERENCES TRACKS(url)
) ;


CREATE TABLE LOG
(
id INTEGER NOT NULL,
retrieval_date DATE NOT NULL,

PRIMARY KEY(id)
);

--First insertions

INSERT INTO SOURCES_TYPES VALUES (1,"LOCAL");
INSERT INTO SOURCES_TYPES VALUES (2,"ONLINE");
INSERT INTO SOURCES_TYPES VALUES (3,"DEVICE");
