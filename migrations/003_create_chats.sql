CREATE TABLE chats (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(), 
    type TEXT NOT NULL, 
    created_by UUID,                
    title TEXT,                              
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    FOREIGN KEY (created_by) REFERENCES users(id)
);

CREATE TABLE chat_members (
    chat_id UUID NOT NULL,
    user_id UUID NOT NULL,

    PRIMARY KEY (chat_id, user_id),
    FOREIGN KEY (chat_id) REFERENCES chats(id) ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
);