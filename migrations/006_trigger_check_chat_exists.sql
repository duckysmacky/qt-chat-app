CREATE OR REPLACE FUNCTION check_chat_exists()
RETURNS TRIGGER AS $$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM chats WHERE id = NEW.chat_id) THEN
        RAISE EXCEPTION 'Чат с ID % не существует', NEW.chat_id;
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trigger_check_chat_exists
    BEFORE INSERT ON messages
    FOR EACH ROW
    EXECUTE FUNCTION check_chat_exists();