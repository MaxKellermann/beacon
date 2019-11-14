--
--  Grant all necessary privileges to the "beacon-receiver"
--  user/daemon.
--
--  author: Max Kellermann <max.kellermann@gmail.com>
--

CREATE ROLE "beacon-receiver" WITH LOGIN;

GRANT INSERT ON fixes TO "beacon-receiver";
GRANT UPDATE, SELECT ON fixes_id_seq TO "beacon-receiver";
