CREATE TABLE user_stats (
    user_id UUID PRIMARY KEY,
    messages_sent INTEGER DEFAULT 0,
    chats_joined INTEGER DEFAULT 0,
    ex_usernames jsonb
);