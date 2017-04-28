CREATE TABLE IF NOT EXISTS tracks(
  id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
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

CREATE TABLE IF NOT EXISTS albums(
  id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
  title text,
  artist text,
  art text,
  location text
);

CREATE TABLE IF NOT EXISTS playlists(
  id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
  title text,
  art text unique
);

CREATE TABLE IF NOT EXISTS artists(
  id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
  title text,
  art text,
  location text
);
