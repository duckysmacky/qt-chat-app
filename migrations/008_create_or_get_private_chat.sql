CREATE OR REPLACE PROCEDURE create_or_get_private_chat(
    user1_id UUID,
    user2_id UUID,
    INOUT chat_id UUID DEFAULT NULL
)
LANGUAGE plpgsql AS $$
DECLARE
    existing_chat_id UUID;
    new_chat_id UUID;
BEGIN
    SELECT DISTINCT cm1.chat_id INTO existing_chat_id
    FROM chat_members cm1
    JOIN chat_members cm2 ON cm1.chat_id = cm2.chat_id
    JOIN chats ch ON cm1.chat_id = ch.id
    WHERE ch.type = 'private'
        AND cm1.user_id = user1_id
        AND cm2.user_id = user2_id;
    
    IF existing_chat_id IS NOT NULL THEN
        chat_id := existing_chat_id;
        RAISE NOTICE 'Найден существующий чат между пользователями % и %: %', user1_id, user2_id, chat_id;
        RETURN;
    END IF;
    
    INSERT INTO chats (type) VALUES ('private') RETURNING id INTO new_chat_id;
    INSERT INTO chat_members (chat_id, user_id) VALUES (new_chat_id, user1_id), (new_chat_id, user2_id);
    
    chat_id := new_chat_id;
    RAISE NOTICE 'Создан новый приватный чат между пользователями % и %: %', user1_id, user2_id, chat_id;
END;
$$;