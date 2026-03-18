CREATE OR REPLACE FUNCTION stats_message_insert()
RETURNS TRIGGER AS $$
BEGIN
    INSERT INTO user_stats (user_id, messages_sent, last_activity)
    VALUES (NEW.from_id, 1, CURRENT_TIMESTAMP)
    ON CONFLICT (user_id)
    DO UPDATE SET
        messages_sent = user_stats.messages_sent + 1,
        last_activity = CURRENT_TIMESTAMP;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION stats_message_delete()
RETURNS TRIGGER AS $$
BEGIN
    UPDATE user_stats
    SET messages_sent = messages_sent - 1
    WHERE user_id = OLD.from_id;

    RETURN OLD;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION stats_chat_join()
RETURNS TRIGGER AS $$
BEGIN
    INSERT INTO user_stats (user_id, chats_joined)
    VALUES (NEW.user_id, 1)
    ON CONFLICT (user_id)
    DO UPDATE SET
        chats_joined = user_stats.chats_joined + 1;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION stats_chat_leave()
RETURNS TRIGGER AS $$
BEGIN
    UPDATE user_stats
    SET chats_joined = chats_joined - 1
    WHERE user_id = OLD.user_id;
    RETURN OLD;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION track_old_username_sql()
RETURNS TRIGGER AS $$
BEGIN
    UPDATE user_stats
    SET ex_usernames = COALESCE(ex_usernames, '[]'::jsonb) || to_jsonb(OLD.username)
    WHERE user_id = NEW.id;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

---------------------------------------------

CREATE TRIGGER trigger_stats_message_insert
AFTER INSERT ON messages
FOR EACH ROW
EXECUTE FUNCTION stats_message_insert();

CREATE TRIGGER trigger_stats_message_delete
AFTER DELETE ON messages
FOR EACH ROW
EXECUTE FUNCTION stats_message_delete();

CREATE TRIGGER trigger_stats_chat_join
AFTER INSERT ON chat_members
FOR EACH ROW
EXECUTE FUNCTION stats_chat_join();

CREATE TRIGGER trigger_stats_chat_leave
AFTER DELETE ON chat_members
FOR EACH ROW
EXECUTE FUNCTION stats_chat_leave();

CREATE TRIGGER trigger_track_old_username
BEFORE UPDATE OF username ON users
FOR EACH ROW
EXECUTE FUNCTION track_old_username_sql();
