CREATE TYPE chat_type AS ENUM ('direct', 'group');

UPDATE chats
SET type = CASE
   WHEN type = 'group' THEN 'group'
   ELSE 'direct'
END;

ALTER TABLE chats
    ALTER COLUMN type TYPE chat_type USING type::chat_type;

ALTER TABLE chats
    DROP COLUMN IF EXISTS title;

DROP FUNCTION IF EXISTS search_messages(TEXT, UUID, UUID, INT);

CREATE OR REPLACE FUNCTION search_messages(
    search_query TEXT,
    user_id UUID DEFAULT NULL,
    chat_id UUID DEFAULT NULL,
    limit_count INT DEFAULT 15)
RETURNS TABLE(
    message_id UUID,
    content TEXT,
    created_at TIMESTAMP,
    from_user VARCHAR(255),
    chat_type chat_type
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY
    SELECT m.id, c.content, m.created_at, u.username, ch.type
    FROM messages m
    JOIN content c ON m.content_id = c.id
    JOIN users u ON m.from_id = u.id
    JOIN chats ch ON m.chat_id = ch.id
    WHERE c.content ILIKE '%' || search_query || '%'
        AND (user_id IS NULL OR m.from_id = user_id OR m.to_id = user_id)
        AND (chat_id IS NULL OR m.chat_id = chat_id)
    ORDER BY m.created_at DESC
    LIMIT limit_count;
END $$;

CREATE OR REPLACE PROCEDURE create_or_get_private_chat(
    user1_id UUID,
    user2_id UUID,
    INOUT chat_id UUID DEFAULT NULL
)
LANGUAGE plpgsql
AS $$
DECLARE
    existing_chat_id UUID;
    new_chat_id UUID;
BEGIN
    SELECT DISTINCT cm1.chat_id INTO existing_chat_id
    FROM chat_members cm1
    JOIN chat_members cm2 ON cm1.chat_id = cm2.chat_id
    JOIN chats ch ON cm1.chat_id = ch.id
    WHERE ch.type = 'direct'
        AND cm1.user_id = user1_id
        AND cm2.user_id = user2_id;

    IF existing_chat_id IS NOT NULL THEN
        chat_id := existing_chat_id;
        RETURN;
    END IF;

    INSERT INTO chats (type, created_by) VALUES ('direct'::chat_type, user1_id) RETURNING id INTO new_chat_id;
    INSERT INTO chat_members (chat_id, user_id) VALUES (new_chat_id, user1_id), (new_chat_id, user2_id);

    chat_id := new_chat_id;
END $$;
