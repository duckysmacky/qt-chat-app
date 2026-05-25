CREATE TABLE chat_keys (
    chat_id UUID NOT NULL,
    user_id UUID NOT NULL,
    salt BYTEA NOT NULL,
    encrypted_key BYTEA NOT NULL,
    checksum TEXT NOT NULL,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY (chat_id, user_id),

    FOREIGN KEY (chat_id)
        REFERENCES chats(id)
        ON DELETE CASCADE,

    FOREIGN KEY (user_id)
        REFERENCES users(id)
        ON DELETE CASCADE
);
