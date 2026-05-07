ALTER TABLE users RENAME COLUMN name TO display_name;

UPDATE users
SET username = lower(username);

ALTER TABLE users
    ADD CONSTRAINT users_username_format_check
        CHECK (username ~ '^[a-z0-9_]{2,20}$');