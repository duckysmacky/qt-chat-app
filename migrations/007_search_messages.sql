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
    chat_title VARCHAR(255)
) AS $$
BEGIN
    RETURN QUERY
    SELECT m.id, c.content, m.created_at, u.username, ch.title
    FROM messages m
    JOIN content c ON m.content_id = c.id
    JOIN users u ON m.from_id = u.id
    JOIN chats ch ON m.chat_id = ch.id
    WHERE c.content ILIKE '%' || search_query || '%'
        AND (user_id IS NULL OR m.from_id = user_id OR m.to_id = user_id)
        AND (chat_id IS NULL OR m.chat_id = chat_id)
    ORDER BY m.created_at DESC
    LIMIT limit_count;
END;
$$ LANGUAGE plpgsql;