--
--  Create the "fixes" table
--
--  author: Max Kellermann <max.kellermann@gmail.com>
--

CREATE EXTENSION IF NOT EXISTS postgis;

CREATE TABLE IF NOT EXISTS fixes (
        id bigserial PRIMARY KEY,

        key bigint NOT NULL,

        time timestamp NOT NULL DEFAULT now(),

        client_address inet NULL,

        location geometry(Point,4326) NULL,

        direction int NULL,
        speed real NULL,
        altitude real NULL
);

CREATE INDEX IF NOT EXISTS fixes_key_time ON fixes(key, time);
