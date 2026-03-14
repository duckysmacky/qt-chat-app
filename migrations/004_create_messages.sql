CREATE TABLE messages (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    to_id UUID NOT NULL,
    from_id UUID NOT NULL,
    content_id UUID NOT NULL,
    chat_id UUID NOT NULL,

    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP,
    -------
    FOREIGN KEY (to_id) REFERENCES users(id),
    FOREIGN KEY (from_id) REFERENCES users(id),
    FOREIGN KEY (content_id) REFERENCES contents(id),
    FOREIGN KEY (chat_id) REFERENCES chats(id)
);