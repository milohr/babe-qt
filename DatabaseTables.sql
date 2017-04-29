CREATE TABLE IF NOT EXISTS tracks (
  track integer,
  title text,
  artist text,
  album text,
  genre text,
  location text unique,
  stars integer,
  babe integer,
  art text,
  played integer,
  playlist text
);

CREATE TABLE IF NOT EXISTS albums (
  title text,
  artist text,
  art text,
  location text
);

CREATE TABLE IF NOT EXISTS playlists (
  title text,
  art text unique
);

CREATE TABLE IF NOT EXISTS artists (
  title text,
  art text,
  location text
);
