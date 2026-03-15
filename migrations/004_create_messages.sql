CREATE TABLE messages (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),

    to_id UUID,
    from_id UUID,

    content_id UUID NOT NULL,
    chat_id UUID NOT NULL,

    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP,

    FOREIGN KEY (to_id) 
        REFERENCES users(id) 
        ON DELETE SET NULL,

    FOREIGN KEY (from_id) 
        REFERENCES users(id) 
        ON DELETE SET NULL,

    FOREIGN KEY (content_id) 
        REFERENCES contents(id) 
        ON DELETE CASCADE,

    FOREIGN KEY (chat_id) 
        REFERENCES chats(id) 
        ON DELETE CASCADE
);